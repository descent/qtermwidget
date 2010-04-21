/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <cstdio>

#include <QDir>

#include <QtGui>
#include <QtNetwork>
#include <QAction>

#include "ftp_bookmark.h"

#include <errno.h>
//#include "/usr/local/include/iconv.h"


#include "ftp_transfer.h"
#include "file_list_widget.h"
#include "ftpwindow.h"
#include "ftp_server_list.h"

#define OUTBUF_SIZE	32768
char convert_str[OUTBUF_SIZE];
int convert_str_len=0;

const int BIG5=0;
const int UTF8=1;

static unsigned int line;
static unsigned int column;
static int discard_unconvertible = 0;
static int silent = 0;

#define ADD_ACTIONS(addobj, qa, qa_title, slot_func) \
{ \
  qa = new QAction(tr(qa_title), this);\
  addobj->addAction(qa);\
  connect(qa, SIGNAL(triggered()), this, SLOT(slot_func() ) );\
}


//#define DEBUG_SERVER_PATH

//const char *encoding_name[]={"BIG5", "UTF-8"};

int process_block (iconv_t cd, ICONV_CONST char *addr, size_t len);
int convert(iconv_t cd, ICONV_CONST char *str, int len);

int str_encoding(const char *enc_name, const char *org_str, QString &qstr)
{
  QTextCodec *tc;
  tc = QTextCodec::codecForName(enc_name);

  if (tc==0)
  {
    return -1;
  }

  qstr=(tc->toUnicode(org_str) );
  // need delete  tc ??
  //delete tc;
  return 0;
}

// org_dir : /usr/local/ => /usr/
int get_parent_path(const QString &org_dir, QString &parent_dir)
{
  parent_dir  = org_dir.left(org_dir.lastIndexOf('/'));
  if (parent_dir.isEmpty()) 
  {
    parent_dir="/";
  }
  return 0;
}

const char *qstr_to_c_str(const QString &qstr)
{
  return qstr.toStdString().c_str() ;
}

//FtpWindow::FtpWindow(QWidget *parent) : QDialog(parent), ftp(0)

//FtpWindow::FtpWindow ( QWidget * parent, Qt::WindowFlags flags ):QMainWindow (parent, flags), ftp(0)

//FtpWindow::FtpWindow(QWidget *parent) : QDialog(parent), ftp(0)

void layout_widget_set_hidden(QLayout *l, bool b)
{
  for (int i=0 ; i < l->count() ; ++i)
  {
    QWidget *w= l->itemAt(i)->widget();
    if (b==true)
      w->setHidden(b);
    else
      w->show();
  }
}


int qstr_to_hexstr(const QString &qstr, std::string &str)
{
  const char hexchar[]="0123456789abcdef";

  for (int i=0 ; i < qstr.length() ; ++i)
  {
    char c=qstr[i].toAscii();
    str.push_back(hexchar[((c >> 4) & 0xf)]);
    str.push_back(hexchar[c & 0xf]);
    str.push_back(' ');
  }
  return 0;
}


FtpWindow::FtpWindow( QWidget * parent , Qt::WindowFlags f ): QFrame ( parent, f), ftp(0)
{
#if 0
  std::string hexstr;
  qstr_to_hexstr("AB", hexstr);
  QMessageBox::information(this, tr("FTP"), hexstr.c_str());
#endif

#if 0
    iconv_t cd;
    cd = iconv_open("UTF-8", "BIG5");
    //cd = iconv_open("ISO-8859-1", "UTF-8");

    if (cd==(iconv_t)-1)
    {
      qDebug("cd==-1");
      exit(-1);
    }
    ICONV_CONST char c[]="你";
    //ICONV_CONST char *c="ab";

    int ret=process_block(cd, c, 2);
    qDebug("convert_str: %s", convert_str);
    qDebug("convert_str: %x %x %x", convert_str[0], convert_str[1], convert_str[2]);
    qDebug("ret : %d", ret);

    iconv_close(cd);

    file = new QFile(convert_str);
    exit(-1);

#endif



  ftp_bookmark_ = new FtpBookmark();



  list_time_ = 0;
  for (int i=0 ; i < TC_NUM ; ++i)
  {
    tc_[i]=0;
  }
  ftp_status_ = new QTextEdit;
  ftp_status_->setReadOnly(true);
  //ftp_status_->setText("test\nasb");

  splitter_ = new QSplitter;
  vertical_splitter_ = new QSplitter(Qt::Vertical);
  vertical_splitter_->addWidget(ftp_status_);
  vertical_splitter_->addWidget(splitter_);

  //connect(this, SIGNAL(reload(const QUrlInfo &)), this, SLOT(addToList(const QUrlInfo &)));
  //connect(this, SIGNAL(s_begin_download()), this, SLOT(begin_download() ));

    server_cur_enc_ = BIG5;
    client_cur_enc_ = BIG5;

  //iconv_t cd;
    cd_ = iconv_open (encoding_name[client_cur_enc_], encoding_name[server_cur_enc_]);
  if (cd_==(iconv_t)-1)
  {
    qDebug("cd==-1");
    cd_=(iconv_t)-1;
  }

    ftpServerLabel = new QLabel(tr("Ftp &server:"));
    //ftpServerLineEdit = new QLineEdit("ftp.trolltech.com");
    //ftpServerLineEdit = new QLineEdit("172.20.1.150");


    ftpServerLineEdit = new QLineEdit();
    user_name_ = new QLineEdit();
    password_ = new QLineEdit();


    user_name_lable_ = new QLabel(tr("username:") ); 
    password_lable_ = new QLabel(tr("password:") );
    port_label_ = new QLabel(tr("port:") );
    port_edit_ = new QLineEdit();
    //user_name_ = new QLineEdit("starftp");
    //password_ = new QLineEdit("stevengene");
    ftpServerLabel->setBuddy(ftpServerLineEdit);
    user_name_lable_->setBuddy(user_name_);
    password_lable_->setBuddy(password_);

    statusLabel = new QLabel(tr("Please enter the name of an FTP server."));

    file_list = new FtpServerList;
    file_list -> set_ftp_win(this);

    // for sort
    file_list->setSortingEnabled(true);
    qDebug("file_list->isSortingEnabled : %d", file_list->isSortingEnabled());

    transfer_status_ = new FtpTransfer;
    transfer_status_ -> set_ftp_win(this);
    connect(this, SIGNAL(get_complete() ), transfer_status_, SLOT(s_download() ) );
    file_list->set_ftp_transfer(transfer_status_);
    vertical_splitter_->addWidget(transfer_status_);
    local_dir_view_ = new DirTreeView;

    local_file_view_ = new FileList;

    tree_ = new QTreeView;
    dir_model_ = new QDirModel;
    tree_->setModel(dir_model_);
    //tree->setRootIndex(model->index(QDir::currentPath()));
    //tree_->setRootIndex(model_->index(QDir::rootPath()));
    splitter_->addWidget(file_list);
    splitter_->addWidget(local_dir_view_);
    splitter_->addWidget(local_file_view_);
   


    //QModelIndex current = model->modelIndexList.at(i);
    //QModelIndex index = tree->indexFromItem( item, 0);
     

    // ref from Qt-4.2.0/examples/network/torrent/mainwindow.cpp	
    QStringList headers, transfer_headers, local_dir_header;

    headers << tr("filename") << tr("size");
    //transfer_headers << tr("filename") << tr("status");
    file_list->setHeaderLabels(headers);
    file_list->setRootIsDecorated(false);
    file_list->setEnabled(false);

    //local_dir_header << tr("dirname");
    //local_dir_view_->setHeaderLabels(local_dir_header);
    //FileListWidgetItem *fi = new FileListWidgetItem(local_dir_view_, QTreeWidgetItem::UserType);
    //fi->setText(0, QDir::rootPath() );
    
    //transfer_status_->setHeaderLabels(transfer_headers);
    //transfer_status_->setRootIsDecorated(false);
    //transfer_status_->setEnabled(false);


    //fileList = new QListWidget;
  //fileList->setSelectionMode(QAbstractItemView::MultiSelection);
  //file_list->setSelectionMode(QAbstractItemView::MultiSelection);
  file_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //fileList->setEnabled(false);

    connectButton = new QPushButton(tr("Connect"));
    connectButton->setDefault(true);
    
    //downloadButton = new QPushButton(tr("Download"));
    //downloadButton->setEnabled(false);

    cdToParentButton = new QPushButton;
    cdToParentButton->setIcon(QPixmap(":/images/cdtoparent.png"));
    cdToParentButton->setEnabled(false);

    //quitButton = new QPushButton(tr("Quit"));

    progressDialog = new QProgressDialog(this);

    connect(file_list, SIGNAL(itemDoubleClicked(QTreeWidgetItem * , int )), this, SLOT(processItem(QTreeWidgetItem *, int)));
    //connect(fileList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(processItem(QListWidgetItem *)));
    //connect(fileList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(enableDownloadButton()));
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectOrDisconnect()));
    connect(cdToParentButton, SIGNAL(clicked()), this, SLOT(cdToParent()));
    //connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadFile()));
    //connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));


