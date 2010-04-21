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
/*                                                                            */
/* [TEmulation.cpp]        Terminal Emulation Decoder                         */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole - an X terminal for KDE                       */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*									      */
/* Konsole ported to Qt/Embedded by Trolltech                                 */
/*									      */
/* -------------------------------------------------------------------------- */

/* \class TEmulation

    \brief The TEmulation class is the mediator between TEWidget and TEScreen.

   This class is responsible for scanning the escape sequences of the terminal
   emulation and for mapping them to their corresponding semantic complements.
   Therefore this module is mainly concerned with decoding escape sequences and
   is a stateless device with regard to the semantics.

   It is also responsible for refreshing the TEWidget using certain rules.

   \sa TEWidget \sa TEScreen

   \par A note on refreshing

   Although the modifications to the current screen image could immediately
   be propagated via `TEWidget' to the graphical surface, we have chosen
   another way.

   The reason for doing so is twofold.

   Firstly, experiments show that directly displaying the operation slows down the overall performance of emulations, and displaying
   individual characters using X11 creates a lot of overhead.

   Secondly, by using the following refresh method, the screen operations
   can be completed separately from the displaying. This greatly simplifies
   the programmer's task of coding and maintaining the screen operations,
   since there is no need to worry about differential modifications on the
   display effecting the operation of concern.

   We use a refresh algorithm here that has been adopted from rxvt/kvt.

   Using this, refreshing is driven by a timer, which is (re)started whenever
   a new bunch of data arrives at `onRcvBlock' and has to be interpreted by the emulation.
   As soon as no more data arrives for `BULK_TIMEOUT' milliseconds, we trigger
   refresh. This rule suits both the bulk display operation as done by curses?? as
   well as individual typed characters.
   (BULK_TIMEOUT < 1000 / max characters received from keyboard per second).

   Additionally, we trigger refresh by newlines coming in to make visual
   snapshots of lists as produced by `cat', `ls' and other likely programs, thereby
   producing the illusion of a permanent and immediate display operation.

   As a sort of catch-all needed for cases where none of the above
   conditions are applicable, the screen refresh is also triggered by a count
   of incoming bulks (`bulk_incnt').
*/

/* FIXME
   - evtl. the bulk operations could be made more transparent.
*/

#include "TEmulation.h"
#include "TEWidget.h"
#include "TEScreen.h"

#include <QKeyEvent>


#include <cstdlib>

//#include <qkeycode.h>

#include <unistd.h>

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                               TEmulation                                  */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#define CNTL(c) ((c)-'@')

/*
*/

TEmulation::TEmulation(TEWidget* gui)
: decoder((QTextDecoder*)NULL)
{
  this->gui = gui;

  select_encoding(gui->get_encoding());

  screen[0] = new TEScreen(gui->Lines(),gui->Columns());
  screen[1] = new TEScreen(gui->Lines(),gui->Columns());
  scr = screen[0];

  bulk_nlcnt = 0; // reset bulk newline counter
  bulk_incnt = 0; // reset bulk counter
  connected  = FALSE;

  QObject::connect(&bulk_timer, SIGNAL(timeout()), this, SLOT(showBulk()) );
  QObject::connect(gui,SIGNAL(changedImageSizeSignal(int,int)), this,SLOT(onImageSizeChange(int,int)));
  QObject::connect(gui,SIGNAL(changedHistoryCursor(int)), this,SLOT(onHistoryCursorChange(int)));
  QObject::connect(gui,SIGNAL(keyPressedSignal(QKeyEvent*)), this,SLOT(onKeyPress(QKeyEvent*)));
  QObject::connect(gui,SIGNAL(beginSelectionSignal(const int,const int)), this,SLOT(onSelectionBegin(const int,const int)) );
  QObject::connect(gui,SIGNAL(extendSelectionSignal(const int,const int)), this,SLOT(onSelectionExtend(const int,const int)) );
  QObject::connect(gui,SIGNAL(endSelectionSignal(const BOOL)), this,SLOT(setSelection(const BOOL)) );
  QObject::connect(gui,SIGNAL(clearSelectionSignal()), this,SLOT(clearSelection()) );
}

/*
*/

TEmulation::~TEmulation()
{
  delete screen[0];
  delete screen[1];
  bulk_timer.stop();
}

/* change between primary and alternate screen
*/

void TEmulation::setScreen(int n)
{
  scr = screen[n&1];
}

void TEmulation::setHistory(bool on)
{
  screen[0]->setScroll(on);
  if (!connected) return;
  showBulk();
}

bool TEmulation::history()
{
  return screen[0]->hasScroll();
}

void TEmulation::setCodec(int c)
{
  //FIXME: check whether we have to free codec
  codec = c ? QTextCodec::codecForName("utf8")
            : QTextCodec::codecForLocale();
  if (decoder) delete decoder;
  decoder = codec->makeDecoder();
}

void TEmulation::setKeytrans(int no)
{
  keytrans = KeyTrans::find(no);
}

void TEmulation::setKeytrans(const char * no)
{
  keytrans = KeyTrans::find(no);
}

