#include "pitch_detector.hpp"
#include "tuning.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

namespace {
constexpr double pi = 3.14159265358979323846;

std::vector<float> signal(double hz, int sampleRate, bool harmonic = false) {
    const std::size_t n = 4096;
    std::vector<float> samples(n);
    for (std::size_t i = 0; i < n; ++i) {
        const double t = static_cast<double>(i) / sampleRate;
        double s = 0.5 * std::sin(2 * pi * hz * t);
        if (harmonic)
            s += 0.65 * std::sin(2 * pi * hz * 2 * t)
               + 0.24 * std::sin(2 * pi * hz * 3 * t);
        samples[i] = static_cast<float>(s);
    }
    return samples;
}

void expect(bool ok, const char* msg) {
    if (!ok) {
        std::cerr << "FAIL: " << msg << '\n';
        std::exit(1);
    }
}

std::vector<float> weakFundamental(double hz, double fundamental, double phase,
                                   bool decaying) {
    std::vector<float> samples(4096);
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> noise(-0.001, 0.001);
    for (std::size_t i = 0; i < samples.size(); ++i) {
        const double angle = 2 * pi * hz * i / 12000.0 + phase;
        const double envelope = decaying ? std::exp(-static_cast<double>(i) / samples.size()) : 1.0;
        samples[i] = static_cast<float>(envelope * (fundamental * std::sin(angle)
            + 0.65 * std::sin(2 * angle) + 0.02 * std::sin(3 * angle)) + noise(rng));
    }
    return samples;
}

std::vector<float> filteredOddHarmonics(double hz, double phase) {
    std::vector<float> samples;
    double lowpass = 0.0;
    constexpr double alpha = 2 * pi * 1200 / 48000;
    for (int i = 0; i < 18000; ++i) {
        const double angle = 2 * pi * hz * i / 48000 + phase;
        double sample = 0.0;
        for (int harmonic = 1; harmonic <= 11; harmonic += 2)
            sample += 0.2 * std::sin(harmonic * angle) / harmonic;
        lowpass += alpha * (static_cast<float>(sample) - lowpass);
        if (i % 4 == 3) samples.push_back(static_cast<float>(lowpass));
    }
    return {samples.end() - 4096, samples.end()};
}
}

int main() {
    strings::PitchDetector detector;
    for (const double hz : {30.867706, 41.203445, 73.416192, 82.406889,
                            110.0, 146.832384, 164.813778, 196.0, 246.941651, 329.627557}) {
        for (const bool harmonics : {false, true}) {
            const auto result = detector.estimate(signal(hz, 12000, harmonics),
                                                  12000, 25.0, 700.0);
            const double cents = strings::centsBetween(result.hz, hz);
            std::cout << hz << " Hz; harmonics=" << harmonics
                      << "; estimate=" << result.hz
                      << "; confidence=" << result.confidence
                      << "; error(cents)=" << cents << '\n';
            expect(result.valid && std::abs(cents) < 5.0,
                   "Known guitar/bass test tone pitch error >= 5 cents");
        }
    }

    // Review regression: sample-grid error in a rich but genuine single-period
    // tone must not trigger octave lowering after the capture prefilter.
    for (double hz : {82.406889, 329.627557, 600.0, 645.0, 648.0, 648.5, 649.0, 652.0, 680.0}) {
        for (double phase : {0.0, 0.7}) {
            const auto result = detector.estimate(filteredOddHarmonics(hz, phase),
                                                   12000, 25.0, 700.0);
            if (!result.valid || std::abs(strings::centsBetween(result.hz, hz)) >= 5.0) {
                std::cerr << "Odd harmonics: " << hz << " Hz, detected=" << result.hz << '\n';
                expect(false, "Harmonic-rich single-period tone was incorrectly lowered");
            }
        }
    }

    // Include the audit's E2->E3 regression and every preset reference, with
    // detuning, independent phases, noise and a plucked-style decay. A real
    // upper-octave tone must stay in that octave when its fundamental is absent.
    int octaveCases = 0;
    for (const auto& preset : strings::presets()) {
        for (const auto& target : preset.targets) {
            for (double detuning : {-40.0, 0.0, 40.0}) {
                const double hz = target.hz * std::pow(2.0, detuning / 1200.0);
                for (double fundamental : {0.05, 0.1}) {
                    for (bool decaying : {false, true}) {
                        const auto result = detector.estimate(
                            weakFundamental(hz, fundamental, 0.7, decaying), 12000, 25.0, 700.0);
                        if (!result.valid || std::abs(strings::centsBetween(result.hz, hz)) >= 5.0) {
                            std::cerr << "Weak fundamental: " << hz << " Hz, amplitude=" << fundamental
                                      << ", decay=" << decaying << ", detected=" << result.hz << '\n';
                            expect(false, "Dominant second harmonic caused an octave/pitch error");
                        }
                        ++octaveCases;
                    }
                }
                for (bool harmonics : {false, true}) {
                    const auto upper = detector.estimate(signal(2 * hz, 12000, harmonics),
                                                         12000, 25.0, 700.0);
                    expect(upper.valid && std::abs(strings::centsBetween(upper.hz, 2 * hz)) < 5.0,
                           "A real upper-octave tone was incorrectly lowered");
                    ++octaveCases;
                }
            }
        }
    }
    std::cout << octaveCases << " octave regression cases passed.\n";

    expect(!detector.estimate(std::vector<float>(4096, 0.0f), 12000).valid,
           "Silence must not generate a pitch");
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> noise(-0.5f, 0.5f);
    std::vector<float> randomNoise(4096);
    for (float& s : randomNoise) s = noise(rng);
    expect(!detector.estimate(randomNoise, 12000).valid,
           "Broadband noise must not generate a confident pitch");

    expect(strings::measuredNote(82.406889) == "E2", "Measured low E must include octave 2");
    expect(strings::measuredNote(164.813778) == "E3", "Measured E3 must not be relabeled as target D3");
    expect(strings::measuredNote(329.627557) == "E4", "Measured high E must include octave 4");
    expect(strings::measuredNote(0) == "—", "Silence has no note name");
    expect(strings::measuredNote(110.0 * 442.0 / 440.0, 442.0) == "A2", "Note naming must respect calibration");
    const auto& guitar = strings::presets().front();
    expect(strings::closestString(guitar, 110.0)->number == 5,
           "110 Hz must match 5th guitar string");
    expect(strings::closestString(guitar, 82.41)->number == 6,
           "82.4 Hz must match 6th guitar string");
    const auto& bass = strings::presets().back();
    expect(strings::closestString(bass, 30.867706)->number == 5,
           "B0 must match 5th bass string");
    std::cout << "All tests passed.\n";
    return 0;
}

