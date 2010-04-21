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
#include <QTimer>

#include <QtGui>

#include <QSplitter>

#include <QTreeWidgetItemIterator>
#include <QTreeWidgetItem>
#include <QTreeWidget>

#include <iconv.h>

#include <vector>
#include <map>


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
class DirTreeView;
class FtpTransfer;
class FtpServerList;
class FileList;
class FtpBookmark;
class FtpSiteItem;

using namespace Qt;
using namespace std;

//extern const char *encoding_name[];
extern const int BIG5;
extern const int UTF8;
const int TC_NUM=2;


const int LIST_MODE=1000;
const int DOWNLOAD_MODE=1001;

#include <QMainWindow>

typedef vector<pair<QString, bool> > TransferFn;


//class FtpWindow : public QMainWindow
//class FtpWindow : public QDialog
class FtpWindow : public QFrame
{
  //friend class DirTreeView;
  friend class FtpTransfer;
    Q_OBJECT

  signals:
    void get_complete();
public:
    //FtpWindow(QWidget *parent = 0);
    //FtpWindow(QWidget *parent = 0);
    FtpWindow( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~FtpWindow();
    //FtpWindow ( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
  //int get_file(const FileListWidgetItem *fi);
  int get_file(const QString &fn, bool is_dir, const QString &size);
  //int store_file_to_local(ICONV_CONST char *filename);
  //int store_file_to_local(const QString &server_path);
  int store_file_to_local(const QString &server_path, const QString &local_path);


  public slots:
    void show_throughput();
    void s_connect(const FtpSiteItem *item);
    void downloadFile();
    void s_add_bookmark();
    void s_edit_bookmark();
    void s_local_dir_tree_win_show();
    void s_local_file_list_win_show();
    void s_ftp_server_file_list_win_show();
    void s_ftp_status_win_show();
    void s_transfer_status_win_show();
    void s_connect_field_show();


  private slots:
    void s_command_started ( int id );
    //int begin_download();
  void get_file_data();
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
    void cancelDownload();

    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem *item, int column);
    void cdToParent();
    void updateDataTransferProgress(qint64 readBytes,
                                    qint64 totalBytes);
    void enableDownloadButton();

  private:
    bool is_connect();
    int if_convert_encoding_by_iconv(const QString &l_path, QString &enc_str);
    int get_enc_fn(const QString &org_fn, QString &enc_fn);
    int get_local_path(QString &path);
    void change_encode_fn();
    //int get_transfer_file(const QString &cur_path, const TransferFn &fn);
    int make_transfer_dir();
    int download_files();
    void disconnect();
    QLabel *ftpServerLabel;
    QLineEdit *ftpServerLineEdit;
    QLabel *user_name_lable_, *password_lable_;
    QLineEdit *user_name_, *password_, *port_edit_;
    QLabel *statusLabel, *port_label_;
    //QListWidget *fileList;
    FtpTransfer *transfer_status_;
    FtpServerList *file_list;
    DirTreeView *local_dir_view_;
    FileList *local_file_view_;
    //QPushButton *quitButton;
    //QPushButton *downloadButton;
    QPushButton *connectButton;
    QPushButton *cdToParentButton;
    QProgressDialog *progressDialog;

    //QHash<QString, bool> isDirectory;
    //map<QListWidgetItem*, QString> real_char_map_;
    QString currentPath;
    QFtp *ftp;
    QFile *file;
    QMenu *setting_menu_, *encoding_menu_, *transfermode_, *select_mode_;
    QMenu *bookmark_menu_;

    QMenu *window_menu_;
    QAction *connect_field_win_, *local_dir_tree_win_, *local_file_list_win_, *ftp_server_file_list_win_, *ftp_status_win_, *transfer_status_win_;

    QAction *select_encoding_;
    QAction *server_big5_enc_, *server_utf8_enc_;
    QAction *client_big5_enc_, *client_utf8_enc_;
    QAction *transfermode_passive_, *transfermode_active_;
    QAction *single_selection_, *contiguous_selection_, *extended_selection_, *multi_selection_, *no_selection_;
    QAction *edit_bookmark_, *add_bookmark_;
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
    vector<QString> transfer_dir_fn_;
    //vector<QString> transfer_filename_;
    int list_time_;
    QTextCodec *tc_[TC_NUM];
    //QUrlInfo cur_url_info_;
    iconv_t cd_;
    QSplitter *splitter_, *vertical_splitter_; 
    QDirModel *dir_model_;
    QTreeView *tree_;
    QString local_path_;
    QTextEdit *ftp_status_;
    FtpBookmark *ftp_bookmark_;
    FILE *fs_;
    int total_len_;
    QHBoxLayout *topLayout;
    QTimer *show_throughput_timer_;
    int throughput_counter_, cur_size_;
    int diff_byte_;
};

#endif