// add menubar and menu, if no menubar will get strang result
    menubar_ = new QMenuBar(); 
    setting_menu_ = new QMenu("&Settings");
    encoding_menu_ = new QMenu("Select Encoding");


// create bookmark menu
    bookmark_menu_ = new QMenu("&bookmark");
    //add_bookmark_ = new QAction(this);
    //edit_bookmark_ = new QAction(this);
    ADD_ACTIONS(bookmark_menu_, add_bookmark_, "add bookmark", s_add_bookmark)
    ADD_ACTIONS(bookmark_menu_, edit_bookmark_, "edit bookmark", s_edit_bookmark)

// create window menu
  window_menu_ = new QMenu("&window");

  ADD_ACTIONS(window_menu_, connect_field_win_, "connect fields", s_connect_field_show)
  ADD_ACTIONS(window_menu_, ftp_status_win_, "ftp status", s_ftp_status_win_show)
  ADD_ACTIONS(window_menu_, local_dir_tree_win_, "local dir tree", s_local_dir_tree_win_show)
  ADD_ACTIONS(window_menu_, local_file_list_win_, "local file list", s_local_file_list_win_show)
  ADD_ACTIONS(window_menu_, ftp_server_file_list_win_, "ftp server file list", s_ftp_server_file_list_win_show)
  ADD_ACTIONS(window_menu_, transfer_status_win_, "ftp transfer status", s_transfer_status_win_show)



  connect_field_win_ ->setCheckable(true);
  local_dir_tree_win_ ->setCheckable(true);
  local_file_list_win_ ->setCheckable(true);
  ftp_server_file_list_win_ ->setCheckable(true);
  ftp_status_win_ ->setCheckable(true);
  transfer_status_win_ ->setCheckable(true);

  connect_field_win_ ->setChecked(true);
  local_dir_tree_win_ ->setChecked(true);
  local_file_list_win_ ->setChecked(true);
  ftp_server_file_list_win_ ->setChecked(true);
  ftp_status_win_ ->setChecked(true);
  transfer_status_win_ ->setChecked(true);






    select_mode_ = new QMenu("Select Mode");

    single_selection_ = new QAction(tr("Single"), this); 
    contiguous_selection_ = new QAction(tr("Contiguous"), this);
    extended_selection_ = new QAction(tr("Extended"), this);
    multi_selection_ = new QAction(tr("Multi"), this);
    no_selection_ = new QAction(tr("No"), this);

    single_selection_->setCheckable(true);
    contiguous_selection_->setCheckable(true);
    extended_selection_->setCheckable(true);
    multi_selection_->setCheckable(true);
    no_selection_->setCheckable(true);

    extended_selection_->setChecked(true);

    select_mode_group_ = new QActionGroup(this);

    select_mode_group_->addAction(single_selection_);
    select_mode_group_->addAction(contiguous_selection_);
    select_mode_group_->addAction(extended_selection_);
    select_mode_group_->addAction(multi_selection_);
    select_mode_group_->addAction(no_selection_);

    setting_menu_->addMenu(select_mode_);

    select_mode_->addAction(single_selection_);
    select_mode_->addAction(contiguous_selection_);
    select_mode_->addAction(extended_selection_);
    select_mode_->addAction(multi_selection_);
    select_mode_->addAction(no_selection_);

    connect(single_selection_, SIGNAL(triggered()), this, SLOT(single_selection()));
    connect(contiguous_selection_, SIGNAL(triggered()), this, SLOT(contiguous_selection()));
    connect(extended_selection_, SIGNAL(triggered()), this, SLOT(extended_selection()));
    connect(multi_selection_, SIGNAL(triggered()), this, SLOT(multi_selection()));
    connect(no_selection_, SIGNAL(triggered()), this, SLOT(no_selection()));



    setting_menu_->addMenu(encoding_menu_);

    transfermode_ = new QMenu("Select Transfermode");

    menubar_->addMenu(setting_menu_);
    menubar_->addMenu(bookmark_menu_);
    menubar_->addMenu(window_menu_);
    setting_menu_->addMenu(transfermode_);

    server_enc_ = new QActionGroup(this);
    client_enc_ = new QActionGroup(this);
    transfermode_group_ = new QActionGroup(this);


    //connect(fileList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
    //server_enc_->setVisible(true);
    //server_enc_->setExclusive(true);
    //client_enc_->setVisible(true);
    //client_enc_->setExclusive(true);
    //select_encoding_ = new QAction(tr("select encoding"), this);
    server_big5_enc_ = new QAction(tr("server big5"), this);
    server_big5_enc_->setCheckable(true);
    //connect(server_big5_enc_, SIGNAL(triggered()), this, SLOT(server_big5()));

    server_utf8_enc_ = new QAction(tr("server utf8"), this);
    server_utf8_enc_->setCheckable(true);
    //connect(server_utf8_enc_, SIGNAL(triggered()), this, SLOT(server_utf8()));

    client_big5_enc_ = new QAction(tr("local big5"), this);
    client_big5_enc_->setCheckable(true);
    connect(client_big5_enc_, SIGNAL(triggered()), this, SLOT(client_big5()));

    client_utf8_enc_ = new QAction(tr("local utf8"), this);
    client_utf8_enc_->setCheckable(true);
    connect(client_utf8_enc_, SIGNAL(triggered()), this, SLOT(client_utf8()));



    transfermode_passive_ = new QAction(tr("passive"), this);
    transfermode_passive_->setCheckable(true);
    transfermode_active_ = new QAction(tr("active"), this);
    transfermode_active_->setCheckable(true);

    transfermode_->addAction(transfermode_passive_);
    transfermode_->addAction(transfermode_active_);

    transfermode_group_->addAction(transfermode_passive_);
    transfermode_group_->addAction(transfermode_active_);

    transfermode_passive_->setChecked(true);

    connect(transfermode_passive_, SIGNAL(triggered()), this, SLOT(set_transfer_passive_mode()));
    connect(transfermode_active_, SIGNAL(triggered()), this, SLOT(set_transfer_active_mode()));


    encoding_menu_->addSeparator()->setText("Server encoding");
    encoding_menu_->addAction(server_big5_enc_);
    encoding_menu_->addAction(server_utf8_enc_);
    encoding_menu_->addSeparator()->setText("Client encoding");
    encoding_menu_->addAction(client_big5_enc_);
    encoding_menu_->addAction(client_utf8_enc_);


    //server_big5_enc_->setEnabled(true);
    //client_big5_enc_->setEnabled(true);
    server_big5_enc_->setChecked(true);
    client_big5_enc_->setChecked(true);

    server_enc_->addAction(server_big5_enc_);
    server_enc_->addAction(server_utf8_enc_);
    client_enc_->addAction(client_big5_enc_);
    client_enc_->addAction(client_utf8_enc_);

    QHBoxLayout *menu_layout = new QHBoxLayout;
    menu_layout->addWidget(menubar_);

    echo_lable_ = new QLabel(tr("hidden password:"));
    echo_password_ = new QCheckBox();
    connect(echo_password_, SIGNAL(stateChanged(int)), this, SLOT(tag_echo_password(int)));

    topLayout = new QHBoxLayout;
    topLayout->addWidget(ftpServerLabel);
    topLayout->addWidget(ftpServerLineEdit);
    topLayout->addWidget(cdToParentButton);
    topLayout->addWidget(port_label_);
    topLayout->addWidget(port_edit_);
    topLayout->addWidget(user_name_lable_);
    topLayout->addWidget(user_name_);
    topLayout->addWidget(password_lable_);
    topLayout->addWidget(password_);
    topLayout->addWidget(echo_lable_);
    topLayout->addWidget(echo_password_);
    topLayout->addWidget(connectButton);
    
    //QHBoxLayout *buttonLayout = new QHBoxLayout;
    //buttonLayout->addStretch(1);
    //buttonLayout->addWidget(downloadButton);
    //buttonLayout->addWidget(quitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(menu_layout);
    mainLayout->addLayout(topLayout);
    //mainLayout->addWidget(fileList);
    //mainLayout->addWidget(file_list);
    //mainLayout->addWidget(splitter_);
    mainLayout->addWidget(vertical_splitter_);

    //mainLayout->addWidget(transfer_status_);
    //mainLayout->addWidget(statusLabel);
    //mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("FTP"));

  connect(ftp_bookmark_, SIGNAL(connect_to_site(const FtpSiteItem *) ), this, SLOT(s_connect(const FtpSiteItem *)) );

  // init timer for show throughput
  show_throughput_timer_ = new QTimer(this);
  throughput_counter_ = 0;
  //show_throughput_timer_ -> setSingleShot(true);
  connect( show_throughput_timer_, SIGNAL( timeout() ), this, SLOT( show_throughput() ) );

}

