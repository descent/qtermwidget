#ifndef  GRID_THUMB_H
#define  GRID_THUMB_H


#include <qframe.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qlayout.h>

#include "image_widget.h"


class GridThumb: public QScrollView
{
  Q_OBJECT
  public:
    GridThumb(QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    void add_image_thumb(const char *fp);
  protected slots:
    void show_image(const QPixmap &p)
    {
      ImageWidget * im = new ImageWidget;
      im->set_pixmap(p);

      im->show();
    }
  protected:
    /*
    bool event ( QEvent * e )
    {
	      qDebug("in Gridthumb event");
      if (e->type() == QEvent::MouseButtonRelease)
      {
	      qDebug("MouseButtonRelease");
	      return true;
      }
      else
	      qDebug("not MouseButtonRelease");
        return QWidget::event(e);
    }
    */
    
    /*
    void contentsMousePressEvent ( QMouseEvent * e );
    void contentsMouseReleaseEvent ( QMouseEvent * e );
    void resizeContents ( int w, int h )
    {
      QScrollView::resizeContents(viewport()->width(), viewport()->height());
    }
    */
  private:
    QGridLayout * grid_layout_;
    int row_,col_;
    int max_row_, max_col_;
    int thumb_width_, thumb_height_;
    QWidget *background_;
};

class ImageThumb: public QFrame
{
  Q_OBJECT

  public:
    ImageThumb(const char *fp, ShowMode show_mode=FIT_WINDOW, int w=100,int h=100, QWidget* parent = 0, const char * name = 0, WFlags f = 0 );

    ImageThumb(const char *fp, QWidget* parent = 0, const char * name = 0, WFlags f = 0 ):QFrame(parent,name,f), pixmap_(fp)
    {
      
    }
    void set_pixmap(const QPixmap &p){pixmap_=p;}
    const QPixmap & set_pixmap()const {return pixmap_;}
  signals:
    void clicked();
    void clicked(const QPixmap &p);
  protected slots:
    void image_clicked(const QPixmap &p)
    {
      //emit clicked();
      emit clicked(p);
    }
  protected:
    QSizePolicy sizePolicy () const
    {
      return QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ); 
    }
    QSize sizeHint() const
    {
      return QSize(64,64);
    }

  /*
    void resizeEvent ( QResizeEvent * e)
    {
    }
    void paintEvent ( QPaintEvent * e)
    {
    }
    void mousePressEvent ( QMouseEvent * e )
    {
      qDebug("ImageThumb::mousePressEvent");
    }
    void mouseReleaseEvent ( QMouseEvent * e )
    {
      qDebug("ImageThumb::mouseReleaseEvent");
      //emit clicked();
    }
    */

  private:
    ImageWidget *iw_;
    QPixmap pixmap_;
    QLabel *text_;
};

#endif
