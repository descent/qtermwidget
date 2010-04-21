#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

#include <qpoint.h>
#include <qpointarray.h>

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
int main(int argc,char ** argv)
{
 int mouseFD;

 //QPoint oldmouse;
 bool waspressed=false;
 //QPointArray samples=5;
 unsigned int currSample=0;
 unsigned int numSamples=0;
 int mouseIdx=0;
 if ((mouseFD = open( "/dev/h3600_ts", O_RDONLY | O_NDELAY)) == -1) 
 {
  perror("mouseFD = open( \"/dev/h3600_ts\", O_RDONLY | O_NDELAY) error !!");
  return -1;
 }
 
 int n;
 TS_EVENT *data;
 
 unsigned char mouseBuf[mouseBufSize];
 ofstream h3600_data("h3600");
 while (1)
 {
  n = read(mouseFD, mouseBuf,mouseBufSize);
  if (n==-1)
   continue; // the touch screen not be touched
  data = (TS_EVENT *)mouseBuf;
  cout << "------------------------------------------------" << endl;
  cout << n << " " << data->pressure << "  ( " << data->x  << " , "  << data->y << "  )"<< data->pad << endl;
  //h3600_data << n << endl; 
  //cout << sizeof(TS_EVENT) << endl;
  
  /*
  if (n==-1)
  {
   perror("read(mouseFD, mouseBuf,sizeof(TS_EVENT)) error");
   if (mouseFD >= 0)
    close(mouseFD);
   return -1;
  }
  if ( n < sizeof(TS_EVENT))
  {
   cout << "read(mouseFD, mouseBuf,sizeof(TS_EVENT)) maybe read EOF" << endl ;
   if (mouseFD >= 0)
    close(mouseFD);
   return -1;
  }
  data = (TS_EVENT *)mouseBuf;
  if (data->pressure > 0)
   //h3600_data <<  data->pressure << "  ( " << data->x  << " , "  << data->y << "  )"<< endl;
   cout <<  data->pressure << "  ( " << data->x  << " , "  << data->y << "  )"<< endl;
   */
 }

/*
 int idx = 0;

 while (mouseIdx-idx >= (int)sizeof(TS_EVENT))
 {
  unsigned char *mb = mouseBuf+idx;
  idx+=sizeof(TS_EVENT);
 }
 */
 h3600_data.close();
 
	
    if (mouseFD >= 0)
	close(mouseFD);
}
