/*! \mainpage Filemanager by Descent
 *  \section intro_sec ²��
 *  �ѩ�b linux/x window ���S���ںD�Ϊ� filemanager,
 *  �ҥH�ڴN�ۤv�g�F�@�ӡC
 *  \section intro_arch �ؿ���ܬy�{
 *
  connect(this, SIGNAL( doubleClicked ( QListViewItem * ) ), this, SLOT(cd_another_dir(QListViewItem *) ));

  QObject::connect(dir_content_, SIGNAL(open_new_dir(FOListView *) ), dirtree_view_ , SLOT(add_child_item(FOListView *) ));

  QObject::connect(dir_content_, SIGNAL(cd_new_dir(const std::string &) ), dirtree_view_ , SLOT(mark_cur_item(const std::string &) ));

  QObject::connect(dirtree_view_, SIGNAL(clicked ( QListViewItem *) ), dir_content_, SLOT(open_another_dir(QListViewItem *) ));
  connect(dirtree_view_,SIGNAL(currentChanged ( QListViewItem *)), dir_content_, SLOT(open_another_dir(QListViewItem *) ) );

  DirTreeView class �e�X clicked ( QListViewItem *), 
  currentChanged ( QListViewItem * ) �o��� signal ��,
  �K�|�s���� DirTreeView open_another_dir(QListViewItem *) �o�� slot

  QObject::connect(dirtree_view_, SIGNAL(signal_reload ( QListViewItem *) ), dir_content_, SLOT(slot_reload(QListViewItem *) ));

  QObject::connect(dir_content_, SIGNAL(signal_reload(FOListView *) ), dirtree_view_, SLOT(slot_reload(FOListView *) ));


  connect(dir_content_, SIGNAL( opendir ( const std::string &) ), this, SLOT(change_dirtreeview_item(const std::string & )) );
 */

// -style=windows this argument will show ms windows style

#ifdef QTOPIA_APP
#include <qtopia/qpeapplication.h>
#else
#include <qapplication.h>
#include <qwindowsstyle.h>
#endif

#include "dirtree_view.h"

int main(int argc, char *argv[])
{
#ifdef QTOPIA_APP
  QPEApplication a( argc, argv );
#else
  QApplication a( argc, argv );
  QStyle * windows=new QWindowsStyle();
  a.setStyle(windows);
#endif


  //QSplitter *split = new QSplitter( );
  //DirTreeView *dirtree_view = new DirTreeView(split);
  //DirContent *dir_content=new DirContent(split);

  //DirTreeView *dirtree_view = new DirTreeView();
  //DirContent *dir_content=new DirContent();

  FileManager *fm=new FileManager ();
#ifdef QTOPIA_APP
  a.showMainWidget(fm);
  //a.showMainWidget( fm );
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
