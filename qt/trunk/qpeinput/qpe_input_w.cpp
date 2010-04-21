/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: qpe_input_w.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: QPE input method 的 widget 
 */

//#include <qfont.h>

#include <algorithm>
#include <string>

#include <qapplication.h>
#include <qtextcodec.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>




#include "qpe_input_w.h"
#include "etphonetic_input.h"
#include "etchangjei_input.h"

#include "big5str_to_qstr.h"

#include <cstdlib>

const std::string phonetic_fn="/plugins/inputmethods/phonetic.tab";
const std::string changjei_fn="/plugins/inputmethods/changjei.tab";


QPEInputMethod::QPEInputMethod(QWidget *parent,const char *name,WFlags f):QFrame(parent,name,f)
//QPEInputMethod::QPEInputMethod(QWidget *parent,const char *name,WFlags f):QWidget(parent,name,f)
{
 // qt3
 //setBackgroundMode(Qt::NoBackground);
 // qt2
 setBackgroundMode(NoBackground);

 _input[ENGLISH]=new DS::EnglishInput(this);
 _input[ET_PHONETIC]=new DS::EtPhoneticInput(this,std::string(getenv("QPEDIR"))+ phonetic_fn);
 _input[ET_CHANGJEI]=new DS::EtChangjeiInput(this,std::string(getenv("QPEDIR"))+ changjei_fn);


 _change_input_method = new QPopupMenu(this);
 _change_input_method->insertItem("english",this,SLOT(change_input_method(int)),0,ENGLISH);
 _change_input_method->insertItem("et phonetic",this,SLOT(change_input_method(int)),0,ET_PHONETIC);
 _change_input_method->insertItem("et changjei",this,SLOT(change_input_method(int)),0,ET_CHANGJEI);


 QFont font("unifont",16,15,false);
 //QFont font("mingliu",12,50,false);
 setFont(font);
 _input_status=ENGLISH;

 _cell_rows=5; //  QPEInputMethod widget 的 rows 有幾個 cell
 _cell_cols=15; // QPEInputMethod widget 的 col 有幾個 cell
 // QPEInputMethod key width 的基本單位
 _key_unit_width=(width() / _cell_cols) / 2 ; 
 //qDebug("_key_unit_width : %d",_key_unit_width);
 // QPEInputMethod key height 的基本單位
 _key_unit_height=fontMetrics().lineSpacing()+3; 


 _painter.begin(this);
 //_buffer_painter.begin(&_buffer);

 // 設定此 widget 的背景色
 const QColorGroup cg=colorGroup();
 QColor key_color= key_color=cg.dark();
 QColor key_bg=QColor(240,240,230);
 // qt3 使用
 //setEraseColor(key_bg);
 // qt2 使用
 setBackgroundColor(key_bg);
}

QPEInputMethod::~QPEInputMethod()
{
 _painter.end();
 //_buffer_painter.end();
}

void QPEInputMethod::draw_text(int x,int y,DS::Key *key)
{
 // 要將 key->_ch 轉成 unicode

 //QTextCodec *codec=QTextCodec::codecForName("big5");
 //QString str=(codec->toUnicode(key->_ch));
 //if (key->_ch[0]=='~')
  //qDebug("find ~");
 //_painter.drawText(x,y,_key_unit_width*key->_w_unit,_key_unit_height*key->_h_unit,AlignLeft,key->_ch);
 //_painter.drawText(728,21,_key_unit_width*key->_w_unit,_key_unit_height*key->_h_unit,AlignCenter,key->_ch);
 qDebug("x,y : %d,%d",x,y);
 qDebug("_ch : %s",key->_ch);
}

void QPEInputMethod::draw_text(int x,int y,int w,int h,std::string str) // str is big5 string
{
 
  QString qstr;
  DS::big5str_to_qstr(str, qstr); // maybe check return value is fail
  draw_text(x,y,w,h,qstr);

}

