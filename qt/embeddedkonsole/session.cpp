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
/* -------------------------------------------------------------------------- */
/*									      */
/* Konsole ported to Qt/Embedded by Trolltech                                 */
/*									      */
/* -------------------------------------------------------------------------- */

#include "session.h"
//#include <qpushbutton.h>
#include <QPushButton>
// #include <kdebug.h>

#include <cstdlib>


#define HERE fprintf(stderr,"%s(%d): here\n",__FILE__,__LINE__) // No tr

/* \class TESession
    \brief The TESession class is a combination of TEPTy and Emulations.

    Sessions are combinations of TEPTy and Emulations.

    The stuff in here does not belong to the terminal emulation framework,
    but to main.C. It serves it's duty by providing a single reference
    to TEPTy/Emulation pairs. In fact, it is only there to demonstrate one
    of the abilities of the framework - multible sessions.
*/

TESession::TESession(QMainWindow* main, TEWidget* te, const char* _pgm, StrList & _args, const char *_term) : schema_no(0), font_no(3), pgm(_pgm), args(_args)
{
  // sh = new TEPty();
  sh = new MyPty();
  em = new TEmuVt102(te);
  term = _term;

  sh->setSize(te->Lines(),te->Columns()); // not absolutely nessesary
  QObject::connect( sh,SIGNAL(block_in(const char*,int)),
                    em,SLOT(onRcvBlock(const char*,int)) );
  QObject::connect( em,SIGNAL(ImageSizeChanged(int,int)),
                    sh,SLOT(setSize(int,int)));

  // 'main' should preferably do those connects itself.
  // These aren't KTMW's slots, but konsole's.(David)

/*
  QObject::connect( em,SIGNAL(ImageSizeChanged(int,int)),
                    main,SLOT(notifySize(int,int)));
*/
  QObject::connect( em,SIGNAL(sndBlock(const char*,int)),
                    sh,SLOT(send_bytes(const char*,int)) );
  QObject::connect( em,SIGNAL(changeColumns(int)),
                    main,SLOT(changeColumns(int)) );
/*
  QObject::connect( em,SIGNAL(changeTitle(int,const QString&)),
                    main,SLOT(changeTitle(int,const QString&)) );
*/
  QObject::connect( sh,SIGNAL(done(int)), this,SLOT(done(int)) );
}



void TESession::run()
{
  //kdDebug() << "Running the session!" << pgm << "\n";
  sh->run(pgm,args,term.toStdString().c_str(),FALSE);
}

void TESession::kill(int ) // signal)
{
//  sh->kill(signal);
}

TESession::~TESession()
{
 QObject::disconnect( sh, SIGNAL( done(int) ),
		      this, SLOT( done(int) ) );
  delete em;
  delete sh;
}

void TESession::setConnect(bool c)
{
  em->setConnect(c);
}

void TESession::done(int status)
{
  emit done(this,status);
}

void TESession::terminate()
{
  delete this;
}

TEmulation* TESession::getEmulation()
{
  return em;
}

// the following interfaces might be misplaced ///

int TESession::schemaNo()
{
  return schema_no;
}

int TESession::keymap()
{
  return keymap_no;
}

int TESession::fontNo()
{
  return font_no;
}

const char* TESession::emuName()
{
  return term.toStdString().c_str();
}

void TESession::setSchemaNo(int sn)
{
  schema_no = sn;
}

void TESession::setKeymapNo(int kn)
{
  keymap_no = kn;
  em->setKeytrans(kn);
}

void TESession::setFontNo(int fn)
{
  font_no = fn;
}

void TESession::setTitle(const QString& title)
{
  this->title = title;
}

const QString& TESession::Title()
{
  return title;
}

void TESession::setHistory(bool on)
{
  em->setHistory( on );
}

bool TESession::history()
{
  return em->history();
}

// #include "session.moc"
