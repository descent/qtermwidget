/*  Copyright (C) 2008 e_k (e_k@users.sourceforge.net)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
		
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
				
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
						

#include <QtCore>
#include <QtGui>
#include <QApplication>

#include "qtermwidget.h"
#include "main_window.h"

#include <QLabel>


int main(int argc, char *argv[])
{
#if 1
    //Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
#if 1
    MainWindow *mainWindow = new MainWindow();
#else
    QMainWindow *mainWindow = new QMainWindow();
#endif
    QLabel *l = new QLabel();

    QTermWidget *console = new QTermWidget();
    
    QFont font = QApplication::font();
    font.setFamily("Terminus");
    font.setPointSize(12);
    
    console->setTerminalFont(font);
    
    //console->setColorScheme(COLOR_SCHEME_BLACK_ON_LIGHT_YELLOW);
    console->setScrollBarPosition(QTermWidget::ScrollBarRight);
    
#if 1
    mainWindow->setCentralWidget(console);
#else
    mainWindow->setCentralWidget(l);
#endif
    mainWindow->resize(802, 610);
    
    QObject::connect(console, SIGNAL(finished()), mainWindow, SLOT(close()));

    mainWindow->show();    
    return app.exec();
#else
    QApplication app(argc, argv);
    MainWindow mainWin;
    QLabel *l = new QLabel();
    QTermWidget *console = new QTermWidget(1, &mainWin);
    mainWin.setCentralWidget(console);
    //mainWin.setCentralWidget(l);
    mainWin.show();
    return app.exec();
#endif
} 

  
