/****************************************************************************
** $Id:  qt/qwsmouse_qws.cpp   3.0.0   edited Sep 17 08:27 $
**
** Implementation of Qt/Embedded mouse drivers
**
** Created : 991025
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qwindowsystem_qws.h"
#include "qsocketnotifier.h"
#include "qwsevent_qws.h"
#include "qwscommand_qws.h"
#include "qwsutils_qws.h"
#include "qwsmouse_qws.h"

#include "qapplication.h"
#include "qpointarray.h"
#include "qtimer.h"
#include "qfile.h"
#include "qtextstream.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include "qgfx_qws.h"

#ifdef QT_QWS_CASSIOPEIA
#include <linux/tpanel.h>
//#define QWS_TOUCHPANEL
#endif

#ifdef QT_QWS_IPAQ
#define QT_QWS_IPAQ_RAW
typedef struct {
	unsigned short pressure;
	unsigned short x;
	unsigned short y;
	unsigned short pad;
} TS_EVENT;
#endif

/*!
  \class QWSMouseHandler qwsmouse_qws.h
  \ingroup qws
  \brief The QWSMouseHandler class is a mouse driver/handler for Qt/Embedded.

  The mouse driver/handler handles events from system devices and
  generates mouse events.

  A QWSMouseHandler will usually open some system device in its
  constructor, create a QSocketNotifier on that opened device and when
  it receives data, it will call mouseChanged() to send the event
  to Qt/Embedded for relaying to clients.
*/

/*!
  Constructs a mouse handler. This becomes the primary mouse handler.

  Note that once created, mouse handlers are controlled by the system
  and should not be deleted.
*/
QWSMouseHandler::QWSMouseHandler()
{
    QWSServer::setMouseHandler(this);
}

/*!
  Destroys the mouse handler. You should not call this directly.
*/
QWSMouseHandler::~QWSMouseHandler()
{
}

/*!
  To be called by the mouse handler to signal that the
  mouse is at position \a pos and the mouse buttons are
  in state \a bstate.
*/
void QWSMouseHandler::mouseChanged( const QPoint& pos, int bstate )
{
    QWSServer::sendMouseEvent(pos,bstate);
}


typedef struct {
    char *name;
    MouseProtocol id;
} MouseConfig;

static const MouseConfig mouseConfig[] = {
    { "Auto",		Auto },
    { "MouseMan",	MouseMan },
    { "IntelliMouse",	IntelliMouse },
    { "Microsoft",      Microsoft },
    { "QVFbMouse",      QVFBMouse },
    { "TPanel",         TPanel },
    { "BusMouse",       BusMouse },
    { 0,		Unknown }
};

static void limitToScreen( QPoint &pt )
{
    static int w = -1;
    static int h;
    if ( w < 0 ) {
	w = qt_screen->deviceWidth();
	h = qt_screen->deviceHeight();
    }

    pt.setX( QMIN( w-1, QMAX( 0, pt.x() )));
    pt.setY( QMIN( h-1, QMAX( 0, pt.y() )));
}

static QPoint &mousePos = QWSServer::mousePosition;


#ifndef QT_NO_QWS_MOUSE_AUTO
/*
 * Automatic-detection mouse driver
 */

class QAutoMouseSubHandler {
protected:
    enum { max_buf=32 };

    int fd;

    uchar buffer[max_buf];
    int nbuf;

    QPoint motion;
    int bstate;

    int goodness;
    int badness;

    virtual int tryData()=0;

public:
    QAutoMouseSubHandler(int f) : fd(f)
    {
	nbuf = bstate = goodness = badness = 0;
    }

    int file() const { return fd; }

    void closeIfNot(int& f)
    {
	if ( fd != f ) {
	    f = fd;
	    close(fd);
	}
    }

    void worse(int by=1) { badness+=by; }
    bool reliable() const { return goodness >= 5 && badness < 50; }
    int buttonState() const { return bstate; }
    bool motionPending() const { return motion!=QPoint(0,0); }
    QPoint takeMotion() { QPoint r=motion; motion=QPoint(0,0); return r; }

    void appendData(uchar* data, int length)
    {
	memcpy(buffer+nbuf, data, length);
	nbuf += length;
    }

    enum UsageResult { Insufficient, Motion, Button };

    UsageResult useData()
    {
	int pbstate = bstate;
	int n = tryData();
	if ( n > 0 ) {
	    if ( n<nbuf )
		memcpy( buffer, buffer+n, nbuf-n );
	    nbuf -= n;
	    return pbstate == bstate ? Motion : Button;
	}
	return Insufficient;
    }
};

class QAutoMouseSubHandler_intellimouse : public QAutoMouseSubHandler {
    int packetsize;
public:
    QAutoMouseSubHandler_intellimouse(int f) : QAutoMouseSubHandler(f)
    {
	init();
    }

