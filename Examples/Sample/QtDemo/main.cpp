#include "mainwindow.h"
#include <Version.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#ifdef _WIN32
	w.setWindowTitle(QString("Ranger3Studio(SICK_GCN) - Windows_x64 - ") + SickCam::VER::version_number.c_str() + " - " + SickCam::VER::version_time.c_str());
#endif
#ifdef __linux__
#ifdef __arm__
	w.setWindowTitle(QString("Ranger3Studio(SICK_GCN) - Linux_aarch64 - ") + SickCam::VER::version_number.c_str() + " - " + SickCam::VER::version_time.c_str());
#else
    w.setWindowTitle(QString("Ranger3Studio(SICK_GCN) - Linux_x64 - ") + SickCam::VER::version_number.c_str() + " - " + SickCam::VER::version_time.c_str());
#endif
#endif
    w.show();
    return a.exec();
}
