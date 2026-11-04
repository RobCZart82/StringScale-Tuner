#pragma once
#include <QObject>
#include <QString>

class UiLanguage : public QObject {
    Q_OBJECT
public:
    static UiLanguage& instance();
    QString code() const { return code_; }
    void setCode(const QString& code, bool persist = true);
    static QString savedCode();
signals:
    void changed();
private:
    UiLanguage();
    QString code_;
};
QString uiText(const char* hungarian, const char* english);
