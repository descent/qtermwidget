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
/* ---------------------------------------------------------------------- */
/*                                                                        */
/* [main.C]                        Konsole                                */
/*                                                                        */
/* ---------------------------------------------------------------------- */
/*                                                                        */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>        */
/*                                                                        */
/* This file is part of Konsole, an X terminal.                           */
/*                                                                        */
/* The material contained in here more or less directly orginates from    */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org> */
/*                                                                        */
/* ---------------------------------------------------------------------- */
//
// Konsole ported to Qt/Embedded by Trolltech
//  some enhancements added by L.J. Potter <ljp@llornkcor.com>
//


//#include <qtopia/resource.h>
//#include <qtopia/qpemenubar.h>
//#include <qtopia/qpetoolbar.h>
//#include <qtopia/config.h>

#include <QMenuBar>
#include <QToolBar>
#include <qdir.h>
#include <qevent.h>
//#include <qdragobject.h>
//#include <qobjectlist.h>
#include <QFontDatabase>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qfontdialog.h>
#include <qglobal.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include <qcombobox.h>
#include <qevent.h>
//#include <qtabwidget.h>
//#include <QTableWidget>
//#include <qtabbar.h>
#include <qstringlist.h>
#include <qpalette.h>
#include <qlayout.h>

#ifndef Q_OS_WIN32 
#include <sys/wait.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "konsole.h"
#include "keytrans.h"


class EKNumTabBar : public QTabBar {
public:
    void numberTabs()
    {
  // Yes, it really is this messy. QTabWidget needs functions
  // that provide acces to tabs in a sequential way.
  int m=INT_MIN;
#if 0
  for (int i=0; i<count(); i++) {
      QTab* left=0;
      QListIterator<QTab> it(*tabList());
      int x=INT_MAX;
      for( QTab* t; (t=it.current()); ++it ) {
    int tx = t->rect().x();
    if ( tx<x && tx>m ) {
        x = tx;
        left = t;
    }
      }
      if ( left ) {
    left->setText(QString::number(i+1));
    m = left->rect().x();
      }
  }
#endif
    } // end void numberTabs()
};


class EKNumTabWidget : public QTabWidget {
public:
    EKNumTabWidget(QWidget* parent) : QTabWidget(parent)
    {
    }

    void addTab(QWidget* w)
    {
      qDebug("tabBar()->count()+1 : %d",tabBar()->count()+1);
  //QTab* t = new QTab(QString::number(tabBar()->count()+1));
      QTabWidget::addTab(w,QString::number(tabBar()->count()+1));
    }

    void removeTab(QWidget* w)
    {
      //removePage(indexOf(w));
      //removeTab(w);
      ((EKNumTabBar*)tabBar())->numberTabs();
    }
};


// This could be configurable or dynamically generated from the bash history
// file of the user.
static const char *commonCmds[] =
{
    "ls ", // I left this here, cause it looks better than the first alpha
    "cardctl eject", // No tr
    "cat ", // No tr
    "cd ",
    "chmod ",
    "cp ",
    "dc ",
    "df ",
    "dmesg",
    "echo ", // No tr
    "find ", // No tr
    "free", // No tr
    "grep ",
    "ifconfig ",
    "ipkg ",
    "mkdir ",
    "mv ",
    "nc localhost 7776",
    "nc localhost 7777",
    "nslookup ",
    "ping ", // No tr
    "ps aux",
    "pwd ",
    "rm ",
    "rmdir ",
    "route ", // No tr
    "set ", // No tr
    "traceroute",

/*
    "gzip",
    "gunzip",
    "chgrp",
    "chown",
    "date",
    "dd",
    "df",
    "dmesg",
    "fuser",
    "hostname",
    "kill",
    "killall",
    "ln",
    "ping",
    "mount",
    "more",
    "sort",
    "touch",
    "umount",
    "mknod",
    "netstat",
*/

    "exit", // No tr
    NULL
};

/*!
  \class Konsole
  \brief The Konsole class is the top-level widget for the
  Terminal application.

  \legalese
  The Terminal allows command-line access to the system and
  is used for development and debugging activities.

  The Terminal (embeddedkonsole) is based on the KDE Konsole
  application and is distributed under the terms of the GNU
  General Public License. A primary copyright holder of the
  code is <a href="mailto:lars.doelle@on-line.de">Lars Doelle</a>.
*/

