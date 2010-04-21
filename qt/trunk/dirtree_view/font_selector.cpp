#include "font_selector.h"

#include <qhbox.h>
#include <qpushbutton.h>


FontInfo FontSelector::get_font_name(QWidget *parent, bool modal)
{
  static FontSelector *fs= new FontSelector (parent);
  if(fs->exec() == QDialog::Accepted)
  {
    //qDebug("fs %s", (fs->cur_fn()).latin1());
    return fs->cur_fn();
  }
  else
  {
    FontInfo fi;
    fi.fn=QString();
    fi.type_view=FontInfo::NO_FONT;
    return fi;
  }
  // I don't delete fs, maybe I need to use smart pointer
}

FontSelector::FontSelector(QWidget * parent , const char * name , bool modal , WFlags f, bool sort ) : QDialog(parent, name, modal, f)
{
  tab_widget_ = new QTabWidget(this);
  setCaption("Font Select");
  QGridLayout *grid_layout=new QGridLayout(this,2,3);
  flist_ = new QListBox(tab_widget_);

  tab_widget_->addTab(flist_,"file view font");
  tab_widget_->addTab(flist_,"others font");

  QPushButton *ok, *cancel;
  ok=new QPushButton(this);
  ok->setText("ok");
  cancel=new QPushButton(this);
  cancel->setText("cancel");

  ok->setDefault(true);

  grid_layout->addMultiCellWidget(tab_widget_,0,0,0,2);
  grid_layout->addWidget(ok,1,0);
  grid_layout->addWidget(cancel,1,2);


  connect(ok, SIGNAL(clicked() ), this, SLOT(accept() ));
  connect(cancel, SIGNAL(clicked() ), this, SLOT(reject() ));



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
