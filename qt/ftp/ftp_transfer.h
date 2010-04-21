#ifndef FTP_TRANSFER_H
#define FTP_TRANSFER_H

#include "tree_view_popup_menu.h"
//#include "ftpwindow.h"

#include <QFtp>

class FtpWindow;
class RenameDialog;
class FileListWidgetItem;

#if 0
// for convenient to create popup menu
#define CREATE_ACTIONS(qa, qa_title, slot_func) \
{ \
      qa = new QAction(tr(qa_title), this);\
      add_action(qa);\
      connect(qa, SIGNAL(triggered()), this, SLOT(slot_func() ) );\
}
#endif

class FtpTransfer: public TreeViewPopupMenu
{
  Q_OBJECT
  public:
    FtpTransfer(QWidget *parent=0);


    void set_ftp(QFtp *ftp)
    {
      ftp_ = ftp;
    }
    QFtp *get_ftp_ptr(){return ftp_;}
    #if 1
    void set_ftp_win(FtpWindow *ftp_win)
    {
      ftp_win_ = ftp_win;
    }
    #endif

  public slots:

    void s_download();
    void s_rename_local_path();
    void remove_all_items()
    {
      clear();
    }
    void remove_selected_item()
    {
      QList<QTreeWidgetItem *> selected_items=selectedItems();
      QList<QTreeWidgetItem *> :: iterator it;
      for (it = selected_items.begin() ; it!=selected_items.end();++it)
      {
        delete *it;
      }

    }
    FileListWidgetItem *get_cur_download_item()
    {
      return cur_item_;
    }
    void s_abort();

  protected:
    FileListWidgetItem *get_next_item();
  private:
    QAction *download_, *remove_, *rename_local_path_, *abort_, *remove_all_items_;
    QFtp *ftp_;
    FtpWindow *ftp_win_;
    RenameDialog *rename_local_path_dialog_;
    FileListWidgetItem *cur_item_;

};


#endif