    void init()
    {
	int n;
	uchar reply[20];

	tcflush(fd,TCIOFLUSH);
	static const uchar initseq[] = { 243, 200, 243, 100, 243, 80 };
	static const uchar query[] = { 0xf2 };
	if (write(fd, initseq, sizeof(initseq))!=sizeof(initseq)) {
	    badness = 100;
	    return;
	}
	usleep(10000);
	tcflush(fd,TCIOFLUSH);
	if (write(fd, query, sizeof(query))!=sizeof(query)) {
	    badness = 100;
	    return;
	}
	usleep(10000);
	n = read(fd, reply, 20);
	if ( n > 0 ) {
	    goodness = 10;
	    switch ( reply[n-1] ) {
	      case 3:
	      case 4:
		packetsize = 4;
		break;
	     default:
		packetsize = 3;
	    }
	} else {
	    badness = 100;
	}
    }

    int tryData()
    {
	if ( nbuf >= packetsize ) {
	    //int overflow = (buffer[0]>>6 )& 0x03;

	    if ( /*overflow ||*/ !(buffer[0] & 8) ) {
		badness++;
		return 1;
	    } else {
		motion +=
		    QPoint((buffer[0] & 0x10) ? buffer[1]-256 : buffer[1],
			   (buffer[0] & 0x20) ? 256-buffer[2] : -buffer[2]);
		int nbstate = buffer[0] & 0x7;
		if ( motion.x() || motion.y() || bstate != nbstate ) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
	    }
	    return packetsize;
	}
	return 0;
    }
};

class QAutoMouseSubHandler_serial : public QAutoMouseSubHandler {
public:
    QAutoMouseSubHandler_serial(int f) : QAutoMouseSubHandler(f)
    {
	initSerial();
    }

protected:
    void setflags(int f)
    {
	termios tty;
	tcgetattr(fd, &tty);
	tty.c_iflag     = IGNBRK | IGNPAR;
	tty.c_oflag     = 0;
	tty.c_lflag     = 0;
	tty.c_cflag     = f | CREAD | CLOCAL | HUPCL;
#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_SOLARIS)
	tty.c_line      = 0;
#endif
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN]  = 1;
	tcsetattr(fd, TCSANOW, &tty);
    }

private:
    void initSerial()
    {
	int speed[4] = { B9600, B4800, B2400, B1200 };

	for (int n = 0; n < 4; n++) {
	    setflags(CSTOPB | speed[n]);
	    write(fd, "*q", 2);
	    usleep(10000);
	}
    }
};

class QAutoMouseSubHandler_mousesystems : public QAutoMouseSubHandler_serial {
public:
    // ##### This driver has not been tested

    QAutoMouseSubHandler_mousesystems(int f) : QAutoMouseSubHandler_serial(f)
    {
	init();
    }

    void init()
    {
	setflags(B1200|CS8|CSTOPB);
	// 60Hz
	if (write(fd, "R", 1)!=1) {
	    badness = 100;
	    return;
	}
	tcflush(fd,TCIOFLUSH);
    }

    int tryData()
    {
	if ( nbuf >= 5 ) {
	    if ( (buffer[0] & 0xf8) != 0x80 ) {
		badness++;
		return 1;
	    }
	    motion +=
		QPoint((signed char)buffer[1] + (signed char)buffer[3],
		       -(signed char)buffer[2] + (signed char)buffer[4]);
	    int t = ~buffer[0];
	    int nbstate = ((t&3) << 1) | ((t&4) >> 2);
	    if ( motion.x() || motion.y() || bstate != nbstate ) {
		bstate = nbstate;
		goodness++;
	    } else {
		badness++;
		return 1;
	    }
	    return 5;
	}
	return 0;
    }
};

class QAutoMouseSubHandler_ms : public QAutoMouseSubHandler_serial {
    int mman;
public:
    QAutoMouseSubHandler_ms(int f) : QAutoMouseSubHandler_serial(f)
    {
	mman=0;
	init();
    }

    void init()
    {
	setflags(B1200|CS7);
	// 60Hz
	if (write(fd, "R", 1)!=1) {
	    badness = 100;
	    return;
	}
	tcflush(fd,TCIOFLUSH);
    }

