#include "file_list_widget.h"
#include "rename_dialog.h"

#include <QLocale>
#include <QTextCodec>

// for debug windows version
#include <QMessageBox>

#include <string>

QString myDecoderFunc(const QByteArray &localFileName)
{
  //qDebug("myDecoderFunc");
  return localFileName;
  //return QString::fromAscii(localFileName);
}

QByteArray myEncoderFunc(const QString &fileName)
{
  //qDebug("myEncoderFunc");
  return fileName.toAscii();
}


DirTreeView::DirTreeView( QWidget * parent ): TreeViewPopupMenu (parent)
{
  //QLocale::setDefault(QLocale::c() );
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
    fi->set_real_str(file_info_list.at(i).absoluteFilePath() );

#if QT_VERSION >= 0x40200
    //setSelected only in qt-4.2.0 
    if (i==0)
    {
      fi->setSelected(true);
    }
#endif
  }

  connect(this, SIGNAL(itemDoubleClicked ( QTreeWidgetItem * , int )), this, SLOT(expend_dirs( QTreeWidgetItem * , int )));

#if 0
  popup_menu_timer_ = new QTimer(this);
  popup_menu_timer_->setSingleShot(true);
  connect( popup_menu_timer_, SIGNAL( timeout() ), SLOT( show_fo_menu() ) );
#endif
  // don't encoding in QDir, QFile
  QFile::setDecodingFunction(myDecoderFunc);
  QFile::setEncodingFunction(myEncoderFunc);
  // This let QTextCodec use 8859-1 codec for anything
  // include QDir filename.
  //QTextCodec::setCodecForLocale(QTextCodec::codecForName("8859-1") );
 
  CREATE_ACTIONS(remove_, "remove", s_rm);

  ok_dialog_ = new RenameDialog(this);
  ok_dialog_ -> set_ok_dialog();

#ifdef MAC_ENC
  cur_enc_=UTF8;
  utf8_enc_->setChecked(true);
#else
  cur_enc_=BIG5;
  big5_enc_->setChecked(true);
#endif



}


// get full path
int DirTreeView::get_select_path(QString &path)
{
  QList<QTreeWidgetItem *> select_item = selectedItems ();

  // should only one item
  QTreeWidgetItem *it = select_item.at(0);
  FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(it);
  path=(fi->get_real_str() );

  while (it->parent())
  {
    it=it->parent();
    QString tmp_str=path; 
    fi = dynamic_cast<FileListWidgetItem*>(it);
    path= fi->get_real_str() + QDIR_SEPARATOR + tmp_str;
  }
  return 0;
}

//#define  DIR_NAME_DEG

// remove file include directory.
int remove_file(const QString &path)
{
  QDir dir(path);
  //dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
  //dir.setFilter(QDir::AllDirs | QDir::Hidden);
  dir.setFilter(QDir::AllEntries);

  QFileInfoList file_info_list = dir.entryInfoList();
  qDebug("file_info_list.size() : %d", file_info_list.size());
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
      {
        continue;
      }
      else
      {
        remove_file( path + QDir::separator() + file_info.fileName()); 
      }
      dir.rmdir( path + QDir::separator() + file_info.fileName()); 
    
    }
    else
    {
      qDebug("file_info.fileName(): %s", file_info.fileName().toAscii().data() );
      dir.remove(file_info.fileName() );    
    }
  }

  return 0;
  //qdir.rmpath(path);

}

void DirTreeView::s_rm()
{
  if (ok_dialog_->exec())
  {
    qDebug("ok");
  }
  else
  {
    qDebug("cancel");
    return;
  }


  QString path;

  get_select_path(path);
  qDebug("path: %s", path.toAscii().data()   );

  remove_file(path);
  QDir dir(path);
  dir.rmdir(path);
  QList<QTreeWidgetItem *> select_item = selectedItems ();

  // should only one item
  QTreeWidgetItem *it = select_item.at(0);
  delete it;

}

void DirTreeView::expend_dirs_unix( QTreeWidgetItem * item, int column ) 
{
}


