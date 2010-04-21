#include "dirtree_view.h"





#include "font_selector.h"
#include "image_widget.h"

#include <qheader.h>
#include <qlayout.h>

//#include <qsplitter.h> // this version not include this widget

#include <cstdio> // rename




const char *dir_err_msg(int no)
{
  switch (no)
  {
    case EACCES :
    {
       return "Permission denied.";
    }
    case EMFILE:
    {
       return "Too many file descriptors in use by process.";
    }
  }
/*
	             ENFILE Too many files are currently open in the system.

		            ENOENT Directory does not exist, or name is an empty string.

			           ENOMEM Insufficient memory to complete the operation.

				          ENOTDIR
					  */
  return "";
}


FOListView::FOListView( QWidget * parent , const char * name , WFlags f ):QListView(parent, name, f), le_(0), popup_menu_timer_(this)
{
  setFont(QFont("unifont"));
  connect( &popup_menu_timer_, SIGNAL( timeout() ), SLOT( show_fo_menu() ) );
  setSorting(-1,0);
}

int FOListView::cal_step_size(QListViewItem *item)
{
  QListViewItem *cur_item=item;
  int parent=0;
  while(cur_item->parent())
  {
    ++parent;
    cur_item=cur_item->parent();
  }
  return treeStepSize() * parent;
}

void FOListView::rename()
{

  horizontalScrollBar()->setEnabled(true);
  verticalScrollBar()->setEnabled(true);
  le_->hide();
  std::string new_name;
  DS::qstr_to_big5str(le_->text(), new_name);
  qDebug("old name: %s", rename_item_->get_fullpath().c_str());
  qDebug("new name: %s", (rename_item_->dirname() + "/" + new_name).c_str());
  ::rename(rename_item_->get_fullpath().c_str(), (rename_item_->dirname() + "/" + new_name).c_str());
  rename_item_text();
}

void FOListView::rename_item(QListViewItem *i)
{
  if (le_==0)
  {
    le_ = new RenameLineEdit(this);
    connect(le_, SIGNAL(lost_focus()), this, SLOT(rename()));
  }

  rename_item_=(FOListViewItem *)i;

  qDebug("in rename_item(QListViewItem *i)");
  int header_height = header()->height();
  QRect r = itemRect(i);
  r.setTop( r.top() + frameWidth() + 1 + header_height);
  r.setBottom( r.bottom() + frameWidth() + header_height);
  r.setLeft( r.left() + cal_step_size(i));
  //r.setWidth( current( 0 ) );
  ensureItemVisible(i);
  horizontalScrollBar()->setValue(0);
  horizontalScrollBar()->setEnabled(false);
  verticalScrollBar()->setEnabled(false);
  le_->setGeometry( r );
  QString fn=i->text(0);
  if (fn[fn.length()-1]=='/')
    fn.remove(fn.length()-1,1);
  le_->setText(fn);
  le_->selectAll();
  le_->show();
  le_->setFocus();
}

void FOListView::rename_item()
{
  qDebug("in FOLostView");
  SelectionMode sm=selectionMode();
  if (sm==QListView::Single) // in dirtreeview
  {
    rename_item(selectedItem());
  }
  if (sm==QListView::Multi) // in dircontentview
  {
    FOListViewItem *i=(FOListViewItem *)firstChild();
    for (; i ; i=(FOListViewItem *)(i->nextSibling()) )
    {
      if(isSelected(i))
      {
        qDebug("select: %s", i->basename().c_str());
        rename_item(i);
      }
    }
  }


}

const int OK=0;
const int ALREADY_OPEN=1;
const int OPENDIR_FAIL=2;
const int ERROR=3;