FtpWindow::~FtpWindow()
{
  iconv_close(cd_);
  delete ftp_bookmark_;
}

// generate win display code, hidden or show the specify window

#define GEN_WIN_DISPLAY(memberf, qact, win) \
void FtpWindow::memberf() \
{ \
  if (qact ->isChecked() ) \
  { \
    win ->show(); \
  } \
  else \
  { \
    win ->setHidden(true); \
  } \
}

GEN_WIN_DISPLAY(s_local_dir_tree_win_show, local_dir_tree_win_, local_dir_view_) 
GEN_WIN_DISPLAY(s_local_file_list_win_show, local_file_list_win_, local_file_view_) 
GEN_WIN_DISPLAY(s_ftp_server_file_list_win_show, ftp_server_file_list_win_, file_list) 
GEN_WIN_DISPLAY(s_ftp_status_win_show, ftp_status_win_, ftp_status_)
GEN_WIN_DISPLAY(s_transfer_status_win_show, transfer_status_win_, transfer_status_)

void FtpWindow::s_connect_field_show()
{
  if ( connect_field_win_ ->isChecked() ) 
  { 
    layout_widget_set_hidden(topLayout, false);
  } 
  else 
  { 
    layout_widget_set_hidden(topLayout, true);
  } 
}

bool FtpWindow::is_connect()
{
  return ftp;
}

void FtpWindow::s_add_bookmark()
{
  // check 2 state
  // 1. connect to a ftp site
  // 2. not connect to a ftp site
  if (is_connect() == true)
  { // direct add current ftp url to bookmark list
    //qDebug("connect");
    ftp_bookmark_ -> set_ftp_addr_le(ftpServerLineEdit->text() );
    ftp_bookmark_ -> set_ftp_port_le(port_edit_->text());
    ftp_bookmark_ -> set_ftp_username_le(user_name_->text());
    ftp_bookmark_ -> set_ftp_password_le(password_->text());
  }
  else 
  { // show ftp url to bookmark list, let user to add
    //qDebug("no connect");
  }
  ftp_bookmark_ -> show();
}
void FtpWindow::s_edit_bookmark()
{
    ftp_bookmark_ -> show();
    //ftp_bookmark_ -> add_bookmark();
}

#if 0
int FtpWindow::begin_download()
{
  //qDebug("in FtpWindow::begin_download");
  QTreeWidgetItemIterator it(transfer_status_);
  if (*it)
  {
    FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>(*it);
    //qDebug("fi->get_real_str(): %s", qstr_to_c_str(fi->get_real_str()) );

    store_file_to_local(fi->get_real_str() );
  }
  return 0;
}
#endif


// click connect/disconnect button will run this function
void FtpWindow::connectOrDisconnect()
{

  if (ftp) 
  {
    disconnect();
    return;
  }
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    ftp = new QFtp(this);
    transfer_status_->set_ftp(ftp);
    if (transfermode_passive_->isChecked() )
    {
      ftp->setTransferMode(QFtp::Passive);
    }
    else
    {
      ftp->setTransferMode(QFtp::Active);
    }

    connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(s_command_started(int)));

    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(state_change(int)));
    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(addToList(const QUrlInfo &)));

    connect(ftp, SIGNAL(rawCommandReply(int , const QString & )), this, SLOT(do_with_raw_command(int , const QString &) ));

    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(updateDataTransferProgress(qint64, qint64)));

    ftp->connectToHost(ftpServerLineEdit->text().trimmed(), port_edit_->text().toInt() );

    //statusLabel->setText( "string" + user_name_->text() + " " + password_->text());
    //statusLabel->setText( user_name_->text() );
    //sleep(10);
    if (user_name_->text().isEmpty() && password_->text().isEmpty() )
    {
      qDebug("anonymouse");
      ftp->login();
    }
    else
    {
      ftp->login(user_name_->text(), password_->text() );
    }
    operator_mode_ = LIST_MODE;
    list_1_=ftp->list(); // list current directory
    ftp->rawCommand("PWD");
    //list_2_=ftp->list("/"); // list root directory
    //ftp->login();
    //ftp->list();

    //fileList->setEnabled(true);
    file_list->setEnabled(true);
    connectButton->setText(tr("Disconnect"));
    statusLabel->setText(tr("Connecting to FTP server %1...") .arg(ftpServerLineEdit->text()));
}

#if 0
int FtpWindow::get_transfer_file(const QString &cur_path, const TransferFn &fn)
{
  TransferFn::const_iterator it=fn.begin();
  for ( ; it != fn.end() ; ++it)
  {
    if (it->second == true) // is directory
    {
      int cmd_id = ftp->list(cur_path + "/" + it->first);
      cmd_id_cur_path_[cmd_id]=cur_path + "/" + it->first;
      //qDebug("fn : %s/", qstr_to_c_str(cur_path + "/" + (it->first)) );
    }
    else
    {
      //qDebug("fn : %s", qstr_to_c_str(cur_path + "/" + (it->first)) );
    }
  }
  return 0;
}
#endif

void FtpWindow::downloadFile()
{
  operator_mode_ = DOWNLOAD_MODE;
  //transfer_status_->clear();
  transfer_dir_fn_.clear();

  //QTreeWidgetItemIterator it(file_list);
  selected_item_ = file_list->selectedItems();
  current_item_index_ = selected_item_.size();
  bool has_dir=false;

  QString tmp_cur_path;
  for (int i=0 ; i < selected_item_.size() ; ++i)
  {
    FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>((selected_item_.at(i)) );
    QString name = fi->get_real_str();
    tmp_cur_path = currentPath + "/" + name;
    if (fi->is_dir() )
      has_dir=true;

    get_file(tmp_cur_path, fi->is_dir(), fi->text(1) );
    //get_file(name, fi->is_dir(), fi->text(1) );

  }
  qDebug("begin down");

  if (has_dir==false) // no dir
  {
    //qDebug("has_dir==false emit s_begin_download");
    //make_transfer_dir();
    QItemSelectionModel *itemSelectionModel = tree_->selectionModel();
    QModelIndexList modelIndexList = itemSelectionModel->selectedIndexes();
    qDebug("modelIndexList.size() : %d", modelIndexList.size());

    for (int i=0; i < modelIndexList.size();i++)  
    {
      qDebug("select path: %s", qstr_to_c_str(dir_model_->filePath(modelIndexList.at(i) ) ) );
    }	
    //begin_download();
  }



  return ;

}

void FtpWindow::s_connect(const FtpSiteItem *item)
{
  ftpServerLineEdit->setText(item->get_ftp_addr() );
  user_name_ ->setText(item->get_ftp_username() );
  password_ ->setText(item->get_ftp_password() );
  port_edit_ ->setText(item->get_ftp_port() );
  connectOrDisconnect();
}

void FtpWindow::cancelDownload()
{
    ftp->abort();
}

void FtpWindow::s_command_started ( int id )
{
  qDebug("in FtpWindow::s_command_started ## id : %d", id);
}

