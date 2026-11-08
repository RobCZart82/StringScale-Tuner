#pragma once

#include <QVariant>
#include <algorithm>
#include <cmath>

inline double numericSetting(const QVariant& stored, double fallback,
                             double minimum, double maximum) {
    bool converted = false;
    const double value = stored.toDouble(&converted);
    return converted && std::isfinite(value)
        ? std::clamp(value, minimum, maximum) : fallback;
}
