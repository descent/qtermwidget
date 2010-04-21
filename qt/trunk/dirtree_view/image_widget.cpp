#include "image_widget.h"

#include <qpainter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>

#include <algorithm>
#include <iostream>

using namespace std;


QSizePolicy ImageWidget::sizePolicy() const
{
  return QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
}

QSize ImageWidget::sizeHint() const
{
  return QSize(100,100);
}

void ImageWidget::draw_stuff(QPainter *p)
{
  QWMatrix m;
  
  double r_w, r_h, s_r;

  r_w=(double)width() / (double)pixmap_.width();
  r_h=(double)height() / (double)pixmap_.height();
  //cout << "r_w: " << r_w << " " << "r_h : " << r_h << endl;
  s_r = (r_w < r_h)? r_w: r_h;

  m.scale(s_r, s_r);
  //p->setWorldMatrix(m);
  //p->drawPixmap(0,0  , pixmap_);
}

/*
void ImageWidget::paintEvent(QPaintEvent *event)
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

void ImageWidget::resize_contents ()
{
  if (pixmap_.isNull() )
  {
    QSize s(100,100);
    QScrollView::resizeContents(s.width(), s.height()); // 要執行此 member function 才會出現 scrollbar
  }
  else
  {
    QSize s = pixmap_.size();
    QScrollView::resizeContents(s.width(), s.height()); // 要執行此 member function 才會出現 scrollbar
  }
}

void ImageWidget::drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph )
{
  //qDebug("in ImageWidget::drawContent");
  //p->drawPixmap(0,0  , pixmap_);
  static QPixmap pixmap;
  //QRect rect = event->rect();
  //QSize new_size = rect.size().expandedTo(pixmap.size());
  //pixmap.resize(new_size);
  //pixmap.fill(this, rect.topLeft());
  //QPainter painter(&pixmap, this);
  //painter.translate(-rect.x(), -rect.y());
  //p->translate(-rect.x(), -rect.y());
  //draw_stuff(p);
  switch (show_mode() )
  {
    case FIT_WINDOW:
    {
      QWMatrix m;
  
      double r_w, r_h, s_r;
    
      r_w=(double)viewport()->width() / (double)pixmap_.width();
      r_h=(double)viewport()->height() / (double)pixmap_.height();
      s_r = (r_w < r_h)? r_w: r_h;
    
      m.scale(s_r, s_r);
    
      //p->setWorldMatrix(m);
    
      QRect r(pixmap_.rect());
      QPixmap x_pixmap=pixmap_.xForm(m);
      
      //QPixmap pixmap();
      //x_pixmap.fill(this, r.topLeft());
      //QScrollView::resizeContents(t_r.width(), t_r.height() );
      QScrollView::resizeContents(x_pixmap.width(), x_pixmap.height() );
      p->fillRect(clipx,clipy,clipw,cliph,Qt::white);

      //p->drawPixmap(0,0  , x_pixmap);
      qDebug("pixmap: %d,%d",x_pixmap.width(), x_pixmap.height( ));
      qDebug("viewport: %d,%d",viewport()->width(), viewport()->height() );

      // let pixmap to center, x or y direction
      if ( r_w < r_h) // this mean width fit to viewport->width(), so adjust height
        p->drawPixmap(0,(viewport()->height() -  x_pixmap.height( ) )/2  , x_pixmap);
      else
        p->drawPixmap((viewport()->width() -  x_pixmap.width( ) )/2,0  , x_pixmap);
      break;	    
    }
    case ORG_PIC:
    {
      p->drawPixmap(0,0  , pixmap_);
      break;	    
    }
    case FULL_SCREEN:
    {
      p->drawPixmap(0,0  , pixmap_);
      break;	    
    }
  }
  //bitBlt(this, rect.x(), rect.y(), &pixmap, 0, 0, rect.width(), rect.height());
  //bitBlt(this, clipx, clipy, &pixmap, 0, 0, pixmap_.width(), pixmap_.height());
}

/*
void ImageWidget::resizeEvent( QResizeEvent * e)
{

}
*/

