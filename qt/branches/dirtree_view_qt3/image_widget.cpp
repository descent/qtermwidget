#include "image_widget.h"

#include <qpainter.h>

void ImageWidget::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  qDebug("in ImageWidget::paintEvent");

  painter.setClipRect(e->rect());
  painter.setBrush( black );
  painter.drawRect( 0, 0, width(), height() );

  if ( pixmap_.size() != QSize( 0, 0 ) ) 
  { // is an image loaded?
    painter.drawPixmap((width() - pixmap_.width()) / 2, (height() - pixmap_.height()) , pixmap_);
  }

}