// QDOC_SKIP_BEGIN

#include <qfile.h>

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>


Konsole::Konsole(QWidget* parent, WFlags fl) :
    QMainWindow(parent, fl)
{
    const char* shell = getenv("SHELL");
    if (shell == NULL || *shell == '\0'){
#if !defined (_OS_WIN32_)
	shell = "/bin/sh"; // No tr

	// sh is completely broken on familiar. Let's try to get something better
	if ( strcmp( shell, "/bin/shell" ) == 0 && QFile::exists( "/bin/bash" ) )
	    shell = "/bin/bash";
#else
	shell ="command"; // No tr
#endif
    }

    StrList args;
    qDebug("init(shell,args);");
    init(shell,args);
}

/* not used
Konsole::Konsole(const char* name, const char* _pgm, QStrList & _args, int)
 : QMainWindow(0, name)
{
    init(_pgm,_args);
}
*/

void Konsole::select_encoding()
{
  qDebug("in Konsole::select_encoding()");
}

void Konsole::select_font()
{
  qDebug("Konsole::select_font");

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
      TEWidget *te=getTe();
      te->setFont(font);
        // font is set to the font the user selected
    } else {
        // the user canceled the dialog; font is set to the default
        // application font, QApplication::font()
    }
}

void Konsole::init(const char* _pgm, StrList & _args)
{
    qDebug("in init(shell,args);");
#if !defined (_OS_WIN32_)
  setuid(getuid()); setgid(getgid()); // drop privileges
#endif
#ifdef FAKE_CTRL_AND_ALT
  QPEApplication::grabKeyboard(); // for CTRL and ALT
#endif

    putenv((char*)"COLORTERM="); // to trigger mc's color detection
  //setCaption( Konsole::tr("Terminal") );
  setWindowTitle( Konsole::tr("Terminal") );

  //setMinimumSize(200, 200);

  b_scroll = true; // histon;
  n_keytab = 0;
  n_render = 0;

  //setCaption( tr("Terminal") );

  //setBackgroundMode( PaletteButton );

  //Config cfg("Konsole");
  //cfg.setGroup("Konsole");
  QString tmp;
  // initialize the list of allowed fonts ///////////////////////////////////
  //cfont = cfg.readNumEntry("FontID", 1);
  cfont = 0;
 
  // get system font list
  QFontDatabase fd;
  QStringList fonts_list;
  fonts_list=fd.families();
  for (int i = 0; i < fonts_list.size(); ++i)
  {
    //qDebug("font ascii : %s",fonts_list.at(i).toStdString().c_str());
    QFont f = QFont(fonts_list.at(i), 14, QFont::Normal);
    f.setFixedPitch(TRUE);
    fonts.push_back(new VTFont(fonts_list.at(i), f));
    if (fd.isFixedPitch(fonts_list.at(i)) )
    {
      qDebug("font ascii : %s",fonts_list.at(i).toStdString().c_str());
    }

  }

#if 0
  f = QFont("Fixed", 7, QFont::Normal);
  f.setFixedPitch(TRUE);
  fonts.push_back(new VTFont(tr("Small Fixed"), f));

  f = QFont("Fixed", 12, QFont::Normal);
  f.setFixedPitch(TRUE);
  fonts.push_back(new VTFont(tr("Medium Fixed"), f));
#endif

  // create terminal emulation framework ////////////////////////////////////
  nsessions = 0;

  tab = new EKNumTabWidget(this);
  setCentralWidget(tab);

  connect(tab, SIGNAL(currentChanged(QWidget*)), this, SLOT(switchSession(QWidget*)));

     // create terminal toolbar ////////////////////////////////////////////////
  //setToolBarsMovable( FALSE );
  //QPEToolBar *menuToolBar = new QPEToolBar( this );
  //menuToolBar->setHorizontalStretchable( TRUE );

  //QPEMenuBar *menuBar = new QPEMenuBar( menuToolBar );
  QMenu *menu = new QMenu("&File", this); 
  menuBar()->addMenu(menu);
  uint i;
  fontList = new QMenu("&Font", this );
  //fontList->addAction("show font dialog");
  select_font_ = new QAction(tr("select font"), this);
  select_encoding_ = new QAction(tr("select encoding"), this);

  connect(select_font_, SIGNAL(triggered()), this, SLOT(select_font() ));
  connect(select_encoding_, SIGNAL(triggered()), this, SLOT(select_encoding() ));

  big5_enc_ = new QAction(tr("BIG5"), this);
  utf8_enc_ = new QAction(tr("UTF-8"), this);
  input_big5_enc_ = new QAction(tr("input BIG5"), this);
  input_utf8_enc_ = new QAction(tr("input UTF-8"), this);

  connect(big5_enc_, SIGNAL(triggered()), this, SLOT(big5_enc() ));
  connect(utf8_enc_, SIGNAL(triggered()), this, SLOT(utf8_enc() ));
  connect(input_big5_enc_, SIGNAL(triggered()), this, SLOT(input_big5_enc() ));
  connect(input_utf8_enc_, SIGNAL(triggered()), this, SLOT(input_utf8_enc() ));

  fontList->addAction(select_font_);

#if 0
  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
#endif

  //menuBar()->addMenu(fontList);

#if 0
  for(i = 0; i < fonts.size(); i++) {
    VTFont *fnt = fonts[i];
    fontList->addAction(fnt->getName());
  }
#endif

  fontChanged(cfont);

  encoding_menu_ = new QMenu("&Encoding", this);

  input_enc_g_ = new QActionGroup(this);
  enc_g_ = new QActionGroup(this);;
  encoding_menu_->addAction(select_encoding_);
  encoding_menu_->addAction(big5_enc_);
  encoding_menu_->addAction(utf8_enc_);
  encoding_menu_->addSeparator();
  encoding_menu_->addAction(input_big5_enc_);
  encoding_menu_->addAction(input_utf8_enc_);
  encoding_menu_->addAction("utf16");
  encoding_menu_->addAction("unicode");
  
  input_enc_g_->addAction(input_big5_enc_);
  input_enc_g_->addAction(input_utf8_enc_);
  enc_g_->addAction(big5_enc_);
  enc_g_->addAction(utf8_enc_);


  //utf8_enc_->setCheckable(true);
  //input_utf8_enc_->setCheckable(true);
  //cur_enc_=cur_input_enc_=utf8_enc_;
  //cur_input_enc_->setCheckable(true);
  //cur_enc_->setCheckable(true);

  setting_menu_ = new QMenu("&Settings", this);
  menuBar()->addMenu(setting_menu_);

  setting_menu_ ->addMenu(fontList);
  setting_menu_ ->addMenu(encoding_menu_);

  colorMenu = new QMenu("&Color", this);
  setting_menu_->addMenu(colorMenu);

  QToolBar * fileToolBar = addToolBar(tr("File"));
  //QToolBar *tb = new QToolBar( this );
  //addToolBar(tb);
  new_terminal_= new QAction( QPixmap("konsole.png"),tr("New"), this);
  fileToolBar->addAction(new_terminal_);
  connect( new_terminal_, SIGNAL( triggered() ), this, SLOT( newSession() ) ); 

#if 0

  bool listHidden;
  cfg.setGroup("Menubar");
  if( cfg.readEntry("Hidden","FALSE") == "TRUE")  {
      configMenu->insertItem(tr("Show Command List"));
      listHidden=TRUE;
  } else {
      configMenu->insertItem(tr("Hide Command List"));
      listHidden=FALSE;
  }

  cfg.setGroup("Tabs");
  tmp=cfg.readEntry("Position","Bottom");
  if(tmp=="Top") { // No tr
      tab->setTabPosition(QTabWidget::Top);
      configMenu->insertItem(tr("Tabs on Bottom"));
  } else {
      tab->setTabPosition(QTabWidget::Bottom);
      configMenu->insertItem(tr("Tabs on Top"));
  }
  configMenu->insertSeparator(2);

  colorMenu->insertItem(tr("Green on Black"));
  colorMenu->insertItem(tr("Black on White"));
  colorMenu->insertItem(tr("White on Black"));
  colorMenu->insertItem(tr("Default"));
  colorMenu->insertItem(tr("Black on Red"));
  colorMenu->insertItem(tr("Red on Black"));
  colorMenu->insertItem(tr("Green on Yellow"));
  colorMenu->insertItem(tr("Blue on Magenta"));
  colorMenu->insertItem(tr("Magenta on Blue"));
  colorMenu->insertItem(tr("Cyan on White"));
  colorMenu->insertItem(tr("White on Cyan"));
  colorMenu->insertItem(tr("Blue on Black"));
  configMenu->insertItem(tr("Colors"),colorMenu);

  connect( fontList, SIGNAL( activated(int) ), this, SLOT( fontChanged(int) ));
  connect( configMenu, SIGNAL( activated(int) ), this, SLOT( configMenuSelected(int) ));
  connect( colorMenu, SIGNAL( activated(int) ), this, SLOT( colorMenuSelected(int) ));

  menuBar->insertItem( tr("Font"), fontList );
  menuBar->insertItem( tr("Options"), configMenu );

  QPEToolBar *toolbar = new QPEToolBar( this );

  QAction *a;

  // Button Commands
  a = new QAction( tr("Enter"), Resource::loadIconSet( "konsole/enter" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitEnter() ) ); a->addTo( toolbar );
  a = new QAction( tr("Space"), Resource::loadIconSet( "space" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitSpace() ) ); a->addTo( toolbar );
  a = new QAction( tr("Tab"), Resource::loadIconSet( "tab" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitTab() ) ); a->addTo( toolbar );
  a = new QAction( tr("Up"), Resource::loadIconSet( "up" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitUp() ) ); a->addTo( toolbar );
  a = new QAction( tr("Down"), Resource::loadIconSet( "down" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitDown() ) ); a->addTo( toolbar );
  a = new QAction( tr("Paste"), Resource::loadIconSet( "paste" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitPaste() ) ); a->addTo( toolbar );
/*
  a = new QAction( tr("Up"), Resource::loadIconSet( "up" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitUp() ) ); a->addTo( toolbar );
  a = new QAction( tr("Down"), Resource::loadIconSet( "down" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( hitDown() ) ); a->addTo( toolbar );
*/

  secondToolBar = new QPEToolBar( this );

  commonCombo = new QComboBox( secondToolBar );

  if( listHidden) 
      secondToolBar->hide();
  configMenu->insertItem(tr("Edit Command List"));

  cfg.setGroup("Commands");
  commonCombo->setInsertionPolicy(QComboBox::AtCurrent);

  for (i = 0; commonCmds[i] != NULL; i++) {
      commonCombo->insertItem( commonCmds[i], i );
      tmp = cfg.readEntry( QString::number(i),"");
      if(tmp != "")
          commonCombo->changeItem( tmp,i );
  }

  connect( commonCombo, SIGNAL( activated(int) ), this, SLOT( enterCommand(int) ));

      // create applications /////////////////////////////////////////////////////
  setCentralWidget(tab);
#endif

  // load keymaps ////////////////////////////////////////////////////////////
  KeyTrans::loadAll();
  for (int i = 0; i < KeyTrans::count(); i++)
  { KeyTrans* s = KeyTrans::find(i);
    assert( s );
  }

  se_pgm = strdup(_pgm);
  se_args = _args;

  // read and apply default values ///////////////////////////////////////////
  resize(640, 480); // Dummy.
#if 0
  QSize currentSize = size();
  if (currentSize != size())
     defaultSize = size();

  if(cfg.readEntry("EditEnabled","FALSE")=="TRUE") {
      configMenu->setItemChecked(-20,TRUE);
      commonCombo->setEditable( TRUE );
  } else {
      configMenu->setItemChecked(-20,FALSE);
      commonCombo->setEditable( FALSE );
  }
  configMenu->setItemEnabled(-20,!secondToolBar->isHidden());

  layout()->setResizeMode(QLayout::FreeResize);
#endif
}

