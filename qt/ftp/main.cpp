/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QApplication>
#include <QWindowsStyle>
//#include <QMacStyle>
#include <QWindowsXPStyle>
//#include <QLocale>

//#include <locale.h>



#include "ftpwindow.h"

using namespace Qt;

int main(int argc, char *argv[])
{
    //QLocale::setDefault(QLocale::c() );
    //qDebug("locale : %s", setlocale(LC_ALL, "C") );

    Q_INIT_RESOURCE(ftp);
    qDebug("QT_VERSION_STR : %s", QT_VERSION_STR);
    qDebug("QT_VERSION: %x", QT_VERSION);

    QApplication app(argc, argv);

    QStyle *style = new QWindowsStyle;
    //QStyle *windows_style = new QWindowsXPStyle;

    app.setStyle(style);
    FtpWindow ftpWin;
    ftpWin.show();

    //return ftpWin.exec();
    return app.exec();
}
