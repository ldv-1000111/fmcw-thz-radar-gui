#include <QApplication>
#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FMCW THz Radar Simulator");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("ldv-1000111");

    MainWindow w;
    w.show();

    return app.exec();
}
