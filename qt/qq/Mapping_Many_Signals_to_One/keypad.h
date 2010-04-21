#ifndef KEYPAD
#define KEYPAD

#include <qpushbutton.h>
#include <qsignalmapper.h>
#include <qstring.h>
#include <qlayout.h>


class Keypad:public QWidget
{
Q_OBJECT 
public:
  Keypad (QWidget * parent = 0);
  signals:
void digitClicked (int digit);
  private slots:void button_click(int i)
  {
    qDebug("button %d",i);
  }
  //private slots:void button0Clicked ();
private:
  void createLayout ();
  QPushButton *buttons[10];
};

#endif
