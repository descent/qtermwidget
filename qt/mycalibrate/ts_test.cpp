#include <iostream>
#include <fstream>

using namespace std;


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


// UCB ts 1X00 ªºª©¥»
typedef struct 
{
 unsigned short pressure;
 unsigned short x;
 unsigned short y;
 unsigned short pad;
 //struct timeval  stamp;
}ts_event;


int main(int argc,char ** argv)
{
 int mouseFD;

 cout << sizeof(ts_event) << endl;
 char dev_file[]="/dev/ucb1x00-ts";
 if ((mouseFD = open( dev_file, O_RDONLY | O_NDELAY) < 0))
 //if ((mouseFD = open( dev_file, O_RDONLY) < 0))
 {
 // cerr << "open " << dev_file << " error !!" << endl;
  return -1;
 }
 
 int n;
 ts_event *data;
 unsigned char mouseBuf[sizeof(ts_event)];
 
 while (1)
 {
  n = read(mouseFD, mouseBuf,sizeof(ts_event));
  if (n==-1)
  {
  // cout << "n : " << n << endl;
   continue; // the touch screen not be touched
  }
  if (n==0)
  {
   //cout << "n : " << n << endl;
   continue;
  }
  cout << "n : " << n << endl;
  data = reinterpret_cast<ts_event *>(mouseBuf);
  cout << "------------------------------------------------" << endl;
  cout <<  data->pressure << "  ( " << data->x  << " , "  << data->y << "  )"<< endl;
 }
  
	
  if (mouseFD >= 0)
   close(mouseFD);
}
