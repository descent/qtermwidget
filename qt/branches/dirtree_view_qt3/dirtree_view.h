/*
 * if file name not to be convert unicode, I display ? on the widget.
 *
 */



#ifndef DIRTREE_VIEW_H
#define DIRTREE_VIEW_H

#include "embed_pic.h"

//#include <opie/odevice.h>
//using namespace Opie;


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

//#include <qtopia/qcopenvelope_qws.h>
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

#include <cstring>


#include <qlineedit.h>

#include "rename_line_edit.h"

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "big5str_to_qstr.h"

inline bool is_dir(const std::string &fp)
{
  struct stat buf;
  if (stat(std::string(fp).c_str(), &buf)!=0)
  {
    qDebug("stat error");
    return false;
  }
  if (S_ISDIR(buf.st_mode))
    return true;
  else
    return false;
}

#define INIT_FO_POPUP(popup, fo_view)\
{\
      popup->insertItem("reload",fo_view, SLOT(rename_item()));\
      popup->insertItem("copy",fo_view, SLOT(rename_item()));\
      popup->insertItem("cut",fo_view, SLOT(rename_item()));\
      popup->insertItem("paste",fo_view, SLOT(rename_item()));\
      popup->insertSeparator();\
      /*popup->insertItem("move",fo_view, SLOT(rename_item()));*/\
      popup->insertItem("delete",fo_view, SLOT(rename_item()));\
      popup->insertItem("rename",fo_view, SLOT(rename_item()));\
      popup->insertItem("mkdir",fo_view, SLOT(create_new_dir_input()));\
      popup->insertSeparator();\
      popup->insertItem("fo item info",fo_view, SLOT(fo_item_info()));\
    }


inline void init_tool_button(QMenuBar *mb,QToolButton *b, const char *pic_fp)
{
  b->setAutoRaise(true);
  //b->setPixmap(QPixmap(pic_fp));
  b->setPixmap(qembed_findData(pic_fp));
  mb->insertItem(b);
}

class FontSelector;
class ImageWidget;
class DirTreeViewItem;

inline int split(const std::string &str, const char *delimit, std::vector<std::string> &strings)
{
  std::string::size_type pos, cur_pos=0; 

  while((pos=str.find(delimit, cur_pos))!=std::string::npos)
  {
    //std::string s= str.substr(cur_pos, pos-cur_pos);
    if ((pos-cur_pos)!=0) // not save empty string
      strings.push_back(str.substr(cur_pos, pos-cur_pos));
    cur_pos=pos+strlen(delimit);
    //qDebug("in split : %s", s.c_str());
    //qDebug("in split pos: %d", pos);
    //qDebug("in split cur_pos: %d", cur_pos);
    //if (cur_pos > str.length()) break;
  }
  //qDebug("in split cur_pos: %d", cur_pos);
  //qDebug("in split len: %d", str.length());
  if (cur_pos < str.length()) // not save empty string
    strings.push_back(str.substr(cur_pos));

  return 0;
}

inline int split(const QString &str, const char *delimit, std::vector<QString> &strings)
{
  QString s(str);
  int pos;
  pos=s.find(delimit);
  if (pos==0)
  {
    //qDebug("s.length(): %d, pos: %d, delimit: %d", s.length(),pos,strlen(delimit));
    s=s.right(s.length()-strlen(delimit));
  }
  if (pos==-1) // not found
  {
    return 0;
  }
  //qDebug("remain qstr: %s", s.ascii() );
  while((pos=s.find(delimit))!=-1)
  {
    //qDebug("collect: %s", s.left(pos).ascii() );
    strings.push_back(s.left(pos) );
    s=s.right(s.length()-pos - strlen(delimit));
    //qDebug("s.length(): %d, pos: %d, delimit: %d", s.length(),pos,strlen(delimit));
    //qDebug("remain qstr: %s", s.ascii() );
  }
  if (!s.isEmpty())
    strings.push_back(s);

  return 0;
}


class FOListViewItem;

class FOListView : public QListView
{
  Q_OBJECT

  public:
    explicit FOListView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    int cal_step_size(QListViewItem *item);
    virtual void rename_item_text()=0;
  private slots:
    virtual void create_new_dir_input(){} // this create a widget to input new dir name
    void rename_item();
    void rename_item(QListViewItem *i);
    void rename();
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


