#pragma once

#include <string>
#include <vector>

namespace strings {

struct StringTarget {
    int number;        // Standard manufacturer numbering: 1 = highest string.
    std::string note;
    double hz;         // 12-tone equal temperament, A4 = 440 Hz.
};

struct TuningPreset {
    std::string id;
    std::string displayName;
    std::vector<StringTarget> targets; // ascending frequency: low -> high
};

const std::vector<TuningPreset>& presets();
std::string measuredNote(double hz, double a4Calibration = 440.0);
double centsBetween(double hz, double referenceHz);
const StringTarget* closestString(const TuningPreset& preset,
                                  double hz, double a4Calibration = 440.0);

} // namespace strings