int FOListView::open_dir(const std::string &dir_fp)
{
  // opendir_. dir_fp is fullpath
  if (opendir_==dir_fp) return ALREADY_OPEN;

  DIR * dir;

  qDebug("opendir: %s", dir_fp.c_str() );
  dir=opendir(dir_fp.c_str() );

  if (dir==NULL)
  {
    perror(dir_fp.c_str());
    dirs_.clear();
    files_.clear();
    return OPENDIR_FAIL;
  }

  struct dirent *dir_ent;

  dirs_.clear();
  files_.clear();
  while((dir_ent=readdir(dir))!=0)
  {
    struct stat buf;
    if (strcmp(dir_ent->d_name,"?")==0)
    {
      qDebug("?");
      continue;
    }

    if (stat(std::string(dir_fp  + "/" + dir_ent->d_name).c_str(), &buf)!=0)
    {
      qDebug("stat error");
      continue;
    }
    if (S_ISDIR(buf.st_mode))
      dirs_.push_back(dir_ent->d_name);
    else
      files_.push_back(dir_ent->d_name);
  }
  opendir_=dir_fp;

  closedir(dir);
  std::sort(dirs_.begin(), dirs_.end(), std::greater<std::string >());
  std::sort(files_.begin(), files_.end(), std::greater<std::string >());
  return OK;
}

DirTreeView::DirTreeView( QWidget * parent , const char * name , WFlags f  ):FOListView(parent,name,f)
{
  addColumn("Dir");

  connect(this,SIGNAL(clicked ( QListViewItem *)),SLOT(get_content(QListViewItem *) ) );

  //new DirTreeViewItem(this, "/");
  //DirTreeViewItem *aa=new DirTreeViewItem(this, "aa");
  //QListViewItem *aa=new QListViewItem (this, "aa");
  root_=new DirTreeViewItem (this, "/");
  setCurrentItem(root_);
  /*
  DirTreeViewItem *aa=new DirTreeViewItem(root, "aa");
  new DirTreeViewItem(aa, "a1");
  new DirTreeViewItem(aa, "b1");
  new DirTreeViewItem(aa, "c1");
  new DirTreeViewItem(root, "a");
  new DirTreeViewItem(root, "b");
  new DirTreeViewItem(root, "c");
  */
  //DirTreeViewItem *root=new DirTreeViewItem(this, q);



}
void DirTreeView::rename_item_text()
{
      qDebug("dir tree");
      rename_item_->setText(0,le_->text());
}

/*
std::string DirTreeView::get_fullpath()
{
  return ((DirTreeViewItem *) currentItem())->get_fullpath();
}
*/


std::string DirTreeViewItem::dirname() const
{

  if (parent()==0)
    return "/";	   // if item is "/" dirname return "/"

  const FOListViewItem *cur_item=this;

  std::string fullpath;

  while(cur_item->parent() )
  {
    cur_item=(FOListViewItem*)(cur_item->parent());
    std::string str;
    if (cur_item->have_save_fn()==0)
      DS::qstr_to_big5str(cur_item->text(0), str);
    else
      str=cur_item->get_save_fn();

    if (str!="/")
      fullpath= std::string("/") + str + fullpath;
    //if (cur_item->parent()==0) break;
    //qDebug("str: %s", str.c_str());
  }
  if (fullpath.empty()) return "/";
  else return fullpath;
}

std::string FOListViewItem::basename() const
{
  std::string str;

  if (have_save_fn()==false)
  {
    DS::qstr_to_big5str(text(0), str);
    return str;
  }
  else
    return get_save_fn();

}
#if 0
std::string DirTreeViewItem::get_fullpath()
{
  DirTreeViewItem *cur_item=this;
  std::string fullpath ;
  if (cur_item->have_save_fn()==0)
    DS::qstr_to_big5str(cur_item->text(0), fullpath);
  else
    fullpath=cur_item->get_save_fn();


  while(cur_item->parent() )
  {
    cur_item=(DirTreeViewItem*)(cur_item->parent());
    std::string str;
    if (cur_item->have_save_fn()==false)
      DS::qstr_to_big5str(cur_item->text(0), str);
    else
      str=cur_item->get_save_fn();

    fullpath= str + "/" + fullpath;
    //qDebug("str: %s", str.c_str());
  }
  return fullpath;
}
#endif