    int tryData()
    {
	if ( !(buffer[0] & 0x40) ) {
	    if ( buffer[0] == 0x20 && (bstate & Qt::MidButton) ) {
		mman=1; // mouseman extension
	    }
	    return 1;
	}
	int extra = mman&&(bstate & Qt::MidButton);
	if ( nbuf >= 3+extra ) {
	    int nbstate = 0;
	    if ( buffer[0] == 0x40 && !bstate && !buffer[1] && !buffer[2] ) {
		nbstate = Qt::MidButton;
	    } else {
		nbstate = ((buffer[0] & 0x20) >> 5)
			| ((buffer[0] & 0x10) >> 3);
		if ( extra && buffer[3] == 0x20 )
		    nbstate = Qt::MidButton;
	    }

	    if ( buffer[1] & 0x40 ) {
		badness++;
		return 1;
	    } else {
		motion +=
		    QPoint((signed char)((buffer[0]&0x3)<<6)
			    |(signed char)(buffer[1]&0x3f),
			   (signed char)((buffer[0]&0xc)<<4)
			    |(signed char)(buffer[2]&0x3f));
		if ( motion.x() || motion.y() || bstate != nbstate ) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
		return 3+extra;
	    }
	}
	return 0;
    }
};

/*
QAutoMouseHandler::UsageResult QAutoMouseHandler::useDev(Dev& d)
{
    if ( d.nbuf >= mouseData[d.protocol].bytesPerPacket ) {
	uchar *mb = d.buf;
	int bstate = 0;
	int dx = 0;
	int dy = 0;

	switch (mouseProtocol) {
	    case MouseMan:
	    case IntelliMouse:
	    {
		bstate = mb[0] & 0x7; // assuming Qt::*Button order

		int overflow = (mb[0]>>6 )& 0x03;
		if (mouseProtocol == MouseMan && overflow) {
		    //### wheel events signalled with overflow bit, ignore for now
		}
		else {
		    bool xs = mb[0] & 0x10;
		    bool ys = mb[0] & 0x20;
		    dx = xs ? mb[1]-256 : mb[1];
		    dy = ys ? mb[2]-256 : mb[2];
		}
		break;
	    }
	    case Microsoft:
		if ( ((mb[0] & 0x20) >> 3) ) {
		    bstate |= Qt::LeftButton;
		}
		if ( ((mb[0] & 0x10) >> 4) ) {
		    bstate |= Qt::RightButton;
		}

		dx=(signed char)(((mb[0] & 0x03) << 6) | (mb[1] & 0x3f));
		dy=-(signed char)(((mb[0] & 0x0c) << 4) | (mb[2] & 0x3f));

		break;
	}
    }
    }
*/


bool QAutoMouseHandlerPrivate::sendEvent(QAutoMouseSubHandler& h)
{
	if ( h.reliable() ) {
	    mousePos += h.takeMotion();
	    limitToScreen( mousePos );
/*
qDebug("%d,%d %c%c%c",
mousePos.x(),mousePos.y(),
(h.buttonState()&Qt::LeftButton)?'L':'.',
(h.buttonState()&Qt::MidButton)?'M':'.',
(h.buttonState()&Qt::RightButton)?'R':'.');
*/
	    mouseChanged(mousePos,h.buttonState());
	    return TRUE;
	} else {
	    h.takeMotion();
	    if ( h.buttonState() & (Qt::RightButton|Qt::MidButton) ) {
		// Strange for the user to press right or middle without
		// a moving mouse!
		h.worse();
	    }
	    return FALSE;
	}
}

QAutoMouseHandlerPrivate::QAutoMouseHandlerPrivate()
{
    notifiers.setAutoDelete( TRUE );
    retries = 0;
    openDevices();
}

QAutoMouseHandlerPrivate::~QAutoMouseHandlerPrivate()
{
    closeDevices();
}

void QAutoMouseHandlerPrivate::openDevices()
{
    nsub=0;
    int fd;
    fd = open( "/dev/psaux", O_RDWR | O_NDELAY );
    if ( fd >= 0 ) {
	sub[nsub++] = new QAutoMouseSubHandler_intellimouse(fd);
	notify(fd);
    }

    char fn[] = "/dev/ttyS?";
    for (int ch='0'; ch<='3'; ch++) {
	fn[9] = ch;
	fd = open( fn, O_RDWR | O_NDELAY );
	if ( fd >= 0 ) {
	    //sub[nsub++] = new QAutoMouseSubHandler_intellimouse(fd);
	    sub[nsub++] = new QAutoMouseSubHandler_mousesystems(fd);
	    sub[nsub++] = new QAutoMouseSubHandler_ms(fd);
	    notify(fd);
	}
    }

    // ...
}

void QAutoMouseHandlerPrivate::closeDevices()
{
    int pfd=-1;
    for (int i=0; i<nsub; i++) {
	sub[i]->closeIfNot(pfd);
	delete sub[i];
    }
    notifiers.clear();
}

void QAutoMouseHandlerPrivate::notify(int fd)
{
    QSocketNotifier *mouseNotifier
	= new QSocketNotifier( fd, QSocketNotifier::Read, this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData(int)));
    notifiers.append( mouseNotifier );
}

