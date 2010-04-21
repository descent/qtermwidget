/****************************************************************************
** $Id: qt/examples/hello/main.cpp   2.3.7   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "hello.h"
#include <qapplication.h>

#include <string>


/*
  The program starts here. It parses the command line and builds a message
  string to be displayed by the Hello widget.
*/

int main( int argc, char **argv )
{
    QApplication a(argc,argv);
    std::string s("Hello, §Ú¬O§º¥K¤¯, ¯QÀt");
    if (argc >= 2)
    {
      s = argv[1];
    }

    Hello h( s.c_str() );
    h.setCaption( "Qt says hello" );
    QObject::connect( &h, SIGNAL(clicked()), &a, SLOT(quit()) );
    h.setFont( QFont("times",32,QFont::Bold) );		// default font
    h.setBackgroundColor( Qt::white );			// default bg color
    a.setMainWidget( &h );
    h.show();
    return a.exec();
}
