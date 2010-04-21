/*
 * if file name not to be convert unicode, I display ? on the widget.
 *
 */



#ifndef DIRTREE_VIEW_H
#define DIRTREE_VIEW_H

#include "embed_pic.h"

#include "grid_thumb.h"
#include "image_widget.h"
#include "fo_listview_item.h"
//#include <opie/odevice.h>
//using namespace Opie;


#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "big5str_to_qstr.h"

#include "encode_dialog.h"

#include <qwidgetstack.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qsplitter.h> 

using namespace std;


inline bool is_dir(const std::string &fp)
{
  struct stat buf;
  if (stat(fp.c_str(), &buf)!=0)
  {
    perror(fp.c_str());
    return false;
  }
  if (S_ISDIR(buf.st_mode))
    return true;
  else
    return false;
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


/**
 * 這用來顯示目錄樹清單
 *
 *
 */


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
    void create_file(QListViewItem *i)
    {
      FOListViewItem *foi_item=(FOListViewItem *)i;
      copy_dir_item(foi_item);

    }
    void del_file(const std::string &fn)
    {
      cout << "in del_file" << endl;
      QListViewItem *i=currentItem();
      for(QListViewItem *cur_item=i->firstChild() ; cur_item; cur_item=cur_item->nextSibling())
      {
        cout << "in loop" << endl;
        if(cur_item->text(0)==QString(fn.c_str()))
	{

          cout << "del : " << fn << endl;
          i->takeItem(cur_item);
	  return;
	}
      }
    }
    virtual void get_content(QListViewItem *item) // refresh item
    {
      gen_dirs(item);
    }
  signals:
    void signal_reload(QListViewItem *);
  private slots:
    void slot_reload(FOListView *fo_lv)
    {
      re_add_child_item_=true;
      FOListViewItem *c_i=(FOListViewItem *)currentItem();
      c_i->set_open(false);
      add_child_item(fo_lv);
      re_add_child_item_=false;
    }
    void add_child_item(FOListView *fo_lv);
    void mark_cur_item(const std::string &bn);
    virtual void create_new_dir_input(){} // this create a widget to input new dir name
  private:
    virtual void reload_item()
    { 
      re_add_child_item_=true;
      FOListViewItem *c_i=(FOListViewItem *)currentItem();
      c_i->set_open(false);
      cout << "emit reload signal" << endl;
      emit signal_reload(currentItem());
      re_add_child_item_=false;

    }
    virtual void new_dir()
    {
      cout << "DirTreeView new_dir" << endl;
    }
    void copy_dir_item(FOListViewItem *i);
    void gen_dirs(QListViewItem *item);
    DirTreeViewItem *root_;
    bool re_add_child_item_;
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
    DirTreeViewItem(QListViewItem * parent, const QString &label):FOListViewItem(parent,label)
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

class FileMonitor;
struct FAMEvent;

/**
 * 這用來顯示目錄內容清單
 *
 *
 */

class DirContent : public FOListView
{
  Q_OBJECT
  public:
    explicit DirContent( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    ~DirContent();
    //void set_fullpath(const std::string &fp){fullpath_=fp;}
    void set_dirname(const std::string &fp){fullpath_=fp;}
    /*
    const std::string &get_fullpath() const 
    {
	    return fullpath_;
    }
    */
    const std::string & dirname() const{return fullpath_;}
    // qt3 have a findItem, but qt3 have nothing, so I write it by myself
    QListViewItem *find_item(const std::string &text, int column);
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
      void create_file(QListViewItem *);
      void del_file(const std::string &dir_name);
      void signal_reload(FOListView *);


  private slots:
    void file_alt(int code);
    void file_alt(FAMEvent *ev);
    void slot_reload(QListViewItem *i)
    {
      set_reload(true); 
      open_another_dir(i);
      set_reload(false); 
    }
    void cd_another_dir(QListViewItem *i)
    {
      std::string basename=((FOListViewItem *)i)->basename();
      // this line should be before gen_dir_content(i);
      // because after gen_dir_content(i);
      // i will be delete
      cout << "in cd_another_dir : " << basename << endl;
      if (gen_dir_content(i)!=0)
        return;
      emit cd_new_dir(basename); // for mark dirtree cur dir item
      emit open_new_dir(this);
    }
    /**
     * 僅是呼叫  DirContent::gen_dir_content(QListViewItem *item);
     * 並送出 open_new_dir(this);
     */

    void open_another_dir(QListViewItem *i)
    {
      if (gen_dir_content(i)!=0)
        return;
      emit open_new_dir(this);
    }
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

    }
    virtual void get_content(QListViewItem *item) // refresh item
    {
      gen_dir_content(item);
    }
  private:
    virtual void reload_item()
    {
      cout << "dn_: " << dirname() << endl;
      set_reload(true); 
      gen_dir_content(dirname() );
      emit signal_reload(this);
    }
    virtual void new_dir();
    int  gen_dir_content(QListViewItem *item);
    int gen_dir_content(const std::string &dir_fp);
    void gen_items(const std::vector<std::string> &str, bool is_dir);
    QListViewItem* gen_items(const std::string &str, bool is_dir);
    void del_items(const std::string &str);
    std::string fullpath_;
    std::string dn_;
    bool opendir_;
    FileMonitor *file_monitor_;
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
  signals:
    void signal_change_view(int);
  private slots:
    void show_dirtree()
    {
      widget_stack_->raiseWidget(dir_content_);
    }
    void show_image()
    {
      //gt_->add_image_thumb("p.png");
      //gt_->add_image_thumb("p1.png");
      gt_->add_image_thumb("12.jpg");
      widget_stack_->raiseWidget(gt_);
    }
    void show_status_bar()
    {
      static bool s=true;
      /*
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
        status_bar_->show();
      }
      */
      s=!s;
      view_->setItemChecked(status_idx_,s);
      if (s)
        status_bar_->show();
      else
        status_bar_->hide();
    }

    void change_view(int id)
    {
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
	    //else
              //cur_item->setOpen(true);

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
    void show_encode_dialog()
    {
      ENC enc=EncodeDialog::get_enc(this);
      if (enc.is_valid==true)
	cout << enc.r << " " << enc.w << endl;
      else
	cout << "no change enc" << endl;
    }
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
    void create_menu();
    void create_select_dir();
    void create_main_widget();
    int cal_step_size(const QListView *lv, QListViewItem *item);
    int bright_;

    QVBox *vbox_;

    DirContent *dir_content_;
    DirTreeView *dirtree_view_;
    //FontSelector *font_selector_;
    QPopupMenu *file_, *options_, *view_;
    RenameLineEdit *le_on_dirtree_, *le_on_dir_content_, *cur_le_;
    FOListViewItem *rename_item_;
    FOListView *cur_list_view_;
    //QHBox *h_box_;
    QComboBox *select_dir_;
    QStatusBar *status_bar_;
    QToolButton *up_, *left_, *right_;
    //ImageWidget *image_widget_;
    ImageThumb *image_widget_;
    int status_idx_, dirtree_idx_, image_idx_;
    QWidgetStack *widget_stack_;
    QVBoxLayout *layout_;
    GridThumb *gt_;
};


bool same_fn_item(const FOListViewItem *foi, const std::string &fn);

#endif