void QAutoMouseHandlerPrivate::readMouseData(int fd)
{
    for (;;) {
	uchar buf[8];
	int n = read(fd, buf, 8);
	if ( n<=0 )
	    break;
	for (int i=0; i<nsub; i++) {
	    QAutoMouseSubHandler& h = *sub[i];
	    if ( h.file() == fd ) {
		h.appendData(buf,n);
		for (;;) {
		    switch ( h.useData() ) {
		      case QAutoMouseSubHandler::Button:
			sendEvent(h);
		      case QAutoMouseSubHandler::Insufficient:
			goto breakbreak;
		      case QAutoMouseSubHandler::Motion:
			break;
		    }
		}
		breakbreak:
		    ;
	    }
	}
    }
    bool any_reliable=FALSE;
    for (int i=0; i<nsub; i++) {
	QAutoMouseSubHandler& h = *sub[i];
	if ( h.motionPending() )
	    sendEvent(h);
	any_reliable = any_reliable || h.reliable();
    }
    if ( any_reliable ) {
	// ... get rid of all unreliable ones?  All bad ones?
    } else if ( retries < 2 ) {
	// Try again - maybe the mouse was being moved when we tried to init.
	closeDevices();
	openDevices();
	retries++;
    }
}
#endif //QT_NO_QWS_MOUSE_AUTO

#ifndef QT_NO_QWS_MOUSE_MANUAL

/*
 * Standard mouse driver
 */

typedef struct {
    int bytesPerPacket;
} MouseData;

static const MouseData mouseData[] = {
    { 3 },  // dummy for auto protocal,  correction made by add by YYD
    { 3 },  // MouseMan
    { 4 },  // intelliMouse
    { 3 },  // Microsoft
    { 0 },  // QVFBMouse,
    { 0 },  // TPanel,
    { 3 },  // BusMouse,
};



void QWSMouseHandlerPrivate::readMouseData()
{
    int n;
    if ( BusMouse == mouseProtocol ) {
	// a workaround of linux busmouse driver interface.
	// It'll only read 3 bytes a time and return all other buffer zeroed, thus cause protocol errors
	for (;;) {
	    if ( mouseBufSize - mouseIdx < 3 )
		break;
	    n = read( mouseFD, mouseBuf+mouseIdx, 3 );
	    if ( n != 3 )
		break;
	    mouseIdx += 3;
	}
    } else {
	do {
	    n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	    if ( n > 0 )
		mouseIdx += n;
	} while ( n > 0 );
    }
    handleMouseData();
}


/*
*/