void QPEInputMethod::draw_text(int x,int y,int w,int h,QString str)
{
 const QColorGroup cg=colorGroup();
 QColor key_color= key_color=cg.dark();
 QColor key_bg=QColor(240,240,230);
 QPainter painter;
 painter.begin(this);

 painter.setPen(key_color);
 painter.drawText(x,y,w,h,AlignCenter,str);

 painter.end();
 //_painter.fillRect(geo);
 //_painter.setPen(key_color);
 //_buffer_painter.setPen(key_color);
 //_painter.drawText(x,y,w,h,AlignLeft,str);
 //_painter.drawText(x,y,w,h,AlignCenter,str);
 //_buffer_painter.drawText(x,y,w,h,AlignCenter,str);
 //qDebug("width %d",w);
 //qDebug("height %d",h);
}

void QPEInputMethod::draw_line(int x1, int y1, int x2, int y2)
{
 _painter.drawLine(x1,y1,x2,y2);
}

void QPEInputMethod::paintEvent(QPaintEvent* e)
{
 //hide();
 _painter.setClipRect(e->rect()); // 記得要加，否則重繪時會有錯誤
 //_buffer_painter.setClipRect(e->rect()); // 記得要加，否則重繪時會有錯誤
 //_painter.setClipRect(rect()); // 記得要加，否則重繪時會有錯誤
 _input[_input_status]->draw_keys();
 //show();
 //bitBlt(this,0,0,&_buffer);
}

void QPEInputMethod::resizeEvent(QResizeEvent*)
{
 qDebug("In QPEInputMethod::resizeEvent");
 // QPEInputMethod key width 的基本單位
 _key_unit_width=(width() / _cell_cols) / 2 ; 
 // QPEInputMethod key height 的基本單位
 _key_unit_height=fontMetrics().lineSpacing()+3; 
 //emit repaint();
 int pop_x,pop_y,pop_w,pop_h; // QPopupMenu 的起始位置及寬和高
 pop_x=0;
 pop_y=geometry().topLeft().y();
 pop_w=pop_h=50;
 /*
 qDebug("pop_x : %d",pop_x);
 qDebug("pop_y : %d",pop_y);
 qDebug("pop_w : %d",pop_w);
 qDebug("pop_h : %d",pop_h);
 */
 _change_input_method->setGeometry(pop_x,pop_y,pop_w,pop_h);
}

QSize QPEInputMethod::sizeHint() const
{
 return QSize(qApp->desktop()->width(),_key_unit_height*_cell_rows);
}

void QPEInputMethod::mousePressEvent(QMouseEvent* e)
{
 //_input[_input_status]->draw_keys();
 //e->x();
 //DS::QtKey
 _qt_key = _input[_input_status]->do_with_pos(e->x(),e->y());
 int modifiers=0;
 int unicode=_qt_key._unicode;
 int qkeycode=_qt_key._qt_keycode;

 switch (qkeycode)
 {
  case DS::NOP:
  {
   return;
  }
  case DS::CHANGE_INPUT_METHOD:
  {
   //_input_status=ET_PHONETIC;
   //emit repaint();
   //_change_input_method->show();
   return;
  }
  case DS::SHIFT:
  {
   //_input[_input_status]->set_keyboard_mode(BaseInput::NORMAL);
   //emit repaint();
   return;
  }
  case DS::CAPS_LOCK:
  {
   //_input[_input_status]->set_keyboard_mode(BaseInput::);
   //emit repaint();
   return;
  }
  default:
  {
   emit keyPress( unicode, qkeycode, modifiers, true, false );
   break;
  }
 }

}
void QPEInputMethod::mouseReleaseEvent(QMouseEvent* e)
{
 //DS::QtKey 
 //_qt_key;// = _input[_input_status]->do_with_pos(e->x(),e->y());
 int modifiers=0;
 int unicode=_qt_key._unicode;
 int qkeycode=_qt_key._qt_keycode;
 switch (qkeycode)
 {
  case DS::CHANGE_INPUT_METHOD:
  {
   //_change_input_method->show();
   _change_input_method->exec();
   return;
  }
 }
}

