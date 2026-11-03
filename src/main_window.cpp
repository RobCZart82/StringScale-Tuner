#include "main_window.hpp"

#include "tuning.hpp"

#include <QAudioDevice>
#include <QApplication>
#include <QPermissions>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), capture_(this), mediaDevices_(this) {
    QSettings settings;
    minimumLevelDb_ = std::clamp(settings.value("input/minRmsDb", -45.0).toDouble(), -65.0, -20.0);
    warningPeakDb_ = std::clamp(settings.value("input/warningPeakDb", -3.0).toDouble(), -12.0, -0.5);
    capture_.setMinimumLevelDb(minimumLevelDb_);
    capture_.setWarningPeakDb(warningPeakDb_);

    buildInterface();
    connect(&capture_, &AudioCapture::pitchReady,
            this, &MainWindow::handlePitch);
    connect(&capture_, &AudioCapture::levelReady,
            this, &MainWindow::handleLevel);
    connect(&capture_, &AudioCapture::errorOccurred,
            this, [this](const QString& message) {
                statusLabel_->setText(message);
                statusLabel_->setStyleSheet("color: #f59e85;");
            });
    connect(&mediaDevices_, &QMediaDevices::audioInputsChanged,
            this, &MainWindow::refreshInputDevices);

    refreshInputDevices();
}

void MainWindow::buildInterface() {
    setWindowTitle("StringScale Tuner · v0.1.0");
    setMinimumSize(760, 670);
    resize(980, 790);
    setStyleSheet(R"(
        QMainWindow, QWidget#page { background: #0c1521; color: #ecf2fa; }
        QLabel { color: #ecf2fa; }
        QComboBox, QDoubleSpinBox, QPushButton {
            color: #ecf2fa; background: #203248; border: 1px solid #475b75;
            border-radius: 6px; padding: 7px; min-height: 25px;
        }
        QComboBox QAbstractItemView { color: #ecf2fa; background: #203248;
                                      selection-background-color: #315474; }
        QPushButton:hover { background: #304a65; }
    )");

    auto* page = new QWidget(this);
    page->setObjectName("page");
    auto* mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(20, 17, 20, 20);
    mainLayout->setSpacing(12);

    auto* firstRow = new QHBoxLayout;
    firstRow->setSpacing(10);
    auto* heading = new QLabel("STRINGSCALE");
    QFont titleFont = heading->font();
    titleFont.setPointSize(15);
    titleFont.setBold(true);
    heading->setFont(titleFont);
    firstRow->addWidget(heading);
    firstRow->addStretch();
    auto* hint = new QLabel("Absolute pitch  •  Fine tuning  •  Input level");
    hint->setStyleSheet("color: #8ba2bc;");
    firstRow->addWidget(hint);
    mainLayout->addLayout(firstRow);

    auto* controls = new QHBoxLayout;
    controls->setSpacing(9);
    auto* tuningLabel = new QLabel("Tuning");
    profileChoice_ = new QComboBox;
    profileChoice_->setMinimumWidth(230);
    for (const auto& p : strings::presets())
        profileChoice_->addItem(QString::fromStdString(p.displayName),
                                QString::fromStdString(p.id));
    const QSettings settings;
    const QString preferredId = settings.value("tuning/profile", "guitar-e").toString();
    const int preferredIndex = profileChoice_->findData(preferredId);
    if (preferredIndex >= 0) profileChoice_->setCurrentIndex(preferredIndex);
    selected_ = &strings::presets()[profileChoice_->currentIndex()];
    controls->addWidget(tuningLabel);
    controls->addWidget(profileChoice_, 2);

    controls->addSpacing(10);
    auto* inputLabel = new QLabel("Input");
    inputChoice_ = new QComboBox;
    inputChoice_->setMinimumWidth(160);
    controls->addWidget(inputLabel);
    controls->addWidget(inputChoice_, 2);

    auto* calibrationLabel = new QLabel("A4");
    calibrationSpin_ = new QDoubleSpinBox;
    calibrationSpin_->setRange(430.0, 450.0);
    calibrationSpin_->setSingleStep(0.1);
    calibrationSpin_->setDecimals(1);
    calibrationSpin_->setSuffix(" Hz");
    calibrationSpin_->setValue(settings.value("tuning/a4Hz", 440.0).toDouble());
    controls->addWidget(calibrationLabel);
    controls->addWidget(calibrationSpin_);

    auto* settingsButton = new QPushButton("Settings");
    controls->addWidget(settingsButton);
    mainLayout->addLayout(controls);

    gauge_ = new AnalogGauge;
    pitchScale_ = new PitchScale;
    levelMeter_ = new InputLevelMeter;
    levelMeter_->setLimits(minimumLevelDb_, warningPeakDb_);
    mainLayout->addWidget(gauge_, 3);
    mainLayout->addWidget(pitchScale_, 1);
    mainLayout->addWidget(levelMeter_, 0);

    auto* bottom = new QHBoxLayout;
    bottom->setSpacing(24);
    frequencyLabel_ = new QLabel("— Hz");
    targetLabel_ = new QLabel("Target: —");
    statusLabel_ = new QLabel("Select an audio input and pluck a string");
    QFont big = frequencyLabel_->font();
    big.setPointSize(14);
    big.setBold(true);
    frequencyLabel_->setFont(big);
    bottom->addWidget(frequencyLabel_);
    bottom->addWidget(targetLabel_);
    bottom->addStretch();
    bottom->addWidget(statusLabel_);
    mainLayout->addLayout(bottom);
    setCentralWidget(page);

    pitchScale_->setPreset(selected_, calibrationSpin_->value());
    connect(profileChoice_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this] { updateTuning(); });
    connect(calibrationSpin_, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this] { updateTuning(); });
    connect(inputChoice_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this] { selectInput(); });
    connect(settingsButton, &QPushButton::clicked,
            this, &MainWindow::showSettings);
}

