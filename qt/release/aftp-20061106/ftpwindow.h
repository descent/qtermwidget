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

#ifndef FTPWINDOW_H
#define FTPWINDOW_H


#include <QTextEdit>
#include <QDialog>
//#include <QHash>
#include <QMenu>
#include <QMenuBar>
#include <QCheckBox>
#include <QTextCodec>
#include <QUrlInfo>
#include <QFrame>

#include <QtGui>

#include <QSplitter>

#include <QTreeWidgetItemIterator>
#include <QTreeWidgetItem>
#include <QTreeWidget>

#include <iconv.h>

#include <vector>
#include <map>

#include "file_list_widget.h"

#ifndef ICONV_CONST
# define ICONV_CONST 
//const
#endif

class QFile;
class QFtp;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QProgressDialog;
class QPushButton;
class QUrlInfo;

using namespace Qt;
using namespace std;

const int BIG5=0;
const int UTF8=1;
const int TC_NUM=UTF8+1;


const int LIST_MODE=1000;
const int DOWNLOAD_MODE=1001;

#include <QMainWindow>

typedef vector<pair<QString, bool> > TransferFn;

//class FtpWindow : public QMainWindow
//class FtpWindow : public QDialog
class FtpWindow : public QFrame
{
    Q_OBJECT

  signals:
    //void reload(const QUrlInfo &);
public:
    //FtpWindow(QWidget *parent = 0);
    //FtpWindow(QWidget *parent = 0);
    FtpWindow( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~FtpWindow()
    {
      iconv_close(cd_);
    }
    //FtpWindow ( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
  //int get_file(const FileListWidgetItem *fi);
  int get_file(const QString &fn, bool is_dir);
  //int store_file_to_local(ICONV_CONST char *filename);
  int store_file_to_local(const QString &server_path);


private slots:
    int begin_download();
  void do_with_raw_command(int replyCode, const QString & detail);
  void tag_echo_password(int state);
  void state_change(int state);
  void server_big5();
  void server_utf8();
  void client_big5();
  void client_utf8();
  void set_transfer_passive_mode();
  void set_transfer_active_mode();

  void single_selection();
  void contiguous_selection();
  void extended_selection();
  void multi_selection();
  void no_selection();




    void connectOrDisconnect();
    void downloadFile();
    void cancelDownload();

    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem *item, int column);
    void cdToParent();
    void updateDataTransferProgress(qint64 readBytes,
                                    qint64 totalBytes);
    void enableDownloadButton();

private:
    int get_local_path(QString &path);
    void change_encode_fn();
    int get_transfer_file(const QString &cur_path, const TransferFn &fn);
    int make_transfer_dir();
    int download_files();
    QLabel *ftpServerLabel;
    QLineEdit *ftpServerLineEdit;
    QLabel *user_name_lable_, *password_lable_;
    QLineEdit *user_name_, *password_, *port_edit_;
    QLabel *statusLabel, *port_label_;
    QListWidget *fileList;
    QTreeWidget *file_list, *transfer_status_;
    DirTreeView *local_dir_view_;
    QPushButton *quitButton;
    QPushButton *connectButton;
    QPushButton *downloadButton;
    QPushButton *cdToParentButton;
    QProgressDialog *progressDialog;

    //QHash<QString, bool> isDirectory;
    //map<QListWidgetItem*, QString> real_char_map_;
    QString currentPath;
    QFtp *ftp;
    QFile *file;
    QMenu *setting_menu_, *encoding_menu_, *transfermode_, *select_mode_;
    QAction *select_encoding_;
    QAction *server_big5_enc_, *server_utf8_enc_;
    QAction *client_big5_enc_, *client_utf8_enc_;
    QAction *transfermode_passive_, *transfermode_active_;
    QAction *single_selection_, *contiguous_selection_, *extended_selection_, *multi_selection_, *no_selection_;
    QActionGroup *server_enc_, *client_enc_, *transfermode_group_, *select_mode_group_;
    QMenuBar *menubar_;
    int server_cur_enc_, client_cur_enc_;
    QCheckBox *echo_password_;
    QLabel *echo_lable_;
    int operator_mode_;

    // for download
    QList<QTreeWidgetItem *> selected_item_;
    int current_item_index_;
    //vector<pair<QString, bool> > 
    TransferFn transfer_fn_;
    //int pwd_cmd_id_;
    bool list_finish_;
    int list_1_, list_2_, list_id_;
    map<int, QString > cmd_id_cur_path_;
    //map<int, vector<QString> > transfer_filename_;
    vector<QString> transfer_filename_, transfer_dir_fn_;
    int list_time_;
    QTextCodec *tc_[TC_NUM];
    //QUrlInfo cur_url_info_;
    iconv_t cd_;
    QSplitter *splitter_, *vertical_splitter_; 
    QDirModel *dir_model_;
    QTreeView *tree_;
    QString local_path_;
    QTextEdit *ftp_status_;



};

#endif
