#include "config_widget.h"

#include <QtGlobal>

ConfWidget::ConfWidget(QWidget * parent, Qt::WindowFlags f):QWidget(parent,f)
{
  button_ = new QPushButton(this);
  button_->resize(100,100);
  setMouseTracking(true); // if set to false, mouseMoveEvent need press mouse button

  timer_ = new QTimer(this);

  connect(timer_, SIGNAL(timeout()), this, SLOT(move_cursor()));

  m_cursor_ = new QCursor();
  x_diff_ = 50;
  y_diff_ = 0;
  qDebug("qVersion(): %s", qVersion());
  qDebug("QT_VERSION: %x", QT_VERSION);

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
  qDebug("move cursor");
  QPoint p(x,y);
  m_cursor_->setPos(p);
  // (Type type, const QPoint & position, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers )
  //QMouseEvent *me = new QMouseEvent(QEvent::MouseMove, p, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
  QMouseEvent *me = new QMouseEvent(QEvent::MouseButtonPress, p, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
  //QApplication::desktop()
  //QCoreApplication::postEvent(QApplication::desktop(), me, Qt::HighEventPriority);
  //if (QCoreApplication::notify(QApplication::desktop(), me))
  if (QCoreApplication::sendEvent(QApplication::desktop(), me))
  {
    qDebug("t");
  }
  else
  {
    qDebug("f");
  }

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
      timer_->start(1000);
      grabMouse();
      break;
    }
    case (Qt::Key_End):
    {
      qDebug("end key => release mouse");
      releaseMouse();
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
