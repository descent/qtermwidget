#include "tree_view_popup_menu.h"
#include "file_list_widget.h"

#include <QTextCodec>

#include <cstdio>

const char *encoding_name[]={"BIG5", "UTF-8"};


TreeViewPopupMenu::TreeViewPopupMenu(QWidget * parent): QTreeWidget(parent)
{

  cur_enc_=BIG5;
  popup_menu_timer_ = new QTimer(this);
  popup_menu_timer_->setSingleShot(true);
  connect( popup_menu_timer_, SIGNAL( timeout() ), this, SLOT( show_fo_menu() ) );

  m_= new QMenu(this);
  font_select_ = new QAction("select font", this);
  m_->addAction(font_select_);
  connect(font_select_, SIGNAL(triggered()), this, SLOT(font_select()));

  // create create encode menu
  encode_m_ = new QMenu(tr("Select Encoding") );
  m_->addMenu(encode_m_);
  big5_enc_ = new QAction("big5", this);
  utf8_enc_ = new QAction("utf8", this);
  enc_ = new QActionGroup(this);
  enc_->addAction(big5_enc_);
  enc_->addAction(utf8_enc_);
  encode_m_->addAction(big5_enc_);
  encode_m_->addAction(utf8_enc_);
  big5_enc_->setCheckable(true);
  utf8_enc_->setCheckable(true);
#if 1
  if (cur_enc_ == BIG5)
    big5_enc_->setChecked(true);
  else
    utf8_enc_->setChecked(true);
#endif
  connect(big5_enc_, SIGNAL(triggered()), this, SLOT(s_big5_enc()));
  connect(utf8_enc_, SIGNAL(triggered()), this, SLOT(s_utf8_enc()));


  CREATE_ACTIONS(more_info_, "more_info", s_more_info);




}

#include <string>

int TreeViewPopupMenu::dump_raw_str(const QString &str)
{
  std::string s=str.toStdString();

  printf("str.length: %d\n", str.length());
  for (int i=0; i < str.length() ; ++i)
  {
    printf("%x ", s[i]);
  }
  printf("\n");

}

#include <map>
#include <vector>

void  TreeViewPopupMenu::change_encode_fn()
{
  QTextCodec *tc;
  tc = QTextCodec::codecForName(encoding_name[cur_enc_]);
  qDebug("encoding_name[cur_enc_]: %s", encoding_name[cur_enc_]);

  typedef std::pair<QTreeWidgetItem *, QString> ITEM_FN;
  std::vector <ITEM_FN> item_fn;

  QTreeWidgetItemIterator it(this);
   while (*it)
   {
     static int c=0;
#if 1
     FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>(*it);
     QString fn(tc->toUnicode(fi->get_real_str().toAscii() ) );
     qDebug("%d ## fi: %x", c++, fi);
     //dump_raw_str(fi->get_real_str().toAscii());
     if (fn=="")
     {
       fn="???";
     }
     //fi->setText(0, fn);
#endif
     qDebug("%d ## *it: %x", c++, *it);
     item_fn.push_back(std::make_pair(*it, fn) );
     ++it;
     //(*it++)->setText(0, fn);


     //if ((*it)->text(0) == itemText)
      //treeWidget->setItemSelected(*it, true);

   }

     // if I use the code
     // fi->setText(0, fn);
     // ++it
     // I will get the same FileListWidgetItem pointer, very strange
     // now the code is work around, I don't the root cause.
     // but the performace is very slow. ugly!!

  std::vector <ITEM_FN> :: iterator iter=item_fn.begin();
  for ( ; iter != item_fn.end() ; ++iter)
  {
    ((*iter).first)->setText(0, (*iter).second);
  }
#if 0
  for (int i=0 ; i < fileList->count() ; ++i)
  {
    FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem *>(fileList->item(i) );
    QString fn(tc->toUnicode(fi->get_real_str().toAscii() ) );
    fi->setText(0, fn);
  }
#endif
}

void  TreeViewPopupMenu::s_more_info()
{
  FILE *fs;

  QList<QTreeWidgetItem *> select_item = selectedItems();
  //QTreeWidgetItem *it = select_item.at(0);
  FileListWidgetItem *fi = dynamic_cast<FileListWidgetItem*>(select_item.at(0));



  fs=fopen("more_info", "w");

  QString qstr=fi->get_real_str();

  std::string s=qstr.toStdString();
  for (int i=0; i < qstr.length() ; ++i)
  {
    fprintf(fs, "%x ", s[i]);
  }

  fclose(fs);
}

