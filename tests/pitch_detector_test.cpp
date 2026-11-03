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
}

int main() {
    strings::PitchDetector detector;
    for (const double hz : {30.867706, 41.203445, 73.416192, 82.406889,
                            110.0, 146.832384, 196.0, 329.627557}) {
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

    expect(!detector.estimate(std::vector<float>(4096, 0.0f), 12000).valid,
           "Silence must not generate a pitch");
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> noise(-0.5f, 0.5f);
    std::vector<float> randomNoise(4096);
    for (float& s : randomNoise) s = noise(rng);
    expect(!detector.estimate(randomNoise, 12000).valid,
           "Broadband noise must not generate a confident pitch");

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
