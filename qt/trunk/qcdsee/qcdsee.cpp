
#include <qapplication.h>

#include "main_win.h"

int main(int argc, char *argv[])
{
  QApplication qapp(argc, argv);
  DS::MainWin *main_win = new DS::MainWin();
  qapp.setMainWidget(main_win);
  main_win->show();
  qapp.exec();
  return 0;
}
