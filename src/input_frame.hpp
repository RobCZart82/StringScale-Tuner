#pragma once
#include <algorithm>
#include <cmath>
#include <span>

namespace strings {
struct InputFrame {
    float sample = 0;
    double peak = 0;
    bool clipped = false;
    bool valid = false;
};
// Channel numbers here are zero-based. Only the chosen channel affects tuning
// and the input meter; unrelated channels must not dilute or cancel the signal.
inline InputFrame selectInputFrame(std::span<const float> channels, int channel) {
    if (channel < 0 || static_cast<std::size_t>(channel) >= channels.size()) return {};
    const double value = channels[channel];
    if (!std::isfinite(value)) return {0, 0, true, false};
    const double peak = std::abs(value);
    return {static_cast<float>(std::clamp(value, -1.0, 1.0)), peak, peak >= 0.999, true};
}
}
