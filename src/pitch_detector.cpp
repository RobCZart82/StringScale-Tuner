#include "pitch_detector.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace strings {

PitchEstimate PitchDetector::estimate(const std::vector<float>& data,
                                       int sampleRate,
                                       double minHz,
                                       double maxHz) const {
    PitchEstimate result;
    if (sampleRate <= 0 || minHz <= 0 || minHz >= maxHz || data.size() < 512)
        return result;

    const int maxLag = std::min(static_cast<int>(std::floor(sampleRate / minHz)),
                                static_cast<int>(data.size() / 2));
    const int minLag = std::max(2, static_cast<int>(std::floor(sampleRate / maxHz)));
    if (maxLag <= minLag + 2) return result;

    // Remove DC and reject digital silence. A clean signal gate is also applied
    // by AudioCapture, so this is a second defensive check for direct callers.
    double mean = 0.0;
    for (float s : data) mean += s;
    mean /= static_cast<double>(data.size());

    double energy = 0.0;
    std::vector<double> x(data.size());
    for (std::size_t i = 0; i < data.size(); ++i) {
        x[i] = static_cast<double>(data[i]) - mean;
        energy += x[i] * x[i];
    }
    if (energy / data.size() < 1e-9) return result;

    // Identical comparison length at every lag avoids end-of-buffer bias.
    const int count = static_cast<int>(data.size()) - maxLag;
    std::vector<double> difference(maxLag + 1, 0.0);
    for (int lag = 1; lag <= maxLag; ++lag) {
        double sum = 0.0;
        for (int i = 0; i < count; ++i) {
            const double d = x[i] - x[i + lag];
            sum += d * d;
        }
        difference[lag] = sum;
    }

    std::vector<double> cmnd(maxLag + 1, 1.0);
    double cumulative = 0.0;
    for (int lag = 1; lag <= maxLag; ++lag) {
        cumulative += difference[lag];
        cmnd[lag] = cumulative > 1e-15
            ? difference[lag] * lag / cumulative : 1.0;
    }

    // Choose the first good local minimum rather than the globally lowest
    // harmonic/subharmonic valley, which reduces octave errors.
    constexpr double threshold = 0.16;
    int bestLag = -1;
    for (int lag = minLag; lag < maxLag; ++lag) {
        if (cmnd[lag] >= threshold) continue;
        while (lag + 1 <= maxLag && cmnd[lag + 1] < cmnd[lag]) ++lag;
        bestLag = lag;
        break;
    }
    // Relaxed fallback for mildly distorted/plucked sounds, but reject noise.
    if (bestLag < 0) {
        const auto begin = cmnd.begin() + minLag;
        const auto end = cmnd.begin() + maxLag + 1;
        const auto it = std::min_element(begin, end);
        if (it == end || *it > 0.27) return result;
        bestLag = static_cast<int>(std::distance(cmnd.begin(), it));
    }

    double lag = static_cast<double>(bestLag);
    if (bestLag > 1 && bestLag < maxLag) {
        const double a = cmnd[bestLag - 1];
        const double b = cmnd[bestLag];
        const double c = cmnd[bestLag + 1];
        const double denominator = a - 2.0 * b + c;
        if (std::abs(denominator) > 1e-12)
            lag += std::clamp(0.5 * (a - c) / denominator, -1.0, 1.0);
    }

    const double hz = static_cast<double>(sampleRate) / lag;
    if (!std::isfinite(hz) || hz < minHz || hz > maxHz) return result;
    result.hz = hz;
    result.confidence = std::clamp(1.0 - cmnd[bestLag], 0.0, 1.0);
    result.valid = result.confidence >= 0.73;
    return result;
}

} // namespace strings
