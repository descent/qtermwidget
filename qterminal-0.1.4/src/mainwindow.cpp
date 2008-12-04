/***************************************************************************
 *   Copyright (C) 2006 by Vladimir Kuznetsov                              *
 *   vovanec@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui>

#include "mainwindow.h"
#include "tabwidget.h"
#include "config.h"
#include "version.h"

MainWindow::MainWindow( QWidget * parent, Qt::WindowFlags f) : QMainWindow(parent,f)
{
    setupUi(this);
    loadSettings();

    connect(consoleTabulator, SIGNAL(quit_notification()), SLOT(quit()));
    consoleTabulator->addTerminal();
    setWindowTitle(STR_VERSION);
    setWindowIcon(QIcon(":/icons/main.png"));
    this->addActions();
}


void MainWindow::addActions()
{
    QAction* act = new QAction(this);
    act->setShortcut(tr(ADD_TAB_SHORTCUT));
    connect(act, SIGNAL(triggered()), consoleTabulator, SLOT(addTerminal()));
    addAction(act);

    act = new QAction(this);
    act->setShortcut(tr(TAB_RIGHT_SHORTCUT));
    connect(act, SIGNAL(triggered()), consoleTabulator, SLOT(traverseRight()));
    addAction(act);

    act = new QAction(this);
    act->setShortcut(tr(TAB_LEFT_SHORTCUT));
    connect(act, SIGNAL(triggered()), consoleTabulator, SLOT(traverseLeft()));
    addAction(act);

    act = new QAction(this);
    act->setShortcut(tr(MOVE_LEFT_SHORTCUT));
    connect(act, SIGNAL(triggered()), consoleTabulator, SLOT(moveLeft()));
    addAction(act);

    act = new QAction(this);
    act->setShortcut(tr(MOVE_RIGHT_SHORTCUT));
    connect(act, SIGNAL(triggered()), consoleTabulator, SLOT(moveRight()));
    addAction(act);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_consoleTabulator_currentChanged(int)
{
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    if(QMessageBox::question(this,
                                tr("Close qterminal"),
                                tr("Are you sure you want to exit?"),
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        saveSettings();
        ev->accept();
    }
    else
    {
        ev->ignore();
    }
}

void MainWindow::quit()
{
    saveSettings();
    QApplication::exit(0);
}

void MainWindow::loadSettings()
{
    QSettings settings("Vovan Soft", "QTerminal");
    //qDebug("Loading settings...");
    int width = settings.value("width", QVariant(DEFAULT_WIDTH)).toInt();
    int height = settings.value("height", QVariant(DEFAULT_HEIGHT)).toInt();
    int x = settings.value("x", QVariant(0)).toInt();
    int y = settings.value("y", QVariant(0)).toInt();
    this->setGeometry(QRect(x, y, width, height));
}

void MainWindow::saveSettings()
{
    //qDebug("Saving settings...");
    QSettings settings("Vovan Soft", "QTerminal");
    settings.setValue("width", this->width());
    settings.setValue("height", this->height());
    settings.setValue("x", this->pos().x());
    settings.setValue("y", this->pos().y());
    consoleTabulator->saveSettings();
}