void FtpWindow::ftpCommandFinished(int commandId, bool error)
{
  switch (ftp->currentCommand() )
  {
    case QFtp::None:
    {
      qDebug("00");
      qDebug("ftp->currentCommand() == QFtp::None");
      break;
    }
    case QFtp::SetTransferMode:
    {
      QString transfer_mode;
      if (transfermode_passive_->isChecked() )
      {
        transfer_mode="passive";
      }
      else
      {
        transfer_mode="active";
      }

      if (error)
      {
        ftp_status_->append("set transfer mode to " + transfer_mode + " fail: " + ftp->errorString());
      }
      else
      {
        ftp_status_->append("set transfer mode to " + transfer_mode);
      }
      break;
    }
    case QFtp::SetProxy:
    {
      qDebug("02");
      break;
    }
    case QFtp::ConnectToHost:
    {
      if (error)
      {
        ftp_status_->append("connect to " + ftpServerLineEdit->text() + " port : " +  port_edit_->text() + " fail:" + ftp->errorString() );
      }
      else
      {
        ftp_status_->append("connect to " + ftpServerLineEdit->text() + " port : " +  port_edit_->text());
      }
      //qDebug("03 error: %d", error);
        //statusLabel->setText((ftp->errorString ()));
      break;
    }
    case QFtp::Login:
    {
      qDebug("04 error: %d", error);
      //if (error)
        //statusLabel->setText((ftp->errorString ()));
      qDebug("ftp->state() : %d", ftp->state());	
      qDebug("ftp->hasPendingCommands (): %d", ftp->hasPendingCommands ());

      if (error==0)
        ftp_status_->append("login sucess");
      else
        ftp_status_->append("login fail: " + ftp->errorString() );


      break;
    }
    case QFtp::Close:
    {
      qDebug("05");
      break;
    }
    case QFtp::List:
    {
      qDebug("06");
      if (error)
        statusLabel->setText((ftp->errorString ()));
      if  (commandId == list_1_)
      {
        qDebug("list_1_");
      }
      if  (commandId == list_2_)
      {
        qDebug("list_2_");
      }
      break;
    }
    case QFtp::Cd:
    {
      qDebug("07");
      // in QT 4.1.2 put in processItem funcion will get error message:
      // ASSERT failure in QVector<T>::at: "index out of range", file ../../include/QtCore/../../src/corelib/tools/qvector.h, line 211
      // but in 4.2.0 it is ok.
      // I think don't do clear() in QTreeWidget itemDoubleClicked slot function
      if (operator_mode_ == LIST_MODE)
      {
        file_list->clear();
        ftp_status_->append("change server current path: " + currentPath);
      }
      break;
    }
    case QFtp::Get:
    {
      qDebug("08");
      show_throughput_timer_ -> stop();

      QString size;

      size.setNum(cur_size_); 
      FileListWidgetItem *fi = transfer_status_->get_cur_download_item();
      fi->setText(2, size);
      break;
    }
    case QFtp::Put:
    {
      qDebug("09");
      break;
    }
    case QFtp::Remove:
    {
      qDebug("010");
      break;
    }
    case QFtp::Mkdir:
    {
      qDebug("011");
      break;
    }
    case QFtp::Rmdir:
    {
      qDebug("012");
      break;
    }
    case QFtp::Rename:
    {
      qDebug("013");
      break;
    }
    case QFtp::RawCommand:
    {
      qDebug("014");
      //qDebug("pwd_cmd_id_: error: %d", error);
      //statusLabel->setText((ftp->errorString ()));
      break;
      break;
    }
  }

    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct.")
                                     .arg(ftpServerLineEdit->text()));
            return;
        }

        statusLabel->setText(tr("Logged onto %1.")
                             .arg(ftpServerLineEdit->text()));
        //fileList->setFocus();
        //downloadButton->setDefault(true);
        return;
    }

    if (ftp->currentCommand() == QFtp::Get) 
    {
        QApplication::restoreOverrideCursor();
        if (error) {
            statusLabel->setText(tr("Canceled download of %1.")
                                 .arg(file->fileName()));
            file->close();
            file->remove();
        } else {
            statusLabel->setText(tr("Downloaded %1 to current directory.")
                                 .arg(file->fileName()));
            file->close();
        }
        delete file;

        //fclose(fs_);
        //download_files();
	//emit s_get_next_file();

#if 0
        QTreeWidgetItemIterator it(transfer_status_);
	if (*it)
          delete *it;
#endif
        //begin_download();
        ftp_status_->append("after begin_download() ## server current path: " + currentPath);
	emit get_complete();

	//get_next_file();
        //enableDownloadButton();
    } else if (ftp->currentCommand() == QFtp::List) {
        QApplication::restoreOverrideCursor();
        QTreeWidgetItemIterator it(file_list);
        if (*it==0)
	{
            //fileList->addItem(tr("<empty>"));
            //fileList->setEnabled(false);
            //file_list->setEnabled(false);
        }
        if (operator_mode_ == DOWNLOAD_MODE)
	{
          --list_time_;
	  qDebug("QFtp::List commandId : %d", commandId);
	  if (error)
	  {
	    qDebug("QFtp::List commandId error: %s", qstr_to_c_str(ftp->errorString()) );
	  }

	  if (list_time_==0)
	  {
	    //qDebug("all files list");

	    qDebug("qftp:: download");
            make_transfer_dir();
	    //begin_download();
	    //download_files();
	    //transfer_status_->clear();
#if 0
	    for (; it != transfer_filename_.end() ; ++it)
	    {
              QTreeWidgetItem *si = new QTreeWidgetItem(transfer_status_);
              QTextCodec *tc;
              tc = QTextCodec::codecForName(encoding_name[server_cur_enc_]);
              QString fn(tc->toUnicode(it->toAscii() ) );
	      si->setText(0, fn);
              qDebug("it->toAscii().data() : %s", it->toAscii().data() );
              //store_file_to_local(it->toAscii().data() );

	    }

            vector<QString> :: iterator it=transfer_filename_.begin();
            for (it = transfer_dir_fn_.begin() ; it != transfer_dir_fn_.end() ; ++it)
	    {
              qDebug("all dirs  : %s", it->toAscii().data() );
	    }
#endif

	  }
	}
      list_finish_=true;
    }
}

int FtpWindow::get_local_path(QString &path)
{
  return local_dir_view_->get_select_path(path);
}

int FtpWindow::make_transfer_dir()
{

  //vector<QString> :: iterator it;
  //QDir cur_dir("/home/descent/svn/ftp/t");
  QString local_path;

  get_local_path(local_path);
  QDir cur_dir(local_path);


  QTextCodec *tc;
  tc = QTextCodec::codecForName(encoding_name[client_cur_enc_]);
  vector<QString> :: iterator it;






  QString enc_str;

  for (it=transfer_dir_fn_.begin() ; it!=transfer_dir_fn_.end(); ++it)
  {

  // maybe I set QFile 
  // QFile::setDecodingFunction(myDecoderFunc);
  // QFile::setEncodingFunction(myEncoderFunc);
  // so I need not do any code convert.

  if (get_enc_fn(*it, enc_str) != 0)
  {
    QMessageBox::information(this, "get_enc_fn()", "get_enc_fn() fail!!");
    return -1;
  }
  cur_dir.mkdir(enc_str);
#if 0
    if (if_convert_encoding_by_iconv(*it, enc_str) != -1)
    {
    }
    else
    {
      qDebug("if_convert_encoding_by_iconv() fail!!");
    }
#endif

    //cur_dir.mkdir(fn);

  }
  return 0;


}