void DirTreeView::gen_dirs(QListViewItem *item)
{
  qDebug("in gen_dirs");
  if (item==0) 
  {
    qDebug("no point a item");
    return;
  }


  DirTreeViewItem *cur_item=(DirTreeViewItem*)item;
  /*
  DirTreeViewItem *cur_item=dynamic_cast<DirTreeViewItem*>(item);
  if (cur_item==0)
  {
    qDebug("cannot type cast to DirTreeViewItem *");
  }
  */

  /*
  root_=cur_item;
  new QListViewItem(item,"a");
  new QListViewItem(item,"b");
  new QListViewItem(item,"c");
  return;
  */
   

  //DirTreeViewItem *cur_item=(DirTreeViewItem*)item;
  qDebug("dirname: %s", cur_item->dirname().c_str());
  qDebug("basename: %s", cur_item->basename().c_str());


  if ( cur_item -> is_open()) 
  {
    qDebug("%s already open", cur_item->get_fullpath().c_str());
    return;
  }

  if (open_dir(cur_item->get_fullpath().c_str())==OPENDIR_FAIL) return;

  for (size_t i=0 ; i < dirs_.size() ; ++i)
  {
    if (dirs_[i]=="." || dirs_[i]=="..") continue;

    QString qstr;
    if (DS::big5str_to_qstr(dirs_[i], qstr)!=0)
    {
      DirTreeViewItem* node=new DirTreeViewItem(cur_item, "?");
      qDebug("d_name: %s", dirs_[i].c_str());
      node->save_fn(dirs_[i].c_str());
    }
    else
      new DirTreeViewItem(cur_item, qstr);
  }

#if 0
  DIR * dir;
  dir=opendir(cur_item->get_fullpath().c_str() );
  if (!dir)
  {
    qDebug("opendir fail");
    return;
  }

  struct dirent *dir_ent;
  while((dir_ent=readdir(dir)))
  {
    struct stat buf;
    stat((std::string(cur_item->get_fullpath()  + "/" + dir_ent->d_name)).c_str(), &buf);
    if (strcmp(dir_ent->d_name,".")==0 || strcmp(dir_ent->d_name,"..")==0)
      continue;
    if (S_ISDIR(buf.st_mode))
    {
      QString qstr;
      if (DS::big5str_to_qstr(dir_ent->d_name, qstr)!=0)
      {
        DirTreeViewItem* node=new DirTreeViewItem(cur_item, "?");
	qDebug("d_name: %s",dir_ent->d_name);
	node->save_fn(dir_ent->d_name);
      }
      else
        new DirTreeViewItem(cur_item, qstr);
    }

    //dir_content.push_back(dir_ent->d_name);
  }
  closedir(dir);
#endif
  cur_item -> set_open(true);

  //setOpen(item, true);

}

DirContent::DirContent( QWidget * parent , const char * name , WFlags f ):FOListView(parent,name,f)
{
  setFont(QFont("unifont"));
  addColumn("Name");
  setSelectionMode(QListView::Multi);

  connect(this, SIGNAL( doubleClicked ( QListViewItem * ) ), this, SLOT(open_next_content(QListViewItem *) ));
}

// this create a widget to input new dir name
void DirContent::create_new_dir_input()
{
  
}


void DirContent::gen_items(const std::vector<std::string> &str, bool is_dir)
{
  for (size_t i=0 ; i < str.size() ; ++i)
  {
    QString qstr;
    bool can_unicode=true;

    if (DS::big5str_to_qstr(str[i], qstr)!=0) // convert to unicode error
    {
       can_unicode=false;
       qstr="?";
    }

    if (is_dir) qstr = qstr + "/";

    DirContentViewItem *node = new DirContentViewItem(this, qstr);
    if (can_unicode==false)
      node->save_fn(str[i].c_str());

  }
}

void DirContent::gen_dir_content(const std::string &dir_fp)
{
  qDebug("in gen_dirs_content(const std::string &dir_fp)");


  if (open_dir(dir_fp.c_str())==OPENDIR_FAIL) 
  { 
    opendir_=false;
    return;
  }
  opendir_=true;

  clear();
  gen_items(files_, false);
  gen_items(dirs_, true);


  set_dirname(dir_fp);
  qDebug("set_dirname: %s", dir_fp.c_str());
}

