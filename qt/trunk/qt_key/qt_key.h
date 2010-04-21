#ifndef QT_KEY_H
#define QT_KEY_H

#include <qframe.h>
#include <qlabel.h>
#include <qvbox.h>


class QtKey: public QVBox
{
  Q_OBJECT

  public:
    QtKey(QWidget * parent = 0, const char * name = 0, WFlags f = 0):QVBox(parent, name, f)
    {
      //setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
      //QVBox *vbox = new QVBox(this);
      key_press_label_ = new QLabel("key press:", this);
      key_release_label_ = new QLabel("ker release:", this);

    }
  protected:
    //void paintEvent(QPaintEvent *e);
    void keyPressEvent ( QKeyEvent * e );
    void keyReleaseEvent ( QKeyEvent * e );


    //void resizeEvent ( QResizeEvent * e);
  private:
    QLabel *key_press_label_;
    QLabel *key_release_label_;
};

#endif
