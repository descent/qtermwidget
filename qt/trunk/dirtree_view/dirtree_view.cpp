#include "dirtree_view.h"
#include "file_monitor.h"


#include "font_selector.h"

#include <qheader.h>

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



DirTreeView::DirTreeView( QWidget * parent , const char * name , WFlags f  ):FOListView(parent,name,f),re_add_child_item_(false)
{
  addColumn("Dir");

  root_=new DirTreeViewItem (this, "/");
  setCurrentItem(root_);
}

// copy dircontent dir item
void DirTreeView::copy_dir_item(FOListViewItem *i)
{
  QString txt=i->text(0);
  if (txt[txt.length()-1]=='/')
  {
    txt.remove(txt.length()-1,1);
    DirTreeViewItem *dti=new DirTreeViewItem(currentItem(), txt);
    if (i->have_save_fn())
    {
      dti->save_fn(i->get_save_fn()); 
    }
  }
}

void DirTreeView::mark_cur_item(const std::string &bn)
{
  if (bn=="..")
  {
    if (currentItem()->parent())
    {
      setCurrentItem(currentItem()->parent());  
      // because now currentItem already changed.
      ensureItemVisible(currentItem() );
    }
  }
  for (QListViewItem *cur_i=currentItem()->firstChild() ; cur_i ; cur_i=cur_i->nextSibling() )
  {
    std::string txt=((FOListViewItem *)cur_i)->basename();
    //if (txt[txt.length()-1]=='/')
      //txt.erase(txt.length()-1);
    //cout << "mark item : " << txt.c_str() << endl;
    if (txt==bn)
    {
      setCurrentItem(cur_i);  
      ensureItemVisible(cur_i);
    }
  }
}

void DirTreeView::add_child_item(FOListView *fo_lv)
{
  FOListViewItem *c_i=(FOListViewItem *)currentItem();
  //FOListViewItem *cur_foi=(FOListViewItem *)currentItem();
  if (c_i->is_open() ) return;
  //if (re_add_child_item_==false) return;
  //if (cur_foi->firstChild()!=0) return;
  del_child_item(currentItem());
  cout << "del child item" << endl;
  cout << "add child item" << endl;
  QListViewItem *i=fo_lv->firstChild();
  for (; i ; i=i->nextSibling())
  {
    FOListViewItem *foi=(FOListViewItem *)i;
    if (i->text(0)=="../" || i->text(0)=="./")
      continue;
    copy_dir_item(foi);
  }
  c_i->set_open(true);
  //re_add_child_item_=false;
}


void DirTreeView::rename_item_text()
{
  qDebug("dir tree");
  rename_item_->setText(0,le_->text());
  rename_item_->save_fn(0);
}

int FOListViewItem::compare ( QListViewItem * i, int col, bool ascending ) const
{
  FOListViewItem *foi=(FOListViewItem *)i;

  std::string fp=get_fullpath();
  std::string fpi=foi->get_fullpath();


  if ((is_dir(fp) && is_dir(fpi)) || (!is_dir(fp) && !is_dir(fpi)) )
  {
    // compare filename
    std::string fn=basename();
    std::string fni=foi->basename();
    if (fn > fni)
      return 1;
    else
      return -1;

  }

  if (is_dir(fp) && !is_dir(fpi))
    return -1;
  if (!is_dir(fp) && is_dir(fpi))
    return 1;
  return 0; // should not go here
}




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

  if (text(0)=="/")
    return "/";
  if (have_save_fn()==false)
  {
    DS::qstr_to_big5str(text(0), str);
    if (str[str.length()-1]=='/')
      str.erase(str.length()-1);
    return str;
  }
  else
    return get_save_fn();

}

// use combobox select dir need this member function
void DirTreeView::gen_dirs(QListViewItem *item)
{
  qDebug("in gen_dirs");
  if (item==0) 
  {
    qDebug("no point a item");
    return;
  }


  DirTreeViewItem *cur_item=(DirTreeViewItem*)item;
   

  //DirTreeViewItem *cur_item=(DirTreeViewItem*)item;
  qDebug("dirname: %s", cur_item->dirname().c_str());
  qDebug("basename: %s", cur_item->basename().c_str());


  if (open_dir(cur_item->get_fullpath() )==OPENDIR_FAIL) return;

  // erase all child item
  del_child_item(item);
  

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

  cur_item -> set_open(true);

  //setOpen(item, true);

}

