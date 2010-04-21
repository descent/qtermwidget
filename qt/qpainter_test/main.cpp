
#include "TEWidget.h"
#include <QtGui>


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  TEWidget konsole;
  konsole.show();
  return app.exec();
}

