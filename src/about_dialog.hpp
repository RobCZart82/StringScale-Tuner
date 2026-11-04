#pragma once
#include <QDialog>
class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr);
protected:
    void changeEvent(QEvent* event) override;
private:
    void refreshProjectLink();
};