void QWSMouseHandlerPrivate::handleMouseData()
{
    static const int accel_limit = 5;
    static const int accel = 2;

    int idx = 0;
    int bstate = 0;
    int dx = 0, dy = 0;
    bool sendEvent = false;
    int tdx = 0, tdy = 0;

    while ( mouseIdx-idx >= mouseData[mouseProtocol].bytesPerPacket ) {
#if 0 // debug
	qDebug( "Got mouse data" );
#endif
	uchar *mb = mouseBuf+idx;
	bstate = 0;
	dx = 0;
	dy = 0;
	sendEvent = false;
	switch (mouseProtocol) {
	    case MouseMan:
	    case IntelliMouse:
	    {
		if (mb[0] & 0x01)
		    bstate |= Qt::LeftButton;
		if (mb[0] & 0x02)
		    bstate |= Qt::RightButton;
		if (mb[0] & 0x04)
		    bstate |= Qt::MidButton;

		int overflow = (mb[0]>>6 )& 0x03;
		if (mouseProtocol == MouseMan && overflow) {
		    //### wheel events signalled with overflow bit, ignore for now
		}
		else {
		    bool xs = mb[0] & 0x10;
		    bool ys = mb[0] & 0x20;
		    dx = xs ? mb[1]-256 : mb[1];
		    dy = ys ? mb[2]-256 : mb[2];

		    sendEvent = true;
		}
#if 0 //debug
		if (mouseProtocol == MouseMan)
		    printf("(%2d) %02x %02x %02x ", idx, mb[0], mb[1], mb[2]);
		else
		    printf("(%2d) %02x %02x %02x %02x ",idx,mb[0],mb[1],mb[2],mb[3]);
		const char *b1 = (mb[0] & 0x01) ? "b1":"  ";//left
		const char *b2 = (mb[0] & 0x02) ? "b2":"  ";//right
		const char *b3 = (mb[0] & 0x04) ? "b3":"  ";//mid

		if ( overflow )
		    printf( "Overflow%d %s %s %s  (%4d,%4d)\n", overflow,
			    b1, b2, b3, mousePos.x(), mousePos.y() );
		else
		    printf( "%s %s %s (%+3d,%+3d)  (%4d,%4d)\n",
			    b1, b2, b3, dx, dy, mousePos.x(), mousePos.y() );
#endif
		break;
	    }
	    case Microsoft:
		if ( ((mb[0] & 0x20) >> 3) ) {
		    bstate |= Qt::LeftButton;
		}
		if ( ((mb[0] & 0x10) >> 4) ) {
		    bstate |= Qt::RightButton;
		}

		dx=(signed char)(((mb[0] & 0x03) << 6) | (mb[1] & 0x3f));
		dy=-(signed char)(((mb[0] & 0x0c) << 4) | (mb[2] & 0x3f));
		sendEvent=true;

		break;
	    case BusMouse:
	        if ( ((mb[0] & 0x4))  ) {
		    bstate |= Qt::LeftButton;
		}
		if ( ((mb[0] & 0x01)) ) {
		    bstate |= Qt::RightButton;
		}

		dx=(signed char)mb[1];
		dy=(signed char)mb[2];
		sendEvent=true;
		break;

	    default:
		qWarning( "Unknown mouse protocol in QWSMouseHandlerPrivate" );
		break;
	}
	if (sendEvent) {
	    if ( QABS(dx) > accel_limit || QABS(dy) > accel_limit ) {
		dx *= accel;
		dy *= accel;
	    }
	    tdx += dx;
	    tdy += dy;
	    if ( bstate != obstate ) {
		mousePos += QPoint(tdx,-tdy);
		limitToScreen( mousePos );
		mouseChanged(mousePos,bstate);
		sendEvent = FALSE;
		tdx = 0;
		tdy = 0;
		obstate = bstate;
	    }
	}
	idx += mouseData[mouseProtocol].bytesPerPacket;
    }
    if ( sendEvent ) {
	mousePos += QPoint(tdx,-tdy);
	limitToScreen( mousePos );
	mouseChanged(mousePos,bstate);
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
}


QWSMouseHandlerPrivate::QWSMouseHandlerPrivate( MouseProtocol protocol,
					  QString mouseDev )
{
    mouseProtocol = protocol;

    if ( mouseDev.isEmpty() )
	mouseDev = "/dev/mouse";

    obstate = -1;
    mouseFD = -1;
    mouseFD = open( mouseDev.local8Bit(), O_RDWR | O_NDELAY);
    if ( mouseFD < 0 ) {
	mouseFD = open( mouseDev.local8Bit(), O_RDONLY | O_NDELAY);
	if ( mouseFD < 0 )
	    qDebug( "Cannot open %s (%s)", mouseDev.ascii(),
		    strerror(errno));
    } else {
	// Clear pending input

	tcflush(mouseFD,TCIFLUSH);

	bool ps2 = false;

	switch (mouseProtocol) {

	    case MouseMan:
		ps2 = true;
		write(mouseFD,"",1);
		usleep(50000);
		write(mouseFD,"@EeI!",5);
		break;

	    case IntelliMouse: {
//		    ps2 = true;
		    const unsigned char init1[] = { 243, 200, 243, 100, 243, 80 };
		    const unsigned char init2[] = { 246, 230, 244, 243, 100, 232, 3 };
		    write(mouseFD,init1,sizeof(init1));
		    usleep(50000);
		    write(mouseFD,init2,sizeof(init2));
		}
		break;

	    case Microsoft:
		struct termios tty;

		tcgetattr(mouseFD, &tty);

		tty.c_iflag = IGNBRK | IGNPAR;
		tty.c_oflag = 0;
		tty.c_lflag = 0;
#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_SOLARIS)
		tty.c_line = 0;
#endif
		tty.c_cc[VTIME] = 0;
		tty.c_cc[VMIN] = 1;
		tty.c_cflag = B1200 | CS7 | CREAD | CLOCAL | HUPCL;
		tcsetattr(mouseFD, TCSAFLUSH, &tty); /* set parameters */
		break;

	    case BusMouse:
		usleep(50000);
		break;

	    default:
		qDebug("Unknown mouse protocol");
		exit(1);
	}

	if (ps2) {
	    char buf[] = { 246, 244 };
	    write(mouseFD,buf,1);
	    write(mouseFD,buf+1,1);
	}

	usleep(50000);
	tcflush(mouseFD,TCIFLUSH);	    // ### doesn't seem to work.
	usleep(50000);
	tcflush(mouseFD,TCIFLUSH);	    // ### doesn't seem to work.

	char buf[100];				// busmouse driver will not read if bufsize < 3,  YYD
	while (read(mouseFD, buf, 100) > 0) { }  // eat unwanted replies

	mouseIdx = 0;

	QSocketNotifier *mouseNotifier;
	mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read, this );
	connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    }
}

QWSMouseHandlerPrivate::~QWSMouseHandlerPrivate()
{
    if (mouseFD >= 0) {
	tcflush(mouseFD,TCIFLUSH);	    // yyd.
	close(mouseFD);
    }
}
#endif //QT_NO_QWS_MOUSE_MANUAL


#ifndef QT_NO_QWS_MOUSE_CALIBRATED
/*
 *
 */

