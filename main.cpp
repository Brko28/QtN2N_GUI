#include "mainwindow.h"

#include <QApplication>
#include <singleapplication.h>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(icons);

    SingleApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    QApplication::setApplicationName("{QtN2N GUI}");
    QApplication::setApplicationVersion("{1.0.0.0}");

    MainWindow window;

    QObject::connect(&app, &SingleApplication::instanceStarted, [ &window ]() {window.raise();window.activateWindow();});
    QObject::connect(&app, &SingleApplication::instanceStarted, [ &window ]() {window.raise();window.show();});

    qDebug() << "App path : " << qApp->applicationDirPath();

    window.show();
    return app.exec();
}
