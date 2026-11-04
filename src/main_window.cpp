#include "ui_language.hpp"
#include "main_window.hpp"
#include "tuning.hpp"
#include "about_dialog.hpp"
#include <QMenu>
#include <QAction>
#include <QSignalBlocker>
#include <QApplication>
#include <QPermissions>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QPalette>
#include <QPainter>
#include <QIcon>
#include <QPixmap>
#include <algorithm>
#include <cmath>

MainWindow::MainWindow(QWidget* parent, bool preview)
    : QMainWindow(parent), capture_(this), mediaDevices_(this), preview_(preview) {
    QSettings settings;
    minimumLevelDb_ = std::clamp(settings.value("input/minRmsDb", -45.0).toDouble(), -65.0, -20.0);
    warningPeakDb_ = std::clamp(settings.value("input/warningPeakDb", -3.0).toDouble(), -12.0, -0.5);
    calibration_ = std::clamp(settings.value("tuning/a4Hz", 440.0).toDouble(),430.0,450.0);
    capture_.setMinimumLevelDb(minimumLevelDb_);
    capture_.setWarningPeakDb(warningPeakDb_);
    buildInterface();
    applyTheme(settings.value("appearance/dark", false).toBool());
    connect(&UiLanguage::instance(), &UiLanguage::changed, this, &MainWindow::retranslate);
    retranslate();
    if(preview_) { channelCount_=2; emit inputChannelsChanged(); }
    if (!preview_) {
    connect(&capture_, &AudioCapture::pitchReady, this, &MainWindow::handlePitch);
    connect(&capture_, &AudioCapture::levelReady, this, &MainWindow::handleLevel);
    connect(&capture_, &AudioCapture::errorOccurred, this, [this](const QString& message) {
        handlePitch(0,0); channelCount_=0; emit inputChannelsChanged(); errorKey_=message; status_=Status::Error; refreshStatus();
    });
    connect(&mediaDevices_, &QMediaDevices::audioInputsChanged, this, &MainWindow::refreshInputDevices);
    refreshInputDevices();
    }
}

void MainWindow::applyTheme(bool dark) {
    dark_=dark;
    QPalette pal;
    const QColor bg(dark?"#252D38":"#fffaf0"), base(dark?"#303B49":"#fffdf8");
    const QColor fg(dark?"#EDF1F5":"#14263c"), mid(dark?"#718096":"#c9c8c1");
    const QColor active(dark?"#39dcc1":"#00846f");
    pal.setColor(QPalette::Window,bg);pal.setColor(QPalette::Base,base);
    pal.setColor(QPalette::AlternateBase,bg);pal.setColor(QPalette::WindowText,fg);
    pal.setColor(QPalette::Text,fg);pal.setColor(QPalette::Button,base);
    pal.setColor(QPalette::ButtonText,fg);pal.setColor(QPalette::Mid,mid);
    pal.setColor(QPalette::Highlight,active);pal.setColor(QPalette::HighlightedText,dark?QColor("#14263c"):QColor("#ffffff"));
    if(dark) {
        const QColor secondary("#B8C4D2");
        pal.setColor(QPalette::PlaceholderText,secondary);
        pal.setColor(QPalette::Disabled,QPalette::WindowText,secondary);
        pal.setColor(QPalette::Disabled,QPalette::Text,secondary);
        pal.setColor(QPalette::Disabled,QPalette::ButtonText,secondary);
    }
    pal.setColor(QPalette::Link,active);pal.setColor(QPalette::LinkVisited,active);
    pal.setColor(QPalette::ToolTipBase,base);pal.setColor(QPalette::ToolTipText,fg);
    qApp->setPalette(pal);
    QString styles("QComboBox, QPushButton, QDoubleSpinBox { padding: 9px; border: 1px solid palette(mid); border-radius: 9px; background: palette(base); color: palette(text); } QComboBox:focus, QPushButton:focus { border: 2px solid palette(highlight); }");
    if(dark) styles += " QDialog { background-color: #303B49; border: 1px solid #718096; } QLabel#statusLabel { color: #B8C4D2; }";
    setStyleSheet(styles);
    // Draw the gear as vector strokes to avoid font-dependent missing glyphs.
    QPixmap gear(32,32);gear.fill(Qt::transparent);QPainter p(&gear);p.setRenderHint(QPainter::Antialiasing);p.setPen(QPen(fg,2.5));
    p.drawEllipse(QPointF(16,16),9,9);p.drawEllipse(QPointF(16,16),3,3);
    for(int i=0;i<8;++i){p.save();p.translate(16,16);p.rotate(i*45);p.drawLine(QPointF(0,-9),QPointF(0,-14));p.restore();}
    if(auto* b=findChild<QPushButton*>("settingsButton"))b->setIcon(QIcon(gear));
    for(auto* w:findChildren<QWidget*>())w->update();
}

