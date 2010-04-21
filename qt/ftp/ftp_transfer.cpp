#include "ftp_transfer.h"
#include "rename_dialog.h"
#include "file_list_widget.h"
#include "ftpwindow.h"

#include <QTextCodec>


FtpTransfer::FtpTransfer(QWidget *parent):TreeViewPopupMenu(parent)
{
      QStringList headers;
      headers << tr("server path") << tr("local path") << tr("current bytes")  << tr("total bytes") << tr("throughput");
      setHeaderLabels(headers);
      setRootIsDecorated(false);

  rename_local_path_dialog_ = new RenameDialog(this);
      CREATE_ACTIONS(download_, "download", s_download)
      CREATE_ACTIONS(rename_local_path_, "rename local path", s_rename_local_path)
      CREATE_ACTIONS(abort_, "abort", s_abort)
      CREATE_ACTIONS(remove_, "remove selected items", remove_selected_item)
      CREATE_ACTIONS(remove_all_items_, "remove all items", remove_all_items)
#if 0
      download_ = new QAction(tr("download"), this);
      remove_ = new QAction(tr("remove selected items"), this);
      rename_local_path_ = new QAction(tr("rename local path"), this);

      add_action(download_);
      add_action(remove_);

      connect(remove_, SIGNAL(triggered()), this, SLOT(remove_selected_item()));
      connect(download_, SIGNAL(triggered()), this, SLOT(s_download()));
#endif


    //next_item_ = 0;
}

FileListWidgetItem *FtpTransfer::get_next_item()
{
#if 0
  if (next_item_ == 0)
  {

    QTreeWidgetItemIterator it(this);
    if (*it)
    {
      next_item_ = dynamic_cast<FileListWidgetItem *>(*it);
    }
    else
    {
      return 0;
    }
  }
#endif
  return 0;
}

void FtpTransfer::s_abort()
{
  ftp_win_->cancelDownload();
}

void FtpTransfer::s_download()
{
  QTreeWidgetItemIterator it(this);
  while (*it)
  {
    FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>(*it);
    cur_item_ = fi;

    if (fi->text(2)=="")
    {
      ftp_win_->store_file_to_local(fi->get_real_str(), fi->get_real_str2() );
      return;
    }
    ++it;
  }


}

void FtpTransfer::s_rename_local_path()
{
  QList<QTreeWidgetItem *> select_item = selectedItems ();

  // should only one item
  QTreeWidgetItem *it = select_item.at(0);
  FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(it);


  //rename_local_path_dialog_->set_filename(fi->get_real_str2() );

  QTextCodec *tc;
  //tc = QTextCodec::codecForName( ftp_win_->local_dir_view_->get_cur_enc_str() );
  tc = QTextCodec::codecForName( "big5");
  QString fn(tc->toUnicode(fi->get_real_str2().toAscii() ) );

  rename_local_path_dialog_->set_filename(fn);


  if (rename_local_path_dialog_->exec())
  {
    qDebug("ok");
    QString new_fpath;
    rename_local_path_dialog_->get_filename(new_fpath);
    QByteArray dec_str=tc->fromUnicode(new_fpath);
    fi->set_real_str2(dec_str);
    fn=(tc->toUnicode(fi->get_real_str2().toAscii() ) );
    fi->setText(1, fn);
  }
  else
  {
    qDebug("cancel");
  }
}