  protected:
    RenameLineEdit *le_;
    FOListViewItem *rename_item_;
  private:
    QTimer      popup_menu_timer_;
    std::string opendir_;

}; // end FOListView

class DirTreeView : public FOListView
{
  Q_OBJECT
  friend class FileManager;

  public:
    explicit DirTreeView( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    //std::string get_fullpath();
    DirTreeViewItem *root_item() const{return root_;}
    virtual void rename_item_text();
  private slots:
    virtual void get_content(QListViewItem *item) // refresh item
    {
      gen_dirs(item);
    }
  private slots:
    virtual void create_new_dir_input(){} // this create a widget to input new dir name
  private:
    void gen_dirs(QListViewItem *item);
    DirTreeViewItem *root_;
};



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

	      /*
	std::string str=dirname();
	std::string::size_type pos=str.rfind("/");
	str.erase(pos+1);
	*/
	return dirname();
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


class DirTreeViewItem : public FOListViewItem
{
  public:
    DirTreeViewItem(FOListView * parent, const QString &label):FOListViewItem(parent,label)
    {
    }
    DirTreeViewItem(DirTreeViewItem * parent, const QString &label):FOListViewItem(parent,label)
    {
      //init();
    }
    ~DirTreeViewItem()
    {
      //free(fn_); 
    }
    virtual std::string dirname() const;

  private:
};

class DirContent : public FOListView
{
  Q_OBJECT
  public:
    explicit DirContent( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    //void set_fullpath(const std::string &fp){fullpath_=fp;}
    void set_dirname(const std::string &fp){fullpath_=fp;}
    /*
    const std::string &get_fullpath() const 
    {
	    return fullpath_;
    }
    */
    const std::string & dirname() const{return fullpath_;}
    virtual void rename_item_text()
    {
      qDebug("dir content");
      qDebug("rename dir name: %s", rename_item_->dirname().c_str());
      qDebug("rename base name: %s", rename_item_->basename().c_str());
      qDebug("rename fullpath: %s", rename_item_->get_fullpath().c_str());
      std::string big5_str;
      DS::qstr_to_big5str(le_->text(), big5_str); // not chect return value
      if (is_dir(rename_item_->dirname() + "/" + big5_str) )
        rename_item_->setText(0,le_->text()+"/");
      else
        rename_item_->setText(0,le_->text());

    }

    signals:
      void opendir(const std::string &dir_name);
      void cur_dir(const std::string &dir_name); // fullpath

  private slots:
    virtual void create_new_dir_input(); // this create a widget to input new dir name
    virtual void open_next_content(QListViewItem *item)
    {
      qDebug("in open_next_content");
      if (item==0) 
      {
        qDebug("no point a item");
        return;
      }
      std::string dir_fp;
      if (((FOListViewItem*)item)->basename()=="../")
      {
        dir_fp=((FOListViewItem*)item)->dirname();
	std::string::size_type pos=dir_fp.rfind("/");
	dir_fp.erase(pos+1);
      }
      else
      {
        dir_fp=((FOListViewItem*)item)->get_fullpath();
      }
      qDebug("opendir fp: %s", dir_fp.c_str());
      emit opendir(dir_fp);

      // I only let dirtree_view select item->get_fullpath() item
      //get_content(item);


      /*
      if (opendir_)
      {
        emit opendir(dn_.c_str());
        qDebug("dn_: %s", dn_.c_str());
      }
      */

    }
    virtual void get_content(QListViewItem *item) // refresh item
    {
      gen_dir_content(item);
    }
  private:
    void gen_dir_content(QListViewItem *item);
    void gen_dir_content(const std::string &dir_fp);
    void gen_items(const std::vector<std::string> &str, bool is_dir);
    std::string fullpath_;
    std::string dn_;
    bool opendir_;
};

class DirContentViewItem : public FOListViewItem
{
  public:
    DirContentViewItem(FOListView * parent, const QString &label):FOListViewItem(parent,label)
    {
      //init();
    }
    DirContentViewItem(DirContentViewItem * parent, const QString &label):FOListViewItem(parent,label)
    {
      //init();
    }
    ~DirContentViewItem()
    {
      //free(fn_); 
    }

    virtual std::string dirname() const
    {
      return ((DirContent*)listView())->dirname();
    }



  private:
};

class FileManager : public QMainWindow
{
  Q_OBJECT
  //friend class DirTreeView;

