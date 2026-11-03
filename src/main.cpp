#include "main_window.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("StringScale Tuner");
    QCoreApplication::setOrganizationName("RobCZart");
    QCoreApplication::setApplicationVersion("0.1.0");
    app.setStyle("Fusion");
    QFont font = app.font();
    font.setPointSize(10);
    app.setFont(font);
    MainWindow window;
    window.show();
    return app.exec();
}
