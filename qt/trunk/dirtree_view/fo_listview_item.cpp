#include "fo_listview_item.h"

const int OK=0;
const int ALREADY_OPEN=1;
const int OPENDIR_FAIL=2;
const int ERROR=3;

//bool FOListView::reload_=false;

FOListView::FOListView( QWidget * parent , const char * name , WFlags f ):QListView(parent, name, f), le_(0), popup_menu_timer_(this), opendir_(""), reload_(false)
{
  setFont(QFont("unifont"));
  connect( &popup_menu_timer_, SIGNAL( timeout() ), SLOT( show_fo_menu() ) );
  //setSorting(-1,0);
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

void FOListView::del_child_item(QListViewItem *i)
{
  QListViewItem *child_item;
  while((child_item = i->firstChild()) )
  {
    //qDebug("take item : %s", child_item->text(0).ascii() );
    i->takeItem(child_item);
  }
}


void FOListView::del_item()
{

  QListViewItem *i=currentItem();
  QListViewItem *child_item;
  while((child_item = i->firstChild()) )
  {
    qDebug("take item : %s", child_item->text(0).ascii() );
    i->takeItem(child_item);
  }
}

void FOListView::rename()
{
  qDebug("in FOListView::rename()");

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
    qDebug("le_==0");
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
  qDebug("exit rename_item(QListViewItem *i)");
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
        setSelected(i, false);
      }
    }
  }


}



int FOListView::open_dir(const std::string &dir_fp)
{
  cout << "dir_fp : " << dir_fp << endl;
  cout << "opendir_ : " << opendir_ << endl;
  cout << "reload_ : " << reload() << endl;
  // opendir_. dir_fp is fullpath
  if (reload()!=true && opendir_==dir_fp) return ALREADY_OPEN;

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
      perror(std::string(dir_fp  + "/" + dir_ent->d_name).c_str());
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
