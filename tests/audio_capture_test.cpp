#include "audio_capture.hpp"
#include "tuning.hpp"
#include "numeric_setting.hpp"
#include <limits>
#include <QCoreApplication>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <random>

static void require(bool condition, const char* message) {
    if (!condition) { std::cerr << message << '\n'; std::exit(1); }
}

// Feed the production PCM decoder and measurement path with a deterministic
// clock. No audio device, microphone permission, sleeping or QtTest dependency.
class AudioCaptureTest {
public:
    static void run() {
        // Signal receivers outlive capture, which also emits reset on shutdown.
        double pitch = 0, level = -120;
        int pitchClears = 0, levelClears = 0;
        AudioCapture capture;
        capture.measureTimer_.stop();
        capture.format_.setSampleRate(48000);
        capture.format_.setChannelCount(2);
        capture.format_.setSampleFormat(QAudioFormat::Float);
        capture.channelSamples_.resize(2);
        QObject::connect(&capture, &AudioCapture::pitchReady,
            [&](double hz, double) { pitch = hz; if (hz == 0) ++pitchClears; });
        QObject::connect(&capture, &AudioCapture::levelReady,
            [&](double rms, double, bool) { level = rms; if (rms <= -120) ++levelClears; });
        int sampleIndex = 0;
        auto tone = [&](int frames, double hz = 146.8324, double amplitude = 0.1) {
            QByteArray pcm(frames * 2 * sizeof(float), '\0');
            for (int i = 0; i < frames; ++i) {
                const float value = amplitude * std::sin(2.0 * 3.141592653589793 * hz * sampleIndex++ / 48000.0);
                std::memcpy(pcm.data() + i * 2 * sizeof(float), &value, sizeof(value));
            }
            return pcm;
        };
        // 100 ms packets versus a 65 ms GUI timer: old code cleared every
        // packet-free tick even though the transport was healthy.
        int nextPacket = 0;
        int startupPitchClears = 0;
        for (int now = 0; now <= 1950; now += 65) {
            if (now >= nextPacket) {
                capture.processBytes(tone(4800), now);
                nextPacket += 100;
            }
            capture.updateMeasurementsAt(now);
            if (now == 325) startupPitchClears = pitchClears;
            if (now >= 390) require(std::abs(pitch - 146.8324) < 0.5, "steady D3 blinked between packets");
        }
        require(pitchClears == startupPitchClears && levelClears == 0, "packet jitter emitted false silence");
        const int last = capture.lastAudioMs_;
        capture.updateMeasurementsAt(last + 195);
        require(pitch > 0 && level > -120, "short gap cleared display");
        capture.updateMeasurementsAt(last + 260);
        require(pitch == 0 && level == -120 && capture.ringCount_ == 0, "stalled transport retained stale data");
        // Resume with E4: the old D3 window must not leak into the new session.
        capture.processBytes(tone(4800, 329.6276), last + 300);
        capture.updateMeasurementsAt(last + 300);
        require(pitch == 0, "resumption reused pre-stall pitch window");
        capture.processBytes(tone(16000, 329.6276), last + 400);
        capture.updateMeasurementsAt(last + 400);
        require(std::abs(pitch - 329.6276) < 0.7, "resumed E4 not recognized");
        // A single uncertain window is held briefly; repeated failures expire.
        const int validAt = last + 400;
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> noise(-0.1f, 0.1f);
        QByteArray randomPcm(18000 * 2 * sizeof(float), '\0');
        for (int i = 0; i < 18000; ++i) {
            const float value = noise(rng);
            std::memcpy(randomPcm.data() + i * 2 * sizeof(float), &value, sizeof(value));
        }
        capture.processBytes(randomPcm, validAt + 65);
        capture.updateMeasurementsAt(validAt + 65);
        require(pitch > 0 && capture.lastPitchMs_ == validAt, "isolated failed estimate blinked or renewed deadline");
        for (int delta : {130, 195, 260}) {
            capture.processBytes(randomPcm, validAt + delta);
            capture.updateMeasurementsAt(validAt + delta);
        }
        require(pitch == 0, "uncertain signal retained pitch indefinitely");
        capture.processBytes(tone(18000), validAt + 325);
        capture.updateMeasurementsAt(validAt + 325);
        require(pitch > 0, "D3 failed to reacquire");
        // Gate opens at MIN, remains open 3 dB below it, and releases for silence.
        capture.processBytes(tone(18000, 146.8324, 0.007), validAt + 390);
        capture.updateMeasurementsAt(validAt + 390);
        require(pitch > 0 && capture.lastPitchMs_ == validAt + 390, "gate chattered near MIN");
        for (int delta : {455, 520, 585, 650}) {
            capture.processBytes(QByteArray(4800 * 2 * sizeof(float), '\0'), validAt + delta);
            capture.updateMeasurementsAt(validAt + delta);
        }
        require(pitch == 0 && !capture.gateOpen_, "real silence did not release pitch gate");
        capture.processBytes(tone(18000), validAt + 715);
        capture.updateMeasurementsAt(validAt + 715);
        require(pitch > 0, "pitch did not recover after silence");
        capture.processBytes(tone(4800, 146.8324, 1.2), validAt + 780);
        capture.updateMeasurementsAt(validAt + 780);
        require(pitch == 0, "clipped pitch was held as valid");
        capture.processBytes(tone(3120), validAt + 845);
        capture.updateMeasurementsAt(validAt + 845);
        require(pitch == 0, "short clean packet revived pitch from the clipped window");
        capture.processBytes(tone(14000), validAt + 910);
        capture.updateMeasurementsAt(validAt + 910);
        require(std::abs(pitch - 146.8324) < 0.5, "clean window did not recover after clipping");
        capture.resetAnalysis();
        require(level == -120 && capture.ringCount_ == 0 && capture.lastPitchMs_ == -1,
                "device/channel reset retained state");
        // A stall can end before the timer sees it; ingress must reset too.
        capture.processBytes(tone(18000), 5000);
        capture.updateMeasurementsAt(5000);
        require(pitch > 0, "setup tone failed");
        capture.processBytes(tone(4800, 329.6276), 5300);
        capture.updateMeasurementsAt(5300);
        require(pitch == 0 && capture.ringCount_ < capture.pitchRing_.size(), "hidden transport gap retained old window");
        // Exercise octave correction after the real 48 kHz PCM decoder,
        // channel selection, low-pass and 12 kHz decimation path.
        for (double hz : {30.867706, 82.406889, 329.627557}) {
            capture.resetAnalysis();
            QByteArray pcm(18000 * 2 * sizeof(float), '\0');
            for (int i = 0; i < 18000; ++i) {
                const double angle = 2.0 * 3.141592653589793 * hz * i / 48000.0;
                const float value = 0.05 * std::sin(angle) + 0.65 * std::sin(2 * angle)
                                  + 0.02 * std::sin(3 * angle);
                std::memcpy(pcm.data() + i * 2 * sizeof(float), &value, sizeof(value));
            }
            capture.processBytes(pcm, 6000);
            capture.updateMeasurementsAt(6000);
            require(pitch > 0 && std::abs(strings::centsBetween(pitch, hz)) < 5.0,
                    "capture path mistook dominant second harmonic for fundamental");
        }
        // Review regression through the actual capture path. This genuine
        // 648.5 Hz tone used to be incorrectly lowered to about 324.37 Hz.
        for (double phase : {0.0, 0.7}) {
            capture.resetAnalysis();
            QByteArray pcm(18000 * 2 * sizeof(float), '\0');
            for (int i = 0; i < 18000; ++i) {
                const double angle = 2.0 * 3.141592653589793 * 648.5 * i / 48000.0 + phase;
                double sample = 0.0;
                for (int harmonic = 1; harmonic <= 11; harmonic += 2)
                    sample += 0.2 * std::sin(harmonic * angle) / harmonic;
                const float value = static_cast<float>(sample);
                std::memcpy(pcm.data() + i * 2 * sizeof(float), &value, sizeof(value));
            }
            capture.processBytes(pcm, 6000);
            capture.updateMeasurementsAt(6000);
            require(pitch > 0 && std::abs(strings::centsBetween(pitch, 648.5)) < 5.0,
                    "capture path incorrectly lowered a rich single-period tone");
        }
        capture.resetAnalysis();
        capture.processBytes(tone(18000), 10000);
        capture.updateMeasurementsAt(10000);
        require(pitch > 0, "sparse-input setup failed");
        for (int tick = 1; tick <= 8; ++tick) {
            capture.processBytes(tone(16), 10000 + tick * 65);
            capture.updateMeasurementsAt(10000 + tick * 65);
        }
        require(pitch == 0, "sparse input held an expired pitch beyond 220 ms");

        // Verify anti-alias rejection and the passband through the PCM adapter.
        for (int rate : {12000, 22050, 44100, 48000, 96000}) {
            capture.format_.setSampleRate(rate);
            for (double hz : {30.867706, 82.406889, 329.627557,
                              11917.593111, 12082.406889, 23917.593111}) {
                if (hz >= rate / 2.0) continue;
                capture.resetAnalysis();
                const int frames = rate / 2;
                QByteArray pcm(frames * 2 * sizeof(float), '\0');
                for (int i = 0; i < frames; ++i) {
                    const float value = 0.9 * std::sin(2 * 3.141592653589793 * hz * i / rate);
                    std::memcpy(pcm.data() + i * 2 * sizeof(float), &value, sizeof(value));
                }
                capture.processBytes(pcm, 12000);
                capture.updateMeasurementsAt(12000);
                if (hz < 700)
                    require(pitch > 0 && std::abs(strings::centsBetween(pitch, hz)) < 2,
                            "prefilter changed a supported clean reference pitch");
                else
                    require(pitch == 0, "out-of-band tone aliased into the pitch readout");
            }
        }
        for (const QVariant& bad : {QVariant(), QVariant("garbage"), QVariant("nan"),
                                    QVariant(std::numeric_limits<double>::infinity())})
            require(numericSetting(bad, 440, 430, 450) == 440,
                    "invalid saved calibration did not restore the default");
        require(numericSetting(QVariant("442"), 440, 430, 450) == 442,
                "valid legacy calibration changed");
        require(numericSetting(QVariant(500), 440, 430, 450) == 450,
                "finite saved calibration did not respect its range");
        std::cout << "Capture packet jitter, stall, recovery, uncertainty, gate, silence, clipping and octaves passed\n";
    }
};

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    AudioCaptureTest::run();
}

