#include "ftp_bookmark.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif


namespace
{
  const QString FTP_BOOKMARK_FNAME=".aftp_bookmark";
  const QString FTP_BOOKMARK_PATH=QDir::homePath() + QDir::separator() + FTP_BOOKMARK_FNAME;
}


FtpBookmark::FtpBookmark(QWidget * parent, Qt::WindowFlags f):QFrame(parent, f)//, ftp_bookmark_(QDir::homePath() + QDir::separator() + FTP_BOOKMARK_FNAME)
{

  ftp_site_ = new QTreeWidget();


  // read file data
  ftp_bookmark_.open(FTP_BOOKMARK_PATH.toStdString().c_str(), std::ios::in | std::ios::out);
  //ftp_bookmark_.open((QDir::homePath() + QDir::separator() + FTP_BOOKMARK_FNAME).toStdString().c_str(), std::ios::in | std::ios::app) ;

  //ftp_bookmark_.open((QDir::homePath() + QDir::separator() + FTP_BOOKMARK_FNAME).toStdString().c_str(), std::ios::in | std::ios::out | std::ios::trunc) ;
  if (!ftp_bookmark_) 
  { // create a file
    qDebug("create a file");
    ftp_bookmark_.open(FTP_BOOKMARK_PATH.toStdString().c_str(), std::ios::in | std::ios::out | std::ios::trunc) ;
  }
  else
  { // read data from file
    string line;

    while (!ftp_bookmark_.eof() )
    {
      FtpSiteItem *item;

      if (getline(ftp_bookmark_, line) )
      {
        item = new FtpSiteItem(ftp_site_);
        item->setText(0, line.c_str() );
      }
      else
      {
        break;
      }

      if (getline(ftp_bookmark_, line) )
      {
        item->set_ftp_addr(line.c_str() );
      }
      else
      {
        break;
      }

      if (getline(ftp_bookmark_, line) )
      {
        item->set_ftp_port(line.c_str() );
      }
      else
      {
        break;
      }

      if (getline(ftp_bookmark_, line) )
      {
        item->set_ftp_username(line.c_str() );
      }
      else
      {
        break;
      }


      if (getline(ftp_bookmark_, line) )
      {
        item->set_ftp_password(line.c_str() );
      }
      else
      {
        break;
      }

    }
  }



#if 0
  if (!ftp_bookmark_.exists())
  {
    ftp_bookmark_.open(QIODevice::ReadWrite);
  }
#endif

  ftp_site_label_  = new QLabel(tr("ftp site name:"));
  ftp_site_line_edit_ = new QLineEdit();

  ftp_port_  = new QLabel(tr("port:"));
  ftp_port_line_edit_ = new QLineEdit();

  ftp_username_ = new QLabel(tr("username:"));
  ftp_username_line_edit_ = new QLineEdit();

  ftp_password_ = new QLabel(tr("password:"));
  ftp_password_line_edit_ = new QLineEdit();

  ftp_addr_ = new QLabel(tr("ftp address:"));
  ftp_addr_line_edit_ = new QLineEdit(); 

  //ftp_addr_line_edit_ ->setText("1.2.3.4");

  //ftp_addr_->setBuddy(ftp_addr_line_edit_);

  QVBoxLayout *frame_layout = new QVBoxLayout(this);

  QHBoxLayout *main_layout = new QHBoxLayout();

  QVBoxLayout *field_layout = new QVBoxLayout();

  QHBoxLayout *field_layout0 = new QHBoxLayout();
  QHBoxLayout *field_layout1 = new QHBoxLayout();
  QHBoxLayout *field_layout2 = new QHBoxLayout();
  QHBoxLayout *field_layout3 = new QHBoxLayout();
  QHBoxLayout *field_layout4 = new QHBoxLayout();

  field_layout->addLayout(field_layout0);
  field_layout->addLayout(field_layout1);
  field_layout->addLayout(field_layout2);
  field_layout->addLayout(field_layout3);
  field_layout->addLayout(field_layout4);

  field_layout0->addWidget(ftp_site_label_);
  field_layout0->addWidget(ftp_site_line_edit_);

  field_layout1->addWidget(ftp_addr_);
  field_layout1->addWidget(ftp_addr_line_edit_);

  field_layout2->addWidget(ftp_port_);
  field_layout2->addWidget(ftp_port_line_edit_);

  field_layout3->addWidget(ftp_username_);
  field_layout3->addWidget(ftp_username_line_edit_);

  field_layout4->addWidget(ftp_password_);
  field_layout4->addWidget(ftp_password_line_edit_);




  QStringList headers;
  headers << tr("Ftp Site Name");
  ftp_site_->setHeaderLabels(headers);


  QHBoxLayout *button_layout = new QHBoxLayout();

  frame_layout->addLayout(main_layout);
  frame_layout->addLayout(button_layout);

  add_button_ = new QPushButton(tr("add"));
  edit_button_  = new QPushButton(tr("edit"));
  del_button_ = new QPushButton(tr("del"));
  connect_button_ = new QPushButton(tr("connect"));
  button_layout->addWidget(add_button_);
  button_layout->addWidget(edit_button_);
  button_layout->addWidget(del_button_);
  button_layout->addWidget(connect_button_);

  main_layout->addWidget(ftp_site_);
  main_layout->addLayout(field_layout);

  //FtpSiteItem *item = new FtpSiteItem(ftp_site_);
  //item->setText(0, "text");

  setWindowTitle(tr("bookmark"));


  // connect button slot/single

  connect(add_button_, SIGNAL(clicked() ), this, SLOT(s_add_bookmark()));
  connect(edit_button_, SIGNAL(clicked() ), this, SLOT(s_edit_bookmark()));
  connect(del_button_, SIGNAL(clicked() ), this, SLOT(s_del_bookmark()));
  connect(ftp_site_, SIGNAL(itemClicked( QTreeWidgetItem *, int) ), this, SLOT(s_show_info(QTreeWidgetItem *, int)));
  connect(connect_button_, SIGNAL(clicked() ), this, SLOT(s_connect()));
}


