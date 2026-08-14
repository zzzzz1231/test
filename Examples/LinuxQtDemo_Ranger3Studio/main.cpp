#include "mainwindow.h"
#include <Version.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowTitle("Ranger3Studio(SICK_GCN) - Linux_X64 - " + QString(SickCam::VER::version_number.c_str()));
    w.show();
    return a.exec();
}