void DirContent::gen_dir_content(QListViewItem *item)
{
  qDebug("in gen_dirs_content");
  if (item==0) 
  {
    opendir_=false;
    qDebug("no point a item");
    return;
  }

  FOListViewItem *cur_item=(FOListViewItem*)item;

  /*
  if ( cur_item -> is_open()) 
  {
    qDebug("already open");
    return;
  }
  */

  qDebug("dirname: %s", cur_item->dirname().c_str());
  qDebug("basename: %s", cur_item->basename().c_str());
  qDebug("qstring basename: %s", cur_item->text(0).ascii());


  std::string opendir_fp=(cur_item->get_fullpath());

  if (open_dir(opendir_fp.c_str())==OPENDIR_FAIL) 
  { 
    opendir_=false;
    return;
  }
  opendir_=true;

  /*
  if (cur_item->basename()=="../")
    dn_=cur_item->dirname();
  else
  */
    dn_=cur_item->get_fullpath();

  // because clear() will delete all item, so after claer(),
  // don't use item object;
  clear();
  gen_items(files_, false);
  gen_items(dirs_, true);


  set_dirname(dn_);
  qDebug("set_dirname: %s", dn_.c_str());
  emit cur_dir(dn_);

  //parent()->parent()->setCaption(dn.c_str()); // not yet convert to unicode
  //qApp->setCaption(dn.c_str()); // not yet convert to unicode


    //if (S_ISDIR(buf.st_mode))
     // qstr=qstr + "/";


#if 0
  DIR * dir;
  
  dir=opendir(opendir_fp.c_str());

  if (!dir)
  {
    qDebug("opendir fail: %s", opendir_fp.c_str());
    perror(opendir_fp.c_str());
    //dir_err_msg();
    return;
    //return -1;
  }


  //std::string s=cur_item->dirname() + "/" + cur_item->basename();
  std::string s=cur_item->get_fullpath();
  set_dirname(s);
  qDebug("set_dirname: %s", s.c_str());



  struct dirent *dir_ent;
  while((dir_ent=readdir(dir))!=0)
  {
    struct stat buf;
    if (strcmp(dir_ent->d_name,"?")==0)
    {
      qDebug("?");
      continue;
    }

    if (stat(std::string(opendir_fp  + "/" + dir_ent->d_name).c_str(), &buf)!=0)
    {
      qDebug("stat error");
      continue;
    }

    QString qstr;
    bool can_unicode=true;

    if (DS::big5str_to_qstr(dir_ent->d_name, qstr)!=0) // convert to unicode error
    {
       can_unicode=false;
       qstr="?";
    }

    if (S_ISDIR(buf.st_mode))
      qstr=qstr + "/";

    DirContentViewItem *node = new DirContentViewItem(this, qstr);
    if (can_unicode==false)
      node->save_fn(dir_ent->d_name);


    //dir_content.push_back(dir_ent->d_name);
  }
  closedir(dir);
#endif
}

