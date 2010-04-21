#include "ftp_server_list.h"
#include "ftp_transfer.h"
#include "ftpwindow.h"

FtpServerList::FtpServerList( QWidget * parent):TreeViewPopupMenu (parent)
{

#if 0
  popup_menu_timer_ = new QTimer(this);
  popup_menu_timer_->setSingleShot(true);
  connect( popup_menu_timer_, SIGNAL( timeout() ), this, SLOT( show_fo_menu() ) );
#endif

  QAction *add_to_down_list = new QAction("add to down list", this);
  add_action(add_to_down_list);
  connect(add_to_down_list, SIGNAL(triggered()), this, SLOT(s_add_to_down_list()));

  transfer_status_ = 0;
  ftp_ = 0;
  //m_->addAction(add_to_down_list);

}

void FtpServerList::connect_or_disconnect()
{
#if 0
  if (ftp_) 
  {
    disconnect();
    return;
  }
    
  //QApplication::setOverrideCursor(Qt::WaitCursor);
    
  ftp_ = new QFtp(this);
  //transfer_status_->set_ftp_(ftp_);

#if 0
    if (transfermode_passive_->isChecked() )
    {
      ftp_->setTransferMode(QFtp::Passive);
    }
    else
    {
      ftp_->setTransferMode(QFtp::Active);
    }
#endif
  connect(ftp_, SIGNAL(commandStarted(int)), this, SLOT(s_command_started(int)));

  connect(ftp_, SIGNAL(stateChanged(int)), this, SLOT(state_change(int)));
  connect(ftp_, SIGNAL(commandFinished(int, bool)), this, SLOT(ftp_CommandFinished(int, bool)));
  connect(ftp_, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(addToList(const QUrlInfo &)));

  connect(ftp_, SIGNAL(rawCommandReply(int , const QString & )), this, SLOT(do_with_raw_command(int , const QString &) ));

  connect(ftp_, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(updateDataTransferProgress(qint64, qint64)));

  ftp_->connectToHost(ftp_ServerLineEdit->text(), port_edit_->text().toInt() );

    //statusLabel->setText( "string" + user_name_->text() + " " + password_->text());
    //statusLabel->setText( user_name_->text() );
    //sleep(10);


    if (user_name_->text().isEmpty() && password_->text().isEmpty() )
    {
      qDebug("anonymouse");
      ftp_->login();
    }
    else
    {
      ftp_->login(user_name_->text(), password_->text() );
    }
    operator_mode_ = LIST_MODE;
    list_1_=ftp_->list(); // list current directory
    ftp_->rawCommand("PWD");
    //list_2_=ftp_->list("/"); // list root directory
    //ftp_->login();
    //ftp_->list();

    //fileList->setEnabled(true);
    file_list->setEnabled(true);
    connectButton->setText(tr("Disconnect"));
    statusLabel->setText(tr("Connecting to FTP server %1...") .arg(ftp_ServerLineEdit->text()));
#endif
}

void FtpServerList::s_add_to_down_list()
{
  ftp_win_->downloadFile();
}
