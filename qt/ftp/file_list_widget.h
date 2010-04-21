/*
 * This class to recode filename item
 *
 */
#include <QListWidgetItem>
#include <QListWidget>


#include <QTreeWidgetItemIterator>
#include <QTreeWidgetItem>
#include <QTreeWidget>

#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDir>

#include "tree_view_popup_menu.h"


#define QDIR_SEPARATOR QString(QDir::separator())

using namespace Qt;

class RenameDialog;

class FileWidget: public QTreeWidget
{
  Q_OBJECT
  public:
    FileWidget(QWidget * parent = 0):QTreeWidget(parent){
}
  public slots:

  protected:
    
};

class FileListWidgetItem : public QTreeWidgetItem
{
  //Q_OBJECT
  public:
    FileListWidgetItem( QTreeWidget * parent, int type = Type ):QTreeWidgetItem(parent, type){is_dir_=false;}
    FileListWidgetItem( QTreeWidgetItem * parent, int type = Type ):QTreeWidgetItem(parent, type){is_dir_=false;}
    bool operator< ( const QTreeWidgetItem & other ) const;
    void set_real_str(const QString &str){real_str_=str;}
    void set_real_str2(const QString &str){real_str2_=str;}
    QString get_real_str() const {return real_str_;}
    QString get_real_str2() const {return real_str2_;}
    void set_total_bytes(int len){ total_bytes_ = len; }
    int get_total_bytes(){ return total_bytes_;}

#if 0
    FileListWidgetItem( const QString & text, QListWidget * parent = 0, int type = Type ){}
    FileListWidgetItem ( const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type ){}
    FileListWidgetItem( const QListWidgetItem & other ){}
#endif
    bool is_dir() const {return is_dir_;}
    void set_dir(bool t){is_dir_=t;}
  private:
    bool is_dir_;
    QString real_str_, real_str2_; // recode non encoding ascii code for file name.
    int encode_;
    int total_bytes_; // for record a file size
    
};

class DirTreeView : public TreeViewPopupMenu
{
  Q_OBJECT
  public:
    DirTreeView( QWidget * parent = 0);
    int get_select_path(QString &path);
  public slots:
    void expend_dirs( QTreeWidgetItem * item, int column ) ;
    void s_rm();
#if 0
    void show_fo_menu()
    {
      qDebug("show_fo_menu");
      //QMenu * m = new QMenu(this);
      //QAction *rename = new QAction("rename", this);
      //m->addAction(rename);
      //m->popup(QCursor::pos());
      TreeViewPopupMenu::show_fo_menu();



    }
#endif
  protected:
    void expend_dirs_unix( QTreeWidgetItem * item, int column ) ;

#if 0
    void contentsMousePressEvent( QMouseEvent * e )
    {
      //QTreeWidget::contentsMousePressEvent( e );
      //popup_menu_timer_.start( 750, TRUE );

    } 
    void contextMenuEvent(QContextMenuEvent *event)
    {
        QMenu menu(this);
	qDebug("123");
	QAction *c=new QAction("test", this);
	    menu.addAction(c);
	        //menu.addAction(copyAct);
		    //menu.addAction(pasteAct);
		        menu.exec(event->globalPos());


    }
#endif

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
#endif


  private:
    QAction *remove_;
    RenameDialog *ok_dialog_;


    //QTimer *popup_menu_timer_;

};



class FileList : public TreeViewPopupMenu
{
  Q_OBJECT
  public:
    FileList( QWidget * parent = 0);
    int cur_dir(QString &cur_dir) const;
  public slots:
    void change_dir( QTreeWidgetItem * item, int column );
    void change_dir(const QString &cd_dir);
    void change_dir(const QFileInfoList &file_info_list);
    void change_dir(const char *dir_path);
    void s_rename();
    void s_cd_parent_dir();
  private:
    void change_title_to_cur_path();
    void list_first_layer_dir();
  private:
    // if in win32 system
    // cur_dir_ init to ""
    // else init to QDIR_SEPARATOR 
    // if cur_dir_ is "" mean win32 and list win32 drives
    // else list unix "/"
    QString cur_dir_;	
    QAction *rename_, *cd_parent_dir_;

};
