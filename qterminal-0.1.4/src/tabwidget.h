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

#ifndef TAB_WIDGET
#define TAB_WIDGET

#include <QTabWidget>

class QTermWidget;
class QAction;
class QActionGroup;

class TabWidget : public QTabWidget
{
Q_OBJECT
public:
    TabWidget(QWidget* parent = 0);

public slots:
    int addTerminal();
    void removeTerminal(int);
    void removeCurrentTerminal();
    int traverseRight();
    int traverseLeft();
    void removeFinished();
    void moveLeft();
    void moveRight();
    void renameSession();
    void refreshWindow();
    void changeFont();
    void changeColorSchema();
    void changeStyle(QAction*);
    void loadSettings();
    void saveSettings();
signals:
    void quit_notification();

public:
    void removeTab(int);
protected:
    enum Direction{Left = 1, Right};
    bool eventFilter(QObject* watched, QEvent* event);
    void mouseDoubleClickEvent ( QMouseEvent * event );
    void contextMenuEvent ( QContextMenuEvent * event );
    QTermWidget* createNewTerminal();
    void recountIndexes();
    void move(Direction);
private:
    void createActions(QWidget*);
    int currentColorScheme;
    QFont currentFont;
    QActionGroup* styleAct;
};

#endif