#if 1
FtpBookmark::~FtpBookmark()
{
  write_bookmark_to_file();
}
#endif

void FtpBookmark::s_connect()
{
  QList<QTreeWidgetItem *> select_items=ftp_site_->selectedItems();

  for (int i = 0; i < select_items.size(); ++i) 
  {
    emit connect_to_site(dynamic_cast<FtpSiteItem *>(select_items[0]) );
    break;
  }
  setHidden(true);
}

void FtpBookmark::s_show_info( QTreeWidgetItem * item, int column )
{
  if (!item) return;

  FtpSiteItem *i= dynamic_cast<FtpSiteItem *>(item);
  ftp_site_line_edit_->setText(i->text(0) );
  ftp_addr_line_edit_->setText(i->get_ftp_addr() );
  ftp_port_line_edit_->setText(i->get_ftp_port() );
  ftp_username_line_edit_->setText(i->get_ftp_username() );
  ftp_password_line_edit_->setText(i->get_ftp_password() );
// *ftp_addr_line_edit_, *ftp_port_line_edit_, *ftp_username_line_edit_, *ftp_password_line_edit_;

}

#ifdef Q_OS_WIN
#include <QMessageBox>

// copy from qfsfileengine_win.cpp
int truncate_win32(const char *fn, int len)
{
  QFile::resize (fn, len);
#if 0
  int accessRights = GENERIC_READ;
  DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
  SECURITY_ATTRIBUTES securityAtts = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };

  // WriteOnly can create files, ReadOnly cannot.
  DWORD creationDisp = OPEN_ALWAYS; //OPEN_EXISTING;
  


  HANDLE fileHandle = CreateFileA(fn , accessRights, shareMode, &securityAtts, creationDisp, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fileHandle  == INVALID_HANDLE_VALUE)
  {
    int ret = QMessageBox::warning(0, "truncate_win32", "win32", QMessageBox::Save);
    return -1;
  }
  //DWORD newFilePointer = 
  if (SetFilePointer(fileHandle, len, NULL, FILE_BEGIN) && SetEndOfFile(fileHandle) )
  {
    int ret = QMessageBox::warning(0, "truncate_win32", "success", QMessageBox::Save);
  }
  CloseHandle(fileHandle);
