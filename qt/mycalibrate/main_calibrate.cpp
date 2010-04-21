#include "calibrate.h"
#include <qapplication.h>

void main(int argc,char **argv)
{
 QApplication myapp(argc,argv);

 QTranslator translator(0);
 translator.load("calibrate_tw",".");
 myapp.installTranslator(&translator);

 Calibrate *mycalibrate=new Calibrate;
 myapp.setMainWidget(mycalibrate);
 mycalibrate->show();
 return myapp.exec();


}