/*! */
void Konsole::show()
{
	  qDebug("in Konsole::show() nsessions : %d",nsessions);
  if ( !nsessions ) {
    newSession();
  }
  QMainWindow::show();
}

void Konsole::initSession(const char*, StrList &)
{
  QMainWindow::show();
}

/*! */
Konsole::~Konsole()
{
    while (nsessions > 0) {
      doneSession(getTe()->currentSession, 0);
    }

  //Config cfg("Konsole");
  //cfg.setGroup("Konsole");
  //cfg.writeEntry("FontID", cfont);
  free(se_pgm);
}

void Konsole::fontChanged(int f)
{
#if 0
    VTFont* font = fonts.at(f);
    if (font != 0) {
        for(uint i = 0; i < fonts.count(); i++) {
            fontList->setItemChecked(i, (i == (uint) f) ? TRUE : FALSE);
        }

        cfont = f;

        TEWidget* te = getTe();
        if (te != 0) {
            te->setVTFont(font->getFont());
        }
    }
#endif
}


void Konsole::enterCommand(int c)
{
    TEWidget* te = getTe();
    if (te != 0) {
        if(!commonCombo->isEditable()) {
            QString text = commonCombo->itemText(c); //commonCmds[c];
            te->emitText(text);
        } else {
            changeCommand( commonCombo->itemText(c), c);            
        }
    }
}

