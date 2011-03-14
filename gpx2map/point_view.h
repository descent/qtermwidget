#ifndef POINT_VIEW_H
#define POINT_VIEW_H

#include "route_item.h"

#include <QWidget>

class PointView: public QWidget
{
  Q_OBJECT

  public:
    PointView(QWidget * parent = 0, Qt::WindowFlags f = 0 );
    void set_point(const vector<Point> &p){points_=p;}
  protected:
    void paintEvent(QPaintEvent *event);
  private:
    vector<Point> points_;
    
};

#endif
