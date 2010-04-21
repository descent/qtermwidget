#include "font_selector.h"

#include <qhbox.h>


FontSelector::FontSelector(QWidget * parent , const char * name , bool modal , WFlags f, bool sort ) : QDialog(parent, name, modal, f)
{
  //QHBox *hbox=new QHBox(this);
  flist_ = new QListBox(this);

  QStringList fonts = get_font_names();
  if (sort==true)
    fonts.sort();
  for(unsigned int i = 0; i < fonts.count(); i++)
  {
    new QListBoxText(flist_, fonts[i]);
    //new FontNameItem(flist_, fonts[i]);
  }

  //qDebug("in fontselector");

}