#endif
}
#endif


int FtpBookmark::write_bookmark_to_file()
{
  // because open file maybe get bad state,
  // so call clear() member function.
  ftp_bookmark_.clear();


  ftp_bookmark_.seekg(0, std::ios::beg);
  if (!ftp_bookmark_)
  {
    qDebug("seekp error");;
  }

  QTreeWidgetItemIterator it(ftp_site_);
  while (*it) 
  {
    FtpSiteItem *item = dynamic_cast<FtpSiteItem *>(*it);
    ftp_bookmark_ << item->text(0).toStdString() << endl;
    ftp_bookmark_ << item->get_ftp_addr().toStdString() << endl;
    ftp_bookmark_ << item->get_ftp_port().toStdString() << endl;
    ftp_bookmark_ << item->get_ftp_username().toStdString() << endl;
    ftp_bookmark_ << item->get_ftp_password().toStdString() << endl;
    ++it;
  }
  int size_len=ftp_bookmark_.tellg();
  ftp_bookmark_.close();
#ifdef Q_OS_WIN
  truncate_win32(FTP_BOOKMARK_PATH.toStdString().c_str(), size_len);
#else
  truncate(FTP_BOOKMARK_PATH.toStdString().c_str(), size_len); // in linux and mac osX
#endif

  return 0;
}

void FtpBookmark::s_del_bookmark()
{
  QList<QTreeWidgetItem *> select_items=ftp_site_->selectedItems();
  for (int i = 0; i < select_items.size(); ++i) 
  {
    delete select_items[i];
  }
}

// only edit the first item
void FtpBookmark::s_edit_bookmark()
{
  QList<QTreeWidgetItem *> select_items=ftp_site_->selectedItems();
  for (int i = 0; i < select_items.size(); ++i) 
  {
    FtpSiteItem *i= dynamic_cast<FtpSiteItem *>(select_items[0]);
    i->setText(0, ftp_site_line_edit_->text() );
    i->set_ftp_addr(ftp_addr_line_edit_->text());
    i->set_ftp_port(ftp_port_line_edit_->text());
    i->set_ftp_username(ftp_username_line_edit_->text());
    i->set_ftp_password(ftp_password_line_edit_->text());
  }
}

void FtpBookmark::s_add_bookmark()
{
  FtpSiteItem *item = new FtpSiteItem(ftp_site_);

  item->setText(0, ftp_site_line_edit_->text() );
  item->set_ftp_addr(ftp_addr_line_edit_->text() );
  item->set_ftp_port(ftp_port_line_edit_->text() );
  item->set_ftp_username(ftp_username_line_edit_->text() );
  item->set_ftp_password(ftp_password_line_edit_->text() );
#if 0
    QList<QTreeWidgetItem *> select_items=ftp_site_->selectedItems();

    for (int i = 0; i < select_items.size(); ++i) 
    {
      ftp_bookmark_ << select_items[0]->text(0).toStdString() << endl;
    }
    ftp_bookmark_ << ftp_addr_line_edit_->text().toStdString() << endl;
    ftp_bookmark_ << ftp_port_line_edit_->text().toStdString() << endl;
    ftp_bookmark_ << ftp_username_line_edit_->text().toStdString() << endl;
    ftp_bookmark_ << ftp_password_line_edit_->text().toStdString() << endl;
#endif
}



FtpSiteItem::FtpSiteItem(QTreeWidget *parent):QTreeWidgetItem(parent)
{
}

