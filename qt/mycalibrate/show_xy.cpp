#include <qapplication.h>

#include "mylabel.h"

int main(int argc,char **argv)
{
 QApplication myapp(argc,argv);
 Mylabel * showXY = new Mylabel;
 showXY->setGeometry(10,10,150,150);
 myapp.setMainWidget(showXY);
 showXY->show();
 
 return myapp.exec();	
	
}	
