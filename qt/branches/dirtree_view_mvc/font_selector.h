#ifndef FONT_SELECTOR_H
#define FONT_SELECTOR_H

#include <qdialog.h>
#include <qlistbox.h>
#include <qfontdatabase.h>

#include <qlistbox.h>


/*
class FontNameItem : public QListBoxText
{
  //Q_OBJECT
  public:
    FontNameItem(QListBox * listbox, const QString & text = QString::null):QListBoxText(listbox, text), str_(text)
    {
    }
    const QString &font_name() const {return str_;}
  private:
    QString str_;
};
*/

class FontSelector : public QDialog
{
  Q_OBJECT
  public:
    FontSelector(QWidget * parent = 0, const char * name = 0, bool modal = true, WFlags f = 0, bool sort=true);

    static QStringList get_font_names()
    {
      QFontDatabase fd;
      return fd.families();
    }
    void resizeEvent ( QResizeEvent * e)
    {
      //setGeometry( e->size() );
      flist_->resize(e->size().width(), e->size().height());
    }

    QString cur_fn() const 
    {
      //qDebug("in cur_fn %s",flist_->currentText().latin1());
      return flist_->currentText().latin1();
    }

  private:
    QListBox* flist_;


};


#endif
