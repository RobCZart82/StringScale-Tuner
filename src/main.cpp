#include <utility>
#include "main_window.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QFontDatabase>
#include <QDir>
#include <QPixmap>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("StringScale Tuner");
    QCoreApplication::setOrganizationName("RobCZart");
    QCoreApplication::setApplicationVersion("0.1.0");
    app.setStyle("Fusion");
    const int previewArg = app.arguments().indexOf("--render-preview");
    const bool preview = previewArg >= 0;
    QFont font = app.font();
    if (preview) {
#ifdef Q_OS_WIN
        const QString fontPath = qEnvironmentVariable("WINDIR") + "/Fonts/segoeui.ttf";
#elif defined(Q_OS_MACOS)
        const QString fontPath = "/System/Library/Fonts/Supplemental/Arial.ttf";
#else
        const QString fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif
        const int id = QFontDatabase::addApplicationFont(fontPath);
        if (id < 0) return 5;
        font.setFamily(QFontDatabase::applicationFontFamilies(id).first());
    }
    font.setPointSize(13);
    app.setFont(font);
    MainWindow window(nullptr, preview);
    window.show();
    if (preview) {
        if (previewArg + 1 >= app.arguments().size()) return 2;
        const QString folder = app.arguments().at(previewArg + 1);
        if (!QDir().mkpath(folder)) return 3;
        for (bool dark : {false, true}) {
            for (const auto& example : {std::pair<const char*, double>{"in-tune",110.0},
                                       {"octave",164.813778}, {"silence",0.0}}) {
                window.renderPreview(dark, example.second);
                app.processEvents();
                if (!window.grab().save(folder + "/" + (dark ? "dark-" : "light-") + example.first + ".png")) return 4;
            }
        }
        return 0;
    }
    return app.exec();
}