void Konsole::hitEnter()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->emitText(QString("\r"));
    }
}

void Konsole::hitSpace()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->emitText(QString(" "));
    }
}

void Konsole::hitTab()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->emitText(QString("\t"));
    }
}

void Konsole::hitPaste()
{
    TEWidget* te = getTe();
    if (te != 0) {
    te->pasteClipboard();
    }
}

void Konsole::hitUp()
{
    TEWidget* te = getTe();
    if (te != 0) {
    QKeyEvent ke( QKeyEvent::KeyPress, Qt::Key_Up, 0, 0);
    QApplication::sendEvent( te, &ke );
    }
}

void Konsole::hitDown()
{
    TEWidget* te = getTe();
    if (te != 0) {
    QKeyEvent ke( QKeyEvent::KeyPress, Qt::Key_Down, 0, 0);
    QApplication::sendEvent( te, &ke );
    }
}


/**
   This function calculates the size of the external widget
   needed for the internal widget to be.............
 */
QSize Konsole::calcSize(int columns, int lines) {
    TEWidget* te = getTe();
    if (te != 0) {
    QSize size = te->calcSize(columns, lines);
    return size;
    } else {
  QSize size;
  return size;
    }
}

/*!
    sets application window to a size based on \a columns X \a lines of the 
    guest widget. Call with (0,0) for setting default size.
*/

