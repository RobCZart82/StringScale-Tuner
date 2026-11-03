#pragma once

#include "tuning.hpp"

#include <QWidget>

class AnalogGauge : public QWidget {
    Q_OBJECT
public:
    explicit AnalogGauge(QWidget* parent = nullptr);
    void setCents(double cents, bool audibleTarget);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    double cents_ = 0.0;
    bool valid_ = false;
};

class PitchScale : public QWidget {
    Q_OBJECT
public:
    explicit PitchScale(QWidget* parent = nullptr);
    void setPreset(const strings::TuningPreset* preset, double calibration);
    void setFrequency(double frequencyHz);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    const strings::TuningPreset* preset_ = nullptr;
    double calibration_ = 440.0;
    double frequencyHz_ = 0.0;
};

class InputLevelMeter : public QWidget {
    Q_OBJECT
public:
    explicit InputLevelMeter(QWidget* parent = nullptr);
    void setLevel(double rmsDb, double peakDb, bool clipped);
    void setLimits(double minDb, double warningDb);
protected:
    void paintEvent(QPaintEvent*) override;
private:
    double rmsDb_ = -120;
    double peakDb_ = -120;
    double minDb_ = -45;
    double warningDb_ = -3;
    bool clipped_ = false;
};
