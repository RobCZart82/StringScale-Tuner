#include <utility>
#include "main_window.hpp"
#include "ui_language.hpp"
#include <QTemporaryDir>
#include <QSettings>
#include <QProcess>
#include <QComboBox>
#include <QAction>
#include <QDialog>
#include <QFile>
#include <QSvgRenderer>
#include <QDebug>

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
    QTemporaryDir testSettings;
    const int settingsArg = app.arguments().indexOf("--test-settings");
    if(preview || settingsArg>=0) {
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,
            settingsArg>=0 ? app.arguments().value(settingsArg+1) : testSettings.path());
        QCoreApplication::setOrganizationName("RobCZartTests");
    }
    const int checkArg=app.arguments().indexOf("--check-language");
    if(checkArg>=0)return UiLanguage::savedCode()==app.arguments().value(checkArg+1)?0:10;
    if(preview)UiLanguage::instance().setCode("hu",false);
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
        const auto require=[](bool ok,const char* message){if(!ok)qCritical()<<message;return ok;};
        auto* settings=window.openSettings();auto* audio=window.openSettings(true);auto* about=window.openAbout();
        auto* language=settings->findChild<QComboBox*>("languageChoice");
        auto* theme=settings->findChild<QComboBox*>("themeChoice");
        auto* profile=window.findChild<QComboBox*>("profileChoice");
        auto* input=window.findChild<QComboBox*>("inputChoice");
        const auto profileId=profile->currentData();const int inputIndex=input->currentIndex();
        for(const auto& code:{QString("hu"),QString("en"),QString("hu")}) {
            language->setCurrentIndex(code=="hu"?0:1);app.processEvents();
            const bool hu=code=="hu";
            if(!require(UiLanguage::instance().code()==code && settings->windowTitle()==(hu?"Beállítások":"Settings")
                && audio->windowTitle()==(hu?"Hang":"Audio") && about->windowTitle()==(hu?"Névjegy":"About"),"Open dialogs did not switch language"))return 11;
            if(!require(profile->currentData()==profileId && input->currentIndex()==inputIndex,"Language changed tuning or input selection"))return 12;
            if(!require(language->itemText(0)=="Magyar" && language->itemText(1)=="English","Language choices must keep native names"))return 13;
        }
        // A separate process reads the isolated saved profile, exercising restart persistence.
        UiLanguage::instance().setCode("en");QSettings().sync();
        QProcess child;child.start(app.applicationFilePath(),{"--test-settings",testSettings.path(),"--check-language","en"});
        if(!child.waitForFinished(10000)||child.exitCode()!=0||child.exitStatus()!=QProcess::NormalExit)return 14;
        for(const auto& path:{":/branding/GYR-black.svg",":/branding/GYR-light.svg",":/branding/Alairas-Vektor.svg",":/branding/Alairas-Vektor-light.svg"}) {
            QSvgRenderer svg{QString(path)};if(!require(svg.isValid()&&!svg.viewBoxF().isEmpty(),"Missing/invalid embedded SVG"))return 15;
        }
        for(const auto& code:{QString("hu"),QString("en")}) {
            language->setCurrentIndex(code=="hu"?0:1);
            for (bool dark : {false, true}) {
                theme->setCurrentIndex(dark?1:0);
                for (const auto& example : {std::pair<const char*, double>{"in-tune",110.0},
                                           {"octave",164.813778}, {"silence",0.0}}) {
                    window.renderPreview(dark, example.second);app.processEvents();
                    if (!window.grab().save(folder + "/" + code + (dark ? "-dark-" : "-light-") + example.first + ".png")) return 4;
                }
                app.processEvents();
                for(auto* dialog:{settings,audio,about})
                    if(!dialog->grab().save(folder+"/"+code+(dark?"-dark-":"-light-")+dialog->objectName()+".png"))return 4;
            }
        }
        settings->close();audio->close();about->close();
        return 0;
    }
    return app.exec();
}