void QPEInputMethod::change_input_method(int id)
{
 hide();
 _input_status=static_cast<INPUT_STATUS>(id);
 qDebug("you choose method : %d",_input_status);
 
 //if (isVisible())
 hide();
 // 更改 widget 的大小, 因為 change_size() 會用到 resize() 所以會送出 repaint 
 // 的 event, 故我們不用 emit repaint();
 _input[_input_status]->change_size();

 QPoint old_tl=geometry().topLeft();

 int old_h=height();
 // min 是 generic algorithm
 // 134 是參考 QPE 的 inputmethods.cpp 裡的 code
 int h = std::min( sizeHint().height(), 134 );
 //int h = std::min( sizeHint().height(), 200 );

 // 當 frame buffer 的寬度超過 320 時
 // input method widget 會縮為寬度的一半
 /*
 if (qt_screen->width() <= 500)
 {
  resize( qApp->desktop()->width(), h);
 }
 else
 {
  resize( qApp->desktop()->width()/2, h);
 }
 */
 //move( 0, 621 - height - 10);
 //move( 0, 621 - height()-10 );
 //geometry (); topLeft ()
 // 在 resize 之後將 widget move 到正確的位置
 qDebug("old_tl.y() : %d",old_tl.y());
 qDebug("h : %d",h);
 qDebug("old_h : %d",old_h);
 if (h==114) // 當使用注音倉頡時
  move( 0, 459);
 if (h==95) // 當使用英數輸入時
  move( 0, 478);

 //move( 0, old_tl.y() - (h - old_h));
 qDebug("move postion %d",old_tl.y() - (h - old_h));
 //move( 0, old_tl.y() - h - old_h);
 //qDebug("mapToGlobal(QPoint()).y() %d",mapToGlobal(QPoint()).y());
 //move( 0, 581 - height );
 

 //if (!isVisible())
 show();

 // 這段程式碼從 taskbar.cpp void TaskBar::clacMaxWindowRect() 參考而來
 calc_max_window_rect(); 



 //emit repaint();
}

void QPEInputMethod::draw_icon(int x,int y,int w,int h,const QPixmap *pixmap)
{
 //_painter.drawPixmap(x,y,*pixmap);
 // drawTiledPixmap 可以將該 pixmap 全部畫到 w,h 的寬高矩形裡
 _painter.drawTiledPixmap(x,y,w,h,*pixmap);
}

void QPEInputMethod::calc_max_window_rect()
{
 #ifdef Q_WS_QWS
 QRect wr;
 int displayWidth  = qApp->desktop()->width();
 //QRect ir = inputMethods->inputRect();
 QRect ir = geometry();
 if ( ir.isValid() ) 
  wr.setCoords( 0, 0, displayWidth-1, ir.top()-1 );
 else 
  wr.setCoords( 0, 0, displayWidth-1, y()-1 );

 #if QT_VERSION < 300
  QWSServer::setMaxWindowRect( qt_screen->mapToDevice(wr,QSize(qt_screen->width(),qt_screen->height())));
 #else
  QWSServer::setMaxWindowRect( wr );
#endif
#endif

	/*
#ifdef Q_WS_QWS

 QRect wr,ir;
 int displayWidth  = qApp->desktop()->width();

 ir=geometry();
 //wr.setCoords(0,0,displayWidth,621-1);
 wr.setCoords(0,0,displayWidth,ir.top()-1);
#if QT_VERSION < 300

 QWSServer::setMaxWindowRect( qt_screen->mapToDevice(wr,QSize(qt_screen->width(),qt_screen->height())));
#else
  QWSServer::setMaxWindowRect( wr );
#endif
#endif
*/
}