DirContent::DirContent( QWidget * parent , const char * name , WFlags f ):FOListView(parent,name,f), fullpath_("/")
{
  file_monitor_= new FileMonitor;
  file_monitor_->open_fam();
  connect(file_monitor_, SIGNAL(file_alt(FAMEvent *)), this, SLOT(file_alt(FAMEvent *)));
  setFont(QFont("unifont"));
  addColumn("Name");
  setSelectionMode(QListView::Multi);

  connect(this, SIGNAL( doubleClicked ( QListViewItem * ) ), this, SLOT(cd_another_dir(QListViewItem *) ));
}

DirContent::~DirContent()
{
  delete file_monitor_;
}

void DirContent::file_alt(FAMEvent *ev)
{
  std::string fn(ev->filename);
  //cout << "file event: " << ev->code << " / "<< event_name(ev->code) << endl;
  switch (ev->code)
  {
    case FAMCreated:
    {
      cout << "file : " << fn << " be created" << endl;
      if (is_dir(dirname() + "/" + fn) )
      {
	emit create_file(gen_items(fn, true) );
      }
      else
        gen_items(fn, false);
      break;
    }
    case FAMDeleted:
    {
      cout << "file : " << fn << " be deleted" << endl;
      del_items(fn);
      emit del_file(fn);
/*
      if (is_dir(dn_ + "/" + fn) )
      {
        cout << "emit del_file(fn)" << endl;
      }
      */
      break;
    }
    case FAMChanged:
    {
      cout << "file : " << dn_ << " be changed" << endl;
      break;
    }
    default:
    {
      //cout << "I don't care" << endl;
      break;
    }

  }
}

void DirContent::file_alt(int code)
{
  cout << "file event: " << code << endl;
}

// this create a widget to input new dir name
void DirContent::create_new_dir_input()
{
  
}


// str 是 a 或是 a/ (目錄格式)
void DirContent::del_items(const std::string &str)
{
  // if str cannot conver unicode
  // use save_fn to delete
  QString qstr;

  if (DS::big5str_to_qstr(str,qstr)!=0)
  {
  }
  else
  {
  }
  QListViewItem *i=find_item(str.c_str(),0);
  if (i==0)  // this should be run
  {
    std::string text=str+"/"; // maybe a directory
    i=find_item(text.c_str(),0); // find again
  }
  if (i!=0) takeItem(i);

}

QListViewItem *DirContent::find_item(const std::string &text, int column)
{
  // this should compare save_fn(), now I skip it 
  QListViewItem *i=firstChild();
  for (QListViewItem * cur_item=i ; cur_item ; cur_item=cur_item->nextSibling() )
  {
    if (cur_item->text(0) == QString(text.c_str()))
      return cur_item;
  } 

  return 0;
}

QListViewItem* DirContent::gen_items(const std::string &str, bool is_dir)
{
    QString qstr;
    bool can_unicode=true;

    if (DS::big5str_to_qstr(str, qstr)!=0) // convert to unicode error
    {
       can_unicode=false;
       qstr="?";
    }

    if (is_dir) qstr = qstr + "/";

    DirContentViewItem *node = new DirContentViewItem(this, qstr);
    if (can_unicode==false)
      node->save_fn(str.c_str());
    return node;
}

void DirContent::gen_items(const std::vector<std::string> &str, bool is_dir)
{
  for (size_t i=0 ; i < str.size() ; ++i)
  {
    gen_items(str[i], is_dir);
  }
}

void DirContent::new_dir()
{
  cout << "DirContentView new_dir" << endl;
  char *fn="new_dir";

  if(fn)
  {
    DirContentViewItem *i=new DirContentViewItem(this,fn);
    //ensureItemVisible(item);
    //FOListView::rename_item(item);
    le_ = new RenameLineEdit(this);

  int header_height = header()->height();
  header_height =0;
  QRect r = itemRect(i);
  r.setTop( r.top() + frameWidth() + 1 + header_height + 20);
  r.setBottom( r.bottom() + frameWidth() + header_height + 20);
  r.setLeft( r.left() + cal_step_size(i));

  le_->setGeometry( r );
    le_->setText("abc");
    le_->setFocus();
    le_->show();

  }
}
int DirContent::gen_dir_content(const std::string &dir_fp)
{
  qDebug("in gen_dirs_content(const std::string &dir_fp)");


  int open_dir_status=open_dir(dir_fp.c_str());
  if (open_dir_status==OPENDIR_FAIL || open_dir_status==ALREADY_OPEN) 
  { 
    cout << "OPENDIR_FAIL or ALREADY_OPEN: " << open_dir_status << endl;
    opendir_=false;
    return -2;
  }

  clear();
  gen_items(files_, false);
  gen_items(dirs_, true);


  set_dirname(dir_fp);

  file_monitor_->monitor_file(dir_fp.c_str());
  qDebug("set_dirname: %s", dir_fp.c_str());
  return 0;
}

