/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qwsmouse_qws.h>

#if defined(Q_WS_QWS) || defined(_WS_QWS_)

#include <qwidget.h>
#include <qpixmap.h>

class QTimer;

class Calibrate : public QWidget
{
    Q_OBJECT
public:
    Calibrate(QWidget* parent=0, const char * name=0, WFlags=0);
    ~Calibrate();

private:
    QPoint fromDevice( const QPoint &p );
    bool sanityCheck();
    void moveCrosshair( QPoint pt );
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );

private slots:
    void timeout();

private:
    QPixmap logo;
    QWSPointerCalibrationData cd;
    QWSPointerCalibrationData::Location location;
    QPoint crossPos;
    QPoint penPos;
    QPixmap saveUnder;
    QTimer *timer;
    int dx;
    int dy;
};

#endif // _WS_QWS_

