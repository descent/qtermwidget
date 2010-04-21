#ifndef ENCODE_DIALOG_H
#define ENCODE_DIALOG_H

#include <qdialog.h>
#include <qcombobox.h>

struct ENC
{
  bool is_valid;
  QString r,w;
};

class EncodeDialog : public QDialog
{
  Q_OBJECT
  public:
    static ENC get_enc(QWidget *parent, bool modal=true);
  private:
    EncodeDialog(QWidget * parent = 0, const char * name = 0, bool modal = FALSE, WFlags f = 0);
    static QComboBox *read_, *write_;  // read filename use encode, write filename use encode
};

#endif
