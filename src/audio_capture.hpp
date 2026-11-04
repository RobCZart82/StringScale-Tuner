#pragma once

#include "pitch_detector.hpp"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSource>
#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <memory>
#include <vector>

class QIODevice;

// Audio input and own pitch tracking. All read() and DSP work takes place on
// the Qt GUI thread in v0.1, without a blocking callback on the audio thread.
// A DSP worker is planned for later if profiling shows UI contention.
class AudioCapture : public QObject {
    Q_OBJECT
public:
    explicit AudioCapture(QObject* parent = nullptr);
    ~AudioCapture() override;

    bool start(const QAudioDevice& device, int channel = 0);
    bool setInputChannel(int channel);
    int inputChannel() const { return inputChannel_; }
    int channelCount() const { return source_ ? format_.channelCount() : 0; }
    void stop();
    void setMinimumLevelDb(double db) { minimumLevelDb_ = db; }
    void setWarningPeakDb(double db) { warningPeakDb_ = db; }
    double minimumLevelDb() const { return minimumLevelDb_; }
    double warningPeakDb() const { return warningPeakDb_; }

signals:
    void levelReady(double rmsDb, double peakDb, bool clipped);
    void pitchReady(double frequencyHz, double confidence);
    void errorOccurred(const QString& explanation);

private slots:
    void consumeAudio();
    void updateMeasurements();

private:
    friend class AudioCaptureTest;
    void processBytes(const QByteArray& bytes, qint64 nowMs);
    void updateMeasurementsAt(qint64 nowMs);
    void clearPitch();
    void rejectPitch(qint64 nowMs);
    void acceptSample(float sample);
    void resetAnalysis();
    std::unique_ptr<QAudioSource> source_;
    QIODevice* input_ = nullptr; // Owned by QAudioSource.
    QAudioFormat format_;
    QByteArray partialFrame_;
    int inputChannel_ = 0;
    std::vector<float> channelSamples_;
    QTimer measureTimer_;
    QElapsedTimer clock_;
    qint64 lastAudioMs_ = -1;
    qint64 lastPitchMs_ = -1;
    bool gateOpen_ = false;
    static constexpr qint64 kInputTimeoutMs = 250;
    static constexpr qint64 kPitchHoldMs = 220;
    strings::PitchDetector detector_;
    std::vector<float> pitchRing_;
    std::size_t ringWrite_ = 0;
    std::size_t ringCount_ = 0;
    double lowpass_ = 0.0;
    double resamplePhase_ = 0.0;
    double sumSquares_ = 0.0;
    std::size_t blockSamples_ = 0;
    double peak_ = 0.0;
    bool clipping_ = false;
    double minimumLevelDb_ = -45.0;
    double warningPeakDb_ = -3.0;
    double smoothedHz_ = 0.0;
    int framesSinceLastEstimate_ = 0;
    static constexpr int kAnalysisRate = 12000;
    static constexpr int kWindowSamples = 4096;
};