int DirContent::gen_dir_content(QListViewItem *item)
{
  qDebug("in gen_dirs_content(QListViewItem *item)");
  if (item==0) 
  {
    opendir_=false;
    qDebug("no point a item");
    return -1;
  }

  FOListViewItem *cur_item=(FOListViewItem*)item;

  std::string opendir_fp=(cur_item->get_fullpath());
  return gen_dir_content(cur_item->get_fullpath());


  int open_dir_status=open_dir(opendir_fp.c_str());
  if (open_dir_status==OPENDIR_FAIL || open_dir_status==ALREADY_OPEN) 
  { 
    qDebug("OPENDIR_FAIL or ALREADY_OPEN");
    opendir_=false;
    return -2;
  }
  opendir_=true;

    dn_=cur_item->get_fullpath();

  // because clear() will delete all item, so after claer(),
  // don't use item object;
  clear();
  gen_items(files_, false);
  gen_items(dirs_, true);


  set_dirname(dn_);
  file_monitor_->monitor_file(dn_.c_str());
  qDebug("set_dirname: %s", dn_.c_str());
  emit cur_dir(dn_);
  return 0;
}

void FileManager::create_menu()
{
  layout_->addWidget(menuBar() );
  file_ = new QPopupMenu( this );  
  menuBar()->insertItem( tr( "File" ), file_ );

  INIT_FO_POPUP(file_, this);


  view_ = new QPopupMenu( this );  

  menuBar()->insertItem( tr( "View" ), view_ );

  dirtree_idx_=view_->insertItem(tr("list"),this, SLOT(show_dirtree()));
  cout << "id : " << dirtree_idx_ << endl;
  image_idx_=view_->insertItem(tr("image"),this, SLOT(show_image()));
  status_idx_ = view_->insertItem(tr("status bar"),this, SLOT(show_status_bar()));
  view_->setItemChecked(status_idx_, true);

  options_ = new QPopupMenu( this );  
  menuBar()->insertItem( tr( "Options" ), options_ );
  options_->insertItem(tr("Font"),this, SLOT(select_font()));
  options_->insertItem(tr("encode"),this, SLOT(show_encode_dialog()));

  up_ = new QToolButton(menuBar(),"up");
  left_ = new QToolButton(0,"left");  
  right_ = new QToolButton(0,"right"); 

  init_tool_button(menuBar(),up_,"up.png");
  init_tool_button(menuBar(),left_,"left.png");
  init_tool_button(menuBar(),right_,"right.png");
   
  connect(up_, SIGNAL(clicked() ) , this, SLOT(cd_up()));
  connect(left_, SIGNAL(clicked() ) , this, SLOT(screen_off()));
  connect(right_, SIGNAL(clicked() ) , this, SLOT(screen_on()));
}

void FileManager::create_select_dir()
{
  QHBox *line_box = new QHBox(this);
  layout_->addWidget(line_box);
  select_dir_ = new QComboBox(line_box);

  //line_box -> setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
  select_dir_->setEditable(true);
  select_dir_->setAutoCompletion(true);
  std::string select_str[]=
  {
    "/",
    "/home/",
    "/etc/",
    "/mnt/",
    "/root/",
    "/home/descent/",
    "/home/sam/",
    "/home/sam1/",

  };
  for (int i=0 ; i < sizeof(select_str)/sizeof(std::string) ; ++i)
  {
    select_dir_->insertItem(select_str[i]);
  }

  connect(select_dir_, SIGNAL(activated ( const QString &) ), this, SLOT(change_dirtreeview_item(const QString & )) );
}

