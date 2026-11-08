#include "audio_capture.hpp"
#include "tuning.hpp"

#include <QCoreApplication>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>

namespace {
constexpr double pi = 3.14159265358979323846;

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void appendSample(QByteArray& pcm, double value, QAudioFormat::SampleFormat format) {
    switch (format) {
    case QAudioFormat::UInt8: {
        const auto sample = static_cast<quint8>(std::lround((value + 1) * 127.5));
        pcm.append(reinterpret_cast<const char*>(&sample), sizeof(sample));
        break;
    }
    case QAudioFormat::Int16: {
        const auto sample = static_cast<qint16>(std::lround(value * 32767));
        pcm.append(reinterpret_cast<const char*>(&sample), sizeof(sample));
        break;
    }
    case QAudioFormat::Int32: {
        const auto sample = static_cast<qint32>(std::llround(value * 2147483647));
        pcm.append(reinterpret_cast<const char*>(&sample), sizeof(sample));
        break;
    }
    case QAudioFormat::Float: {
        const float sample = static_cast<float>(value);
        pcm.append(reinterpret_cast<const char*>(&sample), sizeof(sample));
        break;
    }
    default:
        require(false, "unsupported fixture format");
    }
}

QByteArray tone(const QAudioFormat& format, int selectedChannel, double hz) {
    QByteArray pcm;
    const int frames = format.sampleRate() / 2;
    pcm.reserve(frames * format.bytesPerFrame());
    for (int i = 0; i < frames; ++i) {
        for (int channel = 0; channel < format.channelCount(); ++channel) {
            // Other channels are clipped. They must not suppress the selected input.
            const double value = channel == selectedChannel
                ? 0.4 * std::sin(2 * pi * hz * i / format.sampleRate()) : 1.0;
            appendSample(pcm, value, format.sampleFormat());
        }
    }
    return pcm;
}

struct Measurement {
    double pitch = 0;
    double rms = -120;
    double peak = -120;
    bool clipped = false;
};
}

// Separate executable uses the same production adapter test seam as the
// existing capture regression. No QAudioSource or hardware is opened.
class AudioCaptureTest {
public:
    static Measurement measure(const QAudioFormat& format, int selectedChannel,
                               const QByteArray& pcm, bool fragmented) {
        Measurement result;
        AudioCapture capture;
        capture.measureTimer_.stop();
        capture.format_ = format;
        capture.inputChannel_ = selectedChannel;
        capture.channelSamples_.resize(format.channelCount());
        QObject::connect(&capture, &AudioCapture::pitchReady,
                         [&](double hz, double) { result.pitch = hz; });
        QObject::connect(&capture, &AudioCapture::levelReady,
                         [&](double rms, double peak, bool clipped) {
            result.rms = rms;
            result.peak = peak;
            result.clipped = clipped;
        });
        if (fragmented) {
            constexpr std::array<int, 6> chunks{1, 7, 31, 4093, 2, 17};
            qsizetype offset = 0;
            std::size_t chunk = 0;
            while (offset < pcm.size()) {
                const auto count = std::min<qsizetype>(chunks[chunk++ % chunks.size()],
                                                      pcm.size() - offset);
                capture.processBytes(pcm.mid(offset, count), 1000);
                offset += count;
            }
        } else {
            capture.processBytes(pcm, 1000);
        }
        require(capture.partialFrame_.isEmpty(), "complete PCM left partial bytes");
        require(capture.blockSamples_ == pcm.size() / format.bytesPerFrame(),
                "PCM frames were dropped or counted twice");
        capture.updateMeasurementsAt(1000);
        // Copy before capture's destructor emits its final reset signals.
        return Measurement{result.pitch, result.rms, result.peak, result.clipped};
    }

    static void run() {
        int cases = 0;
        for (auto sampleFormat : {QAudioFormat::UInt8, QAudioFormat::Int16,
                                  QAudioFormat::Int32, QAudioFormat::Float}) {
            for (int rate : {44100, 48000, 96000}) {
                for (int channels : {1, 2, 6}) {
                    QAudioFormat format;
                    format.setSampleRate(rate);
                    format.setChannelCount(channels);
                    format.setSampleFormat(sampleFormat);
                    const int selected = channels - 1;
                    for (double hz : {30.867706, 82.406889, 329.627557}) {
                        std::cout << "PCM " << sampleFormat << " rate " << rate
                                  << " channels " << channels << " hz " << hz << '\n';
                        const auto pcm = tone(format, selected, hz);
                        const auto bulk = measure(format, selected, pcm, false);
                        const auto split = measure(format, selected, pcm, true);
                        require(!bulk.clipped && !split.clipped,
                                "clipped unrelated channel poisoned the selected input");
                        require(bulk.pitch > 0 &&
                                std::abs(strings::centsBetween(bulk.pitch, hz)) < 2,
                                "PCM tone was not detected within two cents");
                        require(std::abs(bulk.rms - 20 * std::log10(0.4 / std::sqrt(2.0))) < 0.25,
                                "selected input RMS was diluted or decoded incorrectly");
                        require(std::abs(bulk.peak - 20 * std::log10(0.4)) < 0.25,
                                "selected peak was decoded incorrectly");
                        require(bulk.pitch == split.pitch && bulk.rms == split.rms &&
                                bulk.peak == split.peak,
                                "fragmentation changed the decoded signal or measurement");
                        ++cases;
                    }

                    // A complete selected-channel saturated block must clear pitch
                    // and report clipping in every supported format.
                    QByteArray clipped;
                    for (int i = 0; i < 128; ++i)
                        for (int channel = 0; channel < channels; ++channel)
                            appendSample(clipped, channel == selected ? 1.0 : 0.0, sampleFormat);
                    const auto clip = measure(format, selected, clipped, true);
                    require(clip.clipped && clip.pitch == 0,
                            "selected-channel saturation was not reported");

                    // Check every possible split inside one interleaved frame.
                    const auto frame = clipped.first(format.bytesPerFrame());
                    for (qsizetype split = 1; split < frame.size(); ++split) {
                        AudioCapture capture;
                        capture.measureTimer_.stop();
                        capture.format_ = format;
                        capture.channelSamples_.resize(channels);
                        capture.processBytes(frame.first(split), 2000);
                        require(capture.blockSamples_ == 0 &&
                                capture.partialFrame_.size() == split,
                                "an incomplete PCM frame was consumed");
                        capture.processBytes(frame.sliced(split), 2000);
                        require(capture.blockSamples_ == 1 && capture.partialFrame_.isEmpty(),
                                "a split PCM frame was not reconstructed exactly once");
                    }
                }
            }
        }

        QAudioFormat floating;
        floating.setSampleRate(48000);
        floating.setChannelCount(2);
        floating.setSampleFormat(QAudioFormat::Float);
        auto pcm = tone(floating, 1, 82.406889);
        const float nan = std::numeric_limits<float>::quiet_NaN();
        std::memcpy(pcm.data() + sizeof(float), &nan, sizeof(nan));
        const auto invalid = measure(floating, 1, pcm, true);
        require(invalid.clipped && invalid.pitch == 0,
                "nonfinite selected Float PCM was not rejected");
        std::cout << cases << " format/rate/layout/pitch cases passed, bulk and fragmented\n";
    }
};

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    AudioCaptureTest::run();
}
