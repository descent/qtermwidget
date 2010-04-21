#include "grid_thumb.h"



void GridThumb::add_image_thumb(const char *fp)
{
  
  ImageThumb *img = new ImageThumb(fp,FIT_WINDOW, 100,100,this);
  //ImageThumb *img = new ImageThumb(fp,FIT_WINDOW, 100,100,background_);
  qDebug("b img: %d,%d", img->width(), img->height() );
  //grid_layout_->addWidget(img, row_++,0);
  qDebug("a img: %d,%d", img->width(), img->height() );
  //img->resize(100,100);
  //QScrollView::resizeContents(img->width(), img->height());
  //addChild(img,0,0);
  connect(img, SIGNAL(clicked(const QPixmap &) ),this, SLOT(show_image(const QPixmap &) ));
}



GridThumb::GridThumb(QWidget * parent , const char * name , WFlags f ):QScrollView(parent, name, f | WMouseNoMask)
{
  thumb_width_= width();
  thumb_height_=height();
  row_=0;
  //QScrollView::resizeContents(1000,1000);
  background_ = new QWidget(0) ;
  addChild(background_);
  grid_layout_ = new QGridLayout(background_);
  //add_image_thumb("p.png");
  /*
  const char *fp="p.png";
  ImageThumb *img = new ImageThumb(fp,FIT_WINDOW, this);
  */
  //img->resize(80,50);
  //QScrollView::resizeContents(img->width(), img->height());
  //QGridLayout * grid_layout = new QGridLayout(this);
  //QLabel *text = new QLabel("test", this);
  //text->resize(100,100);

  //grid_layout->addWidget(text, 0,0);
  //QScrollView::resizeContents(100,100);

  //ImageThumb *image_1 = new ImageThumb(fp,FIT_WINDOW, this);
  //image_1->resize(100,100);
  //QScrollView::resizeContents(img->width(), img->height());
  //grid_layout->addWidget(image_1, 0,0);
  //qDebug("gridthudb %d", isEnabled());
  //qDebug("parent gridthudb %d", parentWidget()->parentWidget()->parentWidget()->isEnabled());

  /*
  ImageThumb *image_2 = new ImageThumb("/home/descent/kyok_005c.jpg",FIT_WINDOW,  this);
  grid_layout->addWidget(image_2, 1,0);
  grid_layout->setSpacing(5);
  grid_layout->setMargin(5);
  ImageThumb *image_3 = new ImageThumb("p.png",FIT_WINDOW, this);
  ImageThumb *image_4 = new ImageThumb("p1.png",FIT_WINDOW, this);
  grid_layout->addWidget(image_3, 0,1);
  grid_layout->addWidget(image_4, 1,1);
  */

  //setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding) );
}
/*
void GridThumb:: contentsMousePressEvent ( QMouseEvent * e )
{
  qDebug("press mouse");
}

void GridThumb:: contentsMouseReleaseEvent ( QMouseEvent * e )
{
  qDebug("release mouse");
  ImageWidget * im = new ImageWidget("p.png"); 
  im->show();
}
*/

ImageThumb::ImageThumb(const char *fp, ShowMode show_mode, int w,int h, QWidget * parent, const char * name , WFlags f ):QFrame(parent,name,f)
{
  //QGridLayout *grid_layout=new QGridLayout(this,4,1);
  //grid_layout->setRowSpacing(3,10);
  //grid_layout->setRowStretch(2,10); // add this let QLabel 不會跟著放大縮小 
  //QVBox * v_box= new QVBox(this);

  iw_ = new ImageWidget(fp, show_mode, this);
  connect(iw_, SIGNAL(clicked(const QPixmap & ) ), this, SLOT(image_clicked(const QPixmap &) ) );
  //iw->setFrameStyle(QFrame::Panel | QFrame::Raised);
  iw_->setFrameStyle(QFrame::Box | QFrame::Raised);
  //iw_->setGeometry(0,0,width(), height()-24);

  //QVBoxLayout *v_boxlayout = new QVBoxLayout(this);

  resize(w,h);
  text_ = new QLabel(fp, this);
  text_->setAlignment(QLabel::AlignHCenter | QLabel::AlignVCenter);
  text_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  text_->setGeometry(0, height()-24, width(), 24);
  iw_->resize(width(),  height()-24 );

  qDebug("image: %d,%d", iw_->height(), iw_->width());
  qDebug("text: %d,%d", text_->height(), text_->width());
  qDebug("this:%d,%d", height(), width());

}

