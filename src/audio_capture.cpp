#include "audio_capture.hpp"
#include "input_frame.hpp"

#include <QIODevice>

#include <algorithm>
#include <cmath>
#include <limits>

AudioCapture::AudioCapture(QObject* parent)
    : QObject(parent), pitchRing_(kWindowSamples, 0.0f) {
    clock_.start();
    measureTimer_.setInterval(65);
    connect(&measureTimer_, &QTimer::timeout,
            this, &AudioCapture::updateMeasurements);
    measureTimer_.start();
}

AudioCapture::~AudioCapture() { stop(); }

bool AudioCapture::start(const QAudioDevice& device, int channel) {
    stop();
    if (device.isNull()) {
        emit errorOccurred("No audio input device selected.");
        return false;
    }

    // Preserve the advertised multichannel layout rather than requesting a mono downmix.
    QAudioFormat preferred;
    preferred.setSampleRate(48000);
    preferred.setChannelCount(std::max(1, device.maximumChannelCount()));
    preferred.setSampleFormat(QAudioFormat::Float);
    if (device.isFormatSupported(preferred)) {
        format_ = preferred;
    } else {
        preferred.setSampleFormat(QAudioFormat::Int16);
        format_ = device.isFormatSupported(preferred)
            ? preferred : device.preferredFormat();
    }
    if (!format_.isValid() || format_.sampleRate() < kAnalysisRate ||
        format_.bytesPerFrame() == 0) {
        emit errorOccurred("Selected input has no supported audio format.");
        return false;
    }

    inputChannel_ = channel >= 0 && channel < format_.channelCount() ? channel : 0;
    channelSamples_.resize(format_.channelCount());
    source_ = std::make_unique<QAudioSource>(device, format_);
    // Keep the platform-specific buffer moderately small; the analysis window
    // determines detection latency and can be optimized independently.
    source_->setBufferSize(format_.bytesForFrames(format_.sampleRate() / 12));
    input_ = source_->start();
    if (!input_) {
        emit errorOccurred("Could not open audio input. Check microphone permission and device availability.");
        source_.reset();
        return false;
    }
    connect(input_, &QIODevice::readyRead, this, &AudioCapture::consumeAudio);
    // Derive enum types from the API to support both Qt 6.5's QAudio and
    // Qt 6.7+'s QtAudio namespace.
    using AudioState = decltype(source_->state());
    using AudioError = decltype(source_->error());
    connect(source_.get(), &QAudioSource::stateChanged, this,
            [this](AudioState state) {
        if (state == AudioState::StoppedState && source_) {
            input_ = nullptr;
            partialFrame_.clear();
            resetAnalysis();
            if (source_->error() != AudioError::NoError)
                emit errorOccurred("Audio input stopped unexpectedly. Choose another device or check permissions.");
        }
    });
    consumeAudio(); // Drain any bytes queued before readyRead was connected.
    return true;
}

void AudioCapture::stop() {
    if (source_) {
        source_->stop();
        source_.reset();
    }
    input_ = nullptr;
    format_ = QAudioFormat{};
    partialFrame_.clear();
    channelSamples_.clear();
    resetAnalysis();
}

void AudioCapture::resetPitchWindow() {
    ringWrite_ = 0;
    ringCount_ = 0;
    prefilter_.reset();
    resamplePhase_ = 0.0;
    framesSinceLastEstimate_ = 0;
}

void AudioCapture::resetAnalysis() {
    resetPitchWindow();
    sumSquares_ = 0.0;
    blockSamples_ = 0;
    peak_ = 0.0;
    clipping_ = false;
    smoothedHz_ = 0.0;
    lastAudioMs_ = -1;
    lastPitchMs_ = -1;
    gateOpen_ = false;
    emit pitchReady(0, 0);
    emit levelReady(-120, -120, false);
}

void AudioCapture::acceptSample(float sample) {
    const double sampleRate = static_cast<double>(format_.sampleRate());
    const double filtered = prefilter_.process(sample, format_.sampleRate());
    resamplePhase_ += kAnalysisRate;
    if (resamplePhase_ >= sampleRate) {
        resamplePhase_ -= sampleRate;
        pitchRing_[ringWrite_] = static_cast<float>(filtered);
        ringWrite_ = (ringWrite_ + 1) % pitchRing_.size();
        ringCount_ = std::min(ringCount_ + 1, pitchRing_.size());
        ++framesSinceLastEstimate_;
    }
}

void AudioCapture::consumeAudio() {
    if (!input_ || format_.bytesPerFrame() <= 0) return;
    processBytes(input_->readAll(), clock_.elapsed());
}

