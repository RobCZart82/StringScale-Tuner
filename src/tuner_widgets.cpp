#include "tuner_widgets.hpp"

#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>
#include <cmath>

namespace {
const QColor ink(236, 242, 250);
const QColor muted(133, 153, 178);
const QColor line(76, 95, 120);
const QColor cyan(79, 202, 236);
const QColor red(239, 67, 69);
const QColor background(19, 32, 49);
constexpr double pi = 3.14159265358979323846;

void paintPanel(QPainter& p, const QRect& rect) {
    p.setPen(QPen(QColor(55, 74, 96), 1));
    p.setBrush(background);
    p.drawRoundedRect(rect.adjusted(1, 1, -2, -2), 13, 13);
}

QPointF positionOnArc(QPointF origin, double radius, double cents) {
    const double theta = cents / 50.0 * 69.0 * pi / 180.0;
    return {origin.x() + radius * std::sin(theta),
            origin.y() - radius * std::cos(theta)};
}

void centerText(QPainter& painter, const QRectF& rect,
                const QString& text, const QColor& color, int size, bool bold = false) {
    QFont f = painter.font();
    f.setPointSize(size);
    f.setBold(bold);
    painter.setFont(f);
    painter.setPen(color);
    painter.drawText(rect, Qt::AlignCenter, text);
}
}

