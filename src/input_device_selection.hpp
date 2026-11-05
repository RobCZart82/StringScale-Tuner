#pragma once

#include <QByteArray>
#include <QComboBox>
#include <initializer_list>

// An automatic fallback must not take precedence over the saved interface
// when that interface becomes available again.
inline int restoredInputDeviceIndex(const QComboBox& choices,
                                    const QByteArray& preferredId,
                                    const QByteArray& previousId,
                                    const QByteArray& defaultId) {
    for (const auto& id : {preferredId, previousId, defaultId}) {
        if (id.isEmpty()) continue;
        const int index = choices.findData(id);
        if (index >= 0) return index;
    }
    return choices.count() > 0 ? 0 : -1;
}
