/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: qpe_input_w.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: QPE input method 的 widget 
 */

#ifndef QPE_INPUT_W_H
#define QPE_INPUT_W_H


//#include <qwidget.h>
#include <qframe.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qtextcodec.h>

#include "input_handler.h"

#include <string>

//namespace DS
//{
 class QPEInputMethod : public QFrame
 //class QPEInputMethod : public QWidget
 {
  Q_OBJECT
  enum INPUT_STATUS {ENGLISH=0,ET_PHONETIC=1,ET_CHANGJEI=2,LAST_INPUT_METHOD};

  public:

   QPEInputMethod(QWidget *parent,const char *name,WFlags f);
   virtual ~QPEInputMethod();
   QSize sizeHint() const;
   void draw_line(int x1, int y1, int x2, int y2);
   void draw_text(int x,int y,DS::Key *key);
   void draw_text(int x,int y,int w,int h,QString str);
   void draw_text(int x,int y,int w,int h,std::string str); // str is big5 string
   void draw_icon(int x,int y,int w,int h,const QPixmap *pixmap);
   void resizeEvent(QResizeEvent*);
   void paintEvent(QPaintEvent* e);
   void mousePressEvent(QMouseEvent*);
   void mouseReleaseEvent(QMouseEvent*);
   int key_unit_width(){return _key_unit_width;}
   int key_unit_height(){return _key_unit_height;}
   void set_row(int row){_cell_rows=row;}
   void set_col(int col){_cell_cols=col;}

  private:
  //protected:
   signals:
    void keyPress( ushort unicode, ushort keycode, ushort modifiers, bool press,bool repeat );
  private slots:
    void change_input_method(int id);
  private:
   void calc_max_window_rect();
   int _cell_rows; //  QPEInputMethod widget 的 rows 有幾個 cell
   int _cell_cols; // QPEInputMethod widget 的 col 有幾個 cell
   int _key_unit_width; // QPEInputMethod key width 的基本單位
   int _key_unit_height; // QPEInputMethod key height 的基本單位
   QPainter _painter;
   //QPainter _buffer_painter; // dobule buffer 用
   //QPixmap _buffer;
   DS::BaseInput *_input[LAST_INPUT_METHOD];
   INPUT_STATUS _input_status;
   QPopupMenu *_change_input_method;
   DS::QtKey _qt_key;



 };

//} // end namespace DS

#endif