void AudioCapture::processBytes(const QByteArray& bytes, qint64 nowMs) {
    if (format_.bytesPerFrame() <= 0) return;
    // A genuine transport gap invalidates the old analysis window, even when
    // readyRead resumes before the next UI timer tick. Preserve byte alignment.
    if (!bytes.isEmpty() && lastAudioMs_ >= 0 &&
        nowMs - lastAudioMs_ >= kInputTimeoutMs) resetAnalysis();
    partialFrame_.append(bytes);
    const qsizetype bytesPerFrame = format_.bytesPerFrame();
    const qsizetype bytesPerSample = format_.bytesPerSample();
    const qsizetype available = (partialFrame_.size() / bytesPerFrame) * bytesPerFrame;
    if (available <= 0) return;
    lastAudioMs_ = nowMs;

    const char* data = partialFrame_.constData();
    for (qsizetype offset = 0; offset < available; offset += bytesPerFrame) {
        for (int ch = 0; ch < format_.channelCount(); ++ch)
            channelSamples_[ch] = format_.normalizedSampleValue(data + offset + ch * bytesPerSample);
        const auto frame = strings::selectInputFrame(channelSamples_, inputChannel_);
        const float sample = frame.sample;
        sumSquares_ += static_cast<double>(sample) * sample;
        ++blockSamples_;
        peak_ = std::max(peak_, frame.peak);
        clipping_ = clipping_ || frame.clipped || !frame.valid;
        acceptSample(sample);
    }
    partialFrame_.remove(0, available);
}

void AudioCapture::clearPitch() {
    smoothedHz_ = 0.0;
    lastPitchMs_ = -1;
    gateOpen_ = false;
    emit pitchReady(0, 0);
}

void AudioCapture::rejectPitch(qint64 nowMs) {
    // One uncertain estimate or a short level dip must not blink the readout.
    // This deadline is measured from the last valid estimate, never extended
    // by further failures, so a dead signal cannot stay on screen indefinitely.
    if (lastPitchMs_ < 0 || nowMs - lastPitchMs_ >= kPitchHoldMs) clearPitch();
}

void AudioCapture::updateMeasurements() {
    if (!source_) return; // stop()/resetAnalysis() already clears the display.
    consumeAudio(); // readyRead and the GUI timer need not arrive in lockstep.
    updateMeasurementsAt(clock_.elapsed());
}

void AudioCapture::updateMeasurementsAt(qint64 nowMs) {
    if (lastAudioMs_ >= 0 && nowMs - lastAudioMs_ >= kInputTimeoutMs) {
        resetAnalysis();
        return;
    }
    if (blockSamples_ == 0) {
        // No new packet is different from a packet containing silence.
        // Keep the last meter value until the transport timeout expires.
        rejectPitch(nowMs);
        return;
    }
    const double rms = std::sqrt(sumSquares_ / blockSamples_);
    const double rmsDb = 20.0 * std::log10(std::max(1e-9, rms));
    const double peakDb = 20.0 * std::log10(std::max(1e-9, peak_));
    const bool clipped = clipping_;
    sumSquares_ = 0.0;
    blockSamples_ = 0;
    peak_ = 0.0;
    clipping_ = false;
    emit levelReady(rmsDb, peakDb, clipped);

    if (clipped) {
        // Require a fresh window so the next clean packet cannot revive
        // an estimate from samples collected before or during clipping.
        resetPitchWindow();
        clearPitch();
        return;
    }
    // Small hysteresis prevents repeated gate opening/closing near MIN.
    const double gateThreshold = minimumLevelDb_ - (gateOpen_ ? 3.0 : 0.0);
    if (rmsDb < gateThreshold || ringCount_ < pitchRing_.size()) {
        rejectPitch(nowMs);
        return;
    }
    gateOpen_ = true;
    // Avoid reanalyzing the same buffer when the capture source stalls.
    if (framesSinceLastEstimate_ < kAnalysisRate / 100) {
        rejectPitch(nowMs);
        return;
    }
    framesSinceLastEstimate_ = 0;

    std::vector<float> frame(pitchRing_.size());
    for (std::size_t i = 0; i < frame.size(); ++i)
        frame[i] = pitchRing_[(ringWrite_ + i) % pitchRing_.size()];

    const auto estimate = detector_.estimate(frame, kAnalysisRate, 25.0, 700.0);
    if (!estimate.valid) {
        rejectPitch(nowMs);
        return;
    }
    // Light smoothing only for nearby successive estimates; switching strings
    // intentionally jumps to the new absolute pitch rather than sweeping.
    if (smoothedHz_ > 0 &&
        std::abs(1200.0 * std::log2(estimate.hz / smoothedHz_)) < 230.0)
        smoothedHz_ *= std::pow(estimate.hz / smoothedHz_, 0.45);
    else
        smoothedHz_ = estimate.hz;
    lastPitchMs_ = nowMs;
    emit pitchReady(smoothedHz_, estimate.confidence);
}

bool AudioCapture::setInputChannel(int channel) {
    if (!source_ || channel < 0 || channel >= format_.channelCount()) return false;
    if (channel != inputChannel_) {
        inputChannel_ = channel;
        // Keep partialFrame_: dropping its bytes would lose interleaved frame alignment.
        resetAnalysis();
    }
    return true;
}
