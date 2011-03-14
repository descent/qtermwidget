#include "point_view.h"

#include <QPainter>
//#include <QPointF>
#include <QtDebug> // for qDebug()




PointView::PointView(QWidget * parent, Qt::WindowFlags f):QWidget(parent, f)
{
}

void PointView::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);

  vector<Point>::const_iterator it=points_.begin();

  for (; it !=points_.end(); ++it)
  {
    //QPointF p(it->first,
    //(*it)->first; //it->second;
    bool ok;
    QPoint p((it->first).toDouble(&ok) * 10000000 - 222900000, (it->second).toDouble(&ok)*10000000 - 1208880000);
    qDebug() << "p: " << p;
    painter.drawPoint(p);
  }


  QPointF p2(22.3653542, 120.9007247);
  QPointF p3(22.3653669, 120.9007376);

#if 0
  painter.drawPoint(p2);
  painter.drawPoint(p3);
  painter.drawLine(p1, p3);
#endif
}
