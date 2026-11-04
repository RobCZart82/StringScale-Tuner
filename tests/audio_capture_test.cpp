#include "audio_capture.hpp"
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
        AudioCapture capture;
        capture.measureTimer_.stop();
        capture.format_.setSampleRate(48000);
        capture.format_.setChannelCount(2);
        capture.format_.setSampleFormat(QAudioFormat::Float);
        capture.channelSamples_.resize(2);
        double pitch = 0, level = -120;
        int pitchClears = 0, levelClears = 0;
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
        std::cout << "Capture packet jitter, stall, recovery, uncertainty, gate, silence and clipping passed\n";
    }
};

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    AudioCaptureTest::run();
}