void FtpWindow::addToList(const QUrlInfo &urlInfo)
{
  qDebug("FtpWindow::addToList");
/*
  QTextCodec *tc;
  tc = QTextCodec::codecForName(encoding_name[server_cur_enc_]);
  QString fn(tc->toUnicode(urlInfo.name().toAscii() ) );
*/
  QString fn;

  //str_encoding(encoding_name[server_cur_enc_], urlInfo.name().toAscii(), fn);
  str_encoding(file_list->get_cur_enc_str(), urlInfo.name().toAscii(), fn);


  if (operator_mode_ == LIST_MODE)
  {



  FileListWidgetItem *fi = new FileListWidgetItem(file_list, QTreeWidgetItem::UserType);
  fi->setText(0, fn);
  QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
  fi->setIcon(0,pixmap);
  fi->set_dir(urlInfo.isDir());
  fi->set_real_str(urlInfo.name());
  QString file_size;
  fi->setText(1, file_size.setNum(urlInfo.size()) );
  //qDebug("urlInfo.name(): %s", urlInfo.name().toStdString().c_str() );

  }
  if (operator_mode_ == DOWNLOAD_MODE)
  {
    //transfer_filename_.push_back(make_pair(fn, urlInfo.isDir()) );
    //transfer_filename_[]
    
    QString cur_path=cmd_id_cur_path_[ftp->currentId()] + "/" + urlInfo.name();

    QString file_size;

    get_file(cur_path, urlInfo.isDir(), file_size.setNum(urlInfo.size()) );
#if 0
    if(urlInfo.isDir() )
    {
      int cmd_id = ftp->list(cur_path + "/" + urlInfo.name());
      ++list_time_;
      cmd_id_cur_path_[cmd_id]=cur_path + "/" + urlInfo.name();

      qDebug("in addToList # cmd_id : %d # cur path: %s/", cmd_id, qstr_to_c_str(cur_path + "/" + urlInfo.name()) );
    }
    else
    {
      qDebug("in addToList # cur path: %s", qstr_to_c_str(cur_path + "/" + urlInfo.name()) );
      transfer_filename_.push_back(cur_path + "/" + urlInfo.name());
      //qDebug("ftp->hasPendingCommands() : %d", ftp->hasPendingCommands());
    }
#endif

  }

#ifdef LIST_ITEM
    //QListWidgetItem *item = new QListWidgetItem;
    FileListWidgetItem *item = new FileListWidgetItem;
    //cur_url_info_=urlInfo;
    //fileList->clear();

    // do encoding transform



  
  //tc = QTextCodec::codecForName("BIG5");
  //QTextCodec *tc;
  tc = QTextCodec::codecForName(encoding_name[server_cur_enc_]);
  //tc->fromUnicode(urlInfo.name()) ;
  
  //QTextCodec::setCodecForCStrings(tc);
  //urlInfo.name().toAscii();

  //QString fn(tc->fromUnicode(urlInfo.name()) );
  //QString fn(tc->toUnicode("宋世人") );
  //QString fn(tc->toUnicode(urlInfo.name().toAscii() ) );
  //real_char_map_[item]=urlInfo.name();
  item->set_real_str(urlInfo.name());

    item->setText(fn);
    QPixmap pixmap(urlInfo.isDir() ? ":/images/dir.png" : ":/images/file.png");
    item->setIcon(pixmap);
    item->set_dir(urlInfo.isDir());


    isDirectory[fn] = urlInfo.isDir();
    #if 0
    fileList->addItem(item);
    if (!fileList->currentItem()) {
        fileList->setCurrentItem(fileList->item(0));
        fileList->setEnabled(true);
    }
    #endif
    qDebug("server end: %d", server_cur_enc_);
    qDebug("client end: %d", client_cur_enc_);
#endif
}

void FtpWindow::processItem(QTreeWidgetItem * item, int column)
{
  operator_mode_ = LIST_MODE;
  FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(item);
  //FileListWidgetItem *fi = (FileListWidgetItem*)(item);
  if (fi==0)
  {
    qDebug("fi==0");
    return;
  }
    qDebug("fi!=0");
    //QString name = item->text(0);
    QString name;
    //if (isDirectory.value(name)) {
    if (fi->is_dir() )
    {
        //name=real_char_map_[item];
        name=fi->get_real_str();
        //fileList->clear();
        //isDirectory.clear();
        currentPath += "/" + name;
        ftp->cd(currentPath);

#ifdef DEBUG_SERVER_PATH
    std::string hexstr;
    qstr_to_hexstr(currentPath, hexstr);
    QMessageBox::information(this, tr("FTP"), hexstr.c_str());
#endif

        qDebug("FtpWindow::processItem # currentPath: %s", currentPath.toStdString().c_str() );
        //real_char_map_.clear();
	
        ftp->list();

        //file_list->clear();
        //ftp->list(currentPath);

	//setWindowTitle(currentPath);

        cdToParentButton->setEnabled(true);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        //return;
    }
}

void FtpWindow::cdToParent()
{
    operator_mode_ = LIST_MODE;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //fileList->clear();
    file_list->clear();
    //isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    if (currentPath.isEmpty()) {
        cdToParentButton->setEnabled(false);
        ftp->cd("/");
    } else {
        ftp->cd(currentPath);
    }
    ftp->list();
}

void FtpWindow::show_throughput()
{
  //qDebug("FtpWindow::show_throughput");
  static int c=1;

  cur_size_ += throughput_counter_;
  QString size;

  size.setNum(cur_size_); 

  FileListWidgetItem *fi = transfer_status_->get_cur_download_item();

  fi->setText(2, size);

  throughput_counter_ = 0;

  //qDebug("  XX  %d   ## throughput_counter_: %d", c, throughput_counter_);

  size.setNum(cur_size_/c);

  fi->setText(4, size + " bytes/sec");

  ++c;
}

#include <iostream>

using std::cout;

void FtpWindow::updateDataTransferProgress(qint64 readBytes, qint64 )
{
// TODO: The code will get more cpu usage, 
// need change it.
  //time_t t=time(NULL);
  //qDebug("t: %d", t);
  //qDebug("readBytes: %d", readBytes);
  //static int c=0;

  //diff_byte_ = readBytes - diff_byte_;
  cur_size_ = readBytes;
  //cout << c << " ## readBytes: " << readBytes << endl;
  //++c;
  //qDebug("%d  ## throughput_counter_: %d", c++, throughput_counter_);
#if 0
  //QTreeWidgetItemIterator it(transfer_status_);
  
  QString status_str;
  QString num;
  num.setNum(readBytes);
  //num.setNum((*it)->text(1) );
  status_str=num;// + "/" + ((*it)->text(1));
  //status_str += " / "; 
  //status_str += (*it)->text(1);
  transfer_status_->get_cur_download_item()->setText(2, status_str);
#if 0
  if (totalBytes != 0)
  {
    progressDialog->setMaximum(totalBytes);
    progressDialog->setValue(readBytes);
  }
  else
  {
    qDebug("totalBytes: %d", totalBytes);
  }
#endif
#endif
}

void FtpWindow::enableDownloadButton()
{
    //FileListWidgetItem *current = dynamic_cast<FileListWidgetItem *>(fileList->currentItem());
    FileListWidgetItem *current = dynamic_cast<FileListWidgetItem *>(file_list->currentItem());
    if (current) {
        //QString currentFile = current->text(0);
        //downloadButton->setEnabled(!current->is_dir());
    } else {
        //downloadButton->setEnabled(false);
    }
}

void FtpWindow::disconnect()
{
  if (ftp) 
  { // disconnect
    ftp->abort();
    ftp->deleteLater();
    ftp = 0;
    file_list->clear();
    file_list->setEnabled(false);
    //downloadButton->setEnabled(false);
    connectButton->setText(tr("Connect"));
    QApplication::restoreOverrideCursor();
  }
}

void FtpWindow::state_change(int state)
{
  switch (state)
  {
    case QFtp::Unconnected:
    {
      qDebug("0");
      ftp_status_->append("state change: Unconnected");
      #if 0
      if (ftp)
      {
        disconnect();
      }
      #endif


      break;
    }
    case QFtp::HostLookup:
    {
      qDebug("1");
      ftp_status_->append("state change: HostLookup");
      break;
    }
    case QFtp::Connecting:
    {
      qDebug("2");
      ftp_status_->append("state change: Connecting");
      break;
    }
    case QFtp::Connected:
    {
      qDebug("3");
      ftp_status_->append("state change: Connected");
      break;
    }
    case QFtp::LoggedIn:
    {
      qDebug("4");
      ftp_status_->append("state change: LoggedIn");
      break;
    }
    case QFtp::Closing:
    {
      qDebug("5");
      ftp_status_->append("state change: Closing");
      break;
    }
  }
}

void FtpWindow::change_encode_fn()
{
  QTextCodec *tc;
  tc = QTextCodec::codecForName(encoding_name[server_cur_enc_]);

  QTreeWidgetItemIterator it(file_list);
   while (*it) 
   {
     FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>(*it);
     QString fn(tc->toUnicode(fi->get_real_str().toAscii() ) );
     fi->setText(0, fn);
     ++it;

     //if ((*it)->text(0) == itemText)
      //treeWidget->setItemSelected(*it, true);

   }
#if 0
  for (int i=0 ; i < fileList->count() ; ++i)
  {
    FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>(fileList->item(i) );
    QString fn(tc->toUnicode(fi->get_real_str().toAscii() ) );
    fi->setText(0, fn);
  }
#endif
}

void FtpWindow::server_big5()
{
    server_cur_enc_ = BIG5;
    qDebug("FtpWindow::server_big5");
    //fileList->clear();
    //emit reload(cur_url_info_);

    change_encode_fn();

    //action->setChecked(true);
}
void FtpWindow::server_utf8()
{
    server_cur_enc_ = UTF8;
    qDebug("FtpWindow::server_utf8");
    //fileList->clear();
    change_encode_fn();
    //action->setChecked(true);
}
void FtpWindow::client_big5()
{
    client_cur_enc_ = BIG5;
    //action->setChecked(true);
}
void FtpWindow::client_utf8()
{
    client_cur_enc_ = UTF8;
    //action->setChecked(true);
}

