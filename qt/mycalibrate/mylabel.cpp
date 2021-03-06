#include "mylabel.h"

#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
	unsigned short pressure;
	unsigned short x;
	unsigned short y;
	unsigned short pad;
} TS_EVENT;

const int mouseBufSize=128;

Mylabel::Mylabel()
{
 _h3600_label=new QLabel(this);
 //_mouse_event_label=new QLabel(this);  	
 _h3600_label->setGeometry(15,40,30,60);
 //_mouse_event_label->setGeometry(30,10,2,12);
 _h3600_label->setText("h3600");
 //_mouse_event_label->setText("mouse_event");
	
}	

Mylabel::~Mylabel()
{

}

/*

void Mylabel::mousePressEvent(QMouseEvent*)
{
 
}

void Mylabel::mouseReleaseEvent(QMouseEvent*)
{
 char *mouseDevice="/dev/h3600_ts";
 readMouseDevice(mouseDevice);
 
}

int Mylabel::readMouseDevice(char *mouseDevice)
{
 int mouseFD;

 bool waspressed=false;
 int mouseIdx=0;
 unsigned char mouseBuf[mouseBufSize];
 if ((mouseFD = open( "/dev/h3600_ts", O_RDONLY | O_NDELAY)) == -1) 
 {
  perror("mouseFD = open( \"/dev/h3600_ts\", O_RDONLY | O_NDELAY) error !!");
  return -1;
 }
 
 int n;
 TS_EVENT *data;
 
 ofstream h3600_data("h3600");
 //while (1)
 {
  n = read(mouseFD, mouseBuf,sizeof(TS_EVENT));
  if (n==-1)
   cout << "Can not read h3600_ts data !" << endl;
   //continue; // the touch screen not be touched
  data = (TS_EVENT *)mouseBuf;
  cout << "------------------------------------------------" << endl;
  cout <<  data->pressure << "  ( " << data->x  << " , "  << data->y << "  )"<< data->pad << endl;
 }


 h3600_data.close();
 
	
 if (mouseFD >= 0)
 {
  close(mouseFD); 
  return 0;
  }
  else
   return -1;
}

*/