FileManager::FileManager(QWidget * parent , const char * name , WFlags f ):QMainWindow(parent,name,f)
{

  QVBoxLayout *layout = new QVBoxLayout( this );
  layout->setSpacing(2);
  layout->setMargin(2);

  h_box_ = new QHBox(this);
  dirtree_view_ = new DirTreeView(h_box_);
  //dirtree_view_->hide();
  dir_content_ = new DirContent(h_box_);
  //dir_content_->hide();
  //image_widget_ = new ImageWidget(h_box_);
  //setCentralWidget(h_box_);
  font_selector_ = new FontSelector();

  dirtree_view_->setGeometry(0,0,width(),height()*2);
  dir_content_->setGeometry(width(),0,width(),height()*2);
  //image_widget_->setGeometry(0,0,width(),height()*3);

  le_on_dirtree_ = new RenameLineEdit(dirtree_view_);
  le_on_dirtree_ -> hide();

  le_on_dir_content_ = new RenameLineEdit(dir_content_);
  le_on_dir_content_ -> hide();

  connect(le_on_dirtree_, SIGNAL( lost_focus() ), SLOT( rename() ) );
  connect(le_on_dirtree_, SIGNAL(returnPressed()), this, SLOT(rename()));

  connect(le_on_dir_content_, SIGNAL( lost_focus() ), SLOT( rename() ) );
  connect(le_on_dir_content_, SIGNAL(returnPressed()), this, SLOT(rename()));




  QObject::connect(dirtree_view_, SIGNAL(clicked ( QListViewItem *) ), dir_content_, SLOT(get_content(QListViewItem *) ));

  connect(dirtree_view_,SIGNAL(currentChanged ( QListViewItem *)), dir_content_, SLOT(get_content(QListViewItem *) ) );

  QObject::connect(dir_content_, SIGNAL(cur_dir ( const std::string &) ), this, SLOT(modify_caption(const std::string &) ));

  QMenuBar *menubar = new QMenuBar(this);
  //QPEToolBar* toolBar = new QPEToolBar( this );
  //toolBar->setHorizontalStretchable( TRUE );


  //QPEMenuBar* menuBar = new QPEMenuBar( toolBar );

  //layout->addWidget(toolBar);
  layout->addWidget(menubar);


  file_ = new QPopupMenu( this );  
  menubar->insertItem( tr( "File" ), file_ );

  INIT_FO_POPUP(file_, this);


  view_ = new QPopupMenu( this );  

  menubar->insertItem( tr( "View" ), view_ );

  view_->insertItem(tr("status bar"),this, SLOT(show_status_bar()));
  //view_->setCheckable(true);
  view_->setItemChecked(view_->idAt(0),false);

  options_ = new QPopupMenu( this );  
  menubar->insertItem( tr( "Options" ), options_ );
  options_->insertItem("Font",this, SLOT(select_font()));

  up_ = new QToolButton(0,"up");
  left_ = new QToolButton(0,"left");  
  right_ = new QToolButton(0,"right"); 

  init_tool_button(menubar,up_,"up.png");
  init_tool_button(menubar,left_,"left.png");
  init_tool_button(menubar,right_,"right.png");
   
  connect(up_, SIGNAL(clicked() ) , this, SLOT(cd_up()));
  connect(left_, SIGNAL(clicked() ) , this, SLOT(screen_off()));
  connect(right_, SIGNAL(clicked() ) , this, SLOT(screen_on()));

  QHBox *line_box = new QHBox( this );
  layout->addWidget(line_box);

  layout->addWidget(h_box_);

  select_dir_ = new QComboBox(line_box);
  select_dir_->insertItem("/");
  select_dir_->insertItem("/home/");
  select_dir_->insertItem("/etc/");
  select_dir_->insertItem("/mnt");
  select_dir_->insertItem("/root");
  select_dir_->insertItem("/home/descent/12/");
  select_dir_->insertItem("/home/descent/aaa/");

  connect(select_dir_, SIGNAL(activated ( const QString &) ), this, SLOT(change_dirtreeview_item(const QString & )) );

  connect(dir_content_, SIGNAL( opendir ( const std::string &) ), this, SLOT(change_dirtreeview_item(const std::string & )) );

  status_bar_ = statusBar ();


  if (status_bar_!=0)
  {
    qDebug("have status bar");
    layout->addWidget(status_bar_);
  }
  else
    qDebug("don't have status bar");
  //status_bar = new QStatusBar(this);
//#endif
}

int FileManager::cal_step_size(const QListView *lv, QListViewItem *item)
{
  QListViewItem *cur_item=item;
  int parent=0;
  while(cur_item->parent())
  {
    ++parent;
    cur_item=cur_item->parent();
  }
  return lv->treeStepSize() * parent;
}

int FileManager::rename()
{
  std::string old_fp;
  //fp=rename_item_->get_fullpath();
  std::string new_fp;

  //QListView *cur_list_view_;

  std::string le_text;

  DS::qstr_to_big5str(cur_le_->text(), le_text); // maybe check return fail

#if 0
  if (dir_content_->hasFocus())
  {
    old_fp=std::string(dir_content_->dirname() )
       + "/"
       + std::string(dir_content_->basename() );

    new_fp=std::string(dir_content_->dirname())
           + "/"
	   + le_text;
  }

  if (dirtree_view_->hasFocus())
  {
    old_fp=std::string(rename_item_->dirname() )
           + "/"
           + std::string(rename_item_->basename() );
    new_fp=std::string(rename_item_->dirname())
           + "/"
	   + le_text;
  }
#endif

    old_fp=std::string(rename_item_->dirname() )
           + "/"
           + std::string(rename_item_->basename() );
    new_fp=std::string(rename_item_->dirname())
           + "/"
	   + le_text;


  qDebug("old fn: %s", old_fp.c_str());
  qDebug("new fn: %s", new_fp.c_str());

  if (::rename(old_fp.c_str(), new_fp.c_str() ) !=0 )
  {
    qDebug("rename fail");
  }
  else
  {
    //std::string item_new_name;
    rename_item_->setText(0, cur_le_->text());
    rename_item_->save_fn(0);


  }

  cur_le_->hide();

   

  return 0;
}

