#ifndef SCREEN_WIDGET_H
#define SCREEN_WIDGET_H

#include <qpushbutton.h>

#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>


// from odevice_abstractmobiledevice.cpp


//#include <linux/fb.h> better not rely on kernel headers in userspace ...

// _IO and friends are only defined in kernel headers ...
#define OD_IOC(dir,type,number,size)    (( dir << 30 ) | ( type << 8 ) | ( number ) | ( size << 16 ))
#define OD_IO(type,number)              OD_IOC(0,type,number,0)

#define FBIOBLANK             OD_IO( 'F', 0x11 )	// 0x4611

/* VESA Blanking Levels */
#define VESA_NO_BLANKING      0
#define VESA_VSYNC_SUSPEND    1
#define VESA_HSYNC_SUSPEND    2
#define VESA_POWERDOWN        3

extern int fd;

inline int init_screen()
{
#ifdef DEVFS
  if ((fd =::open ("/dev/fb/0", O_RDWR)) < 0)
#else
  if ((fd =::open ("/dev/fb0", O_RDWR)) < 0)
#endif
  {
    return -1;
  }
  return 0;
}

inline void switch_screen(bool on)
{
  ::ioctl (fd, FBIOBLANK, on ? VESA_NO_BLANKING : VESA_POWERDOWN);
}


namespace DS
{
  class Light:public QWidget
  {
    Q_OBJECT 
    public:
      Light(QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
       ~Light ()
      {
        ::close (fd);
      }
    public slots:
      void screen ()	// turn on/off screen
      {
        switch_screen (on_);
	on_=!on_;
	qDebug("%d",on_);
      }
    private:
      bool on_;
      QPushButton *button_;
  };
}				// end namespace DS


#endif
