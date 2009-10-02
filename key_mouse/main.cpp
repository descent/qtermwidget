
#include <QtCore>
#include <QtGui>
#include <QApplication>

//#include "qtermwidget.h"
#include "config_widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ConfWidget cw;

    //mainWindow->setCentralWidget(console);
    //mainWin.setCentralWidget(console);

    //mainWindow->resize(802, 610);

    cw.resize(300,200);    
    cw.show();    
    return app.exec();
} 

  
