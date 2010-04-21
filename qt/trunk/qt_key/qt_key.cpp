#include "qt_key.h"

#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>

#include <algorithm>
#include <iostream>

#include <cstdio>

using namespace std;

void QtKey::keyPressEvent ( QKeyEvent * e )
{
  char i[10];
  sprintf(i,"%d", e->key());
  QString key="key press: " + e->text() + "/" + i;
  key_press_label_->setText(key);
  qDebug("key press: %d",e->key());
  //qDebug("key press: %s",e->text().ascii() );
}

void QtKey::keyReleaseEvent ( QKeyEvent * e )
{
  char i[10];
  sprintf(i,"%d", e->key());
  QString key="key release: " + e->text() + "/" + i;
  key_release_label_->setText(key);
  qDebug("key release: %d",e->key());
  //qDebug("key release: %s",e->text().ascii() );
}

/*
QSizePolicy QtKey::sizePolicy() const
{
  return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

QSize QtKey::sizeHint() const
{
  return QSize(100,100);
}
*/


/*
void QtKey::paintEvent(QPaintEvent *event)
{
  static QPixmap pixmap;
  QRect rect = event->rect();
  QSize new_size = rect.size().expandedTo(pixmap.size());
  pixmap.resize(new_size);
  pixmap.fill(this, rect.topLeft());
  QPainter painter(&pixmap, this);
  painter.translate(-rect.x(), -rect.y());
  draw_stuff(&painter);
  bitBlt(this, rect.x(), rect.y(), &pixmap, 0, 0, rect.width(), rect.height());

}
*/