void MainWindow::refreshInputDevices() {
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
    if (!inputChoice_ || inputChoice_->currentIndex() < 0) {
        capture_.stop();
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
        statusLabel_->setText("Microphone access denied · enable it in OS privacy settings");
        statusLabel_->setStyleSheet("color: #f59e85;");
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
    if (profileChoice_->currentIndex() < 0) return;
    selected_ = &strings::presets()[profileChoice_->currentIndex()];
    const double a4 = calibrationSpin_->value();
    pitchScale_->setPreset(selected_, a4);
    QSettings settings;
    settings.setValue("tuning/profile", profileChoice_->currentData());
    settings.setValue("tuning/a4Hz", a4);
    handlePitch(detectedHz_, 1.0);
}

void MainWindow::handlePitch(double hz, double confidence) {
    Q_UNUSED(confidence);
    detectedHz_ = hz;
    pitchScale_->setFrequency(hz);
    if (hz <= 0 || !selected_) {
        gauge_->setCents(0, false);
        frequencyLabel_->setText("— Hz");
        targetLabel_->setText("Target: —");
        statusLabel_->setText("Pluck a string / waiting for a stable pitch");
        statusLabel_->setStyleSheet("color: #8ba2bc;");
        return;
    }
    const auto* target = strings::closestString(*selected_, hz,
                                                calibrationSpin_->value());
    if (!target) return;
    const double targetHz = target->hz * calibrationSpin_->value() / 440.0;
    const double cents = strings::centsBetween(hz, targetHz);
    const bool nearTarget = std::abs(cents) <= 50.0;
    gauge_->setCents(cents, nearTarget);
    frequencyLabel_->setText(QString("%1 Hz").arg(hz, 0, 'f', 2));
    targetLabel_->setText(QString("String %1  ·  %2  ·  %3 Hz")
        .arg(target->number)
        .arg(QString::fromStdString(target->note))
        .arg(targetHz, 0, 'f', 2));
    if (!nearTarget) {
        statusLabel_->setText(cents < 0 ? "Below target · tune higher →" :
                                          "Above target · tune lower ←");
        statusLabel_->setStyleSheet("color: #f7b267;");
    } else if (std::abs(cents) <= 2.0) {
        statusLabel_->setText("IN TUNE");
        statusLabel_->setStyleSheet("color: #37cd95; font-weight: bold;");
    } else {
        statusLabel_->setText(cents < 0 ? "Fine tune higher →" :
                                          "Fine tune lower ←");
        statusLabel_->setStyleSheet("color: #4fcaec;");
    }
}

void MainWindow::handleLevel(double rmsDb, double peakDb, bool clipped) {
    levelMeter_->setLevel(rmsDb, peakDb, clipped);
}

void MainWindow::showSettings() {
    QDialog dialog(this);
    dialog.setWindowTitle("Measurement settings");
    auto* layout = new QVBoxLayout(&dialog);
    auto* form = new QFormLayout;
    auto* minSpin = new QDoubleSpinBox;
    minSpin->setRange(-65.0, -20.0);
    minSpin->setDecimals(1);
    minSpin->setSuffix(" dBFS RMS");
    minSpin->setValue(minimumLevelDb_);
    form->addRow("Minimum usable signal", minSpin);

    auto* maxSpin = new QDoubleSpinBox;
    maxSpin->setRange(-12.0, -0.5);
    maxSpin->setDecimals(1);
    maxSpin->setSuffix(" dBFS peak");
    maxSpin->setValue(warningPeakDb_);
    form->addRow("High-level warning", maxSpin);
    layout->addLayout(form);
    auto* description = new QLabel(
        "The minimum is an RMS gate. The high limit warns about loud input; "
        "clipping is detected separately. Neither level alone guarantees pitch accuracy.");
    description->setWordWrap(true);
    layout->addWidget(description);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    dialog.resize(470, 210);
    if (dialog.exec() != QDialog::Accepted) return;

    minimumLevelDb_ = minSpin->value();
    warningPeakDb_ = maxSpin->value();
    capture_.setMinimumLevelDb(minimumLevelDb_);
    capture_.setWarningPeakDb(warningPeakDb_);
    levelMeter_->setLimits(minimumLevelDb_, warningPeakDb_);
    QSettings settings;
    settings.setValue("input/minRmsDb", minimumLevelDb_);
    settings.setValue("input/warningPeakDb", warningPeakDb_);
}
