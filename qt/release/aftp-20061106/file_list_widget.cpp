#include "file_list_widget.h"

#include <QDir>

#ifdef UNIX_PATH
const char PATH_DELIMIT[]="/";
#endif

#ifdef WIN_PATH
const char PATH_DELIMIT[]="\\";
#endif

DirTreeView::DirTreeView( QWidget * parent ):QTreeWidget (parent)
{
  //FileListWidgetItem *fi = new FileListWidgetItem (this);
  QStringList headers;
  headers << tr("dirname");
  setHeaderLabels(headers );
  //fi->setText(0, QDir::rootPath() );

  QFileInfoList file_info_list=QDir::drives();
  qDebug("file_info_list.size(): %d", file_info_list.size());
  for (int i=0 ; i < file_info_list.size() ; ++i)
  {
    FileListWidgetItem *fi = new FileListWidgetItem (this);
    qDebug("file_info_list.at(i).fileName(): %s", file_info_list.at(i).absoluteFilePath().toAscii().data() );
    //absoluteFilePath();
    fi->setText(0, file_info_list.at(i).absoluteFilePath() );
    //setSelected only in qt-4.2.0 
    if (i==0)
    {
      fi->setSelected(true);
    }

  }

  connect(this, SIGNAL(itemDoubleClicked ( QTreeWidgetItem * , int )), this, SLOT(expend_dirs( QTreeWidgetItem * , int )));
}

// get full path
int DirTreeView::get_select_path(QString &path)
{
  QList<QTreeWidgetItem *> select_item = selectedItems ();

  // should only one item
  QTreeWidgetItem *it = select_item.at(0);
  path=(it->text(0) );

  while (it->parent())
  {
    it=it->parent();
    QString tmp_str=path; 
    path= it->text(0) + PATH_DELIMIT + tmp_str;
  }
  return 0;
}

void DirTreeView::expend_dirs( QTreeWidgetItem * item, int column ) 
{
  if (!item)
    return;
  if (item->childCount() != 0)
    return;

  QTreeWidgetItem *it=item;
  QString dir_path(item->text(0) );
  while (it->parent())
  {
    it=it->parent();
    QString tmp_str=dir_path; 
    dir_path= it->text(0) + PATH_DELIMIT + tmp_str;
  }
  QDir dir(dir_path);
  //dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
  dir.setFilter(QDir::AllDirs | QDir::Hidden);
  QFileInfoList file_info_list = dir.entryInfoList();
  for (int i = 0; i < file_info_list.size(); ++i) 
  {
    QFileInfo file_info = file_info_list.at(i);
#ifdef DIR_NAME_DEG
    qDebug("file_info.fileName() : %s", file_info.fileName().toAscii().data()  );
    qDebug("file_info.isDir() : %d", file_info.isDir() );
#endif
    if (file_info.isDir() )
    {
      if ( file_info.fileName() =="." ||  file_info.fileName()=="..")
        continue;
      FileListWidgetItem *fi = new FileListWidgetItem (item);
      fi->setText(0, file_info.fileName() ); 
    }
  }

}

#if 0
FileListWidgetItem::FileListWidgetItem( QListWidget * parent = 0, int type = Type )
{
}
#endif

bool FileListWidgetItem::operator< ( const QTreeWidgetItem & other ) const
    {
      qDebug("in FileListWidgetItem::operator<");
#if 0
      //FileListWidgetItem &item =dynamic_cast<FileListWidgetItem&>(other);
      FileListWidgetItem *item =(FileListWidgetItem *)(&other);
      if (is_dir() && item->is_dir() )
      {
        if (text() > item->text() )
	{
          return true;
	}
	else
	{
          return false;
	}
      }
      else 
        if (is_dir() )
        {
          return true; 
	}
        else
	{
          return false; 
	}
#endif
    }
