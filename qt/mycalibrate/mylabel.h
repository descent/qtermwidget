#include <qlabel.h>
#include <qwidget.h>

#ifndef mylabel_h
 #define mylabel_h 1

// �� label widget ��ܮy�� h3600 device�Bmouse event ���y��
// �@�k�G�Ψ�� qlabel ���O����ܮy��
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