QCalibratedMouseHandler::QCalibratedMouseHandler()
    : samples(5), currSample(0), numSamples(0)
{
    clearCalibration();
    readCalibration();
}

void QCalibratedMouseHandler::clearCalibration()
{
    a = 1;
    b = 0;
    c = 0;
    d = 0;
    e = 1;
    f = 0;
    s = 1;
}

void QCalibratedMouseHandler::writeCalibration()
{
    QString calFile = "/etc/pointercal";
#ifndef QT_NO_TEXTSTREAM
    QFile file( calFile );
    if ( file.open( IO_WriteOnly ) ) {
	QTextStream t( &file );
	t << a << " " << b << " " << c << " ";
	t << d << " " << e << " " << f << " " << s;
    } else
#endif
    {
	qDebug( "Could not save calibration: %s", calFile.latin1() );
    }
}

void QCalibratedMouseHandler::readCalibration()
{
    QString calFile = "/etc/pointercal";
#ifndef QT_NO_TEXTSTREAM
    QFile file( calFile );
    if ( file.open( IO_ReadOnly ) ) {
	QTextStream t( &file );
	t >> a >> b >> c >> d >> e >> f >> s;
    } else
#endif
    {
	qDebug( "Could not read calibration: %s", calFile.latin1() );
    }
}

void QCalibratedMouseHandler::calibrate( QWSPointerCalibrationData *cd )
{
    QPoint dev_tl = cd->devPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint dev_br = cd->devPoints[ QWSPointerCalibrationData::BottomRight ];
    QPoint screen_tl = cd->screenPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint screen_br = cd->screenPoints[ QWSPointerCalibrationData::BottomRight ];

    s = 1 << 16;

    a = s * (screen_tl.x() - screen_br.x() ) / (dev_tl.x() - dev_br.x());
    b = 0;
    c = s * screen_tl.x() - a * dev_tl.x();

    d = 0;
    e = s * (screen_tl.y() - screen_br.y() ) / (dev_tl.y() - dev_br.y());
    f = s * screen_tl.y() - e * dev_tl.y();

    writeCalibration();
}

QPoint QCalibratedMouseHandler::transform( const QPoint &p )
{
    QPoint tp;

    tp.setX( (a * p.x() + b * p.y() + c) / s );
    tp.setY( (d * p.x() + e * p.y() + f) / s );

    return tp;
}

void QCalibratedMouseHandler::setFilterSize( int s )
{
    samples.resize( s );
    numSamples = 0;
    currSample = 0;
}

bool QCalibratedMouseHandler::sendFiltered( const QPoint &p, int button )
{
    if ( !button ) {
	if ( numSamples >= samples.count() )
	    mouseChanged( mousePos, 0 );
	currSample = 0;
	numSamples = 0;
	return TRUE;
    }

    bool sent = FALSE;
    samples[currSample] = p;
    numSamples++;
    if ( numSamples >= samples.count() ) {
	int maxd = 0;
	unsigned int ignore = 0;
	// throw away the "worst" sample
	for ( unsigned int i = 0; i < samples.count(); i++ ) {
	    int d = ( mousePos - samples[i] ).manhattanLength();
	    if ( d > maxd ) {
		maxd = d;
		ignore = i;
	    }
	}
	bool first = TRUE;
	QPoint pos;
	// average the rest
	for ( unsigned int i = 0; i < samples.count(); i++ ) {
	    if ( ignore != i ) {
		if ( first ) {
		    pos = samples[i];
		    first = FALSE;
		} else {
		    pos += samples[i];
		}
	    }
	}
	pos /= (int)(samples.count() - 1);
	pos = transform( pos );
	if ( pos != mousePos || numSamples == samples.count() ) {
	    mousePos = pos;
	    mouseChanged( mousePos, button );
	    sent = TRUE;
	}
    }
    currSample++;
    if ( currSample >= samples.count() )
	currSample = 0;

    return sent;
}

#endif //QT_NO_QWS_MOUSE_CALIBRATED


#ifdef QT_QWS_CASSIOPEIA
QVrTPanelHandlerPrivate::QVrTPanelHandlerPrivate( MouseProtocol, QString dev ) :
    QCalibratedMouseHandler()
{
    if ( dev.isEmpty() )
	dev = "/dev/tpanel";

    if ((mouseFD = open( dev, O_RDONLY)) < 0) {
	qFatal( "Cannot open %s (%s)", dev.latin1(), strerror(errno));
    } else {
	sleep(1);
    }

    struct scanparam s;
    s.interval = 20000;
    s.settletime = 480;
    if ( ioctl(mouseFD, TPSETSCANPARM, &s) < 0
      || fcntl(mouseFD, F_SETFL, O_NONBLOCK) < 0 )
	qWarning("Error initializing touch panel.");

    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));

    rtimer = new QTimer( this );
    connect( rtimer, SIGNAL(timeout()), this, SLOT(sendRelease()));
    mouseIdx = 0;
    setFilterSize( 3 );

    printf("\033[?25l"); fflush(stdout); // VT100 cursor off
}