void DirTreeView::expend_dirs( QTreeWidgetItem * item, int column ) 
{
  if (!item)
    return;
  if (item->childCount() != 0)
    return;

  QTreeWidgetItem *it=item;
  FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(it);
  QString dir_path(fi->get_real_str() );
  while (fi->parent())
  {
    fi=dynamic_cast<FileListWidgetItem*>(fi->parent() );
    QString tmp_str=dir_path; 
    dir_path= fi->get_real_str() + QDIR_SEPARATOR + tmp_str;
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
    
      fi = new FileListWidgetItem (item);

      fi->set_real_str(file_info.fileName() );

#if 1
      QTextCodec *tc;
      tc = QTextCodec::codecForName(get_cur_enc_str() );
      QString fn(tc->toUnicode(file_info.fileName().toAscii() ) );


      fi->setText(0, fn);
#else
      qDebug("get_cur_enc_str() : %s", get_cur_enc_str() );
      fi->setText(0, file_info.fileName() );
#endif
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
  //qDebug("in FileListWidgetItem::operator<");
  //FileListWidgetItem &item =dynamic_cast<FileListWidgetItem&>(other);
  FileListWidgetItem *item =(FileListWidgetItem *)(&other);
  // if the 2 items are all directories, or all not are directories,
  // compare the item->text(0)
  if ((is_dir() && item->is_dir()) || (!is_dir() && !item->is_dir()))
  {
    if (text(0) > item->text(0) )
      return true;
    else
      return false;
  }
  else  
  {
    if (is_dir() )
      return false; 
    else
      return true; 
  }
}

#ifdef Q_OS_WIN
#define ROOT_PATH ""
#else
#define ROOT_PATH (QDir::Path() )
#endif

FileList::FileList( QWidget * parent): TreeViewPopupMenu(parent)
{
  setRootIsDecorated(false);
  // for sort
  setSortingEnabled(true);


  qDebug("root cur_dir_ : %s", cur_dir_.toAscii().data() );
  //cur_dir_ = ROOT_PATH;

#ifdef Q_OS_WIN
  cur_dir_="";
#else
  cur_dir_=QDIR_SEPARATOR; 
#endif

  list_first_layer_dir();



#if 0
  for (int i = 0; i < file_info_list.size(); ++i) 
  {
    if ( file_info_list.at(i).fileName() =="." ||  file_info_list.at(i).fileName()=="..")
    {
      continue;
    }
 
    FileListWidgetItem *fi = new FileListWidgetItem (this);
    fi->setText(0, file_info_list.at(i).absoluteFilePath() );
    fi->set_real_str(file_info_list.at(i).absoluteFilePath() );
  }
#endif
  connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)) , this, SLOT(change_dir(QTreeWidgetItem *,int) ) );	

  change_title_to_cur_path();
  CREATE_ACTIONS(rename_, "rename", s_rename );
  CREATE_ACTIONS(cd_parent_dir_, "cd parent dir", s_cd_parent_dir);
}

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void FileList::list_first_layer_dir()
{
  QFileInfoList file_info_list;


  if (cur_dir_=="") // win 
  {
    file_info_list=QDir::drives();
    change_dir(file_info_list);
  }
  if (cur_dir_==QDIR_SEPARATOR) // unix 
  {
    QDir root_dir=QDir::root();
    root_dir.setFilter(QDir::AllEntries | QDir::AllDirs| QDir::System | QDir::Hidden);
    file_info_list = root_dir.entryInfoList();
    change_dir(cur_dir_.toStdString().c_str());
  }


}


void FileList::change_dir(const char *dir_path)
{
  std::string dir_p(dir_path);

  dir_p += QDir::separator().toAscii();
  DIR *d=opendir((dir_p).c_str() );
  if (!d) return;


  //int ret = QMessageBox::warning(this, "change_dir(const char *dir_path)", dir_path, QMessageBox::Save);
  clear();
  dirent *d_ent;
  while((d_ent=readdir(d)) )
  {
    if ( (strcmp(d_ent->d_name, ".")==0) ||  (strcmp(d_ent->d_name, "..")==0) )
      continue;
    struct stat statbuf;
	    
    QString fn=QString(dir_path) + QDir::separator() + QString(d_ent->d_name);

    int stat_err=0;
    if(stat(fn.toStdString().c_str(), &statbuf)==-1)
    {
      //printf("Get stat on %s Error:%s\n", filename,strerror(errno));
      //return;
      stat_err=1;
    } 

    FileListWidgetItem *fi = new FileListWidgetItem (this);

    QString qfn = QString(d_ent->d_name);

    // encoding the qfn
    QTextCodec *tc;
    tc = QTextCodec::codecForName(encoding_name[cur_enc_]);
    QString e_fn(tc->toUnicode(d_ent->d_name ) );

    if (stat_err==0)
    {
      if(S_ISDIR(statbuf.st_mode))
      {
        e_fn += QDir::separator();
        fi->set_dir(true);
      }
      else
      {
        fi->set_dir(false);
      }

    }
    else
    {
      QMessageBox::information(this, "stat err", d_ent->d_name);
    }




    fi->setText(0, e_fn);

    fi->set_real_str(d_ent->d_name);
  }

  closedir(d);

  // reset cur_dir_
  QString cd_dir_str(dir_path);

  if (cur_dir_=="") /* win , remove c:\'s \ */
  {
    cur_dir_ = cd_dir_str.left(cd_dir_str.length()-1);
  }
  else
  {
    cur_dir_ = cd_dir_str;
  }
  // cur_dir_ +=  QDir::separator();
  change_title_to_cur_path();
}