void FtpWindow::do_with_raw_command(int replyCode, const QString & detail)
{
  // get current directory
  qDebug("PWD: %s", qstr_to_c_str(detail) );
  int m=detail.indexOf("\"")+1;
  int n=detail.lastIndexOf("\"")-1;
  qDebug("m: %d\n", m);
  qDebug("n: %d\n", n);
  currentPath="";
  currentPath=detail.mid(m, n);
  qDebug("currentPath: %s", qstr_to_c_str(currentPath) );
  ftp_status_->append("in do_with_raw_command ## server current path: " + currentPath);
}

void FtpWindow::tag_echo_password(int state)
{
  switch (state)
  {
    case Qt::Unchecked:
    {
      password_->setEchoMode(QLineEdit::Normal);
      break;
    }
    case Qt::PartiallyChecked:
    {
      break;
    }
    case Qt::Checked:
    {
      password_->setEchoMode(QLineEdit::Password);
      break;
    }
  }
  
}



int omit_invalid=0;


// ref glibc-2.5/iconv/iconv_prog.c
int process_block (iconv_t cd, ICONV_CONST char *addr, size_t len)
{
//#define OUTBUF_SIZE	32768
  ICONV_CONST char *start = addr;
  char outbuf[OUTBUF_SIZE];
  char *outptr;
  size_t outlen;
  size_t n;
  int ret = 0;
  //char *outbuf=new char [OUTBUF_SIZE];

  convert_str_len=0;
  qDebug("ZZZ ## len: %d", len);
  qDebug("ZZZ ## addr[0]: %x", addr[0]);
  qDebug("ZZZ ## addr[1]: %x", addr[1]);

  while (len > 0)
    {
      outptr = outbuf;
      outlen = OUTBUF_SIZE;
      n = iconv (cd, &addr, &len, &outptr, &outlen);

      if (n == (size_t) -1 && omit_invalid && errno == EILSEQ)
	{
	  ret = 1;
	  if (len == 0)
	    n = 0;
	  else
	    errno = E2BIG;
	}

      if (outptr != outbuf)
	{
	  qDebug("ZZZZ");
	  /* We have something to write out.  */
	  int errno_save = errno;

	  convert_str_len=outptr - outbuf;
	  qDebug("convert_str_len: %d", convert_str_len);
          memcpy(convert_str, outbuf, convert_str_len);
	  convert_str[convert_str_len]=0;

#if 0
	  if (fwrite (outbuf, 1, outptr - outbuf, output)
	      < (size_t) (outptr - outbuf)
	      || ferror (output))
	    {
	      /* Error occurred while printing the result.  */
	      //error (0, 0, _("\ conversion stopped due to problem in writing the output"));
	      return -1;
	    }
#endif
	  errno = errno_save;
	}
        else
        {
          qDebug("XXYYZZ");
        }

      if (n != (size_t) -1)
	{
	  /* All the input test is processed.  For state-dependent
             character sets we have to flush the state now.  */
	  outptr = outbuf;
	  outlen = OUTBUF_SIZE;
	  n = iconv (cd, NULL, NULL, &outptr, &outlen);

	  if (outptr != outbuf)
	    {
	      /* We have something to write out.  */
	      int errno_save = errno;

          memcpy(convert_str+convert_str_len, outbuf, outptr - outbuf);
	  convert_str_len+=outptr - outbuf;
	  qDebug("xxx convert_str_len: %d", convert_str_len);
	  convert_str[convert_str_len]=0;

#if 0
	      if (fwrite (outbuf, 1, outptr - outbuf, output)
		  < (size_t) (outptr - outbuf)
		  || ferror (output))
		{
		  /* Error occurred while printing the result.  */
		  //error (0, 0, _("\ conversion stopped due to problem in writing the output"));
		  return -1;
		}
#endif

	      errno = errno_save;
	    }

	  if (n != (size_t) -1)
	    break;

	  if (omit_invalid && errno == EILSEQ)
	    {
	      ret = 1;
	      break;
	    }
	}

      if (errno != E2BIG)
	{
	  /* iconv() ran into a problem.  */
	  switch (errno)
	    {
	    case EILSEQ:
	      if (! omit_invalid) 
	        ;//error (0, 0, _("illegal input sequence at position %ld"), (long int) (addr - start));
	        qDebug ("illegal input sequence at position %ld", (long int) (addr - start));
	      break;
	    case EINVAL:
	      //error (0, 0, _("\ incomplete character or shift sequence at end of buffer"));
	      qDebug ("incomplete character or shift sequence at end of buffer");
	      break;
	    case EBADF:
	      //error (0, 0, _("internal error (illegal descriptor)"));
	      qDebug ("internal error (illegal descriptor)");
	      break;
	    default:
	      //error (0, 0, _("unknown iconv() error %d"), errno);
	      qDebug ("unknown iconv() error %d", errno);
	      break;

	    }
            qDebug("AABBCCDD");

	  return -1;
	}
    }
  qDebug("ret");
  return ret;
}

int FtpWindow::get_enc_fn(const QString &org_fn, QString &enc_fn)
{

  if (if_convert_encoding_by_iconv(org_fn, enc_fn) == 0)
  {
    QString lp;

// QFile::setDecodingFunction(myDecoderFunc);
// QFile::setEncodingFunction(myEncoderFunc);
// note: because I disable QFile encode/decode, so convert by iconv
// I can use the enc_str to QFile in unix,
// but disable encode/decode is useless in windows,
// so, need use QTextCodec to conver enc_str.


#ifdef Q_OS_WIN
  QTextCodec *tc;
  //tc = QTextCodec::codecForName(encoding_name[client_cur_enc_]);
  tc = QTextCodec::codecForName(local_dir_view_->get_cur_enc_str() );
  lp = (tc->toUnicode(enc_fn.toAscii().data())) ;


#else
    lp = enc_fn;
#endif

    enc_fn = lp;

//#define DEBUG_NAME_CONVERT
#ifdef DEBUG_NAME_CONVERT
  std::string hexstr;
  //qstr_to_hexstr(full_path_fn, hexstr);
  qstr_to_hexstr(enc_fn, hexstr);
  QMessageBox::information(this, tr("FTP"), hexstr.c_str());
#endif

  }
  else
  {
    qDebug("if_convert_encoding_by_iconv() fail!!");
    QMessageBox::information(this, "if_convert_encoding_by_iconv()", "if_convert_encoding_by_iconv() fail!!");
    return -1;
  }
  return 0;
}

// if need do encode convert
int FtpWindow::if_convert_encoding_by_iconv(const QString &l_path, QString &enc_str)
{
  int ret=0;
  // not big5 encoding, do charset convert
  // TODO: mabe change local_dir_view_->get_cur_enc_str() != ftp server encode
  if (strcmp(local_dir_view_->get_cur_enc_str(), "BIG5")!=0 )
  {


    iconv_t cd;
    qDebug("convert charset to %s", local_dir_view_->get_cur_enc_str() );
    cd = iconv_open (local_dir_view_->get_cur_enc_str(), "BIG5" );
    //cd = iconv_open ("UTF-8", "BIG5" );

    if (cd==(iconv_t)-1)
    {
      qDebug("cd==-1");
      exit(-1);
    }
  //const char c[]={0xa7, 0x41};
    //ICONV_CONST char *c="你";

    //qDebug("str: %x %x %x %x", str[3], str[4], str[6], str[7]);
    //int ret=process_block(cd, str, l_path.toAscii().length() );
    int ret=process_block(cd, l_path.toAscii().data(), l_path.toAscii().length() );
  //int ret=process_block(cd, c, 2);
    //qDebug("convert_str: %x %x %x %x %x", convert_str[0], convert_str[1], convert_str[2], convert_str[3], convert_str[4]);
    qDebug("convert_str: %x %x %x", convert_str[convert_str_len-3], convert_str[convert_str_len-2], convert_str[convert_str_len-1]);
    qDebug("ret : %d", ret);

    iconv_close(cd);

  //const char t[]={0xe6, 0x88, 0x91};
  //const char y[]={0xe4, 0xbd, 0xa0};
    //file = new QFile(convert_str);
    enc_str=convert_str;

  }
  else
  {
    qDebug("don't convert charset");
    //file = new QFile(l_path);
    enc_str=l_path;
  }
  return ret;
}


