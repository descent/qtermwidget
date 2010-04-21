/*
 *
 *
 * --style=windows this argument will show ms windows style
 */

#ifdef QTOPIA_APP
#include <qtopia/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#include "dirtree_view.h"

int main(int argc, char *argv[])
{
#ifdef QTOPIA_APP
  QPEApplication a( argc, argv );
#else
  QApplication a( argc, argv );
#endif


  //QSplitter *split = new QSplitter( );
  //DirTreeView *dirtree_view = new DirTreeView(split);
  //DirContent *dir_content=new DirContent(split);

  //DirTreeView *dirtree_view = new DirTreeView();
  //DirContent *dir_content=new DirContent();

  FileManager *fm=new FileManager ();
#ifdef QTOPIA_APP
  a.showMainWidget(fm);
  a.showMainWidget( fm );
#else
  a.setMainWidget(fm);
  fm->show();
#endif
  /*
  a.setMainWidget(fm);
  fm->show();
  */
  //a.showMainWidget( split );
  //a.showMainWidget( split );
  return a.exec();


}