void FileManager::create_main_widget()
{
  //QVBox *v_box_2 = new QVBox(this);
  //QHBox *h_box = new QHBox(this);
  QSplitter *split = new QSplitter(this);
  //setCentralWidget(split);
  //split->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  //layout_->addWidget(h_box);
  layout_->addWidget(split,2);

  //dirtree_view_ = new DirTreeView(h_box);
  //widget_stack_=new QWidgetStack(h_box);
  dirtree_view_ = new DirTreeView(split);
  widget_stack_=new QWidgetStack(split);

  //widget_stack_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  gt_=new GridThumb(widget_stack_);
  //ImageWidget *gt= new ImageWidget("p.png",FIT_WINDOW,widget_stack_);
  //dirtree_view_->hide();
  dir_content_ = new DirContent(widget_stack_);
  //dir_content_->hide();
  //image_widget_ = new ImageWidget(widget_stack_);
  //image_widget_ = new ImageThumb("p.png",widget_stack_);
  //setCentralWidget(h_box_);
  widget_stack_->addWidget(dir_content_,1);
  widget_stack_->addWidget(gt_,2);
  //widget_stack_->addWidget(gt,2);
  //widget_stack_->raiseWidget(dir_content_);
  widget_stack_->raiseWidget(gt_);

// fam notify file create, if a directory notify dirtree_view
  QObject::connect(dir_content_, SIGNAL(create_file(QListViewItem *) ), dirtree_view_ , SLOT(create_file(QListViewItem *)) );

// fam notify file delete, if a directory notify dirtree_view
  QObject::connect(dir_content_, SIGNAL(del_file(const std::string&) ), dirtree_view_ , SLOT(del_file(const std::string&) ));

  QObject::connect(dir_content_, SIGNAL(open_new_dir(FOListView *) ), dirtree_view_ , SLOT(add_child_item(FOListView *) ));

  QObject::connect(dir_content_, SIGNAL(cd_new_dir(const std::string &) ), dirtree_view_ , SLOT(mark_cur_item(const std::string &) ));


  QObject::connect(dirtree_view_, SIGNAL(clicked ( QListViewItem *) ), dir_content_, SLOT(open_another_dir(QListViewItem *) ));

  QObject::connect(dirtree_view_, SIGNAL(signal_reload ( QListViewItem *) ), dir_content_, SLOT(slot_reload(QListViewItem *) ));

  QObject::connect(dir_content_, SIGNAL(signal_reload(FOListView *) ), dirtree_view_, SLOT(slot_reload(FOListView *) ));

  connect(dirtree_view_,SIGNAL(currentChanged ( QListViewItem *)), dir_content_, SLOT(open_another_dir(QListViewItem *) ) );
  //connect(dirtree_view_,SIGNAL(currentChanged ( QListViewItem *)), dirtree_view_, SIGNAL(clicked ( QListViewItem *) ) );

  // 修改 caption
  QObject::connect(dir_content_, SIGNAL(cur_dir ( const std::string &) ), this, SLOT(modify_caption(const std::string &) ));

}

FileManager::FileManager(QWidget * parent , const char * name , WFlags f ):QMainWindow(parent,name,f)
{
  layout_ = new QVBoxLayout( this );
  layout_->setSpacing(2);
  layout_->setMargin(2);

  create_menu();
  create_select_dir();
  create_main_widget();

  le_on_dirtree_ = new RenameLineEdit(dirtree_view_);
  le_on_dirtree_ -> hide();

  le_on_dir_content_ = new RenameLineEdit(dir_content_);
  le_on_dir_content_ -> hide();

  connect(le_on_dirtree_, SIGNAL( lost_focus() ), SLOT( rename() ) );
  connect(le_on_dirtree_, SIGNAL(returnPressed()), this, SLOT(rename()));

  connect(le_on_dir_content_, SIGNAL( lost_focus() ), SLOT( rename() ) );
  connect(le_on_dir_content_, SIGNAL(returnPressed()), this, SLOT(rename()));



  //QHBox *line_box = new QHBox( this );
  //layout->addWidget(line_box);

  //layout->addWidget(v_box_2);
  //layout->addWidget(widget_stack);


  connect(dir_content_, SIGNAL( opendir ( const std::string &) ), this, SLOT(change_dirtreeview_item(const std::string & )) );

  status_bar_ = statusBar ();


  if (status_bar_!=0)
  {
    qDebug("have status bar");
    //grid_layout->addMultiCellWidget(status_bar_,3,3,0,1);
    layout_->addWidget(status_bar_);
  }
  else
    qDebug("don't have status bar");
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

  FontInfo fi=FontSelector::get_font_name(this);
  if (!fi.fn.isNull())
  {
    //if (fi.type_view==FILE_VIEW)
    {
    }
//#if 0
    switch (fi.type_view)
    {
      case FontInfo::FILE_VIEW:
      {
        dir_content_->setFont(QFont(fi.fn));
        dirtree_view_->setFont(QFont(fi.fn));
	select_dir_->setFont(QFont(fi.fn));
        break;
      }
      case FontInfo::OTHER_VIEW:
      {
        menuBar()->setFont(QFont(fi.fn));
        break;
      }
    }
//#endif
  }
  else
  {
    qDebug("no select font");
  }
  #if 0
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
  #endif

}


bool same_fn_item(const FOListViewItem *foi, const std::string &fn)
{
  std::string filename;
  if (foi->have_save_fn() )
    filename=foi->get_save_fn();
  else
    DS::qstr_to_big5str(foi->text(0), filename); // it should be success, so I don't check it return value

  if(filename==fn)
    return true;
  else
    return false;
}
