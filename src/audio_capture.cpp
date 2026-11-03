#include "audio_capture.hpp"

#include <QIODevice>

#include <algorithm>
#include <cmath>
#include <limits>

AudioCapture::AudioCapture(QObject* parent)
    : QObject(parent), pitchRing_(kWindowSamples, 0.0f) {
    measureTimer_.setInterval(65);
    connect(&measureTimer_, &QTimer::timeout,
            this, &AudioCapture::updateMeasurements);
    measureTimer_.start();
}

AudioCapture::~AudioCapture() { stop(); }

bool AudioCapture::start(const QAudioDevice& device) {
    stop();
    if (device.isNull()) {
        emit errorOccurred("No audio input device selected.");
        return false;
    }

    // Prefer 48 kHz mono Float for a simple calibrated input path; fall back to
    // the audio device's advertised format (with interleaved channel handling).
    QAudioFormat preferred;
    preferred.setSampleRate(48000);
    preferred.setChannelCount(1);
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
    connect(source_.get(), &QAudioSource::stateChanged, this,
            [this](QtAudio::State state) {
        if (state == QtAudio::StoppedState && source_ &&
            source_->error() != QtAudio::NoError)
            emit errorOccurred("Audio input stopped unexpectedly. Choose another device or check permissions.");
    });
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
    std::fill(pitchRing_.begin(), pitchRing_.end(), 0.0f);
    ringWrite_ = 0;
    ringCount_ = 0;
    lowpass_ = 0.0;
    resamplePhase_ = 0.0;
    sumSquares_ = 0.0;
    blockSamples_ = 0;
    peak_ = 0.0;
    clipping_ = false;
    smoothedHz_ = 0.0;
    framesSinceLastEstimate_ = 0;
    emit pitchReady(0, 0);
    emit levelReady(-120, -120, false);
}

void AudioCapture::acceptSample(float sample) {
    // A one-pole prefilter suppresses high-frequency content before decimation.
    // The phase accumulator supports capture rates other than 48 kHz.
    const double sampleRate = static_cast<double>(format_.sampleRate());
    const double alpha = std::min(1.0, 2.0 * 3.14159265358979323846 * 1200.0 / sampleRate);
    lowpass_ += alpha * (static_cast<double>(sample) - lowpass_);
    resamplePhase_ += kAnalysisRate;
    if (resamplePhase_ >= sampleRate) {
        resamplePhase_ -= sampleRate;
        pitchRing_[ringWrite_] = static_cast<float>(lowpass_);
        ringWrite_ = (ringWrite_ + 1) % pitchRing_.size();
        ringCount_ = std::min(ringCount_ + 1, pitchRing_.size());
        ++framesSinceLastEstimate_;
    }
}

void AudioCapture::consumeAudio() {
    if (!input_ || format_.bytesPerFrame() <= 0) return;
    partialFrame_.append(input_->readAll());
    const qsizetype bytesPerFrame = format_.bytesPerFrame();
    const qsizetype bytesPerSample = format_.bytesPerSample();
    const qsizetype available = (partialFrame_.size() / bytesPerFrame) * bytesPerFrame;
    if (available <= 0) return;

    const char* data = partialFrame_.constData();
    for (qsizetype offset = 0; offset < available; offset += bytesPerFrame) {
        double mono = 0.0;
        double hottestChannel = 0.0;
        for (int ch = 0; ch < format_.channelCount(); ++ch) {
            const double channel = format_.normalizedSampleValue(
                data + offset + ch * bytesPerSample);
            mono += channel;
            hottestChannel = std::max(hottestChannel, std::abs(channel));
        }
        mono /= static_cast<double>(format_.channelCount());
        const float sample = static_cast<float>(std::clamp(mono, -1.0, 1.0));
        sumSquares_ += static_cast<double>(sample) * sample;
        ++blockSamples_;
        // A clipped stereo input must not be masked by an empty channel.
        peak_ = std::max(peak_, hottestChannel);
        clipping_ = clipping_ || hottestChannel >= 0.999;
        acceptSample(sample);
    }
    partialFrame_.remove(0, available);
}

void AudioCapture::updateMeasurements() {
    if (!source_ || blockSamples_ == 0) {
        emit levelReady(-120, -120, false);
        emit pitchReady(0, 0);
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

    if (clipped || rmsDb < minimumLevelDb_ || ringCount_ < pitchRing_.size()) {
        smoothedHz_ = 0.0;
        emit pitchReady(0, 0);
        return;
    }
    // Avoid reanalyzing the same buffer when the capture source stalls.
    if (framesSinceLastEstimate_ < kAnalysisRate / 100) return;
    framesSinceLastEstimate_ = 0;

    std::vector<float> frame(pitchRing_.size());
    for (std::size_t i = 0; i < frame.size(); ++i)
        frame[i] = pitchRing_[(ringWrite_ + i) % pitchRing_.size()];

    const auto estimate = detector_.estimate(frame, kAnalysisRate, 25.0, 700.0);
    if (!estimate.valid) {
        smoothedHz_ = 0.0;
        emit pitchReady(0, 0);
        return;
    }
    // Light smoothing only for nearby successive estimates; switching strings
    // intentionally jumps to the new absolute pitch rather than sweeping.
    if (smoothedHz_ > 0 &&
        std::abs(1200.0 * std::log2(estimate.hz / smoothedHz_)) < 230.0)
        smoothedHz_ *= std::pow(estimate.hz / smoothedHz_, 0.45);
    else
        smoothedHz_ = estimate.hz;
    emit pitchReady(smoothedHz_, estimate.confidence);
}