void FileList::change_dir(const QFileInfoList &file_info_list)
{
  clear();
  for (int i = 0; i < file_info_list.size(); ++i) 
  {
    if ( file_info_list.at(i).fileName() =="." || file_info_list.at(i).fileName() =="..")
    {
      continue;
    }
    FileListWidgetItem *fi = new FileListWidgetItem (this);
    QString drives="[";

    // if isDir put first, /etc will determine to dir
    if ( file_info_list.at(i).isSymLink())
    {
        fi->setText(0, file_info_list.at(i).fileName() + "@");
        //fi->setText(0, file_info_list.at(i).absoluteFilePath() + "/");
        //fi->setText(0, file_info_list.at(i).fileName());
    }
    else if ( file_info_list.at(i).isDir())
    {
      if (cur_dir_=="") // list win32 dirves
      {
        drives = file_info_list.at(i).absoluteFilePath();
        drives.remove(drives.length()-1,1); /* remove c:\'s \ */

        fi->setText(0, QString("[") + drives + "]");
      }
      else
      {
        fi->setText(0, file_info_list.at(i).fileName() + QDIR_SEPARATOR);
	fi->set_dir(true);
      }
    }
    else
    {
      fi->setText(0, file_info_list.at(i).fileName() );
    }

    if (cur_dir_=="") // list win32 dirves
    {
      fi->set_real_str(drives);
    }
    else
    {
      fi->set_real_str(file_info_list.at(i).fileName() );
    }





#if 0
    if ( file_info_list.at(i).fileName() =="..")
    {
      fi->setText(0, file_info_list.at(i).absoluteFilePath() + "%");
      fi->set_real_str(file_info_list.at(i).absoluteFilePath() );
    }
    else
    {
#ifndef Q_OS_WIN
      // if isDir put first, /etc will determine to dir
      if ( file_info_list.at(i).isSymLink())
      //if ( file_info_list.at(i).isDir())
      {
        fi->setText(0, file_info_list.at(i).fileName() + "@");
        //fi->setText(0, file_info_list.at(i).absoluteFilePath() + "/");
        //fi->setText(0, file_info_list.at(i).fileName());
      }
      else if ( file_info_list.at(i).isDir())
      //else if ( file_info_list.at(i).isSymLink())
      {
        fi->setText(0, file_info_list.at(i).fileName() + "/");
        //fi->setText(0, file_info_list.at(i).absoluteFilePath() + "/");
        //fi->setText(0, file_info_list.at(i).fileName());
      }
      else
      {
        fi->setText(0, file_info_list.at(i).fileName() );
      }
      fi->set_real_str(file_info_list.at(i).fileName() );
      //fi->set_real_str(file_info_list.at(i).absoluteFilePath() );
#else
      QString drive=file_info_list.at(i).absoluteFilePath() ;

      if (cur_dir_=="") /* win , remove c:\'s \ */
      {
        drive.remove(drive.length()-1,1); /* remove \ */
      }
      fi->setText(0, drive);
      fi->set_real_str(drive);
#endif
    }	
#endif
 
  } // end for (int i = 0; i < file_info_list.size(); ++i) 
  change_title_to_cur_path();
}

