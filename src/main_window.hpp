#pragma once

#include "audio_capture.hpp"
#include "tuner_widgets.hpp"

#include <QMainWindow>
#include <QMediaDevices>

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void buildInterface();
    void refreshInputDevices();
    void selectInput();
    void updateTuning();
    void showSettings();
    void handlePitch(double hz, double confidence);
    void handleLevel(double rmsDb, double peakDb, bool clipped);

    AudioCapture capture_;
    QMediaDevices mediaDevices_;
    QComboBox* profileChoice_ = nullptr;
    QComboBox* inputChoice_ = nullptr;
    QDoubleSpinBox* calibrationSpin_ = nullptr;
    QLabel* frequencyLabel_ = nullptr;
    QLabel* targetLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    AnalogGauge* gauge_ = nullptr;
    PitchScale* pitchScale_ = nullptr;
    InputLevelMeter* levelMeter_ = nullptr;
    const strings::TuningPreset* selected_ = nullptr;
    double detectedHz_ = 0.0;
    double minimumLevelDb_ = -45.0;
    double warningPeakDb_ = -3.0;
};
