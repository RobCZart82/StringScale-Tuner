#pragma once

#include <vector>

namespace strings {

struct PitchEstimate {
    double hz = 0.0;
    double confidence = 0.0; // 0..1; 1 is the strongest periodicity
    bool valid = false;
};

// Original YIN-family difference / cumulative-mean-normalized-difference
// implementation. No third-party pitch detector code is incorporated.
class PitchDetector {
public:
    PitchEstimate estimate(const std::vector<float>& mono,
                           int sampleRate,
                           double minHz = 25.0,
                           double maxHz = 700.0) const;
};

} // namespace strings
