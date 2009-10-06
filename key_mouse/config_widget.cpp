#include "config_widget.h"

ConfWidget::ConfWidget(QWidget * parent, Qt::WindowFlags f):QWidget(parent,f)
{
  button_ = new QPushButton(this);
  button_->resize(100,100);
  setMouseTracking(true); // if set to false, mouseMoveEvent need press mouse button

  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(move_cursor()));
  timer_->start(1000);

  QCursor_ = new QCursor();
  x_diff_ = 50;
  y_diff_ = 0;
}

void ConfWidget::mousePressEvent ( QMouseEvent * event )
{
  qDebug("mousePressEvent");
  //releaseMouse();
}

void ConfWidget::move_cursor()
{
  static int x=0,y=10;

  x+=x_diff_;
  QCursor_->setPos(x,y);
  qDebug("move cursor");
}

void ConfWidget::mouseMoveEvent ( QMouseEvent * event )
{
  QString msg; 

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

void ConfWidget::keyReleaseEvent ( QKeyEvent * event )
{
  switch (event->key())
  {
    case (Qt::Key_Escape):
    {
      qDebug("esc key");
      releaseMouse();
      timer_->stop();
      break;
    }
    case (Qt::Key_Home):
    {
      qDebug("home key => grab mouse");
      grabMouse();
      timer_->stop();
      break;
    }
  }
  QWidget::keyReleaseEvent(event);

}

#if 0
ConfWidget::~ConfWidget()
{
  ~QWidget();
}
#endif
