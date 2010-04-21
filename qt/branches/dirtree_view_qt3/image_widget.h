#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <qwidget.h>
#include <qpixmap.h>

const QString fp("/home/descent/zentai/ska/f020331001.jpg");

class ImageWidget: public QWidget
{
  Q_OBJECT

  public:
    ImageWidget(QWidget * parent = 0):QWidget(parent),pixmap_(fp)
    {
    }
    void set_pixmap(const QPixmap &pm){pixmap_=pm;}
    ~ImageWidget(){}
  protected:
    void paintEvent(QPaintEvent *e);
  private:
    QPixmap pixmap_;
};


#endif