// Interpreting Codes ---------------------------------------------------------

/*
   This section deals with decoding the incoming character stream.
   In this context decoding means that the stream is first seperated into `tokens'
   that are then mapped to a `meaning' provided as operations by the
   `Screen' class.
*/

/*
*/

void TEmulation::onRcvChar(int c)
// process application unicode input to terminal
// this is a trivial scanner
{
  c &= 0xff;
  switch (c)
  {
    case '\b'      : scr->BackSpace();                 break;
    case '\t'      : scr->Tabulate();                  break;
    case '\n'      : scr->NewLine();                   break;
    case '\r'      : scr->Return();                    break;
    case 0x07      : gui->Bell();                      break;
    default        : scr->ShowCharacter(c);            break;
  };
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                             Keyboard Handling                             */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*
*/

void TEmulation::onKeyPress( QKeyEvent* ev )
{
  if (!connected) return; // someone else gets the keys
  if (scr->getHistCursor() != scr->getHistLines());
    scr->setHistCursor(scr->getHistLines());
  if (!ev->text().isEmpty())
  { // A block of text
    // Note that the text is proper unicode.
    // We should do a conversion here, but since this
    // routine will never be used, we simply emit plain ascii.
    emit sndBlock(ev->text().toStdString().c_str(),ev->text().length());
  }
  else if (ev->key()>0)
  { unsigned char c[1];
    c[0] = ev->key();
    emit sndBlock((char*)c,1);
  }
}

// Unblocking, Byte to Unicode translation --------------------------------- --

/*
   We are doing code conversion from locale to unicode first.
*/

void TEmulation::onRcvBlock(const char *s, int len)
{
  bulkStart();
  bulk_incnt += 1;

  select_encoding(gui->get_encoding());

  for (int i = 0; i < len; i++)
  {
    QString result = decoder->toUnicode(&s[i],1);
    int reslen = result.length();
    for (int j = 0; j < reslen; j++)
      onRcvChar(result[j].unicode());
    if (s[i] == '\n') bulkNewline();
  }
  bulkEnd();
}

// Selection --------------------------------------------------------------- --

void TEmulation::onSelectionBegin(const int x, const int y) {
  if (!connected) return;
  scr->setSelBeginXY(x,y);
  showBulk();
}

void TEmulation::onSelectionExtend(const int x, const int y) {
  if (!connected) return;
  scr->setSelExtentXY(x,y);
  showBulk();
}

void TEmulation::setSelection(const BOOL preserve_line_breaks) {
  if (!connected) return;
  QString t = scr->getSelText(preserve_line_breaks);
  if (!t.isNull()) gui->setSelection(t);
}

void TEmulation::clearSelection() {
  if (!connected) return;
  scr->clearSelection();
  showBulk();
}

// Refreshing -------------------------------------------------------------- --

#define BULK_TIMEOUT 20

/*
   called when \n comes in. Evtl. triggers showBulk at endBulk
*/

void TEmulation::bulkNewline()
{
  bulk_nlcnt += 1;
  bulk_incnt = 0;  // reset bulk counter since `nl' rule applies
}

/*
*/

void TEmulation::showBulk()
{
  bulk_nlcnt = 0;                       // reset bulk newline counter
  bulk_incnt = 0;                       // reset bulk counter
  if (connected)
  {
    ca* image = scr->getCookedImage();    // get the image
    gui->setImage(image,
                  scr->getLines(),
                  scr->getColumns());     // actual refresh
    free(image);
    //FIXME: check that we do not trigger other draw events here.
    gui->setScroll(scr->getHistCursor(),scr->getHistLines());
  }
}

void TEmulation::bulkStart()
{
  if (bulk_timer.isActive()) bulk_timer.stop();
}

void TEmulation::bulkEnd()
{
  if ( bulk_nlcnt > gui->Lines() || bulk_incnt > 20 )
    showBulk();                         // resets bulk_??cnt to 0, too.
  else
    bulk_timer.start(BULK_TIMEOUT);
}

void TEmulation::setConnect(bool c)
{
  connected = c;
  if ( connected)
  {
    onImageSizeChange(gui->Lines(), gui->Columns());
    showBulk();
  }
  else
  {
    scr->clearSelection();
  }
}

// ---------------------------------------------------------------------------

/*  triggered by image size change of the TEWidget `gui'.

    This event is simply propagated to the attached screens
    and to the related serial line.
*/

void TEmulation::onImageSizeChange(int lines, int columns)
{
  if (!connected) return;
  screen[0]->resizeImage(lines,columns);
  screen[1]->resizeImage(lines,columns);
  showBulk();
  emit ImageSizeChanged(lines,columns);   // propagate event to serial line
}

void TEmulation::onHistoryCursorChange(int cursor)
{
  if (!connected) return;
  scr->setHistCursor(cursor);
  showBulk();
}

void TEmulation::setColumns(int columns)
{
  //FIXME: this goes strange ways.
  //       Can we put this straight or explain it at least?
  emit changeColumns(columns);
}
