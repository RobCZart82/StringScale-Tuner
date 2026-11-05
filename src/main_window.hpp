#pragma once

#include "audio_capture.hpp"
#include "tuner_widgets.hpp"

#include <QMainWindow>
#include <QMediaDevices>
#include <QPointer>
class QDialog;
class QShowEvent;
class QAction;

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr, bool preview = false);

    void renderPreview(bool dark, double hz);
    QDialog* openSettings(bool audio = false);
    QDialog* openAbout();
signals:
    void inputChannelsChanged();
    void scaleAvailabilityChanged();
protected:
    void showEvent(QShowEvent* event) override;
private:
    void applyTheme(bool dark);
    void applyUiPercent(int percent, bool persist = false);
    QSize availableUiSize() const;
    void watchScreen();
    int preferredPercent_ = 125;
    bool watchingScreen_ = false;
    QMetaObject::Connection geometryConnection_;
    void retranslate();
    void refreshStatus();
    void chooseInputChannel(int channel);
    void buildInterface();
    void refreshInputDevices();
    void selectInput(bool persistDevice = true);
    void updateTuning();
    void handlePitch(double hz, double confidence);
    void handleLevel(double rmsDb, double peakDb, bool clipped);

    AudioCapture capture_;
    QMediaDevices mediaDevices_;
    QComboBox* profileChoice_ = nullptr;
    QComboBox* inputChoice_ = nullptr;
    double calibration_ = 440.0;
    bool dark_ = false;
    bool preview_ = false;
    int inputChannel_ = 0;
    int channelCount_ = 0;

    enum class Status { Listening, NoInput, Denied, Error, Preview };
    Status status_ = Status::Listening;
    QString errorKey_;
    QPointer<QDialog> settingsDialog_, audioDialog_, aboutDialog_;
    QAction* settingsAction_ = nullptr;
    QAction* audioAction_ = nullptr;
    QAction* aboutAction_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    AnalogGauge* gauge_ = nullptr;
    PitchScale* pitchScale_ = nullptr;
    InputLevelMeter* levelMeter_ = nullptr;
    const strings::TuningPreset* selected_ = nullptr;
    double detectedHz_ = 0.0;
    double minimumLevelDb_ = -45.0;
    double warningPeakDb_ = -3.0;
};