int FtpWindow::store_file_to_local(const QString &server_path, const QString &l_path)
{
  qDebug("int FtpWindow::store_file_to_local(const QString &server_path, const QString &local_path)");
#if 1
  // convert local path to selected encoding
  ICONV_CONST char *str=l_path.toAscii().data();
  qDebug("str : %s", str);
  qDebug("strlen(str) : %d", strlen(str) );
  qDebug("1");
  //cd = iconv_open (encoding_name[client_cur_enc_], encoding_name[server_cur_enc_]);
  //cd = iconv_open (file_list->get_cur_enc_str() , local_dir_view_->get_cur_enc_str() );



  QString enc_str;

  if (get_enc_fn(l_path, enc_str) != 0)
  {
    QMessageBox::information(this, "get_enc_fn()", "get_enc_fn() fail!!");
    return -1;
  }
  file = new QFile(enc_str);
#if 0
  if (if_convert_encoding_by_iconv(l_path, enc_str) == 0)
  {
    QString lp;

// QFile::setDecodingFunction(myDecoderFunc);
// QFile::setEncodingFunction(myEncoderFunc);
// note: because I disable QFile encode/decode, so convert by iconv
// I can use the enc_str to QFile in unix,
// but disable encode/decode is useless in windows,
// so, need use QTextCodec to conver enc_str.


#ifdef Q_OS_WIN
  QTextCodec *tc;
  //tc = QTextCodec::codecForName(encoding_name[client_cur_enc_]);
  tc = QTextCodec::codecForName(local_dir_view_->get_cur_enc_str() );
  lp = (tc->toUnicode(enc_str.toAscii().data())) ;


#else
    lp = enc_str;
#endif

    file = new QFile(lp);

#if DEBUG_NAME_CONVERT
  std::string hexstr;
  //qstr_to_hexstr(full_path_fn, hexstr);
  qstr_to_hexstr(enc_str, hexstr);
  QMessageBox::information(this, tr("FTP"), hexstr.c_str());
#endif

  }
  else
  {
    qDebug("if_convert_encoding_by_iconv() fail!!");
    QMessageBox::information(this, "if_convert_encoding_by_iconv()", "if_convert_encoding_by_iconv() fail!!");
    return -1;
  }
#endif

  //tc = QTextCodec::codecForName(encoding_name[client_cur_enc_]);
  //tc = QTextCodec::codecForName(local_dir_view_->get_cur_enc_str() );

  // lp maen local path
  //QString lp(tc->toUnicode(convert_str) );
  //str_encoding(local_dir_view_->get_cur_enc_str(), l_path.toAscii().data(), lp);
  //qDebug("l_path: %s", qstr_to_c_str(l_path) );

  //QString lp("/tmp/aa");
  //lp="/tmp/我是";
  // maybe I set QFile 
  // QFile::setDecodingFunction(myDecoderFunc);
  // QFile::setEncodingFunction(myEncoderFunc);
  // so I need not do any code convert.

  //file = new QFile(t);
  //file = new QFile("/tmp/ccc");

    //ftp_status_->append("download file path:" + lp);
  if (!file->open(QIODevice::WriteOnly)) 
  {
    //QMessageBox::information(this, tr("FTP"), tr("Unable to save the file %1: %2.") .arg(lp).arg(file->errorString()));
    delete file;
    return -1;
  }
  //ftp_status_->append("download file path:" + local_path + "/" + fn);

  throughput_counter_ = 0;
  cur_size_ = 0;
  diff_byte_ = 0;

    QStringList ftp_path = server_path.split("/");

    ftp->cd("/");
    // from 1 not 0, because ftp_path[0] is empty
    for (int i=1 ; i < ftp_path.size()-1 ; ++i)
    {
      ftp->cd(ftp_path[i]);
    }


  ftp->get(ftp_path[ftp_path.size()-1], file);
  show_throughput_timer_ -> start(1000); // 1000 msec
#endif
  return 0;
}


#if 0
// store ftp server file to local file system
int FtpWindow::store_file_to_local(const QString &server_path)
{

  // get local path, if server path is /home/aa => aa
  
  int x=server_path.length() - currentPath.length();

  // because "/" need remove, if not plus 1, will get /asd, but 
  // I need asd file name.
  --x; 

  QString local_fn=server_path.right(x);


  ICONV_CONST char *filename=local_fn.toAscii().data();
  //qDebug("filename: %s", filename);
  //qDebug("in FtpWindow::store_file_to_local filename: %s", filename);

  iconv_t cd;
  //const char client_enc[]="BIG5";
  //const char server_enc[]="UTF-8";
  cd = iconv_open (encoding_name[client_cur_enc_], encoding_name[server_cur_enc_]);
  //cd = iconv_open ("UTF-8", "BIG5");

  if (cd==(iconv_t)-1)
  {
    qDebug("cd==-1");
    exit(-1);
  }

  //FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(fileList->currentItem() );
  process_block(cd, filename, strlen(filename) );

  
  QTextCodec *tc;
  tc = QTextCodec::codecForName(encoding_name[client_cur_enc_]);

  QString fn(tc->toUnicode(convert_str) );
  //QString fn(tc->toUnicode(real_char_map_[fileList->currentItem()].toAscii() ) );
  //QString fn(tc->toUnicode("宋是" ) );
  QString local_path;

  get_local_path(local_path);
    file = new QFile(local_path + "/" + fn);
    //(local_path + "/" + local_fn);
    
    //file = new QFile("a.jpg");
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("FTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(filename).arg(file->errorString()));
        delete file;
        return -1;
    }
    ftp_status_->append("download file path:" + local_path + "/" + fn);
    ftp->get(server_path, file);
    show_throughput_timer_ -> start(1000); // 1000 msec
    fs_=fopen("/tmp/test", "w");
    total_len_=0;
    //ftp->get(server_path);
    connect(ftp, SIGNAL(readyRead() ), this, SLOT(get_file_data()));


  return 0;
}
#endif

void FtpWindow::get_file_data()
{


  qDebug("in FtpWindow::get_file_data()");

  int byte_available=ftp->bytesAvailable();
  total_len_+=byte_available;


  char *data=new char [byte_available];
  int read_count = ftp->read(data, byte_available);
  printf("byte_available: %d\n", byte_available);
  printf("total_len_: %d\n", total_len_);
  printf("read_count: %d\n", read_count);

#if 0
  for (int i=0 ; i < byte_available ; ++i)
  {
    printf("%x ", data[i]);
    if (i % 16 ==0)
      printf("\n");
  }
#endif
  fwrite(data, 1, byte_available, fs_);
  
}

int FtpWindow::get_file(const QString &full_path_fn, bool is_dir, const QString &size)
{
  if (is_dir)
  {
    
    ftp_status_->append("list : " + full_path_fn);

#if 1
    // this code maybe do the same thing with #else code
    // but I think may be QT 4 QFtp bug
    // #else code if directory file name is [name]
    // will fail




    QStringList ftp_path = full_path_fn.split("/");

    ftp->cd("/");
    // from 1 not 0, because ftp_path[0] is empty
    for (int i=1 ; i < ftp_path.size() ; ++i)
    {
      ftp->cd(ftp_path[i]);
    }

    //qDebug("cd_path: %s", qstr_to_c_str(cd_path) );
    //ftp->cd(full_path_fn);
    int cmd_id = ftp->list();
    //ftp->cd(full_path_fn+"/..");
    ftp->cd("..");
    qDebug("full_path_fn : %s/", qstr_to_c_str(full_path_fn) );
#else

    //QString ftp_cmd = "CWD " + full_path_fn;
    //ftp->rawCommand("PWD");
    int cmd_id = ftp->list(full_path_fn);
#endif

#ifdef DEBUG_SERVER_PATH
    std::string hexstr;
    qstr_to_hexstr(full_path_fn, hexstr);
    QMessageBox::information(this, tr("FTP"), hexstr.c_str());
#endif

    ++list_time_;
    cmd_id_cur_path_[cmd_id]=full_path_fn;
    // remove pwd
    //int m=full_path_fn.indexOf(currentPath)+currentPath.size();
    QString dn=full_path_fn.right(full_path_fn.size() - currentPath.size() -1);
    //qDebug("%s => %s", qstr_to_c_str(full_path_fn), qstr_to_c_str(dn) );
    //qDebug("m: %d", m);
    transfer_dir_fn_.push_back(dn);
  }
  else
  {
    //transfer_filename_.push_back(full_path_fn);

    FileListWidgetItem *si = new FileListWidgetItem(transfer_status_);
    QTextCodec *tc;
    tc = QTextCodec::codecForName(encoding_name[server_cur_enc_]);
    QString fn(tc->toUnicode(full_path_fn.toAscii().data() ) );
    si->setText(0, fn);
    QString file_size;
    // set total bytes string
    si->setText(3, size );

    QString local_path;

    get_local_path(local_path);



  // if cur path str not encoding, length is not right
  QString encode_cur_path(tc->toUnicode(currentPath.toAscii().data() ) );

  int x=fn.length() - encode_cur_path.length();

  // because "/" need remove, if not plus 1, will get /asd, but
  // I need asd file name.
  --x;


  qDebug("fn.length() : %d", fn.length());
  qDebug("currentPath.length() : %d", currentPath.length());
  //qDebug("path_.length() : %d", path_.length());
  qDebug("x : %d", x);

  QString local_fn=fn.right(x);

  // this get unencoding str
  x = full_path_fn.length() - currentPath.length();
  --x;
  QString unenc_local_fn=full_path_fn.right(x);


    si->setText(1, local_path + "/" + local_fn);
    si->set_real_str2(local_path + "/" + unenc_local_fn);

    si->set_real_str(full_path_fn);

#ifdef DEBUG_FTP_LIST
  std::string hexstr;
  //qstr_to_hexstr(full_path_fn, hexstr);
  qstr_to_hexstr(unenc_local_fn, hexstr);
  QMessageBox::information(this, tr("FTP"), hexstr.c_str());
#endif

    //qDebug("fn : %s", qstr_to_c_str(full_path_fn) );
    //qDebug("XXX ftp->hasPendingCommands() : %d", ftp->hasPendingCommands());
  }
  return 0;
}

