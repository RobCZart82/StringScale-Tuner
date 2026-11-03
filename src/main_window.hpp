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
    explicit MainWindow(QWidget* parent = nullptr, bool preview = false);

public:
    void renderPreview(bool dark, double hz);
private:
    void applyTheme(bool dark);
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
    double calibration_ = 440.0;
    bool dark_ = false;
    bool preview_ = false;

    QLabel* statusLabel_ = nullptr;
    AnalogGauge* gauge_ = nullptr;
    PitchScale* pitchScale_ = nullptr;
    InputLevelMeter* levelMeter_ = nullptr;
    const strings::TuningPreset* selected_ = nullptr;
    double detectedHz_ = 0.0;
    double minimumLevelDb_ = -45.0;
    double warningPeakDb_ = -3.0;
};