void FileList::change_dir(const QString &cd_dir_str)
{
  QDir cd_dir(cd_dir_str);

  if (!cd_dir.exists())
    return;
  

  // in mac osX, I need this fileter to show all files
  cd_dir.setFilter(QDir::AllEntries | QDir::AllDirs| QDir::System | QDir::Hidden);
  //cd_dir.setFilter(QDir::AllDirs| QDir::System | QDir::Hidden);
  QFileInfoList file_info_list = cd_dir.entryInfoList();

  // reset cur_dir_
  if (cur_dir_=="") /* win , remove c:\'s \ */
  {
    cur_dir_ = cd_dir_str.left(cd_dir_str.length()-1);
  }
  else
  {
    cur_dir_ = cd_dir_str;
  }

  change_dir(file_info_list);
  //change_title_to_cur_path();

#if 0
  clear();

  qDebug("cur_dir_ : %s ## cur_dir_ len: %d", cur_dir_.toAscii().data(), cur_dir_.length() );
  for (int i = 0; i < file_info_list.size(); ++i) 
  {
    //if ( file_info_list.at(i).fileName() =="." ||  file_info_list.at(i).fileName()=="..")
    if ( file_info_list.at(i).fileName() ==".")
    {
      continue;
    }
    FileListWidgetItem *fi = new FileListWidgetItem (this);
#if 0
    if ( file_info_list.at(i).fileName() =="..")
    {
      fi->setText(0, file_info_list.at(i).absoluteFilePath() + "%");
      fi->set_real_str(file_info_list.at(i).absoluteFilePath() );
    }
    else
#endif
    {
      // if isDir put first, /etc will determine to dir
      if ( file_info_list.at(i).isSymLink())
      //if ( file_info_list.at(i).isDir())
      {
        fi->setText(0, file_info_list.at(i).fileName() + "@");
        //fi->setText(0, file_info_list.at(i).absoluteFilePath() + "/");
        //fi->setText(0, file_info_list.at(i).fileName());
      }
      else if ( file_info_list.at(i).isDir())
      //else if ( file_info_list.at(i).isSymLink())
      {
        fi->setText(0, file_info_list.at(i).fileName() + "/");
        //fi->setText(0, file_info_list.at(i).absoluteFilePath() + "/");
        //fi->setText(0, file_info_list.at(i).fileName());
      }
      else
      {
        fi->setText(0, file_info_list.at(i).fileName() );
      }
      fi->set_real_str(file_info_list.at(i).fileName() );
      //fi->set_real_str(file_info_list.at(i).absoluteFilePath() );
    }	
 
  }

  if (cur_dir_=="") /* win , remove c:\'s \ */
  {
    cur_dir_ = cd_dir_str.left(cd_dir_str.length()-1);
  }
  else
  {
    cur_dir_ = cd_dir_str;
  }
  change_title_to_cur_path();
#endif
}

void FileList::change_dir( QTreeWidgetItem * item, int column )
{
  qDebug("in DirTreeView::change_dir");

  FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(item);
  //FileListWidgetItem *fi = (FileListWidgetItem*)(item);
  if (fi==0)
  {
    qDebug("fi==0");
    return;
  } 
  QString cd_dir;	


  //QString select_fn = fi->text(0);

  if (cur_dir_ == "") // in win32, this mean current is list all drives
  {
    cd_dir =  fi->get_real_str() + QDIR_SEPARATOR;
  }
  else
  {
    cd_dir =  cur_dir_ + QDIR_SEPARATOR + fi->get_real_str();
  }

  //change_dir(cd_dir);
  change_dir(cd_dir.toStdString().c_str() );
  //change_dir("c:\\");

  //change_dir(const QString &cd_dir)
}

void FileList::s_rename()
{
}

void get_parent_dir(const QString &cur_dir, QString &parent_dir)
{

  // find before last QDir::separator()
  int index = cur_dir.lastIndexOf(QDIR_SEPARATOR, cur_dir.length() - 1);
  qDebug("index : %d", index);
  //parent_dir=cur_dir.left(index+1);
  // in winsows, do with cur_dir == c: case
  if (index==-1)
    ++index;
  parent_dir=cur_dir.left(index);
}


void FileList::s_cd_parent_dir()
{
  QString parent_dir_str;
  
  get_parent_dir(cur_dir_, parent_dir_str);

  //int ret = QMessageBox::warning(this, "s_cd_parent_dir()", parent_dir_str, QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save); 

  qDebug("parent_dir_str : %s", parent_dir_str.toStdString().c_str() );

  if (parent_dir_str == "")
  {
    cur_dir_="";
    list_first_layer_dir();
  }
  else
  {
    change_dir(parent_dir_str.toStdString().c_str() );
  }
  //change_dir(cur_dir_ + QDIR_SEPARATOR +"..");
  change_title_to_cur_path();
  //QMessageBox::warning("s_cd_par");
}

void FileList::change_title_to_cur_path()
{
  //QStringList headers;
  //headers << ("");
  //setHeaderLabels(headers);
  //headers << (QString("filename") + " " + cur_dir_);
  setHeaderLabel(cur_dir_);
}

int FileList::cur_dir(QString &cur_dir) const
{
  return 0 ;
}
