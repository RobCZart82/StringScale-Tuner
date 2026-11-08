#pragma once

#include <array>
#include <cmath>

// Eighth-order Butterworth low-pass before decimation to 12 kHz.
// The 1.5 kHz cutoff keeps the 25–700 Hz analysis band effectively flat and
// strongly suppresses frequencies that could fold into it during decimation.
class PitchPrefilter {
public:
    void reset() {
        for (auto& section : sections_) {
            section.z1 = 0;
            section.z2 = 0;
        }
    }

    double process(double sample, int sampleRate) {
        if (sampleRate != sampleRate_) configure(sampleRate);
        for (auto& section : sections_) {
            const double output = section.b0 * sample + section.z1;
            section.z1 = section.b1 * sample - section.a1 * output + section.z2;
            section.z2 = section.b0 * sample - section.a2 * output;
            sample = output;
        }
        return sample;
    }

private:
    struct Section {
        double b0 = 0, b1 = 0, a1 = 0, a2 = 0;
        double z1 = 0, z2 = 0;
    };

    void configure(int sampleRate) {
        constexpr double pi = 3.14159265358979323846;
        const double omega = 2 * pi * 1500.0 / sampleRate;
        const double cosine = std::cos(omega);
        const double sine = std::sin(omega);
        for (std::size_t i = 0; i < sections_.size(); ++i) {
            const double q = 1.0 / (2 * std::cos((2 * i + 1) * pi / 16));
            const double alpha = sine / (2 * q);
            auto& section = sections_[i];
            section.b0 = (1 - cosine) / (2 * (1 + alpha));
            section.b1 = 2 * section.b0;
            section.a1 = -2 * cosine / (1 + alpha);
            section.a2 = (1 - alpha) / (1 + alpha);
        }
        sampleRate_ = sampleRate;
        reset();
    }

    std::array<Section, 4> sections_{};
    int sampleRate_ = 0;
};
