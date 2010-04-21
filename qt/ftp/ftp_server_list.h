#ifndef FTP_SERVER_LIST_H
#define FTP_SERVER_LIST_H

#include <QTreeWidgetItem>
#include <QTimer>
#include <QMenu>
#include <QFtp>

#include "tree_view_popup_menu.h"


class FtpWindow;
class FtpTransfer;


class FtpServerList : public TreeViewPopupMenu
{
  Q_OBJECT

  public:
    FtpServerList( QWidget * parent=0);
    void set_ftp_transfer(FtpTransfer *transfer_status)
    {
      transfer_status_ = transfer_status;
    }
    FtpTransfer *get_ftp_transfer()
    {
      return transfer_status_;
    }
    void connect_or_disconnect();
    #if 1
    void set_ftp_win(FtpWindow *ftp_win)
    {
      ftp_win_ = ftp_win;
    }
    #endif
    
  public slots:
    void s_add_to_down_list();

#if 0
    virtual void show_fo_menu()
    {
      qDebug("show_fo_menu");
      //QMenu * m = new QMenu(this);
      //m->popup(QCursor::pos());
      TreeViewPopupMenu::show_fo_menu();
    }
#endif


  protected:
#if 0
    void mousePressEvent ( QMouseEvent * e)
    {
      QTreeWidget::mousePressEvent( e );
      //QTimer::singleShot(1000, this, SLOT(show_fo_menu()));
      popup_menu_timer_->start(1000);
    }

    void mouseReleaseEvent( QMouseEvent * e)
    {
      QTreeWidget::mouseReleaseEvent( e );
      qDebug("in mouseReleaseEvent");
      popup_menu_timer_->stop();
    }

    QTimer *popup_menu_timer_;
#endif
  private:
    FtpTransfer *transfer_status_;
    QFtp *ftp_;
    FtpWindow *ftp_win_;

    

};



#endif