QVrTPanelHandlerPrivate::~QVrTPanelHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

void QVrTPanelHandlerPrivate::sendRelease()
{
    sendFiltered( mousePos, 0 );
}

void QVrTPanelHandlerPrivate::readMouseData()
{
    if(!qt_screen)
	return;
    static bool pressed = FALSE;

    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 && mouseIdx < mouseBufSize );

    int idx = 0;
    while ( mouseIdx-idx >= (int)sizeof( short ) * 6 ) {
	uchar *mb = mouseBuf+idx;
	ushort *data = (ushort *) mb;
	if ( data[0] & 0x8000 ) {
	    if ( data[5] > 750 ) {
		QPoint t(data[3]-data[4],data[2]-data[1]);
		if ( sendFiltered( t, Qt::LeftButton ) )
		    pressed = TRUE;
		if ( pressed )
		    rtimer->start( 200, TRUE ); // release unreliable
	    }
	} else if ( pressed ) {
	    rtimer->start( 50, TRUE );
	    pressed = FALSE;
	}
	idx += sizeof( ushort ) * 6;
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;

}
#endif //QT_QWS_CASSIOPEIA


#ifdef QT_QWS_IPAQ
QIpaqHandlerPrivate::QIpaqHandlerPrivate( MouseProtocol, QString )
    : samples(5), currSample(0), numSamples(0)
{
# ifdef QT_QWS_IPAQ_RAW
    if ((mouseFD = open( "/dev/h3600_tsraw", O_RDONLY | O_NDELAY)) < 0) {
# else
    if ((mouseFD = open( "/dev/h3600_ts", O_RDONLY | O_NDELAY)) < 0) {
# endif
	qWarning( "Cannot open /dev/h3600_ts (%s)", strerror(errno));
	return;
    }

    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    waspressed=FALSE;
    mouseIdx = 0;
}

QIpaqHandlerPrivate::~QIpaqHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

void QIpaqHandlerPrivate::readMouseData()
{
    if(!qt_screen)
	return;

    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 && mouseIdx < mouseBufSize );

    TS_EVENT *data;
    int idx = 0;
    while ( mouseIdx-idx >= (int)sizeof( TS_EVENT ) ) {
	uchar *mb = mouseBuf+idx;
	data = (TS_EVENT *) mb;
	if(data->pressure > 0) {
	    samples[currSample] = QPoint( data->x, data->y );
	    numSamples++;
	    if ( numSamples > samples.count() ) {
		int maxd = 0;
		unsigned int ignore = 0;
		// throw away the "worst" sample
		for ( unsigned int i = 0; i < samples.count(); i++ ) {
		    int d = ( mousePos - samples[i] ).manhattanLength();
		    if ( d > maxd ) {
			maxd = d;
			ignore = i;
		    }
		}
		bool first = TRUE;
		// average the rest
		for ( unsigned int i = 0; i < samples.count(); i++ ) {
		    if ( ignore != i ) {
			if ( first ) {
			    mousePos = samples[i];
			    first = FALSE;
			} else {
			    mousePos += samples[i];
			}
		    }
		}
		mousePos /= (int)(samples.count() - 1);
# ifdef QT_QWS_IPAQ_RAW
		mousePos = transform( mousePos );
# endif
		mouseChanged(mousePos,Qt::LeftButton);
		oldmouse=mousePos;
		waspressed=true;
	    }
	    currSample++;
	    if ( currSample >= samples.count() )
		currSample = 0;
	} else {
	    if(waspressed) {
		currSample = 0;
		numSamples = 0;
		mouseChanged(oldmouse,0);
		waspressed=false;
	    }
	}
	idx += sizeof( TS_EVENT );
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
}
#endif //QT_QWS_IPAQ

#ifdef QT_QWS_CUSTOMTOUCHPANEL
QCustomTPanelHandlerPrivate::QCustomTPanelHandlerPrivate( MouseProtocol, QString )
{
    if ((mouseFD = open( "/dev/ts", O_RDONLY)) < 0) {
	qWarning( "Cannot open /dev/ts (%s)", strerror(errno));
	return;
    } else {
	sleep(1);
    }

    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
}

QCustomTPanelHandlerPrivate::~QCustomTPanelHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

struct CustomTPdata {

  unsigned char status;
  unsigned short xpos;
  unsigned short ypos;

};

void QCustomTPanelHandlerPrivate::readMouseData()
{
    if(!qt_screen)
	return;
    CustomTPdata data;

    unsigned char data2[5];

    int ret;

    ret=read(mouseFD,data2,5);

    if(ret==5) {
	data.status=data2[0];
	data.xpos=(data2[1] << 8) | data2[2];
	data.ypos=(data2[3] << 8) | data2[4];
	QPoint q;
	q.setX(data.xpos);
	q.setY(data.ypos);
	mousePos=q;
	if(data.status & 0x40) {
	    mouseChanged(mousePos,Qt::LeftButton);
	} else {
	    mouseChanged(mousePos,0);
	}
    }
    if(ret<0) {
	qDebug("Error %s",strerror(errno));
    }
}
#endif //QT_QWS_CUSTOMTOUCHPANEL

/*
 * Virtual framebuffer mouse driver
 */

#ifndef QT_NO_QWS_VFB
#include "qvfbhdr.h"
extern int qws_display_id;

QVFbMouseHandlerPrivate::QVFbMouseHandlerPrivate( MouseProtocol, QString mouseDev )
{
    mouseFD = -1;
    if ( mouseDev.isEmpty() )
	mouseDev = QString(QT_VFB_MOUSE_PIPE).arg(qws_display_id);

    if ((mouseFD = open( mouseDev.local8Bit(), O_RDWR | O_NDELAY)) < 0) {
	qDebug( "Cannot open %s (%s)", mouseDev.ascii(),
		strerror(errno));
    } else {
	// Clear pending input
	char buf[2];
	while (read(mouseFD, buf, 1) > 0) { }

	mouseIdx = 0;

	QSocketNotifier *mouseNotifier;
	mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read, this );
	connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    }
}