void MainWindow::buildInterface() {
    setWindowTitle("StringScale Tuner");setMinimumSize(800,760);resize(1000,850);
    auto f=font();f.setPointSize(13);setFont(f);
    auto* page=new QWidget(this);auto* layout=new QVBoxLayout(page);
    layout->setContentsMargins(22,16,22,16);layout->setSpacing(12);
    auto* header=new QHBoxLayout;
    auto* title=new QLabel("StringScale Tuner");auto tf=font();tf.setPointSize(24);tf.setBold(true);title->setFont(tf);
    header->addWidget(title);header->addStretch();
    auto* settings=new QPushButton;settings->setObjectName("settingsButton");settings->setIconSize(QSize(32,32));settings->setAccessibleName(uiText("Beállítások", "Settings"));settings->setToolTip(uiText("Beállítások", "Settings"));header->addWidget(settings);layout->addLayout(header);
    auto* controls=new QHBoxLayout;
    profileChoice_=new QComboBox;profileChoice_->setObjectName("profileChoice");profileChoice_->setAccessibleName(uiText("Hangszer és hangolás", "Instrument and tuning"));
    for(const auto& preset:strings::presets())profileChoice_->addItem(QString::fromStdString(preset.displayName),QString::fromStdString(preset.id));
    const auto stored=QSettings().value("tuning/profile","guitar-e").toString();
    int index=profileChoice_->findData(stored);profileChoice_->setCurrentIndex(index>=0?index:0);
    selected_=&strings::presets()[profileChoice_->currentIndex()];
    inputChoice_=new QComboBox;inputChoice_->setObjectName("inputChoice");
    if(preview_)inputChoice_->addItem(uiText("Tesztbemenet", "Test input"));inputChoice_->setAccessibleName(uiText("Hangbemenet", "Audio input"));inputChoice_->setMinimumWidth(190);
    controls->addWidget(profileChoice_,3);controls->addWidget(inputChoice_,2);layout->addLayout(controls);
    levelMeter_=new InputLevelMeter;levelMeter_->setLimits(minimumLevelDb_,warningPeakDb_);
    gauge_=new AnalogGauge;pitchScale_=new PitchScale;pitchScale_->setPreset(selected_,calibration_);
    layout->addWidget(levelMeter_);layout->addWidget(gauge_,1);layout->addWidget(pitchScale_);
    statusLabel_=new QLabel(uiText("Figyelek…", "Listening…"));statusLabel_->setObjectName("statusLabel");statusLabel_->setWordWrap(true);statusLabel_->setAccessibleName(uiText("Állapot", "Status"));layout->addWidget(statusLabel_);
    setCentralWidget(page);
    auto* menu=new QMenu(settings);
    settingsAction_=menu->addAction(QString());settingsAction_->setObjectName("settingsAction");
    audioAction_=menu->addAction(QString());audioAction_->setObjectName("audioAction");
    menu->addSeparator();
    aboutAction_=menu->addAction(QString());aboutAction_->setObjectName("aboutAction");
    settings->setMenu(menu);
    connect(settingsAction_,&QAction::triggered,this,[this]{openSettings();});
    connect(audioAction_,&QAction::triggered,this,[this]{openSettings(true);});
    connect(aboutAction_,&QAction::triggered,this,[this]{openAbout();});
    connect(profileChoice_,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[this]{updateTuning();});
    connect(inputChoice_,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[this]{selectInput();});
}
void MainWindow::refreshInputDevices() {
    if (preview_) return;
    const QByteArray previousId = inputChoice_->currentData().toByteArray();
    const auto devices = QMediaDevices::audioInputs();
    const QByteArray defaultId = QMediaDevices::defaultAudioInput().id();
    const QSettings settings;
    const QByteArray storedId = settings.value("input/deviceId").toByteArray();
    const QByteArray preferredId = !previousId.isEmpty() ? previousId :
                                   !storedId.isEmpty() ? storedId : defaultId;
    inputChoice_->blockSignals(true);
    inputChoice_->clear();
    for (const auto& device : devices)
        inputChoice_->addItem(device.description(), device.id());
    int index = inputChoice_->findData(preferredId);
    if (index < 0) index = inputChoice_->findData(defaultId);
    if (index >= 0) inputChoice_->setCurrentIndex(index);
    inputChoice_->blockSignals(false);
    selectInput();
}

