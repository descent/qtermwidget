#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTreeWidget>
#include <QSplitter>


#include <map>

using namespace std;

#if 1
//QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QTextEdit;
class QTabWidget;
class QComboBox;
class QLineEdit;
class QPushButton;
class BrowserWindow;

#include <QTextEdit>
#include <QGroupBox>
#include <QFile>
#include <QDomDocument>

#include <QTabWidget>

//QT_END_NAMESPACE
#endif

#include "route_item.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#if 0
struct MapAttribute
{
  QString name, desc, clickable, width, opacity;
  QString outline_color, outline_width, fill_color, fill_opacity;
  int color; // a index, not color number
  QColor qc;
  QString points;
};
#endif

typedef vector<MapAttribute> FileTrkAttr;
//typedef map<MapAttribute> FileTrkAttr;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    void create_form_groupbox();
    void backup_file();
    QTextEdit *text_edit_;
    QTextEdit *route_attr_;
    void search_all(QDomNode &n, const QString &tag_name);
    void get_wpt(const QDomNode &node, WptAttribute &wpt_attr);
    void get_trk_points(QDomNode &n, const QString &tag_name, QString &points);
    void get_trk_name(QDomNode &node, QString &trk_name);
    void get_trk_info(QDomNode &n, const QString &tag_name);
    void get_points(const QDomDocument &dom_doc);
    QString check_gpx_type(const QDomDocument &dom_doc);
    void get_trk(const QString &fn, int index);
    FileTrkAttr* get_cur_file_trk_attr();



  private:
    //QString preview_fn_;
    QSplitter *splitter_;
    QTreeWidget *route_view_, *select_route_view_;
    BrowserWindow *browser_;
    QStringList fn_list_;
    QPushButton *select_route_button_;
    QPushButton *preview_button_;
    QPushButton *rv_add_; // select route view add new route
    QPushButton *sel_rv_remove_all_, *sel_rv_remove_; // select route view
    QPushButton *rv_remove_all_, *rv_remove_;
    QPushButton *rv_save_to_html_;
    QFile qf_;
    QString points_;
    QString center_point_;
    //QComboBox *files_, *track_list_;
    QLineEdit *route_name_, *google_map_key_, *point_, *position_, *direction_;
    QGroupBox *formGroupBox, *sel_group_box_;
    QMenu *file_menu_, *edit_menu_, *help_menu_;
    QAction *orientation_;
    QAction *about_;
    QMenu *setting_menu_;
    QAction *change_font_, *show_debug_log_;
 
    QByteArray qba_;
    QString save_dirname_, dirname_, basename_, file_name_, backup_fn_;
    int color_index_;
    QDomDocument dom_doc_;
    map<QString, FileTrkAttr*> file_trk_attr_;
    void update_no(QTreeWidget *view, int index);


    void create_html_file(QByteArray &template_data);
    void fill_data(int offset=0x4e10);
    int write_to_save_file(const QString &w_fn);
    void open_cfg();
    QString preview_fn_;

  protected:
    void closeEvent ( QCloseEvent * event );

  public slots:
    void switch_orientation_slot();
    void modify_route_name_slot(QTreeWidgetItem * item, int column);
    void rv_save_to_html_slot();
    void select_route_slot();
    void rv_remove_all_slot();
    void rv_remove_slot();
    void sel_rv_remove_all_slot();
    void sel_rv_remove_slot();
    void open_color_dialog(QTreeWidgetItem * item, int column);
    void select_gpx_file(int index);
    void open_file_slot();
    //void save_file_slot();
    //void save_as_slot();
    void change_save_file_offset ( int index );
    void preview_without_save_slot();


  private slots:
    bool close();
    void change_font_slot();
    void show_debug_log_slot();
    void about_slot();
#if 0
    QAction *copy_, *paste_;
    QAction *change_bg_color_;
    QAction *set_encode_;
    QAction *big5_enc_, *utf8_enc_;
    QMenu *encoding_menu_;
    QActionGroup *input_enc_g_;
    QActionGroup *enc_g_;

    void switch_tab_slot(int tab_index);
    void close_tab_slot();
    void big5_enc();
    void utf8_enc();
    void copy_slot();
    void paste_slot();
    bool close();
    void change_bg_color_slot();
    void set_encode_slot();
    //virtual ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    //void change_font_slot();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    //void change_font_slot();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QTextEdit *textEdit;
    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    //QMenu *setting_menu_;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    //QAction *change_font_;
#endif
};

#endif
