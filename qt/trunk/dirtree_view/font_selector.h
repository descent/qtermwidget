#ifndef FONT_SELECTOR_H
#define FONT_SELECTOR_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qfontdatabase.h>

#include <qlistbox.h>
#include <qlayout.h>
#include <qtabwidget.h>

#include <iostream>

using namespace std;

//extern const int FILE_VIEW;
//extern const int OTHER_VIEW;
//extern const int NO_FONT;
//const int FILE_VIEW=0;
//const int OTHER_VIEW=1;
//const int NO_FONT=-1;


struct FontInfo
{
  static const int FILE_VIEW=0;
  static const int OTHER_VIEW=1;
  static const int NO_FONT=-1;
  QString fn;
  int type_view; 
};

// I let FontSelector to be signleton
class FontSelector : public QDialog
{
  Q_OBJECT
  public:

    static FontInfo get_font_name(QWidget *parent, bool modal=true);
    static QStringList get_font_names()
    {
      QFontDatabase fd;
      return fd.families();
    }

    FontInfo cur_fn() const 
    {
      //qDebug("in cur_fn %s",flist_->currentText().latin1());
      //return flist_->currentText().latin1();
      FontInfo fi;
      fi.type_view=cur_page_index();
      fi.fn=flist_->currentText();
      qDebug("fi type %d", fi.type_view);
      qDebug("fi fn %s", fi.fn.latin1());
      //((QListBox*)(tab_widget_->currentPage()))->currentText();
      //qDebug("font name: %s",(((QListBox*)(tab_widget_->currentPage()))->currentText()).latin1());

      return fi;
    }
    int cur_page_index() const
    {
      return tab_widget_->currentPageIndex();
    }

  private:
    QTabWidget *tab_widget_;
    FontSelector(QWidget * parent = 0, const char * name = 0, bool modal = true, WFlags f = 0, bool sort=true);
    QListBox* flist_;


};


#endif
