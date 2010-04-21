/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS and its licensors.
** All rights reserved.
**
** This file is part of the Qtopia Environment.
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
** See below for additional copyright and license information
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* [konsole.h]                      Konsole                                   */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole, an X terminal.                               */
/*                                                                            */
/* The material contained in here more or less directly orginates from        */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org>     */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                        */
/* Konsole ported to Qt/Embedded by Trolltech                                 */
/*                        */
/* -------------------------------------------------------------------------- */

#ifndef KONSOLE_H
#define KONSOLE_H


#include <my_type.h>
#include <QMainWindow>
#include <QActionGroup>

#include <qaction.h>

//#include <qpopupmenu.h>
#include <QMenu>

//#include <qstrlist.h>
//#include <qintdict.h>
//#include <qptrdict.h>
#include <QTableWidget>
//#include <qtopia/qpetoolbar.h>
#include <qcombobox.h>

#include "MyPty.h"
#include "TEWidget.h"
#include "TEmuVt102.h"
#include "session.h"

#include <string>
#include <vector>
#include <map>

using namespace Qt;
using namespace std;


typedef vector<string> StrList;

class EKNumTabWidget;


class Konsole : public QMainWindow
{
Q_OBJECT

public:

  Konsole(QWidget* parent = 0, WFlags fl = 0);
//  Konsole(const char * name, const char* pgm, QStrList & _args, int histon);
  ~Konsole();
  void setColLin(int columns, int lines);
  //QPEToolBar *secondToolBar; 
  void show();
  void setColor();
  int lastSelectedMenu;
  
signals:
  //void signal_encoding(int encoding);

private slots:
  void input_big5_enc()
  {
    //emit signal_encoding(0);
    qDebug("input big5 enc");
    TEWidget* te=getTe();
    te->set_input_encoding(DS::BIG5);
    input_big5_enc_->setCheckable(true);
  }
  void input_utf8_enc()
  {
    qDebug("input utf8 enc");
    TEWidget* te=getTe();
    te->set_input_encoding(DS::UTF8);
    input_utf8_enc_->setCheckable(true);
  }

  void big5_enc()
  {
    //emit signal_encoding(0);
    qDebug("big5_enc");
    TEWidget* te=getTe();
    te->set_encoding(DS::BIG5);
    //cur_enc_=utf8_enc_;
    big5_enc_->setCheckable(true);
  }
  void utf8_enc()
  {
    //emit signal_encoding(1);
    qDebug("utf8_enc");
    TEWidget* te=getTe();
    te->set_encoding(DS::UTF8);
    utf8_enc_->setCheckable(true);
  }
  void select_font();
  void select_encoding();
  void doneSession(TESession*,int);
  void changeColumns(int);
  void fontChanged(int);
  void configMenuSelected(int );
  void colorMenuSelected(int);
  void enterCommand(int);
  void hitEnter();
  void hitSpace();
  void hitTab();
  void hitPaste();
  void hitUp();
  void hitDown();
  void switchSession(QWidget *);
  void newSession();
  void changeCommand(const QString &, int);

private:
  void init(const char* _pgm, StrList & _args);
  void initSession(const char* _pgm, StrList & _args);
  void runSession(TESession* s);
  void setColorPixmaps();
  void setHistory(bool);
  QSize calcSize(int columns, int lines);
  TEWidget* getTe();

private:
  class VTFont 
  {
  public:
    VTFont(QString name, QFont& font) 
    {
      this->name = name;
      this->font = font;
    }

    QFont& getFont() 
    {
      return font;
    }

    QString getName() 
    {
      return name;
    }

  private:
    QString name;
    QFont font;
  };

  EKNumTabWidget* tab;
  int nsessions;
  vector<VTFont *> fonts;
  int cfont;
  char *se_pgm;
  StrList se_args;

  QMenu* fontList,*configMenu,*colorMenu, *setting_menu_, *encoding_menu_;
  QComboBox *commonCombo;
    // history scrolling I think
  bool        b_scroll;

  int         n_keytab;
  int         n_scroll;
  int         n_render;
  QString     pmPath; // pixmap path
  QString     dropText;
  QFont       defaultFont;
  QSize       defaultSize;
  QAction *new_terminal_;
  QAction *select_font_, *select_encoding_;
  QAction *big5_enc_, *utf8_enc_;
  QAction *input_big5_enc_, *input_utf8_enc_;
  //QAction *cur_enc_, *cur_input_enc_;
  QActionGroup *input_enc_g_, *enc_g_;
};

#endif

