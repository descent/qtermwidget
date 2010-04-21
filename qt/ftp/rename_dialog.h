#ifndef RENAME_DIALOG_H
#define RENAME_DIALOG_H

#include <QDialog>

class QPushButton;
class QLineEdit;


class RenameDialog : public QDialog
{
  Q_OBJECT
  public:
    RenameDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    void set_ok_dialog();
    void set_normal_dialog();
    void set_filename(const QString &path);
    int get_filename(QString &path);
  public slots:
    void s_ok()
    {
      qDebug("s_ok"); 
      QDialog::accept();
    }
  private:
    QPushButton *ok_, *cancel_;
    QLineEdit *rename_;
};


#endif
