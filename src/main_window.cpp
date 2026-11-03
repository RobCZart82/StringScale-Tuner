#include "main_window.hpp"
#include "tuning.hpp"
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
    if (!preview_) {
    connect(&capture_, &AudioCapture::pitchReady, this, &MainWindow::handlePitch);
    connect(&capture_, &AudioCapture::levelReady, this, &MainWindow::handleLevel);
    connect(&capture_, &AudioCapture::errorOccurred, this, [this](const QString& message) {
        handlePitch(0,0); statusLabel_->setText(message);
    });
    connect(&mediaDevices_, &QMediaDevices::audioInputsChanged, this, &MainWindow::refreshInputDevices);
    refreshInputDevices();
    }
}

void MainWindow::applyTheme(bool dark) {
    dark_=dark;
    QPalette pal;
    const QColor bg(dark?"#171d25":"#fffaf0"), base(dark?"#202833":"#fffdf8");
    const QColor fg(dark?"#f2f5fa":"#14263c"), mid(dark?"#556170":"#c9c8c1");
    const QColor active(dark?"#39dcc1":"#00846f");
    pal.setColor(QPalette::Window,bg);pal.setColor(QPalette::Base,base);
    pal.setColor(QPalette::AlternateBase,bg);pal.setColor(QPalette::WindowText,fg);
    pal.setColor(QPalette::Text,fg);pal.setColor(QPalette::Button,base);
    pal.setColor(QPalette::ButtonText,fg);pal.setColor(QPalette::Mid,mid);
    pal.setColor(QPalette::Highlight,active);pal.setColor(QPalette::HighlightedText,dark?QColor("#14263c"):QColor("#ffffff"));
    pal.setColor(QPalette::ToolTipBase,base);pal.setColor(QPalette::ToolTipText,fg);
    qApp->setPalette(pal);
    setStyleSheet("QComboBox, QPushButton, QDoubleSpinBox { padding: 9px; border: 1px solid palette(mid); border-radius: 9px; background: palette(base); color: palette(text); } QComboBox:focus, QPushButton:focus { border: 2px solid palette(highlight); }");
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
    auto* settings=new QPushButton;settings->setObjectName("settingsButton");settings->setIconSize(QSize(32,32));settings->setAccessibleName("Beállítások");settings->setToolTip("Beállítások");header->addWidget(settings);layout->addLayout(header);
    auto* controls=new QHBoxLayout;
    profileChoice_=new QComboBox;profileChoice_->setAccessibleName("Hangszer és hangolás");
    for(const auto& preset:strings::presets())profileChoice_->addItem(QString::fromStdString(preset.displayName),QString::fromStdString(preset.id));
    const auto stored=QSettings().value("tuning/profile","guitar-e").toString();
    int index=profileChoice_->findData(stored);profileChoice_->setCurrentIndex(index>=0?index:0);
    selected_=&strings::presets()[profileChoice_->currentIndex()];
    inputChoice_=new QComboBox;inputChoice_->setAccessibleName("Hangbemenet");inputChoice_->setMinimumWidth(190);
    controls->addWidget(profileChoice_,3);controls->addWidget(inputChoice_,2);layout->addLayout(controls);
    levelMeter_=new InputLevelMeter;levelMeter_->setLimits(minimumLevelDb_,warningPeakDb_);
    gauge_=new AnalogGauge;pitchScale_=new PitchScale;pitchScale_->setPreset(selected_,calibration_);
    layout->addWidget(levelMeter_);layout->addWidget(gauge_,1);layout->addWidget(pitchScale_);
    statusLabel_=new QLabel("Figyelek…");statusLabel_->setWordWrap(true);statusLabel_->setAccessibleName("Állapot");layout->addWidget(statusLabel_);
    setCentralWidget(page);
    connect(settings,&QPushButton::clicked,this,&MainWindow::showSettings);
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
    if (!inputChoice_ || inputChoice_->currentIndex() < 0) {
        capture_.stop();
        statusLabel_->setText("Nincs hangbemenet csatlakoztatva");
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
        statusLabel_->setText("Mikrofonhozzáférés letiltva – engedélyezd a rendszerbeállításokban");
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }

    const QByteArray id = inputChoice_->currentData().toByteArray();
    const auto devices = QMediaDevices::audioInputs();
    for (const auto& device : devices) {
        if (device.id() == id) {
            QSettings().setValue("input/deviceId", id);
            capture_.start(device);
            return;
        }
    }
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
    if(!std::isfinite(hz)||hz<=0||!selected_){gauge_->setCents(0,false);gauge_->setReadout("—","","Pengess meg egy húrt");return;}
    auto* target=strings::closestString(*selected_,hz,calibration_);if(!target)return;
    const double cents=strings::centsBetween(hz,target->hz*calibration_/440);
    QString state=std::abs(cents)<=2?"✓ Jól szól!":cents<0?"Túl mély – hangolj feljebb":"Túl magas – hangolj lejjebb";
    if(std::abs(cents)>50)state+=" · több mint 50 cent";
    gauge_->setCents(cents,true);
    gauge_->setReadout(QString::fromStdString(strings::measuredNote(hz,calibration_)),
        QString("Cél: %1. húr · %2").arg(target->number).arg(QString::fromStdString(target->note)),state);
}
void MainWindow::handleLevel(double rmsDb,double peakDb,bool clipped){levelMeter_->setLevel(rmsDb,peakDb,clipped);}
void MainWindow::renderPreview(bool dark,double hz){applyTheme(dark);handlePitch(hz,1);handleLevel(-22,-12,false);}
void MainWindow::showSettings() {
    QDialog dialog(this);dialog.setWindowTitle("Beállítások");auto* layout=new QVBoxLayout(&dialog);auto* form=new QFormLayout;
    auto* theme=new QComboBox;theme->addItems({"Világos","Sötét"});theme->setCurrentIndex(dark_?1:0);
    form->addRow("Megjelenés",theme);
    connect(theme,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[this](int i){applyTheme(i==1);if(!preview_)QSettings().setValue("appearance/dark",dark_);});
    auto* minSpin=new QDoubleSpinBox;minSpin->setRange(-65,-20);minSpin->setSuffix(" dBFS RMS");minSpin->setValue(minimumLevelDb_);form->addRow("Minimum jelszint",minSpin);
    auto* maxSpin=new QDoubleSpinBox;maxSpin->setRange(-12,-0.5);maxSpin->setSuffix(" dBFS peak");maxSpin->setValue(warningPeakDb_);form->addRow("Maximum jelszint jelzése",maxSpin);
    layout->addLayout(form);auto* hint=new QLabel("A megjelenés azonnal mentődik. A jelszintküszöbök nem garantálják a hangmagasságmérés pontosságát.");hint->setWordWrap(true);layout->addWidget(hint);
    auto* buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);buttons->button(QDialogButtonBox::Ok)->setText("Mentés");buttons->button(QDialogButtonBox::Cancel)->setText("Mégse");
    connect(buttons,&QDialogButtonBox::accepted,&dialog,&QDialog::accept);connect(buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);layout->addWidget(buttons);dialog.resize(550,240);
    if(dialog.exec()!=QDialog::Accepted)return;
    minimumLevelDb_=minSpin->value();warningPeakDb_=maxSpin->value();capture_.setMinimumLevelDb(minimumLevelDb_);capture_.setWarningPeakDb(warningPeakDb_);levelMeter_->setLimits(minimumLevelDb_,warningPeakDb_);
    if(!preview_){QSettings s;s.setValue("input/minRmsDb",minimumLevelDb_);s.setValue("input/warningPeakDb",warningPeakDb_);}
}
