#include "screen_widget.h"

int fd;

DS::Light::Light(QWidget * parent , const char * name , WFlags f  )
{
  init_screen ();
  on_ = true;
  button_ = new QPushButton(this);
  connect(button_, SIGNAL(clicked()),this,SLOT(screen()));
  int w=240,h=240;
  resize(w,h);
  button_->resize(w,h);
}