void FileManager::fo_item_info()
{
  if (dir_content_->hasFocus())
  {
    cur_list_view_=dir_content_;
    cur_le_=le_on_dir_content_;
  }
  if (dirtree_view_->hasFocus())
  {
    cur_list_view_=dirtree_view_;
    cur_le_=le_on_dirtree_;
  }

  rename_item_=(FOListViewItem*)cur_list_view_->currentItem();

  status_bar_->clear();

  //QLabel *l1, *l2, *l3;

  QString dir_str, base_str, fp_str;
  DS::big5str_to_qstr(rename_item_->dirname(), dir_str);
  dir_str=QString("dn: ") + dir_str;

  DS::big5str_to_qstr(rename_item_->basename(), base_str);
  base_str=QString("bn: ") + base_str;

  DS::big5str_to_qstr(rename_item_->get_fullpath(), fp_str);
  fp_str=QString("fpn: ") + fp_str;
  //l1=new QLabel(qstr, status_bar);
  //status_bar->addWidget(l1);
  status_bar_->message(dir_str + "  " + base_str + "  " + fp_str);

  qDebug("dirname: %s", rename_item_->dirname().c_str());
  qDebug("basename: %s", rename_item_->basename().c_str());
  qDebug("fullpath: %s", rename_item_->get_fullpath().c_str());
}

void FileManager::rename_item()
{
  //RenameLineEdit *le;

  if (dir_content_->hasFocus())
  {
    cur_list_view_=dir_content_;
    cur_le_=le_on_dir_content_;
  }
  if (dirtree_view_->hasFocus())
  {
    cur_list_view_=dirtree_view_;
    cur_le_=le_on_dirtree_;
  }



  rename_item_=(FOListViewItem*)cur_list_view_->currentItem();

  qDebug("dirname: %s", rename_item_->dirname().c_str());
  qDebug("basename: %s", rename_item_->basename().c_str());
  //return ;

  if (rename_item_==0) return;

  cur_list_view_->ensureItemVisible( rename_item_ );

/*
  cur_list_view->horizontalScrollBar()->setValue( 0 );
  cur_list_view->horizontalScrollBar()->setEnabled( FALSE );
  cur_list_view->verticalScrollBar()->setEnabled( FALSE );
  */

  //QRect header_r = dir_content_->header()->sectionRect(0);
  int header_height = cur_list_view_->header()->height();
  QRect r = cur_list_view_->itemRect(rename_item_);
  r.setTop( r.top() + cur_list_view_->frameWidth() + 1 + header_height);
  r.setBottom( r.bottom() + cur_list_view_->frameWidth() + header_height);
  //cur_list_view_->treeStepSize();
  r.setLeft( r.left() + cal_step_size(cur_list_view_, rename_item_));
  qDebug("TreeStepSize: %d", cur_list_view_->treeStepSize());
  //r.setWidth( cur_list_view_->columnWidth( 0 ) - cur_list_view_->treeStepSize());


  cur_le_->setGeometry( r );
  cur_le_->setText( rename_item_->text( 0 ) );
  cur_le_->selectAll();
  cur_le_->show();
  cur_le_->setFocus();

}

void FileManager::select_font()
{
  if (font_selector_->exec()==QDialog::Accepted)
  {
      //font_selector_->cur_fn();
    //font_selector_->currentItem();
    if (QString::null==font_selector_->cur_fn() )
    {
      qDebug("(QString::null");
      return;
    }
    //else
      //qDebug("ac: %s", (font_selector_->cur_fn()).ascii());	    
    //h_box_->setFont(QFont(font_selector_->cur_fn()));
    dir_content_->setFont(QFont(font_selector_->cur_fn()));
    dirtree_view_->setFont(QFont(font_selector_->cur_fn()));
    status_bar_->setFont(QFont(font_selector_->cur_fn()));
    //dir_treeview_->setFont(QFont(font_selector_->cur_fn()));
  }
  #if 0
  else
  {
    qDebug("no ac");	    
  }
  #endif

}