  public:
    FileManager(QWidget * parent = 0, const char * name = 0, WFlags f = WType_TopLevel);
  private slots:
    void show_status_bar()
    {
      if(view_->isItemEnabled(view_->idAt(0))==true)
      {
        qDebug("true");
        view_->setItemChecked(view_->idAt(0),false);
        view_->setCheckable(false);
        status_bar_->hide();
      }
      else
      {
        qDebug("not true");
        view_->setItemChecked(view_->idAt(0),true);
        status_bar_->show();
      }
    }



    void modify_caption(const std::string &dir_name)
    {
      QString qstr;
      // this should check return value, because maybe return error
      DS::big5str_to_qstr(dir_name, qstr);

      setCaption(qstr);
    }
    void change_dirtreeview_item(const std::string &dir_name)
    {
      if (dir_name=="/")
      {
        dirtree_view_->setCurrentItem(dirtree_view_->root_item());
        //dirtree_view_->setSelected(dirtree_view_->root_item(), true);
        dirtree_view_->ensureItemVisible(dirtree_view_->root_item() );
      }

      std::vector<std::string> big5_strings;
      split(dir_name, "/", big5_strings);

      QListViewItem* first_child=dirtree_view_->root_item()->firstChild();
      if (first_child==0)
      {
        // gen directory items
        qDebug("gen directory items");
        dirtree_view_->gen_dirs(dirtree_view_->root_item());
        first_child=dirtree_view_->root_item()->firstChild();
      }
      // if that item don't exist, need gen it.
      for (size_t i=0 ; i < big5_strings.size() ; ++i)
      {

        for (QListViewItem* cur_item=first_child ; cur_item ; cur_item=cur_item-> nextSibling () )
	{
          //qDebug("cur item: %s", cur_item->text(0).ascii());

          if (((FOListViewItem *)cur_item)->basename()==big5_strings[i])
	  {
            //qDebug("find item: %s", cur_item->text(0).ascii());

            dirtree_view_->gen_dirs(cur_item);

	    if (i==big5_strings.size()-1)
	    {
              //dirtree_view_->setSelected(cur_item, true);
              dirtree_view_->setCurrentItem(cur_item);
              dirtree_view_->ensureItemVisible( cur_item );
	    }
	    else
              cur_item->setOpen(true);

	    first_child=cur_item->firstChild();
	    break;
	  }
	  //else // not found the item
           // return;
	}
        //qDebug("%d: %s", i, strings[i].ascii());

      } // end for loop
    }
    // for Combobox
    void change_dirtreeview_item(const QString &dir_name)
    {
      qDebug("d name: %s", dir_name.ascii());

      std::string big5_str;
      std::vector<std::string> big5_strings;
      DS::qstr_to_big5str(dir_name, big5_str);
      change_dirtreeview_item(big5_str);

      //std::vector<QString> strings;
      //split(dir_name, "/", strings);

    }

    void select_font();
    void rename_item();
    void fo_item_info();
    void screen_off()
    {
//ODevice::inst ( )-> setDisplayBrightness ( -3 );
//ODevice::inst ( )-> setDisplayStatus ( false );
      #ifndef QT_NO_COP
        //QCopEnvelope e("QPE/System", "setBacklight(int)" );
        //e << 0;
        //e << -1;
      #endif
    }
    void screen_on()
    {
      #ifndef QT_NO_COP
        //QCopEnvelope e("QPE/System", "setBacklight(int)" );
        //e << 0;
        //e << 255;
      #endif
    }
    void cd_up()
    {
      
      std::string up_dir=dir_content_->dirname();
      if (up_dir[up_dir.length()]=='/')
        up_dir.erase(up_dir.length());
      std::string::size_type pos=up_dir.rfind("/");
      up_dir.erase(pos+1); // let /abc/xyz => /abc/, so /aaa => /
      change_dirtreeview_item(up_dir);
    }

    int rename();
  private:
    int cal_step_size(const QListView *lv, QListViewItem *item);
    int bright_;

    DirContent *dir_content_;
    DirTreeView *dirtree_view_;
    FontSelector *font_selector_;
    QPopupMenu *file_, *options_, *view_;
    RenameLineEdit *le_on_dirtree_, *le_on_dir_content_, *cur_le_;
    FOListViewItem *rename_item_;
    FOListView *cur_list_view_;
    QHBox *h_box_;
    QComboBox *select_dir_;
    QStatusBar *status_bar_;
    QToolButton *up_, *left_, *right_;
    ImageWidget *image_widget_;
};



#endif
