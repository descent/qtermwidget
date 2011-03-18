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
  QPainter painter(this);
  pen_.setWidth(10);
  painter.setPen(pen_);

  vector<Point>::const_iterator it=points_.begin();

  qDebug() << "xxx paint: ";
  QPointF p1(60.3653542, 100.9007247);
  QPointF p3(85.3653669, 120.9007376);
  painter.scale(scale_, scale_);
  painter.drawLine(p1, p3);
  //painter.drawPoint(p1);
  //painter.drawPoint(p3);
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
  it = min_element(points_.begin(), points_.end());
  bool ok;
  //QPoint min_p(-((it->first).toDouble(&ok) * 10000000), -((it->second).toDouble(&ok)*10000000));
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
}


PointDisplay::PointDisplay(QWidget * parent, Qt::WindowFlags f):QFrame(parent, f)
{
  //splitter_ = new QSplitter(this);

  QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
  pv_ = new PointView();
  point_tree_view_ = new QTreeWidget();

  QHBoxLayout *scale_layout = new QHBoxLayout;


  scale_slider_ = new QSlider(Qt::Horizontal);
  scale_slider_->setMinimum(-10);
  scale_slider_->setMaximum(10);
  //scale_slider_->setValue(5);
  
  scale_spin_box_ = new QSpinBox;
  scale_spin_box_->setMinimum(-10);
  scale_spin_box_->setMaximum(10);
  //layout->addWidget(pv_);
  scale_layout->addWidget(scale_spin_box_);
  scale_layout->addWidget(scale_slider_);

  //splitter_->addWidget(pv_);

  //splitter_->setLayout(layout);
  setLayout(layout);
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

void PointDisplay::set_point(const vector<Point> &p)
{
  vector<Point>::const_iterator it=p.begin();
  for (; it!=p.end() ; ++it)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(point_tree_view_);
    item->setText(0, QString("(%1, %2)").arg(it->first).arg(it->second));
  }
}