QVFbMouseHandlerPrivate::~QVFbMouseHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

void QVFbMouseHandlerPrivate::readMouseData()
{
    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 );

    int idx = 0;
    while ( mouseIdx-idx >= int(sizeof( QPoint ) + sizeof( int )) ) {
	uchar *mb = mouseBuf+idx;
	QPoint *p = (QPoint *) mb;
	mb += sizeof( QPoint );
	int *bstate = (int *)mb;
	mousePos = *p;
	limitToScreen( mousePos );
	mouseChanged(mousePos, *bstate);
	idx += sizeof( QPoint ) + sizeof( int );
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
}
#endif //QT_NO_QWS_VFB

/*
 * return a QWSMouseHandler that supports /a spec.
 */

QWSMouseHandler* QWSServer::newMouseHandler(const QString& spec)
{
    static int init=0;
    if ( !init && qt_screen ) {
	init = 1;
	//	mousePos = QPoint(qt_screen->width()/2,
	//			  qt_screen->height()/2);
    }

    int c = spec.find(':');
    QString mouseProto;
    QString mouseDev;
    if ( c >= 0 ) {
	mouseProto = spec.left(c);
	mouseDev = spec.mid(c+1);
    } else {
	mouseProto = spec;
    }

    MouseProtocol mouseProtocol = Unknown;

    int idx = 0;
    while (mouseProtocol == Unknown && mouseConfig[idx].name) {
	if (mouseProto == QString(mouseConfig[idx].name)) {
	    mouseProtocol = mouseConfig[idx].id;
	}
	idx++;
    }


    QWSMouseHandler *handler = 0;

#ifdef QT_QWS_CUSTOMTOUCHPANEL
    handler=new QCustomTPanelHandlerPrivate(mouseProtocol,mouseDev);
#endif

#ifdef QT_QWS_IPAQ
    handler=new QIpaqHandlerPrivate(mouseProtocol,mouseDev);
#endif

#ifndef QWS_CUSTOMTOUCHPANEL
#ifndef QT_QWS_IPAQ
    switch ( mouseProtocol ) {
#ifndef QT_NO_QWS_MOUSE_AUTO
    case Auto:
	handler = new QAutoMouseHandlerPrivate();
	break;
#endif
#ifndef QT_NO_QWS_MOUSE_MANUAL
    case MouseMan:
    case IntelliMouse:
    case Microsoft:
    case BusMouse:
	handler = new QWSMouseHandlerPrivate( mouseProtocol, mouseDev );
	break;
#endif
#ifndef QT_NO_QWS_VFB
    case QVFBMouse:
	handler = new QVFbMouseHandlerPrivate( mouseProtocol, mouseDev );
	break;
#endif
#ifdef QT_QWS_CASSIOPEIA
    case TPanel:
	handler = new QVrTPanelHandlerPrivate( mouseProtocol, mouseDev );
	break;
#endif
    default:
	qDebug( "Mouse type %s unsupported", spec.latin1() );
    }
#endif
#endif

    return handler;
}

/*!
  \fn QWSMouseHandler::clearCalibration()

  This method is reimplemented in the calibrated mouse handler
  to clear calibration information. This version does nothing.
*/

/*!
  \fn QWSMouseHandler::calibrate(QWSPointerCalibrationData * )

  This method is reimplemented in the calibrated mouse handler
  to set calibration information (from, for instance, the QPE
  calibration screen). This version does nothing.
*/
