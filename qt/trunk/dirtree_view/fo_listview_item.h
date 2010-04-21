#ifndef FO_LISTVIEW_ITEM_H
#define FO_LISTVIEW_ITEM_H

extern const int OK;
extern const int ALREADY_OPEN;
extern const int OPENDIR_FAIL;
extern const int ERROR;

#include <qlistview.h>

#include <cstdio>


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rename_line_edit.h"

#include "big5str_to_qstr.h"

#include <qlistview.h>
#include <qmainwindow.h>
#include <qhbox.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qcursor.h>
#include <qheader.h>

#include <cstring>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;


#define INIT_FO_POPUP(popup, fo_view)\
{\
      popup->insertItem("reload",fo_view, SLOT(reload_item()));\
      popup->insertItem("copy",fo_view, SLOT(rename_item()));\
      popup->insertItem("cut",fo_view, SLOT(rename_item()));\
      popup->insertItem("paste",fo_view, SLOT(rename_item()));\
      popup->insertSeparator();\
      /*popup->insertItem("move",fo_view, SLOT(rename_item()));*/\
      popup->insertItem("delete",fo_view, SLOT(del_item()));\
      popup->insertItem("rename",fo_view, SLOT(rename_item()));\
      popup->insertItem("mkdir",fo_view, SLOT(new_dir()));\
      popup->insertSeparator();\
      popup->insertItem("fo item info",fo_view, SLOT(fo_item_info()));\
}

class FOListViewItem;

/**
 * 這個 class 用來抽象化目錄樹清單和目錄內容清單, 
 * 在目錄樹清單和目錄內容清單提供統一的操作。
 *
 */

class FOListView : public QListView
{
  Q_OBJECT

  public:
    explicit FOListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    int cal_step_size(QListViewItem *item);
    virtual void rename_item_text()=0;

  protected:
    void del_child_item(QListViewItem *i);

  signals:
    void open_new_dir(FOListView *self);
    void cd_new_dir(const std::string &basename);

  protected slots:
    virtual void create_new_dir_input(){} // this create a widget to input new dir name
    void rename_item();
    void rename_item(QListViewItem *i);
    void del_item();
    void rename();
    virtual void reload_item(){}
    virtual void new_dir()=0;
    virtual void get_content(QListViewItem *item)=0; // refresh item
    virtual void reload_content(QListViewItem *item){} // reread directory
    virtual void show_fo_menu()
    {
      qDebug("show fo menu!!");
      QPopupMenu * m = new QPopupMenu( this );
      INIT_FO_POPUP(m, this);
      m->popup(QCursor::pos());
    }
  protected:
    void contentsMousePressEvent( QMouseEvent * e )
    {
      QListView::contentsMousePressEvent( e );
      popup_menu_timer_.start( 750, TRUE );

    }
    void contentsMouseReleaseEvent( QMouseEvent * e )
    {
      QListView::contentsMouseReleaseEvent( e );
      popup_menu_timer_.stop();

    }

    std::vector<std::string> dirs_, files_;

    int open_dir(const std::string &dir_fp);
    void set_reload(bool b){reload_=b;}
    bool reload() {return reload_;}


  protected:
    void set_opendir(const std::string &fp){opendir_=fp;}
    RenameLineEdit *le_;
    FOListViewItem *rename_item_;
  private:
    QTimer      popup_menu_timer_;
    std::string opendir_; 
    bool reload_;

}; // end FOListView

class FOListViewItem : public QListViewItem
{
  public:
    FOListViewItem(FOListView * parent, const QString &label):QListViewItem(parent,label)
    {
      init();
    }
    FOListViewItem(FOListViewItem * parent, const QString &label):QListViewItem(parent,label)
    {
      init();
    }
    FOListViewItem(QListViewItem * parent, const QString &label):QListViewItem(parent,label)
    {
      init();
    }
    ~FOListViewItem()
    {
      free(fn_); 
    }

    // dirname 是這個 item 在那個目錄
    // basename 是 item 的 name
    // get_fullpath 一般來說是 dirname + "/" + basename
    // 若是 basename 為 / 則 get_fullpath 是 /
    // 若是 .. 則 為 dirname 的上一層目錄名稱
    virtual std::string dirname() const=0;
    virtual std::string basename() const;
    virtual std::string get_fullpath() const  // this return value use for 
    {
      if (basename()=="/")
        return "/";
      if (basename()=="../" || basename()=="..")
      {
        std::string parent_dir=dirname();
	std::string::size_type pos=parent_dir.rfind("/");
	if (pos==0) // ex: /aaa, /bbb
          return "/";
	else
          return parent_dir.erase(pos);
      }
      if (dirname()=="/")
        return std::string(dirname() + basename());


      return std::string(dirname() + "/" + basename());
    }
                                 // opendir function argument,
				 // not use for display on the widget
    bool is_open() const {return is_open_;}
    void set_open(bool is_open){is_open_=is_open;}
    void save_fn(const char * fn)
    {
      free(fn_);
      if (fn==0)
        fn_=0;
      else
        fn_=strdup(fn);
    }

    // if this file name cannot convert to unicode for display, record its file name
    bool have_save_fn() const
    {
      if(fn_==0)
        return false;
      else
        return true;
    }


    const char *get_save_fn() const {return fn_;}



  private:
    int compare ( QListViewItem * i, int col, bool ascending ) const;
    void init()
    {
      is_open_=false;
      fn_=0;
    }
    bool is_open_;
    char *fn_; // if this file name cannot convert to unicode for display, record its file name
};

#endif

