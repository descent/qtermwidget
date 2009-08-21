
#include <QtCore>
#include <QtGui>
#include <QApplication>

#include "main_window.h"

#include <QLabel>


int main(int argc, char *argv[])
{
#if 1
    //Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    MainWindow *mainWindow = new MainWindow();
    QLabel *l = new QLabel();

    mainWindow->resize(802, 610);

    mainWindow->show();    
    return app.exec();
#else
    QApplication app(argc, argv);
    MainWindow mainWin;
    QLabel *l = new QLabel();
    QTermWidget *console = new QTermWidget(1, &mainWin);
    mainWin.setCentralWidget(console);
    //mainWin.setCentralWidget(l);
    mainWin.show();
    return app.exec();
#endif
} 

  
