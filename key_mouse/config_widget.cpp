#include "config_widget.h"

ConfWidget::ConfWidget(QWidget * parent, Qt::WindowFlags f):QWidget(parent,f)
{
  button_ = new QPushButton(this);
  button_->resize(100,100);
  setMouseTracking(true); // if set to false, mouseMoveEvent need press mouse button
}

void ConfWidget::mousePressEvent ( QMouseEvent * event )
{
  qDebug("mousePressEvent");
  //releaseMouse();
}

void ConfWidget::mouseMoveEvent ( QMouseEvent * event )
{
  QString msg; 

  //grabMouse();
  //char pos[30];

  //event->globalX();
  //event->globalY();
  //sprintf(pos, "(%d, %d)", event->globalX(), event->globalY());
  //msg.sprintf("<center><h1>Move: (%d, %d)</h1></center>", event->x(), event->y()); 
  msg.sprintf("<center><h1>Move: (%d, %d)</h1></center>", event->globalX(), event->globalY()); 
  button_ -> setText(msg);
  //qDebug("%d, %d",  event->globalX(), event->globalY());
}

void ConfWidget::mouseReleaseEvent ( QMouseEvent * event )
{
  qDebug("mouseReleaseEvent");
  //releaseMouse();
}

#if 0
ConfWidget::~ConfWidget()
{
  ~QWidget();
}
#endif
