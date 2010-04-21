#include <qapplication.h>

#include "screen_widget.h"

int main (int argc, char **argv)
{
  //ODevice::inst ( )-> setDisplayStatus ( false );


  QApplication a(argc,argv);

  DS::Light *light = new DS::Light(0);



  a.setMainWidget(light);
  light->show();


  /*

  on = true;

  sleep(5);
  switch_screen(on);
  */



  return a.exec();
}
