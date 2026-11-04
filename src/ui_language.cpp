#include "ui_language.hpp"
#include <QLocale>
#include <QSettings>

QString UiLanguage::savedCode() {
    const QString fallback = QLocale::system().language() == QLocale::Hungarian ? "hu" : "en";
    const auto saved = QSettings().value("appearance/language", fallback).toString();
    return saved == "hu" || saved == "en" ? saved : fallback;
}
UiLanguage::UiLanguage() : code_(savedCode()) {}
UiLanguage& UiLanguage::instance() { static UiLanguage language; return language; }
void UiLanguage::setCode(const QString& code, bool persist) {
    if (code != "hu" && code != "en") return;
    if (persist) QSettings().setValue("appearance/language", code);
    if (code_ == code) return;
    code_ = code;
    emit changed();
}
QString uiText(const char* hu, const char* en) {
    return QString::fromUtf8(UiLanguage::instance().code() == "hu" ? hu : en);
}
