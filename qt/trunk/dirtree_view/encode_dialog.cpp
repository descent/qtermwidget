#include "encode_dialog.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

namespace
{
  void init_encode_combobox(QComboBox *r, QComboBox *w, const QString &s)
  {
    r->insertItem(s);
    w->insertItem(s);
  }
}

// read filename use encode, write filename use encode
QComboBox *EncodeDialog::read_;
QComboBox *EncodeDialog::write_; 

EncodeDialog::EncodeDialog(QWidget * parent , const char * name , bool modal , WFlags f ):QDialog(parent, name, modal,f)
{
  setCaption(tr("Encode Select"));



  QPushButton *ok, *cancel;
  ok=new QPushButton(this);
  ok->setText("ok");
  cancel=new QPushButton(this);
  cancel->setText("cancel");

  ok->setDefault(true);

  connect(ok, SIGNAL(clicked() ), this, SLOT(accept() ));
  connect(cancel, SIGNAL(clicked() ), this, SLOT(reject() ));

  read_=new QComboBox(this);
  write_=new QComboBox(this);
  init_encode_combobox(read_, write_, "big5");
  init_encode_combobox(read_, write_, "utf8");
  init_encode_combobox(read_, write_, "unicode");

  QLabel *read_l, *write_l;  // read filename use encode, write filename use encode
  read_l=new QLabel("read filename encode", this);
  write_l=new QLabel("write filename encode", this);

  QGridLayout *grid_layout=new QGridLayout(this,4,5);
  grid_layout->addMultiCellWidget(read_l,0,0,0,1);
  grid_layout->addMultiCellWidget(write_l,1,1,0,1);
  grid_layout->addMultiCellWidget(read_,0,0,3,4);
  grid_layout->addMultiCellWidget(write_,1,1,3,4);
  grid_layout->addMultiCellWidget(ok,3,3,0,1);
  grid_layout->addMultiCellWidget(cancel,3,3,3,4);
  grid_layout->setMargin(20);

}

ENC EncodeDialog::get_enc(QWidget *parent, bool modal)
{
  ENC enc;
  static EncodeDialog *enc_dia=new EncodeDialog(parent);
  //if (enc_dia!=0)
    //enc_dia=new EncodeDialog(parent);
  if(enc_dia->exec() == QDialog::Accepted)
  {
    enc.is_valid=true;
    enc.r=read_->currentText();
    enc.w=write_->currentText();
  }
  else
  {
    enc.is_valid=false; // mean no change encoding
  }
  // I don't delete enc_dia, maybe I need to use smart pointer
  return enc; 
}
