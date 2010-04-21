#include "qt_key.h"

#include <qapplication.h>

int main(int argc, char *argv[])
{
  QApplication a( argc, argv );

  QtKey *qt_key = new QtKey();
  a.setMainWidget(qt_key);
  qt_key->show();
  return a.exec();


}