void Konsole::setColLin(int columns, int lines)
{
  if ((columns==0) || (lines==0))
  {
    if (defaultSize.isEmpty()) // not in config file : set default value
    {
      defaultSize = calcSize(80,24);
      // notifySize(24,80); // set menu items (strange arg order !)
    }
    resize(defaultSize);
  } else {
    resize(calcSize(columns, lines));
    // notifySize(lines,columns); // set menu items (strange arg order !)
  }
}

/*
void Konsole::setFont(int fontno)
{
  QFont f;
  if (fontno == 0)
    f = defaultFont = QFont( "Helvetica", 12  );
  else
  if (fonts[fontno][0] == '-')
    f.setRawName( fonts[fontno] );
  else
  {
    f.setFamily(fonts[fontno]);
    f.setRawMode( TRUE );
  }
  if ( !f.exactMatch() && fontno != 0)
  {
    QString msg = i18n("Font `%1' not found.\nCheck README.linux.console for help.").arg(fonts[fontno]);
    QMessageBox(this,  msg);
    return;
  }
  if (se) se->setFontNo(fontno);
  te->setVTFont(f);
  n_font = fontno;
}
*/

// --| color selection |-------------------------------------------------------

void Konsole::changeColumns(int columns)
{
  qDebug("in Konsole::changeColumns");
  TEWidget* te = getTe();
  if (te != 0) {
  setColLin(columns,te->Lines());
  te->update();
  }
}

//FIXME: If a child dies during session swap,
//       this routine might be called before
//       session swap is completed.

void Konsole::doneSession(TESession*, int )
{
  qDebug("in Konsole::doneSession(TESession*, int )");
  TEWidget *te = getTe();
  if (te != 0) {
    te->currentSession->setConnect(false);
    tab->removeTab(te);
    delete te->currentSession;
    delete te;
    nsessions--;
#if 0
    if (!newAct->isEnabled())
        newAct->setEnabled(true);
#endif
  }

  if (nsessions == 0) {
  close();
  }
}

