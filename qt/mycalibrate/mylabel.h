#include <qlabel.h>
#include <qwidget.h>

#ifndef mylabel_h
 #define mylabel_h 1

// 用 label widget 顯示座標 h3600 device、mouse event 的座標
// 作法：用兩個 qlabel 分別來顯示座標
class Mylabel : public QWidget
{
 Q_OBJECT

 public: 		
  Mylabel();
  ~Mylabel();
  
 private:
  //void mousePressEvent(QMouseEvent*);
  //void mouseReleaseEvent(QMouseEvent*);
  //int readMouseDevice(char *mouseDevice);
  QLabel *_h3600_label,*_mouse_event_label;  

};





#endif
