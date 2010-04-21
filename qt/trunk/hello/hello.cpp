/****************************************************************************
** $Id: qt/examples/hello/hello.cpp   2.3.7   edited 2001-01-26 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "hello.h"
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>

#include "convertencode.h"
#include "big5str_to_qstr.h"

/*
  Constructs a Hello widget. Starts a 40 ms animation timer.
*/

Hello::Hello( const char *text, QWidget *parent, const char *name )
    : QWidget(parent,name), b(0)
{

    //DS::ConvertEncode convert_encode;
    //std::vector<int> unicode;
    //convert_encode.convert(text, unicode);

 //   qDebug("err 1");
    //QChar *qchar_array = new QChar [unicode.size()];
    //for (int i=0 ; i < unicode.size() ; ++i)
      //qchar_array[i]=QChar(unicode[i]);
    //QChar *unicode[] = new QChar();{25105, 26159, 23435, 20181, 20161};

    QTimer *timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), SLOT(animate()) );
    timer->start( 40 );
    //QChar unicode[]={25105, 26159, 23435, 20181, 20161};
    //t=QString(qchar_array, unicode.size());
    t=DS::big5str_to_qstr(text);
//    qDebug("err 2");

    resize( 260, 130 );
    //delete [] qchar_array;
}


/*
  This private slot is called each time the timer fires.
*/

void Hello::animate()
{
    b = (b + 1) & 15;
    repaint( FALSE );
}


/*
  Handles mouse button release events for the Hello widget.

  We emit the clicked() signal when the mouse is released inside
  the widget.
*/

void Hello::mouseReleaseEvent( QMouseEvent *e )
{
    if ( rect().contains( e->pos() ) )
        emit clicked();
}


/*
  Handles paint events for the Hello widget.

  Flicker-free update. The text is first drawn in the pixmap and the
  pixmap is then blt'ed to the screen.
*/

void Hello::paintEvent( QPaintEvent * )
{
    static int sin_tbl[16] = {
        0, 38, 71, 92, 100, 92, 71, 38,	0, -38, -71, -92, -100, -92, -71, -38};

    if ( t.isEmpty() )
        return;

    // 1: Compute some sizes, positions etc.
    QFontMetrics fm = fontMetrics();
    int w = fm.width(t) + 20;
    int h = fm.height() * 2;
    int pmx = width()/2 - w/2;
    int pmy = height()/2 - h/2;

    // 2: Create the pixmap and fill it with the widget's background
    QPixmap pm( w, h );
    pm.fill( this, pmx, pmy );

    // 3: Paint the pixmap. Cool wave effect
    QPainter p;
    int x = 10;
    int y = h/2 + fm.descent();
    int i = 0;
    p.begin( &pm );
    //p.setFont( font() );
    p.setFont(QFont("unifont"));
    while ( !t[i].isNull() ) {
        int i16 = (b+i) & 15;
        p.setPen( QColor((15-i16)*16,255,255,QColor::Hsv) );
        p.drawText( x, y-sin_tbl[i16]*h/800, t.mid(i,1), 1 );
        x += fm.width( t[i] );
        i++;
    }
    p.end();

    // 4: Copy the pixmap to the Hello widget
    bitBlt( this, pmx, pmy, &pm );
}