void FtpWindow::set_transfer_passive_mode()
{
    transfermode_passive_->setChecked(true);
    //ftp->setTransferMode(QFtp::Passive);
}
void FtpWindow::set_transfer_active_mode()
{
    transfermode_active_->setChecked(true);
    //ftp->setTransferMode(QFtp::Active);
}

void FtpWindow::single_selection()
{
  file_list->setSelectionMode(QAbstractItemView::SingleSelection);
}
void FtpWindow::contiguous_selection()
{
  file_list->setSelectionMode(QAbstractItemView::ContiguousSelection);
}
void FtpWindow::extended_selection()
{
  file_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
}
void FtpWindow::multi_selection()
{
  file_list->setSelectionMode(QAbstractItemView::MultiSelection);
}
void FtpWindow::no_selection()
{
  file_list->setSelectionMode(QAbstractItemView::NoSelection);
}

#if 0
// ref  libiconv-1.11/src/iconv.c
int convert(iconv_t cd, ICONV_CONST char *str, int len)
{
  char inbuf[4096+4096];
  size_t inbufrest = 0;
  char initial_outbuf[4096];
  //char *outbuf = initial_outbuf;
  char *outbuf = convert_str;
  size_t outbufsize = sizeof(initial_outbuf);
  int status = 0;
  size_t inbufsize = len;

  line = 1; column = 0;
  iconv(cd,NULL,NULL,NULL,NULL);
	//return 0;

  // I don't know why offset 4096, descent
  strcpy(inbuf+4096, str);
  //for (;;) 
  {

#if 0
    size_t inbufsize = fread(inbuf+4096,1,4096,infile);
    if (inbufsize == 0) {
      if (inbufrest == 0)
        break;
      else {
        if (ilseq_byte_subst != NULL)
          subst_mb_to_mb_fallback(inbuf+4096-inbufrest, inbufrest);
        if (!silent) {
          fflush(stdout);
          if (column > 0)
            putc('\n',stderr);
          error(0,0,_("%s:%u:%u: incomplete character or shift sequence"),infilename,line,column);
        }
        status = 1;
        goto done;
      }
    } else 
#endif
    {
      const char* inptr = inbuf+4096-inbufrest;
      size_t insize = inbufrest+inbufsize;
      inbufrest = 0;
      while (insize > 0) 
      {
        char* outptr = outbuf;
        size_t outsize = outbufsize;
        size_t res = iconv(cd,(ICONV_CONST char**)&inptr,&insize,&outptr,&outsize);
        if (outptr != outbuf) {
          int saved_errno = errno;
#if 0
          if (fwrite(outbuf,1,outptr-outbuf,stdout) < outptr-outbuf) {
            status = 1;
            goto done;
          }
#endif
          errno = saved_errno;
        }


        if (res == (size_t)(-1)) {
          if (errno == EILSEQ) {
            if (discard_unconvertible == 1) {
              int one = 1;
              iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
              discard_unconvertible = 2;
              status = 1;
            } else {
              if (!silent) {
#if 0
                fflush(stdout);
                if (column > 0)
                  putc('\n',stderr);
                error(0,0,_("%s:%u:%u: cannot convert"),infilename,line,column);
#endif
              }
              status = 1;
              goto done;
            }
          } else if (errno == EINVAL) {
            if (inbufsize == 0 || insize > 4096) {
              if (!silent) {
#if 0
                fflush(stdout);
                if (column > 0)
                  putc('\n',stderr);
                error(0,0,_("%s:%u:%u: incomplete character or shift sequence"),infilename,line,column);
#endif
              }
              status = 1;
              goto done;
            } else {
              inbufrest = insize;
              if (insize > 0) {
                /* Like memcpy(inbuf+4096-insize,inptr,insize), except that
                   we cannot use memcpy here, because source and destination
                   regions may overlap. */
                char* restptr = inbuf+4096-insize;
                do { *restptr++ = *inptr++; } while (--insize > 0);
              }
              break;
            }
          } else if (errno == E2BIG) {
            if (outptr==outbuf) {
              /* outbuf is too small. Double its size. */
              if (outbuf != initial_outbuf)
                free(outbuf);
              outbufsize = 2*outbufsize;
              if (outbufsize==0) /* integer overflow? */
                ;//xalloc_die();
              //outbuf = (char*)xmalloc(outbufsize);
              outbuf = (char*)malloc(outbufsize);
            }
          } else {
            if (!silent) {
              int saved_errno = errno;
#if 0
              fflush(stdout);
              if (column > 0)
                putc('\n',stderr);
              error(0,saved_errno,_("%s:%u:%u"),infilename,line,column);
#endif
            }
            status = 1;
            goto done;
          }
        }
      }
    }
  }
  for (;;) {
    char* outptr = outbuf;
    size_t outsize = outbufsize;
    size_t res = iconv(cd,NULL,NULL,&outptr,&outsize);
    if (outptr != outbuf) {
      int saved_errno = errno;

#if 0
      if (fwrite(outbuf,1,outptr-outbuf,stdout) < outptr-outbuf) {
        status = 1;
        goto done;
      }
#endif

      errno = saved_errno;
    }
    if (res == (size_t)(-1)) {
      if (errno == EILSEQ) {
        if (discard_unconvertible == 1) {
          int one = 1;
          iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
          discard_unconvertible = 2;
          status = 1;
        } else {
          if (!silent) {
#if 0
            fflush(stdout);
            if (column > 0)
              putc('\n',stderr);
            error(0,0,_("%s:%u:%u: cannot convert"),infilename,line,column);
#endif
          }
          status = 1;
          goto done;
        }
      } else if (errno == EINVAL) {
        if (!silent) {
#if 0
          fflush(stdout);
          if (column > 0)
            putc('\n',stderr);
          error(0,0,_("%s:%u:%u: incomplete character or shift sequence"),infilename,line,column);
#endif
        }
        status = 1;
        goto done;
      } else if (errno == E2BIG) {
        if (outptr==outbuf) {
          /* outbuf is too small. Double its size. */
          if (outbuf != initial_outbuf)
            free(outbuf);
          outbufsize = 2*outbufsize;
          if (outbufsize==0) /* integer overflow? */
            ;//xalloc_die();
          outbuf = (char*)malloc(outbufsize);
        }
      } else {
        if (!silent) {
          int saved_errno = errno;
#if 0
          fflush(stdout);
          if (column > 0)
            putc('\n',stderr);
          error(0,saved_errno,_("%s:%u:%u"),infilename,line,column);
#endif
        }
        status = 1;
        goto done;
      }
    } else
      break;
  }
#if 0
  if (ferror(infile)) {
    fflush(stdout);
    if (column > 0)
      putc('\n',stderr);
    error(0,0,_("%s: I/O error"),infilename);
    status = 1;
    goto done;
  }
#endif
 done:
  if (outbuf != initial_outbuf)
    free(outbuf);
  return status;
}

int encoding_convert(const char *tocode, const char *fromcode, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
  return 0;
}
#endif