void Konsole::newSession() {
	qDebug("nsessions : %d", nsessions);
    if(nsessions < 15) {    // seems to be something weird about 16 tabs on the Zaurus.... memory?
        TEWidget* te = new TEWidget(tab);
//  te->setBackgroundMode(PaletteBase); //we want transparent!!
    qDebug("cfont : %d", cfont);
    te->setVTFont(fonts.at(cfont)->getFont());
    tab->addTab(te);
    //qDebug("se_pgm.toStdString().c_str() : %s", se_pgm.toStdString().c_str());
    TESession* se = new TESession(this, te, se_pgm, se_args, "xterm");
    te->currentSession = se;
    te->set_encoding(1);
    connect( se, SIGNAL(done(TESession*,int)), this, SLOT(doneSession(TESession*,int)) );
    se->run();
    se->setConnect(TRUE);
    se->setHistory(b_scroll);
    //tab->setCurrentPage(nsessions);
    tab->setCurrentIndex(nsessions);
    nsessions++;
    if (nsessions == 15) 
        new_terminal_->setEnabled(FALSE);
    setColor();
    input_utf8_enc();
    utf8_enc();

    //te->set_input_encoding(DS::UTF8);
    //te->set_encoding(DS::UTF8);
    //utf8_enc_->setCheckable(true);
    //input_utf8_enc_->setCheckable(true);
    }
}

TEWidget* Konsole::getTe() {
  if (nsessions) {
    //return (TEWidget *) tab->currentPage();
    return (TEWidget *) tab->currentWidget();
  } else {
    return 0;
  }
}

void Konsole::switchSession(QWidget* w) {
  TEWidget* te = (TEWidget *) w;

  QFont teFnt = te->getVTFont();
  for(uint i = 0; i < fonts.size(); i++) {
    VTFont *fnt = fonts[i];
    bool cf = fnt->getFont() == teFnt;
    //fontList->setItemChecked(i, cf);
    if (cf) {
      cfont = i;
    }
  }
}

void Konsole::colorMenuSelected(int iD)
{ // this is NOT pretty, elegant or anything else besides functional
//        QString temp;
//        temp.sprintf("%d", iD);
//      qDebug(temp);
#if 0
    TEWidget* te = getTe();
    Config cfg("Konsole");
    cfg.setGroup("Colors");
    QColor foreground;
    QColor background;
    colorMenu->setItemChecked(lastSelectedMenu,FALSE);
    ColorEntry m_table[TABLE_COLORS];
    const ColorEntry * defaultCt=te->getdefaultColorTable();
      /////////// fore back
    int i;
    if(iD==-8) { // default default
        for (i = 0; i < TABLE_COLORS; i++)  {
            m_table[i].color = defaultCt[i].color;
            if(i==1 || i == 11)
                m_table[i].transparent=1;
            cfg.writeEntry("Schema","8");
            colorMenu->setItemChecked(-8,TRUE);
        }
    } else {
        if(iD==-5) { // green black
            foreground.setRgb(0x18,255,0x18);
            background.setRgb(0x00,0x00,0x00);
            cfg.writeEntry("Schema","5");
            colorMenu->setItemChecked(-5,TRUE);
        }
        if(iD==-6) { // black white
            foreground.setRgb(0x00,0x00,0x00);
            background.setRgb(0xFF,0xFF,0xFF);
            cfg.writeEntry("Schema","6");
            colorMenu->setItemChecked(-6,TRUE);
        }
        if(iD==-7) { // white black
            foreground.setRgb(0xFF,0xFF,0xFF);
            background.setRgb(0x00,0x00,0x00);
            cfg.writeEntry("Schema","7");
            colorMenu->setItemChecked(-7,TRUE);
        }
        if(iD==-9) {// Black, Red
            foreground.setRgb(0x00,0x00,0x00);
            background.setRgb(0xB2,0x18,0x18);
            cfg.writeEntry("Schema","9");
            colorMenu->setItemChecked(-9,TRUE);
        }
        if(iD==-10) {// Red, Black
            foreground.setRgb(230,31,31); //0xB2,0x18,0x18
            background.setRgb(0x00,0x00,0x00);
            cfg.writeEntry("Schema","10");
            colorMenu->setItemChecked(-10,TRUE);
        }
        if(iD==-11) {// Green, Yellow - is ugly
//            foreground.setRgb(0x18,0xB2,0x18);
            foreground.setRgb(36,139,10);
//            background.setRgb(0xB2,0x68,0x18);
            background.setRgb(255,255,0);
            cfg.writeEntry("Schema","11");
            colorMenu->setItemChecked(-11,TRUE);
        }
        if(iD==-12) {// Blue,  Magenta
//            foreground.setRgb(0x18,0xB2,0xB2);
//            background.setRgb(0x18,0x18,0xB2);
	    foreground.setRgb(0x18,0x18,0xB2);
            background.setRgb(0xB2,0x00,0xB2);
            cfg.writeEntry("Schema","12");
            colorMenu->setItemChecked(-12,TRUE);
        }
        if(iD==-13) {// Magenta, Blue
//            foreground.setRgb(0x18,0x18,0xB2);
//            background.setRgb(0x18,0xB2,0xB2);
            foreground.setRgb(0xB2,0x00,0xB2);
            background.setRgb(0x18,0x18,0xB2);
            cfg.writeEntry("Schema","13");
            colorMenu->setItemChecked(-13,TRUE);
        }
        if(iD==-14) {// Cyan,  White
            foreground.setRgb(0x18,0xB2,0xB2);
            background.setRgb(0xFF,0xFF,0xFF);
            cfg.writeEntry("Schema","14");
            colorMenu->setItemChecked(-14,TRUE);
        }
        if(iD==-15) {// White, Cyan
            background.setRgb(0x18,0xB2,0xB2);
            foreground.setRgb(0xFF,0xFF,0xFF);
            cfg.writeEntry("Schema","15");
            colorMenu->setItemChecked(-15,TRUE);
        }
        if(iD==-16) {// Black, Blue
            background.setRgb(0x00,0x00,0x00);
            foreground.setRgb(0x18,0xB2,0xB2);
            cfg.writeEntry("Schema","16");
            colorMenu->setItemChecked(-16,TRUE);
        }

        for (i = 0; i < TABLE_COLORS; i++)  {
            if(i==0 || i == 10) {
                m_table[i].color = foreground;
            }
            else if(i==1 || i == 11) {
                m_table[i].color = background; m_table[i].transparent=0;
            }
            else
                m_table[i].color = defaultCt[i].color;
        }
    }
    lastSelectedMenu = iD;
    te->setColorTable(m_table);
    qDebug("Konsole::colorMenuSelected");
    update();
#endif
}

