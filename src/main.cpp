#include <utility>
#include "main_window.hpp"
#include "ui_language.hpp"
#include "ui_scale.hpp"
#include "input_device_selection.hpp"
#include "build_info.hpp"
#include <QTemporaryDir>
#include <QSettings>
#include <QProcess>
#include <QComboBox>
#include <QAbstractItemView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QStyleOptionComboBox>
#include <QAction>
#include <QScrollArea>
#include <QScrollBar>
#include <QDialog>
#include <QDialogButtonBox>
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
    const int scaleCheckArg=app.arguments().indexOf("--check-scale");
    if(scaleCheckArg>=0)return UiScale::savedPercent()==app.arguments().value(scaleCheckArg+1).toInt()?0:20;
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
        // Reconnection policy without opening a microphone or needing hardware.
        QComboBox devices;
        const QByteArray preferred("ssl2"), fallback("built-in"), replacement("ucg102");
        devices.addItem("Built-in",fallback);
        if(!require(restoredInputDeviceIndex(devices,preferred,preferred,fallback)==0,
                    "Disconnected preferred input has no fallback"))return 27;
        devices.addItem("SSL 2+",preferred);
        if(!require(restoredInputDeviceIndex(devices,preferred,fallback,fallback)==1,
                    "Reconnected preferred input was not restored"))return 27;
        devices.addItem("UCG-102",replacement);
        if(!require(restoredInputDeviceIndex(devices,replacement,preferred,fallback)==2,
                    "Explicit new device preference was ignored"))return 27;
        devices.clear();
        if(!require(restoredInputDeviceIndex(devices,preferred,fallback,fallback)==-1,
                    "Empty device list selected an input"))return 27;
        auto* settings=window.openSettings();auto* audio=window.openSettings(true);auto* about=window.openAbout();
        auto* language=settings->findChild<QComboBox*>("languageChoice");
        auto* theme=settings->findChild<QComboBox*>("themeChoice");
        auto* profile=window.findChild<QComboBox*>("profileChoice");
        auto* input=window.findChild<QComboBox*>("inputChoice");
        // Programmatic fallback must preserve the old preference, but explicit
        // activation of the same selected item makes it the new preference.
        QSettings().setValue("input/deviceId",preferred);
        const int previewInputIndex=input->currentIndex();
        input->addItem("Built-in",fallback);
        const int fallbackIndex=input->count()-1;
        input->setCurrentIndex(fallbackIndex);
        if(!require(QSettings().value("input/deviceId").toByteArray()==preferred,
                    "Automatic fallback overwrote the input preference"))return 29;
        QMetaObject::invokeMethod(input,"activated",Qt::DirectConnection,Q_ARG(int,fallbackIndex));
        if(!require(QSettings().value("input/deviceId").toByteArray()==fallback,
                    "Reselecting the active fallback did not save the new preference"))return 29;
        input->setCurrentIndex(previewInputIndex);input->removeItem(fallbackIndex);
        QSettings().remove("input/deviceId");
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
        auto* sizeChoice=settings->findChild<QComboBox*>("sizeChoice");
        if(!require(sizeChoice && sizeChoice->count()==4,"Missing interface size presets"))return 21;
        const auto fits=[&](QWidget* root) {
            if(root->minimumSize()!=root->maximumSize())return false;
            for(auto* widget:root->findChildren<QWidget*>()) {
                if(widget->isWindow() || widget->window()!=root->window() || !widget->isVisibleTo(root))continue;
                const QRect bounds(widget->mapTo(root,QPoint(0,0)),widget->size());
                if(!root->rect().adjusted(-1,-1,1,1).contains(bounds)) {
                    qCritical()<<"Control outside fixed window"<<widget<<bounds<<root->size();return false;
                }
                if(auto* label=qobject_cast<QLabel*>(widget)) {
                    if(label->text().contains('<'))continue;
                    const int needed=label->hasHeightForWidth()?label->heightForWidth(label->width()):label->sizeHint().height();
                    if(needed>label->height()+1 || (!label->wordWrap() &&
                        label->fontMetrics().horizontalAdvance(label->text())>label->contentsRect().width()+1)) {
                        qCritical()<<"Label clipped"<<label->text()<<label->size()<<needed;return false;
                    }
                }
            }
            return true;
        };
        for(const auto& code:{QString("hu"),QString("en")}) {
            language->setCurrentIndex(code=="hu"?0:1);
            for(bool dark:{false,true}) {
                theme->setCurrentIndex(dark?1:0);
                for(int percent:{75,100,125,150}) {
                    sizeChoice->setCurrentIndex(sizeChoice->findData(percent));
                    window.renderPreview(dark,110.0);app.processEvents();
                    if(!require(window.size()==QSize(1000*percent/125,850*percent/125),"Incorrect fixed preset size"))return 22;
                    for(auto* root:{static_cast<QWidget*>(&window),static_cast<QWidget*>(settings),static_cast<QWidget*>(audio),static_cast<QWidget*>(about)}) {
                        if(!require(fits(root),"Clipping at fixed UI scale"))return 23;
                        const auto name=root==&window?QString("main"):root->objectName();
                        if(!root->grab().save(folder+"/"+code+(dark?"-dark-":"-light-")+QString::number(percent)+"-"+name+".png"))return 4;
                    }
                    const int previous=profile->currentIndex();
                    for(int index=0;index<profile->count();++index) {
                        profile->setCurrentIndex(index);app.processEvents();
                        QStyleOptionComboBox option;option.initFrom(profile);option.currentText=profile->currentText();
                        const auto edit=profile->style()->subControlRect(QStyle::CC_ComboBox,&option,QStyle::SC_ComboBoxEditField,profile);
                        if(!require(profile->fontMetrics().horizontalAdvance(profile->currentText())<=edit.width(),"Tuning name clipped"))return 24;
                    }
                    profile->setCurrentIndex(previous);
                }
            }
        }
        UiScale::instance().setPercent(75,true);QSettings().sync();
        QProcess scaleChild;scaleChild.start(app.applicationFilePath(),{"--test-settings",testSettings.path(),"--check-scale","75"});
        if(!require(scaleChild.waitForFinished(10000) && scaleChild.exitCode()==0 && scaleChild.exitStatus()==QProcess::NormalExit,"UI scale did not persist"))return 25;
        if(!require(UiScale::fitPercent(150,QSize(1000,850))==125 && UiScale::fitPercent(125,QSize(800,680))==100,"Display fit failed"))return 26;
        // Even the smallest preset can exceed a screen at high system scaling.
        // Limit the outer window and keep the complete canvas reachable by scrolling.
        const QSize smallScreen(500,450);
        window.setFixedSize(UiScale::fitWindowSize(75,smallScreen));
        app.processEvents();
        auto* scroll=window.findChild<QScrollArea*>("instrumentScrollArea");
        if(!require(window.size()==smallScreen && scroll && scroll->widget()->size()==QSize(600,510)
            && scroll->horizontalScrollBar()->maximum()>0 && scroll->verticalScrollBar()->maximum()>0,
            "Minimum interface preset cannot scroll on a small screen"))return 28;
        scroll->horizontalScrollBar()->setValue(scroll->horizontalScrollBar()->maximum());
        scroll->verticalScrollBar()->setValue(scroll->verticalScrollBar()->maximum());
        app.processEvents();
        const auto* status=window.findChild<QLabel*>("statusLabel");
        const QRect statusBounds(status->mapTo(scroll->viewport(),QPoint(0,0)),status->size());
        if(!require(scroll->viewport()->rect().intersects(statusBounds),
                    "Bottom of tuner cannot be reached by scrolling"))return 28;
        // Dialogs keep their readable canvases while the outer window fits a
        // small work area. The action buttons must remain reachable by scrolling.
        for(auto* dialog:{settings,audio,about}) {
            const auto preferred=dialog->property("ssPreferredSize").toSize();
            const QSize available(260,180);
            dialog->setFixedSize(UiScale::fitWindowSize(preferred,75,available));
            app.processEvents();
            auto* viewport=dialog->findChild<QScrollArea*>("dialogScrollArea");
            if(!require(viewport && available.width()>=dialog->width() && available.height()>=dialog->height()
                && viewport->horizontalScrollBar()->maximum()>0 && viewport->verticalScrollBar()->maximum()>0,
                "Dialog cannot scroll within a small work area"))return 30;
            auto* buttons=dialog->findChild<QDialogButtonBox*>();
            auto* action=buttons->button(QDialogButtonBox::Close);
            if(!action)action=buttons->button(QDialogButtonBox::Ok);
            viewport->ensureWidgetVisible(action);
            app.processEvents();
            const QRect buttonBounds(action->mapTo(viewport->viewport(),QPoint(0,0)),action->size());
            if(!require(viewport->viewport()->rect().contains(buttonBounds),
                        "Dialog action button is not reachable"))return 30;
        }
        settings->close();audio->close();about->close();
        return 0;
    }
    return app.exec();
}

