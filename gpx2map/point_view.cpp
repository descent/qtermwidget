#include "point_view.h"

#include <QPainter>
#include <QtDebug> // for qDebug()
#include <QBoxLayout>

#include <algorithm>

//PointView::PointView(QWidget * parent, Qt::WindowFlags f):QWidget(parent, f), scale_(1)
PointView::PointView(QWidget * parent, Qt::WindowFlags f):QFrame(parent, f), scale_(1)
{
  //setMinimumSize(400, 400);
}

#if 1
QSize PointView::sizeHint() const
{
  QSize size(400, 400);
}
#endif

void PointView::paintEvent(QPaintEvent *event)
{
#if 0
QPainter pn(this);  
// Y- and X-Axis paint.  
pn.setPen( Qt::blue );  
pn.drawLine( width(), height() / 2 , 0, height() / 2 );  
pn.drawLine( width() / 2, 0 , width() / 2, height() );  
// Y- and X-Axis paint.

QMatrix m;
m.translate( width() / 2, height() / 2 );
m.scale( 1, -1 );

pn.setMatrix( m );
//pn.setPen( Qt::NoPen );
pn.setPen( Qt::red );  
pn.setBrush( QBrush( Qt::blue, Qt::Dense4Pattern ) );
pn.drawRect( -10, -10, 20, 20 );

#else
  QPainter painter(this);


// Y- and X-Axis paint.  
painter.setPen( Qt::blue );  
painter.drawLine( width(), height() / 2 , 0, height() / 2 );  
painter.drawLine( width() / 2, 0 , width() / 2, height() );  
// Y- and X-Axis paint.
QMatrix m;
m.translate( width() / 2, height() / 2 );
m.scale( 1, -1 );

painter.setMatrix( m );


  static unsigned count=0;
  pen_.setWidth(5);
  painter.setPen(pen_);
  qDebug() << "xxx paint: " << count++;

  vector<Point>::const_iterator it=points_.begin();

  it = min_element(points_.begin(), points_.end());
  bool ok;
  //QPoint min_p(-((it->first).toDouble(&ok) * 10000000), -((it->second).toDouble(&ok)*10000000));
  //QPoint min_p(((it->first).toDouble(&ok) * 10000000/scale_), ((it->second).toDouble(&ok)*10000000/scale_));
  QPoint min_p(min_x_/scale_, min_y_/scale_);
  qDebug() << " ## min_p: " << min_p;
  it=points_.begin();
  for (int i=0; it !=points_.end(); ++it, ++i)
  {
    QPoint p((it->first).toDouble(&ok) * 10000/scale_, (it->second).toDouble(&ok)*10000/scale_);
    qDebug() << i << " ## p: " << p;
    qDebug() << i << " ## p-min_p: " << p-min_p;
    painter.drawPoint(p-min_p);
  }

  #if 0
  QPointF p1(22.3653542, 20.9007247);
  QPointF p3(50.3653669, 300.9007376);
  //painter.drawLine(p1, p3);
  painter.drawPoint(p1);
  painter.drawPoint(p3);
  QPointF tp(-22, -120);
  painter.translate(tp);
  painter.scale(0.5, 0.5);
  //painter.drawLine(p1, p3);
  painter.drawPoint(p1);
  painter.drawPoint(p3);
  #endif

#if 0
  QPoint min_p(((it->first).toDouble(&ok) * 10000000), ((it->second).toDouble(&ok)*10000000));
  qDebug() << "min p: " << min_p;

  QPainterPath path;

  path.moveTo(QPoint(0,0));
  for (int i=0; it !=points_.end(); ++it, ++i)
  {
    //QPointF p(it->first,
    //(*it)->first; //it->second;
    //QPoint p((it->first).toDouble(&ok), (it->second).toDouble(&ok));
    QPoint p((it->first).toDouble(&ok) * 10000000, (it->second).toDouble(&ok)*10000000);
    QPoint np=p-min_p;
    painter.drawPoint(np);
    qDebug() << i << " ## np: " << np;

    //QPoint p(20+i*10, 20+i*10);
    //path.lineTo(p);
    //if (i==1) break;
    //qDebug() << i << " ## p: " << p;
  }
  //painter.translate(min_p);
  //qreal s=0.9;
  //painter.scale(s,s);
  //painter.drawPath(path);
#endif


#if 0
  QPointF p2(22.3653542, 120.9007247);
  QPointF p3(22.3653669, 120.9007376);

  painter.drawPoint(p2);
  painter.drawPoint(p3);
  painter.drawLine(p1, p3);
#endif
#endif
}


PointDisplay::PointDisplay(QWidget * parent, Qt::WindowFlags f):QFrame(parent, f)
{
  QHBoxLayout *main_layout = new QHBoxLayout(this);

  splitter_ = new QSplitter(0);
  main_layout->addWidget(splitter_);

  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
  pv_ = new PointView();
  point_tree_view_ = new QTreeWidget();

  QHBoxLayout *scale_layout = new QHBoxLayout;


  scale_slider_ = new QSlider(Qt::Horizontal);
  scale_slider_->setMinimum(-10);
  scale_slider_->setMaximum(10);
  scale_slider_->setValue(1);
  
  scale_spin_box_ = new QSpinBox;
  scale_spin_box_->setMinimum(-10);
  scale_spin_box_->setMaximum(10);
  scale_spin_box_->setValue(1);
  //layout->addWidget(pv_);
  scale_layout->addWidget(scale_spin_box_);
  scale_layout->addWidget(scale_slider_);

  splitter_->addWidget(pv_);

  splitter_->setLayout(layout);
  //setLayout(layout);
  layout->addWidget(point_tree_view_);
  layout->addLayout(scale_layout);
  //layout->addWidget(scale_spin_box_);
  //layout->addWidget(scale_slider_);
  connect(scale_slider_, SIGNAL(valueChanged(int)), this, SLOT(change_scale_slot(int)));
  //connect(scale_spin_box_, SIGNAL(valueChanged(int)), scale_slider_, SIGNAL(valueChanged(int)));
  connect(scale_slider_, SIGNAL(valueChanged(int)), scale_spin_box_, SLOT(setValue(int)));
  connect(scale_spin_box_, SIGNAL(valueChanged(int)), scale_slider_, SLOT(setValue(int)));
}

void PointDisplay::change_scale_slot(int value)
{
  qDebug() << "value: " << value;
  qreal r=value/100;
  pv_->set_scale(value);
  //scale_spin_box_->setValue(value);
  //scale_slider_->setValue(value);
  update();
}

void PointDisplay::closeEvent( QCloseEvent * event )
{
  point_tree_view_->clear();
  scale_slider_->setValue(1);
  scale_spin_box_->setValue(1);
}

void PointDisplay::set_point(const vector<Point> &p)
{
  qreal min_x=999.999, min_y=999.999;
  qreal x=0.0, y=0.0;
  vector<Point>::const_iterator it=p.begin();
  for (; it!=p.end() ; ++it)
  {
    bool ok;

    QTreeWidgetItem *item = new QTreeWidgetItem(point_tree_view_);
    item->setText(0, QString("(%1, %2)").arg(it->first).arg(it->second));
    x=(it->first).toDouble(&ok);
    if (min_x > x)
      min_x=x;
    y=(it->second).toDouble(&ok);
    if (min_y > y)
      min_y=y;

  }
  pv_->set_point(p);
  min_x*=10000;
  min_y*=10000;
  pv_->set_min_point(min_x, min_y);
}