void Konsole::configMenuSelected(int iD)
{
#if 0
//      QString temp;
//      temp.sprintf("%d",iD);
//      qDebug(temp);
    TEWidget* te = getTe();
    Config cfg("Konsole");
    cfg.setGroup("Menubar");
    if( iD  == -2) {
        if(!secondToolBar->isHidden()) {
            secondToolBar->hide();
            configMenu->changeItem( iD,tr("Show Command List"));
            cfg.writeEntry("Hidden","TRUE");
            configMenu->setItemEnabled(-20 ,FALSE);
        } else {
            secondToolBar->show();
            configMenu->changeItem( iD,tr("Hide Command List"));
            cfg.writeEntry("Hidden","FALSE");
            configMenu->setItemEnabled(-20 ,TRUE);
        }
    }
    if( iD  == -3) {
        cfg.setGroup("Tabs");
        QString tmp=cfg.readEntry("Position","Bottom");

        if(tmp=="Top") { // No tr
            tab->setTabPosition(QTabWidget::Bottom);
            configMenu->changeItem( iD,tr("Tabs on Top"));
            cfg.writeEntry("Position","Bottom");
        } else {
            tab->setTabPosition(QTabWidget::Top);
            configMenu->changeItem( iD,tr("Tabs on Bottom"));
            cfg.writeEntry("Position","Top");
        }
    }
    if( iD  == -20) {
        cfg.setGroup("Commands");
//        qDebug("enableCommandEdit");
        if( !configMenu->isItemChecked(iD) ) {
            commonCombo->setEditable( TRUE );
            configMenu->setItemChecked(iD,TRUE);
            commonCombo->setCurrentItem(0);
            cfg.writeEntry("EditEnabled","TRUE");
        } else {
            commonCombo->setEditable( FALSE );
            configMenu->setItemChecked(iD,FALSE);
            cfg.writeEntry("EditEnabled","FALSE");
            commonCombo->setFocusPolicy(QWidget::NoFocus);
            te->setFocus();
        }
    }
#endif
}

void Konsole::changeCommand(const QString &text, int c)
{
#if 0
    Config cfg("Konsole");
    cfg.setGroup("Commands");
    if(commonCmds[c] != text) {
        cfg.writeEntry(QString::number(c),text);
        commonCombo->clearEdit();
        commonCombo->setCurrentItem(c);        
    }
#endif
}

/*! */
void Konsole::setColor()
{
#if 0
    Config cfg("Konsole");
    cfg.setGroup("Colors");
    int scheme = cfg.readNumEntry("Schema",8); 
    colorMenuSelected( -scheme); 
#endif // if 0

}

// QDOC_SKIP_END

