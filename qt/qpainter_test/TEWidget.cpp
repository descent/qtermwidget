#include "TEWidget.h"



TEWidget::TEWidget(QWidget *parent):QFrame(parent)
{
}

void TEWidget::paintEvent( QPaintEvent * )
{
  qDebug("in paintEvent");
}