AnalogGauge::AnalogGauge(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(235);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void AnalogGauge::setCents(double cents, bool audibleTarget) {
    cents_ = std::clamp(cents, -50.0, 50.0);
    valid_ = audibleTarget;
    update();
}

void AnalogGauge::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    paintPanel(p, rect());
    const QPointF origin(width() / 2.0, height() * 0.84);
    const double radius = std::min(width() * 0.40, height() * 0.66);
    centerText(p, QRectF(0, 10, width(), 27), "FINE TUNER  ·  CENTS", muted, 11, true);

    QPainterPath arc;
    for (int c = -50; c <= 50; c += 2) {
        const auto pt = positionOnArc(origin, radius, c);
        if (c == -50) arc.moveTo(pt); else arc.lineTo(pt);
    }
    p.setPen(QPen(line, 3));
    p.setBrush(Qt::NoBrush);
    p.drawPath(arc);

    for (int c = -50; c <= 50; c += 5) {
        const bool major = c % 25 == 0;
        const QPointF inner = positionOnArc(origin, radius - (major ? 16 : 9), c);
        const QPointF outer = positionOnArc(origin, radius + 2, c);
        p.setPen(QPen(c == 0 ? cyan : ink, major ? 2.8 : 1.3));
        p.drawLine(inner, outer);
        if (major) {
            const QPointF text = positionOnArc(origin, radius + 27, c);
            centerText(p, QRectF(text.x() - 25, text.y() - 14, 50, 25),
                       c > 0 ? QString("+%1").arg(c) : QString::number(c),
                       c == 0 ? cyan : ink, 11, c == 0);
        }
    }

    // Never present a centered 'in tune' needle without a valid pitch.
    if (valid_) {
        const QPointF tip = positionOnArc(origin, radius - 26, cents_);
        const QColor needle = std::abs(cents_) <= 2.0 ? QColor(55, 205, 149) : red;
        p.setPen(QPen(needle, 3.2, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(origin, tip);
        p.setBrush(needle);
        p.setPen(Qt::NoPen);
        p.drawEllipse(origin, 7.0, 7.0);
        const QString value = QString("%1%2 ¢")
                                  .arg(cents_ > 0 ? "+" : "")
                                  .arg(cents_, 0, 'f', 1);
        centerText(p, QRectF(width() * .33, height() * .83, width() * .34, 35),
                   value, needle, 15, true);
    } else {
        centerText(p, QRectF(width() * .25, height() * .82, width() * .5, 38),
                   "—", muted, 18);
    }
}

PitchScale::PitchScale(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(152);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void PitchScale::setPreset(const strings::TuningPreset* preset, double calibration) {
    preset_ = preset;
    calibration_ = calibration;
    update();
}

void PitchScale::setFrequency(double frequencyHz) {
    frequencyHz_ = frequencyHz;
    update();
}

void PitchScale::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    paintPanel(p, rect());
    centerText(p, QRectF(0, 7, width(), 23),
               "ABSOLUTE PITCH  ·  STRING MAP", muted, 10, true);
    if (!preset_ || preset_->targets.empty()) return;

    const double low = preset_->targets.front().hz * calibration_ / 440.0;
    const double high = preset_->targets.back().hz * calibration_ / 440.0;
    // Include an extra octave on either side: an incorrectly tuned string
    // must still appear visibly away from its reference position.
    const double minHz = low / 2.0;
    const double maxHz = high * 2.0;
    const double left = 42.0;
    const double right = width() - 42.0;
    const double y = height() * 0.55;
    const auto xForHz = [&](double hz) {
        return left + std::log2(hz / minHz) /
                          std::log2(maxHz / minHz) * (right - left);
    };

    p.setPen(QPen(line, 4.0, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(left, y), QPointF(right, y));
    const int n = static_cast<int>(preset_->targets.size());
    for (int i = 0; i < n; ++i) {
        const auto& target = preset_->targets[i];
        const double x = xForHz(target.hz * calibration_ / 440.0);
        p.setPen(QPen(ink, 1.8));
        p.drawLine(QPointF(x, y - 11), QPointF(x, y + 11));
        centerText(p, QRectF(x - 21, y - 48, 42, 30),
                   QString::number(target.number), ink, 17, true);
        centerText(p, QRectF(x - 28, y + 14, 56, 26),
                   QString::fromStdString(target.note), muted, 12);
    }

    if (frequencyHz_ > 0 && frequencyHz_ >= minHz && frequencyHz_ <= maxHz) {
        const double x = xForHz(frequencyHz_);
        p.setPen(QPen(QColor(255, 239, 238), 2.0));
        p.setBrush(red);
        p.drawEllipse(QPointF(x, y), 9.0, 9.0); // ON the scale, not beside it.
    }
}

InputLevelMeter::InputLevelMeter(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(100);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void InputLevelMeter::setLevel(double rmsDb, double peakDb, bool clipped) {
    rmsDb_ = rmsDb;
    peakDb_ = peakDb;
    clipped_ = clipped;
    update();
}

void InputLevelMeter::setLimits(double minDb, double warningDb) {
    minDb_ = minDb;
    warningDb_ = warningDb;
    update();
}

void InputLevelMeter::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    paintPanel(p, rect());
    const double x0 = 28;
    const double x1 = width() - 28;
    const QRectF bar(x0, 40, x1-x0, 19);
    const auto xDb = [&](double db) {
        return x0 + std::clamp((db + 70.0) / 70.0, 0.0, 1.0) * (x1-x0);
    };
    const bool tooLow = rmsDb_ < minDb_;
    const bool tooHigh = clipped_ || peakDb_ >= warningDb_;
    const QString status = tooHigh ? "REDUCE INPUT" :
                           tooLow ? "LOW SIGNAL" : "SIGNAL OK";
    const QColor statusColor = tooLow ? muted : tooHigh ? red : QColor(55, 205, 149);
    centerText(p, QRectF(x0, 8, 130, 23), "INPUT LEVEL", ink, 10, true);
    centerText(p, QRectF(x1-170, 8, 170, 23), status, statusColor, 10, true);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(59, 75, 95));
    p.drawRoundedRect(bar, 4, 4);
    p.setBrush(QColor(55, 205, 149));
    p.drawRect(QRectF(xDb(minDb_), bar.top(),
                      std::max(0.0, xDb(warningDb_)-xDb(minDb_)), bar.height()));
    p.setBrush(red);
    p.drawRect(QRectF(xDb(warningDb_), bar.top(),
                      xDb(0.0)-xDb(warningDb_), bar.height()));
    p.setBrush(QColor(79, 202, 236, 185));
    p.drawRect(QRectF(bar.left(), bar.top(), std::max(0.0, xDb(rmsDb_)-bar.left()), bar.height()));
    p.setPen(QPen(ink, 2));
    const double xp = xDb(peakDb_);
    p.drawLine(QPointF(xp, bar.top()-2), QPointF(xp, bar.bottom()+2));
    centerText(p, QRectF(x0, 68, 100, 20), "−70 dBFS", muted, 9);
    centerText(p, QRectF(x1-100, 68, 100, 20), "0 dBFS", muted, 9);
    centerText(p, QRectF(width()/2.0-120, 68, 240, 20),
               QString("RMS %1  ·  Peak %2 dBFS")
                   .arg(rmsDb_, 0, 'f', 0).arg(peakDb_, 0, 'f', 0), ink, 9);
}
