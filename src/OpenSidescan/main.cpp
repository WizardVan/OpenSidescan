#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

//TODO: windows-proof this (for sleep())

#ifdef _WIN32
#include <windows.h>
#define sleep Sleep
#else
#include <unistd.h>
#endif

const char * captions[] = {
    "Obtaining Poseidon's blessing...",
    "Initializing Queequeg engine...",
    "Summoning great white whale...",
    "Reticulating splines...",
    "Cleaning up the oceans...",
    "Launching asynchronous dolphin call...",
    "Parity-checking barnacles...",
    "Removing fishbones from backbone...",
    "Bootstrapping pacemaker with magnetometer...",
    "Hiring Captain Ahab...",
    "Ph'nglui mglw'nafh Cthulhu R'lyeh wgah'nagl fhtagn",
    "Warming up planet Earth...",
    "Increasing sunspot activity...",
    "Waking up Cthulhu...",
    "Recalibrating lobster traps...",
    "Freeing up memory from lost fishing nets..."
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Show splash
    srand(time(NULL));
    QPixmap splashImage (":/Images/resources/splash.jpg");
    QSplashScreen splash(splashImage);

    splash.show();
    sleep(1);

    for(int i=0;i<3;i++){
        splash.showMessage(captions[rand()%14],Qt::AlignHCenter|Qt::AlignBottom,QColor("white"));
        sleep(1);
    }

    a.processEvents();

    sleep(1);

    //Show main window
    MainWindow w;
    w.showFullScreen();

    return a.exec();
}