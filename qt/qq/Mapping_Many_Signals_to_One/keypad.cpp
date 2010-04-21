#include "keypad.h"


Keypad::Keypad (QWidget * parent):QWidget (parent)
{
  QSignalMapper *
    signalMapper = new QSignalMapper (this);
  connect (signalMapper, SIGNAL (mapped (int)), this,
	   SIGNAL (digitClicked (int)));
  for (int i = 0; i < 10; ++i)
   {
     QString text = QString::number (i);
     buttons[i] = new QPushButton (text, this);
     signalMapper->setMapping (buttons[i], i);
     connect (buttons[i], SIGNAL (clicked ()), signalMapper, SLOT (map ()));
   }

  createLayout ();
  connect(this, SIGNAL(digitClicked(int)), this, SLOT(button_click(int)));
}

void Keypad::createLayout ()
{
  QGridLayout *layout = new QGridLayout (this, 3, 4);
  layout->setMargin (6);
  layout->setSpacing (6);
  for (int i = 0; i < 9; ++i)
    layout->addWidget (buttons[i + 1], i / 3, i % 3);
  layout->addWidget (buttons[0], 3, 1);
}

#if 0
void Keypad::button0Clicked ()
{
  qDebug("in button0Clicked");
}
#endif
