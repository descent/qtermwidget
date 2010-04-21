#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qscrollview.h> 

#include <cstdlib>

//const QString fp("/home/descent/kyok_005c.jpg");
//const QString fp(getenv("PIC")==0 ? "/home/descent/kyok_005c.jpg": getenv("PIC"));

enum ShowMode {FIT_WINDOW, ORG_PIC, FULL_SCREEN};

class ImageWidget: public QScrollView
{
  Q_OBJECT

  public:
    // WNorthWestGravity WStaticContents
    // qt2 WNorthWestGravity = qt3 WStaticContents
    ImageWidget(const char *fp, ShowMode show_mode=FIT_WINDOW ,QWidget * parent = 0, const char * name = 0, WFlags f = 0):QScrollView(parent, name, f | WRepaintNoErase | WResizeNoErase | WNorthWestGravity),pixmap_(fp)
    //ImageWidget(const char *fp, ShowMode show_mode=FIT_WINDOW ,QWidget * parent = 0, const char * name = 0, WFlags f = 0):QScrollView(parent, name, f),pixmap_(fp)
    {
      //setMinimumSize(64,64);
      //setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
      //setHScrollBarMode(QScrollView::AlwaysOn);
      //setVScrollBarMode(QScrollView::AlwaysOn);
      resize_contents(); 
      set_show_mode(FIT_WINDOW);
      //set_show_mode(ORG_PIC);
    }
    ImageWidget(ShowMode show_mode=FIT_WINDOW ,QWidget * parent = 0, const char * name = 0, WFlags f = 0):QScrollView(parent, name, f | WRepaintNoErase | WResizeNoErase)
    {
      resize_contents(); 
      set_show_mode(FIT_WINDOW);
    }
    /*
    ~ImageWidget()
    {
      free(fp_);
    }
    */
    void set_pixmap(const QPixmap &pm){pixmap_=pm;}
    const QPixmap &get_pixmap() const{return pixmap_;}

    void draw_stuff(QPainter *p);
    ~ImageWidget(){}
    void set_show_mode(ShowMode show_mode){show_mode_=show_mode;}
    ShowMode show_mode(){return show_mode_;}
  signals:
    void clicked();
    void clicked(const char *fp);
    void clicked(const QPixmap &p);
  protected:
    void contentsMousePressEvent ( QMouseEvent * e )
    {
      qDebug("in ImageWidget::contentsMousePressEvent");
    }
    void contentsMouseReleaseEvent ( QMouseEvent * e )
    {
      qDebug("in ImageWidget::contentsMouseReleaseEvent");
      //emit clicked();
      emit clicked(pixmap_);
    }
    /*
    void resizeContents(int w,int h)
    {
      qDebug("in resizeContents(int w,int h)");
    }
    */
    /*
    void resizeEvent ( QResizeEvent * e)
    {
      qDebug("in resizeEvent ( QResizeEvent * e)");
    }
    */
    QSize sizeHint() const;
    QSizePolicy sizePolicy() const;
    //void paintEvent(QPaintEvent *e);
    void drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph );
  private:
    void resize_contents ();
    QPixmap pixmap_;
    ShowMode show_mode_;
    //char *fp_;
};


#endif