void MainWindow::selectInput() {
    if (preview_) return;
    channelCount_=0; emit inputChannelsChanged();
    if (!inputChoice_ || inputChoice_->currentIndex() < 0) {
        capture_.stop();
        status_=Status::NoInput; refreshStatus();
        return;
    }
    // macOS (and other protected platforms) must grant microphone access.
    // Qt 6.5+ exposes this cross-platform permission API.
    const QMicrophonePermission microphonePermission;
    switch (qApp->checkPermission(microphonePermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(microphonePermission, this,
            [this](const QPermission&) { selectInput(); });
        return;
    case Qt::PermissionStatus::Denied:
        capture_.stop();
        status_=Status::Denied; refreshStatus();
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }

    const QByteArray id = inputChoice_->currentData().toByteArray();
    const auto devices = QMediaDevices::audioInputs();
    for (const auto& device : devices) {
        if (device.id() == id) {
            QSettings().setValue("input/deviceId", id);
            const QString channelKey="input/channels/"+QString::fromLatin1(id.toHex());
            const int requested=QSettings().value(channelKey,0).toInt();
            if(capture_.start(device,requested)){
                channelCount_=capture_.channelCount();inputChannel_=capture_.inputChannel();
                QSettings().setValue(channelKey,inputChannel_);
                emit inputChannelsChanged();status_=Status::Listening;refreshStatus();
            }
            return;
        }
    }
    capture_.stop();status_=Status::NoInput;refreshStatus();
}


void MainWindow::updateTuning() {
    if(profileChoice_->currentIndex()<0)return;
    selected_=&strings::presets()[profileChoice_->currentIndex()];
    pitchScale_->setPreset(selected_,calibration_);
    if(!preview_)QSettings().setValue("tuning/profile",profileChoice_->currentData());
    handlePitch(detectedHz_,1);
}
void MainWindow::handlePitch(double hz,double confidence) {
    Q_UNUSED(confidence);
    detectedHz_=hz;pitchScale_->setFrequency(hz);
    if(!std::isfinite(hz)||hz<=0||!selected_){gauge_->setCents(0,false);gauge_->setReadout("—","",uiText("Pengess meg egy húrt", "Pluck a string"));return;}
    auto* target=strings::closestString(*selected_,hz,calibration_);if(!target)return;
    const double cents=strings::centsBetween(hz,target->hz*calibration_/440);
    QString state=std::abs(cents)<=2?uiText("Jól szól!", "In tune!"):cents<0?uiText("Túl mély – hangolj feljebb", "Too low – tune higher"):uiText("Túl magas – hangolj lejjebb", "Too high – tune lower");
    if(std::abs(cents)>50)state=(cents<0?QString(uiText("Túl mély", "Too low")):QString(uiText("Túl magas", "Too high")))+uiText("\nEltérés: több mint 50 cent", "\nMore than 50 cents away");
    gauge_->setCents(cents,true);
    gauge_->setReadout(QString::fromStdString(strings::measuredNote(hz,calibration_)),
        QString(uiText("Cél: %1. húr · %2", "Target: string %1 · %2")).arg(target->number).arg(QString::fromStdString(target->note)),state);
}
void MainWindow::handleLevel(double rmsDb,double peakDb,bool clipped){levelMeter_->setLevel(rmsDb,peakDb,clipped);}
void MainWindow::renderPreview(bool dark,double hz){applyTheme(dark);handlePitch(hz,1);handleLevel(hz>0?-22:-120,hz>0?-12:-120,false);status_=Status::Preview;refreshStatus();}

void MainWindow::refreshStatus() {
    switch(status_) {
    case Status::Listening: statusLabel_->setText(uiText("Figyelek…","Listening…")); break;
    case Status::Preview: statusLabel_->setText(uiText("Tesztelőnézet – nincs élő hangbemenet","Preview – no live audio input")); break;
    case Status::NoInput: statusLabel_->setText(uiText("Nincs hangbemenet csatlakoztatva","No audio input connected")); break;
    case Status::Denied: statusLabel_->setText(uiText("Mikrofonhozzáférés letiltva – engedélyezd a rendszerbeállításokban","Microphone access denied – enable it in system settings")); break;
    case Status::Error:
        if(errorKey_=="No audio input device selected.") statusLabel_->setText(uiText("Nincs hangbemenet kiválasztva.","No audio input device selected."));
        else if(errorKey_=="Selected input has no supported audio format.") statusLabel_->setText(uiText("A kiválasztott bemenet hangformátuma nem támogatott.","Selected input has no supported audio format."));
        else if(errorKey_=="Could not open audio input. Check microphone permission and device availability.") statusLabel_->setText(uiText("A hangbemenet nem nyitható meg. Ellenőrizd a mikrofonengedélyt és az eszközt.","Could not open audio input. Check microphone permission and device availability."));
        else statusLabel_->setText(uiText("A hangbemenet váratlanul leállt. Ellenőrizd az eszközt és az engedélyeket.","Audio input stopped unexpectedly. Check the device and permissions."));
        break;
    }
}
void MainWindow::retranslate() {
    settingsAction_->setText(uiText("Beállítások","Settings"));
    audioAction_->setText(uiText("Hang","Audio"));
    aboutAction_->setText(uiText("Névjegy","About"));
    auto* button=findChild<QPushButton*>("settingsButton");
    button->setToolTip(uiText("Menü","Menu"));button->setAccessibleName(uiText("Menü","Menu"));
    profileChoice_->setAccessibleName(uiText("Hangszer és hangolás","Instrument and tuning"));
    inputChoice_->setAccessibleName(uiText("Hangbemenet","Audio input"));
    statusLabel_->setAccessibleName(uiText("Állapot","Status"));
    gauge_->setAccessibleName(uiText("Analóg hangoló, mínusz ötven és plusz ötven cent között","Analog tuner, minus fifty to plus fifty cents"));
    pitchScale_->setAccessibleName(uiText("Húrok és mért hangmagasság","Strings and measured pitch"));
    levelMeter_->setAccessibleName(uiText("Bemeneti jelszint","Input signal level"));
    const QStringList hu={"Gitár · Normál (6)","Gitár · Drop D (6)","Gitár · Félhanggal lejjebb (6)","Gitár · Open G (6)","Gitár · DADGAD (6)","Gitár · Normál (7)","Basszus · Normál (4)","Basszus · Normál (5)"};
    const QStringList en={"Guitar · Standard (6)","Guitar · Drop D (6)","Guitar · E-flat (6)","Guitar · Open G (6)","Guitar · DADGAD (6)","Guitar · Standard (7)","Bass · Standard (4)","Bass · Standard (5)"};
    const QSignalBlocker blocker(profileChoice_);
    for(int i=0;i<profileChoice_->count();++i)profileChoice_->setItemText(i,UiLanguage::instance().code()=="hu"?hu.value(i):en.value(i));
    if(preview_)inputChoice_->setItemText(0,uiText("Tesztbemenet","Test input"));
    handlePitch(detectedHz_,1);refreshStatus();emit inputChannelsChanged();
    for(auto* w:findChildren<QWidget*>())w->update();
}
QDialog* MainWindow::openAbout() {
    if(!aboutDialog_) {aboutDialog_=new AboutDialog(this);aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);}
    aboutDialog_->show();aboutDialog_->raise();return aboutDialog_;
}
QDialog* MainWindow::openSettings(bool audio) {
    auto& existing=audio?audioDialog_:settingsDialog_;
    if(existing){existing->show();existing->raise();return existing;}
    auto* dialog=new QDialog(this);existing=dialog;dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setObjectName(audio?"audioDialog":"settingsDialog");
    auto* layout=new QVBoxLayout(dialog);layout->setContentsMargins(22,22,22,22);layout->setSpacing(18);
    auto* form=new QFormLayout;form->setSpacing(14);layout->addLayout(form);
    auto* hint=new QLabel;hint->setWordWrap(true);layout->addWidget(hint);
    if(!audio) {
        auto* theme=new QComboBox;theme->setObjectName("themeChoice");theme->addItems({QString(),QString()});theme->setCurrentIndex(dark_?1:0);
        auto* themeLabel=new QLabel;themeLabel->setBuddy(theme);form->addRow(themeLabel,theme);
        auto* language=new QComboBox;language->setObjectName("languageChoice");language->addItem("Magyar","hu");language->addItem("English","en");language->setCurrentIndex(UiLanguage::instance().code()=="hu"?0:1);
        auto* languageLabel=new QLabel("Nyelv / Language");languageLabel->setBuddy(language);form->addRow(languageLabel,language);
        auto* buttons=new QDialogButtonBox(QDialogButtonBox::Close);layout->addWidget(buttons);
        connect(buttons,&QDialogButtonBox::rejected,dialog,&QDialog::close);
        const auto translate=[dialog,theme,themeLabel,language,hint,buttons] {
            dialog->setWindowTitle(uiText("Beállítások","Settings"));themeLabel->setText(uiText("Megjelenés","Appearance"));
            {const QSignalBlocker block(theme);theme->setItemText(0,uiText("Világos","Light"));theme->setItemText(1,uiText("Sötét","Dark"));}
            {const QSignalBlocker block(language);language->setCurrentIndex(UiLanguage::instance().code()=="hu"?0:1);}
            hint->setText(uiText("A megjelenés és a nyelv azonnal mentődik.","Appearance and language are saved immediately."));
            buttons->button(QDialogButtonBox::Close)->setText(uiText("Bezárás","Close"));
        };
        connect(theme,QOverload<int>::of(&QComboBox::currentIndexChanged),dialog,[this](int i){applyTheme(i==1);if(!preview_)QSettings().setValue("appearance/dark",dark_);});
        connect(language,QOverload<int>::of(&QComboBox::currentIndexChanged),dialog,[this,language](int){UiLanguage::instance().setCode(language->currentData().toString(),!preview_);});
        connect(&UiLanguage::instance(),&UiLanguage::changed,dialog,translate);translate();
    } else {
        auto* channel=new QComboBox;channel->setObjectName("inputChannelChoice");
        auto* channelLabel=new QLabel;channelLabel->setBuddy(channel);form->addRow(channelLabel,channel);
        const auto refreshChannels=[this,channel] {
            const QSignalBlocker blocker(channel);channel->clear();
            if(channelCount_<=0)channel->addItem(uiText("Nincs aktív hangbemenet","No active audio input"),-1);
            else for(int i=0;i<channelCount_;++i)channel->addItem(uiText("%1. csatorna","Channel %1").arg(i+1),i);
            channel->setEnabled(channelCount_>1);
            if(channelCount_>0)channel->setCurrentIndex(inputChannel_);
        };
        connect(this,&MainWindow::inputChannelsChanged,dialog,refreshChannels);
        connect(channel,QOverload<int>::of(&QComboBox::currentIndexChanged),dialog,[this,channel](int){chooseInputChannel(channel->currentData().toInt());});
        refreshChannels();
        auto* minSpin=new QDoubleSpinBox;minSpin->setRange(-65,-20);minSpin->setSuffix(" dBFS RMS");minSpin->setValue(minimumLevelDb_);
        auto* maxSpin=new QDoubleSpinBox;maxSpin->setRange(-12,-0.5);maxSpin->setSuffix(" dBFS peak");maxSpin->setValue(warningPeakDb_);
        auto* minLabel=new QLabel;minLabel->setBuddy(minSpin);auto* maxLabel=new QLabel;maxLabel->setBuddy(maxSpin);form->addRow(minLabel,minSpin);form->addRow(maxLabel,maxSpin);
        auto* buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);layout->addWidget(buttons);
        const auto translate=[dialog,channelLabel,refreshChannels,minLabel,maxLabel,hint,buttons] {
            dialog->setWindowTitle(uiText("Hang","Audio"));channelLabel->setText(uiText("Bemeneti csatorna","Input channel"));refreshChannels();minLabel->setText(uiText("Minimum jelszint","Minimum input level"));maxLabel->setText(uiText("Maximum jelszint jelzése","High-level warning"));
            hint->setText(uiText("A csatornaválasztás azonnal mentődik. Válaszd azt a csatornát, ahová a gitárt csatlakoztattad. A jelszintküszöbök önmagukban nem garantálják a mérés pontosságát.","The input channel is saved immediately. Choose the channel connected to your guitar. Input level limits alone do not guarantee pitch accuracy."));
            buttons->button(QDialogButtonBox::Ok)->setText(uiText("Mentés","Save"));buttons->button(QDialogButtonBox::Cancel)->setText(uiText("Mégse","Cancel"));
        };
        connect(&UiLanguage::instance(),&UiLanguage::changed,dialog,translate);translate();
        connect(buttons,&QDialogButtonBox::rejected,dialog,&QDialog::close);
        connect(buttons,&QDialogButtonBox::accepted,dialog,[this,dialog,minSpin,maxSpin] {
            minimumLevelDb_=minSpin->value();warningPeakDb_=maxSpin->value();capture_.setMinimumLevelDb(minimumLevelDb_);capture_.setWarningPeakDb(warningPeakDb_);levelMeter_->setLimits(minimumLevelDb_,warningPeakDb_);
            if(!preview_){QSettings s;s.setValue("input/minRmsDb",minimumLevelDb_);s.setValue("input/warningPeakDb",warningPeakDb_);}
            dialog->close();
        });
    }
    dialog->resize(560,230);dialog->show();return dialog;
}

void MainWindow::chooseInputChannel(int channel) {
    if(channel<0 || channel>=channelCount_)return;
    if(preview_ || capture_.setInputChannel(channel)) {
        inputChannel_=channel;
        if(!preview_) {
            const auto id=inputChoice_->currentData().toByteArray();
            if(!id.isEmpty())QSettings().setValue("input/channels/"+QString::fromLatin1(id.toHex()),channel);
        }
        emit inputChannelsChanged();
    }
}
