#pragma once
#include <QObject>
#include <QSize>
#include <QString>
class QWidget;

// User size presets operate in Qt logical pixels, independently of OS DPI.
class UiScale final : public QObject {
    Q_OBJECT
public:
    static UiScale& instance();
    static int savedPercent();
    static int fitPercent(int requested, QSize available);
    static QSize fitWindowSize(int percent, QSize available);
    int percent() const { return percent_; }
    double factor() const { return percent_ / 125.0; }
    int px(int value) const;
    QString styleSheet(const QString& base) const;
    void setPercent(int percent, bool persist = false);
    void prepare(QWidget* root, QSize preferred);
signals:
    void changed(int percent);
private:
    void apply(QWidget* root);
    int percent_ = 125;
};

