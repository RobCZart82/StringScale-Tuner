#include <utility>
#include "main_window.hpp"
#include "ui_language.hpp"
#include "build_info.hpp"
#include <QTemporaryDir>
#include <QSettings>
#include <QProcess>
#include <QComboBox>
#include <QAbstractItemView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
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
    QCoreApplication::setApplicationVersion(strings::build::version);
    app.setStyle("Fusion");
    QIcon appIcon;
    for(int size : {16,32,48,64,128,256,512,1024})
        appIcon.addFile(QString(":/icons/app-icon-%1.png").arg(size),QSize(size,size));
    app.setWindowIcon(appIcon);
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
        auto* channel=audio->findChild<QComboBox*>("inputChannelChoice");
        if(!require(channel && channel->count()==2 && channel->isEnabled(),"Missing multichannel selector"))return 16;
        channel->setCurrentIndex(1);
        const auto profileId=profile->currentData();const int inputIndex=input->currentIndex();
        for(const auto& code:{QString("hu"),QString("en"),QString("hu")}) {
            language->setCurrentIndex(code=="hu"?0:1);app.processEvents();
            const bool hu=code=="hu";
            if(!require(UiLanguage::instance().code()==code && settings->windowTitle()==(hu?"Beállítások":"Settings")
                && audio->windowTitle()==(hu?"Hang":"Audio") && about->windowTitle()==(hu?"Névjegy":"About"),"Open dialogs did not switch language"))return 11;
            if(!require(profile->currentData()==profileId && input->currentIndex()==inputIndex && channel->currentData().toInt()==1,"Language changed tuning or input selection"))return 12;
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
                // Exercise hover without committing a different tuning profile.
                const auto beforeHover = profile->currentData();
                profile->showPopup();app.processEvents();
                auto* list = profile->view();
                const auto hoverIndex = profile->model()->index(1,profile->modelColumn());
                const auto local = list->visualRect(hoverIndex).center();
                QMouseEvent move(QEvent::MouseMove,QPointF(local),QPointF(list->viewport()->mapToGlobal(local)),
                                 Qt::NoButton,Qt::NoButton,Qt::NoModifier);
                QApplication::sendEvent(list->viewport(),&move);app.processEvents();
                if(!require(list->currentIndex()==hoverIndex && profile->currentData()==beforeHover,
                            "Popup hover changed committed tuning or did not highlight row"))return 17;
                if(!list->window()->grab().save(folder+"/"+code+(dark?"-dark-":"-light-")+"combo-hover.png"))return 4;
                QKeyEvent escape(QEvent::KeyPress,Qt::Key_Escape,Qt::NoModifier);
                QApplication::sendEvent(profile,&escape);profile->hidePopup();app.processEvents();
                if(!require(profile->currentData()==beforeHover,"Closing popup committed hovered tuning"))return 18;
                if(!require(!app.windowIcon().pixmap(16,16).isNull(),"Missing application icon"))return 19;
                for(auto* dialog:{settings,audio,about})
                    if(!dialog->grab().save(folder+"/"+code+(dark?"-dark-":"-light-")+dialog->objectName()+".png"))return 4;
            }
        }
        settings->close();audio->close();about->close();
        return 0;
    }
    return app.exec();
}
