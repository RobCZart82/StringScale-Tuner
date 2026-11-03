#include "tuning.hpp"

#include <cmath>
#include <limits>

namespace strings {

const std::vector<TuningPreset>& presets() {
    static const std::vector<TuningPreset> list = {
        {"guitar-e", "Gitár · Normál (6)", {
            {6, "E2", 82.406889}, {5, "A2", 110.0}, {4, "D3", 146.832384},
            {3, "G3", 195.997718}, {2, "B3", 246.941651}, {1, "E4", 329.627557}}},
        {"guitar-drop-d", "Gitár · Drop D (6)", {
            {6, "D2", 73.416192}, {5, "A2", 110.0}, {4, "D3", 146.832384},
            {3, "G3", 195.997718}, {2, "B3", 246.941651}, {1, "E4", 329.627557}}},
        {"guitar-eb", "Gitár · E-flat Standard (6)", {
            {6, "Eb2", 77.781746}, {5, "Ab2", 103.826174}, {4, "Db3", 138.591315},
            {3, "Gb3", 184.997211}, {2, "Bb3", 233.081881}, {1, "Eb4", 311.126984}}},
        {"guitar-open-g", "Gitár · Open G (6)", {
            {6, "D2", 73.416192}, {5, "G2", 97.998859}, {4, "D3", 146.832384},
            {3, "G3", 195.997718}, {2, "B3", 246.941651}, {1, "D4", 293.664768}}},
        {"guitar-dadgad", "Gitár · DADGAD (6)", {
            {6, "D2", 73.416192}, {5, "A2", 110.0}, {4, "D3", 146.832384},
            {3, "G3", 195.997718}, {2, "A3", 220.0}, {1, "D4", 293.664768}}},
        {"guitar-7", "Gitár · Standard (7)", {
            {7, "B1", 61.735413}, {6, "E2", 82.406889}, {5, "A2", 110.0},
            {4, "D3", 146.832384}, {3, "G3", 195.997718},
            {2, "B3", 246.941651}, {1, "E4", 329.627557}}},
        {"bass-4", "Basszus · Normál (4)", {
            {4, "E1", 41.203445}, {3, "A1", 55.0},
            {2, "D2", 73.416192}, {1, "G2", 97.998859}}},
        {"bass-5", "Basszus · Normál (5)", {
            {5, "B0", 30.867706}, {4, "E1", 41.203445}, {3, "A1", 55.0},
            {2, "D2", 73.416192}, {1, "G2", 97.998859}}}
    };
    return list;
}

std::string measuredNote(double hz, double a4Calibration) {
    if (!std::isfinite(hz) || !std::isfinite(a4Calibration) || hz <= 0 || a4Calibration <= 0)
        return "—";
    const double semitone = 69.0 + 12.0 * std::log2(hz / a4Calibration);
    if (semitone < 0 || semitone > 127) return "—";
    const int midi = static_cast<int>(std::lround(semitone));
    static const char* names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return std::string(names[midi % 12]) + std::to_string(midi / 12 - 1);
}

double centsBetween(double hz, double referenceHz) {
    if (hz <= 0 || referenceHz <= 0) return 0;
    return 1200.0 * std::log2(hz / referenceHz);
}

const StringTarget* closestString(const TuningPreset& preset,
                                  double hz, double a4Calibration) {
    if (hz <= 0 || a4Calibration <= 0) return nullptr;
    const StringTarget* best = nullptr;
    double distance = std::numeric_limits<double>::max();
    for (const auto& target : preset.targets) {
        const double cents = std::abs(centsBetween(hz, target.hz * a4Calibration / 440.0));
        if (cents < distance) {
            best = &target;
            distance = cents;
        }
    }
    return best;
}

} // namespace strings
