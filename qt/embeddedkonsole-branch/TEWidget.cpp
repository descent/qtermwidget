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
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* [TEWidget.C]            Terminal Emulation Widget                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>          */
/*                                                                          */
/* This file is part of Konsole - an X terminal for KDE                     */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                        */
/* Konsole ported to Qt/Embedded by Trolltech                               */
/*                        */
/* ------------------------------------------------------------------------ */

/* \class TEWidget

    \brief The TEWidget class represents the visible screen contents.

   This class is responsible for mapping the `image' of a terminal emulation to the
   display. The emulation's dependency on a specific GUI or toolkit is
   localized here. Furthermore, this widget has no knowledge of being part of an
   emulation, it simply works within the terminal emulation framework by exposing
   size and key events and by being ordered to show a new image.

   <ul>
   <li> The internal image has the size of the widget (evtl. rounded up)
   <li> The external image used in setImage can have any size.
   <li> (internally) the external image is simply copied to the internal
        when a setImage happens. During a resizeEvent no painting is done, but a paintEvent is expected to follow anyway.
   </ul>

   \sa TEScreen \sa Emulation
*/

/* FIXME:
   - 'image' may also be used uninitialized (it isn't in fact) in resizeEvent
   - 'font_a' is not used in mouse events
   - add destructor
*/

/* TODO
   - evtl. be sensitive to `paletteChange' while using default colors.
   - set different 'rounding' styles? I.e. have a mode to show clipped chars?
*/

#include <algorithm>

#include <QTextDecoder>

#include <QKeyEvent>
#include <QPaintEvent>

// #include "config.h"
#include "TEWidget.h"
#include "my_type.h"
//#include "MyPty.h"

//#include <qtopia/resource.h>
//#include "session.h"

//#include <qcursor.h>
#include <QCursor>

//#include <qregexp.h>
#include <QRegExp>

//#include <qpainter.h>
#include <QPainter>

//#include <qclipboard.h>
#include <QClipboard>

//#include <qstyle.h>
#include <QStyle>

//#include <qfile.h>
#include <QFile>

//#include <qdragobject.h>
#include <QMimeData>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#ifndef S_ISDIR
// Need for WIN32
#define S_ISDIR(st_mode) ((st_mode | S_IFDIR) != 0) 
#endif

#include <assert.h>

// #include "TEWidget.moc"
//#include <kapp.h>
//#include <kcursor.h>
//#include <kurl.h>
//#include <kdebug.h>
//#include <klocale.h>

#define HERE printf("%s(%d): %s\n",__FILE__,__LINE__,__FUNCTION__)
#define HCNT(Name) // { static int cnt = 1; printf("%s(%d): %s %d\n",__FILE__,__LINE__,Name,cnt++); }

#define loc(X,Y) ((Y)*columns+(X))

//FIXME: the rim should normally be 1, 0 only when running in full screen mode.
#define rimX 0      // left/right rim width
#define rimY 0      // top/bottom rim high

#define SCRWIDTH 16 // width of the scrollbar

#define yMouseScroll 1
// scroll increment used when dragging selection at top/bottom of window.

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                Colors                                     */
/*                                                                           */
/* ------------------------------------------------------------------------- */

//FIXME: the default color table is in session.C now.
//       We need a way to get rid of this one, here.
static const ColorEntry base_color_table[TABLE_COLORS] =
// The following are almost IBM standard color codes, with some slight
// gamma correction for the dim colors to compensate for bright X screens.
// It contains the 8 ansiterm/xterm colors in 2 intensities.
{
  // Fixme: could add faint colors here, also.
  // normal
  ColorEntry(QColor(0x00,0x00,0x00), 0, 0 ), ColorEntry( QColor(0xB2,0xB2,0xB2), 1, 0 ), // Dfore, Dback
  ColorEntry(QColor(0x00,0x00,0x00), 0, 0 ), ColorEntry( QColor(0xB2,0x18,0x18), 0, 0 ), // Black, Red
  ColorEntry(QColor(0x18,0xB2,0x18), 0, 0 ), ColorEntry( QColor(0xB2,0x68,0x18), 0, 0 ), // Green, Yellow
  ColorEntry(QColor(0x18,0x18,0xB2), 0, 0 ), ColorEntry( QColor(0xB2,0x18,0xB2), 0, 0 ), // Blue,  Magenta
  ColorEntry(QColor(0x18,0xB2,0xB2), 0, 0 ), ColorEntry( QColor(0xB2,0xB2,0xB2), 0, 0 ), // Cyan,  White
  // intensiv
  ColorEntry(QColor(0x00,0x00,0x00), 0, 1 ), ColorEntry( QColor(0xFF,0xFF,0xFF), 1, 0 ),
  ColorEntry(QColor(0x68,0x68,0x68), 0, 0 ), ColorEntry( QColor(0xFF,0x54,0x54), 0, 0 ),
  ColorEntry(QColor(0x54,0xFF,0x54), 0, 0 ), ColorEntry( QColor(0xFF,0xFF,0x54), 0, 0 ),
  ColorEntry(QColor(0x54,0x54,0xFF), 0, 0 ), ColorEntry( QColor(0xB2,0x18,0xB2), 0, 0 ),
  ColorEntry(QColor(0x54,0xFF,0xFF), 0, 0 ), ColorEntry( QColor(0xFF,0xFF,0xFF), 0, 0 )
};

/* Note that we use ANSI color order (bgr), while IBMPC color order is (rgb)

   Code        0       1       2       3       4       5       6       7
   ----------- ------- ------- ------- ------- ------- ------- ------- -------
   ANSI  (bgr) Black   Red     Green   Yellow  Blue    Magenta Cyan    White
   IBMPC (rgb) Black   Blue    Green   Cyan    Red     Magenta Yellow  White
*/

QColor TEWidget::getDefaultBackColor()
{
  return color_table[DEFAULT_BACK_COLOR].color;
}

void TEWidget::inputMethodEvent( QInputMethodEvent * e)  // is a virtual function
{
  static int c=0;
  qDebug("in TEWidget::inputMethodEvent : %d", c++);
  static QString preedit_string;
  preedit_string=e->preeditString();
  if (!e->commitString().isEmpty())
  {
    qDebug("!e->commitString() : %d", e->commitString().length() );
    static QString commit_string;
    commit_string=e->commitString();
    const char *ch=commit_string.toStdString().c_str();
    for (int i=0 ; i < commit_string.toStdString().length() ; ++i)
      qDebug("ch[i] : %x", ch[i]);
    //input_text_->setText(preedit_string+"*"+commit_string);
    input_text_->clear();
    //input_text_->setText("test"+commit_string);
    //input_text_->set_text("commit"+e->commitString() );
    //input_text_->show();
    //onRcvBlock();


    // QKeyEvent ( Type type, int key, Qt::KeyboardModifiers modifiers, const QString & text = QString(), bool autorep = false, ushort count = 1 )
#if 0
    int encode=get_encoding();
    QTextDecoder* decoder;
    QTextCodec *codec;
    QByteArray qba;
    const QChar * qchar;



    if ( get_input_encoding() == DS::BIG5)
    {
      codec = QTextCodec::codecForName("BIG5");
      //qDebug("(encoding==DS::BIG5)");
      //qDebug("(encoding==DS::UTF8)");
      qDebug("in encode==DS::BIG5");

    }
    if ( get_input_encoding() == DS::UTF8)
    {
      codec = QTextCodec::codecForName("UTF-8");
      //qchar= e->commitString().unicode();
      //codec = QTextCodec::codecForName("UTF-8");
      //qDebug("in encode==DS::UTF8");
      //qba=e->commitString().toUtf8();
    }



    QByteArray ba = codec->fromUnicode(ev->text());
    char *ba_data=ba.data();
    emit sndBlock(ba_data ,strlen(ba_data));    


    //for (int i=0 ; i < e->commitString().length() ; ++i)
      //qDebug("qchar[i] : %x", qchar[i].unicode () );
      //qDebug("qchar[i] : %x", i);
    decoder = codec->makeDecoder();
    const char *c_str= e->commitString().toStdString().c_str();
      qDebug("c_str len: %d", strlen(c_str));

    for (int i=0 ; i < strlen(c_str) ; ++i)
      qDebug("c_str[i]: %x", c_str[i]);

    commit_string=decoder->toUnicode(c_str, strlen(c_str));
#endif
    //QString str(qba);

    QKeyEvent* ke = new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier, commit_string);

    //actSel=0; // Key stroke implies a screen update, so TEWidget won't
              // know where the current selection is.

    emit keyPressedSignal(ke); // expose
    //preedit_string="";
    //commit_string="";
    //sleep(3);
  }
  else
  {
    qDebug("e->commitString()");
    #if 0
    if (!preedit_string.isEmpty())
    {
    input_text_->set_text("preedit"+preedit_string);
    input_text_->show();
    }
    #endif
  }
#if 0
  if (!preedit_string.isEmpty())
  {
    input_text_->set_text(preedit_string);
    input_text_->show();
  }
#endif
}

const ColorEntry* TEWidget::getColorTable() const
{
  return color_table;
}

const ColorEntry* TEWidget::getdefaultColorTable() const
{
  return base_color_table;
}


const QPixmap *TEWidget::backgroundPixmap()
{
  //static QPixmap *bg = new QPixmap( Resource::loadPixmap("faded_bg") );
  static QPixmap *bg = new QPixmap( "faded_bg.png" );
  const QPixmap *pm = bg;
  return pm;
}

void TEWidget::setColorTable(const ColorEntry table[])
{
  for (int i = 0; i < TABLE_COLORS; i++) color_table[i] = table[i];
  
  const QPixmap* pm = backgroundPixmap();
  //if (!pm) setBackgroundColor(color_table[DEFAULT_BACK_COLOR].color);
  QPalette palette;
  palette.setColor(QPalette::Background, color_table[DEFAULT_BACK_COLOR].color);
  setPalette(palette);

  qDebug("org code : if (!pm) setBackgroundColor(color_table[DEFAULT_BACK_COLOR].color);");
  update();
}

//FIXME: add backgroundPixmapChanged.

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                   Font                                    */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*
   The VT100 has 32 special graphical characters. The usual vt100 extended
   xterm fonts have these at 0x00..0x1f.

   QT's iso mapping leaves 0x00..0x7f without any changes. But the graphicals
   come in here as proper unicode characters.

   We treat non-iso10646 fonts as VT100 extended and do the required mapping
   from unicode to 0x00..0x1f. The remaining translation is then left to the
   QCodec.
*/

// assert for i in [0..31] : vt100extended(vt100_graphics[i]) == i.

unsigned short vt100_graphics[32] =
{ // 0/8     1/9    2/10    3/11    4/12    5/13    6/14    7/15
  0x0020, 0x25C6, 0x2592, 0x2409, 0x240c, 0x240d, 0x240a, 0x00b0,
  0x00b1, 0x2424, 0x240b, 0x2518, 0x2510, 0x250c, 0x2514, 0x253c,
  0xF800, 0xF801, 0x2500, 0xF803, 0xF804, 0x251c, 0x2524, 0x2534,
  0x252c, 0x2502, 0x2264, 0x2265, 0x03C0, 0x2260, 0x00A3, 0x00b7
};

static QChar vt100extended(QChar c)
{
  switch (c.unicode())
  {
    case 0x25c6 : return  1;
    case 0x2592 : return  2;
    case 0x2409 : return  3;
    case 0x240c : return  4;
    case 0x240d : return  5;
    case 0x240a : return  6;
    case 0x00b0 : return  7;
    case 0x00b1 : return  8;
    case 0x2424 : return  9;
    case 0x240b : return 10;
    case 0x2518 : return 11;
    case 0x2510 : return 12;
    case 0x250c : return 13;
    case 0x2514 : return 14;
    case 0x253c : return 15;
    case 0xf800 : return 16;
    case 0xf801 : return 17;
    case 0x2500 : return 18;
    case 0xf803 : return 19;
    case 0xf804 : return 20;
    case 0x251c : return 21;
    case 0x2524 : return 22;
    case 0x2534 : return 23;
    case 0x252c : return 24;
    case 0x2502 : return 25;
    case 0x2264 : return 26;
    case 0x2265 : return 27;
    case 0x03c0 : return 28;
    case 0x2260 : return 29;
    case 0x00a3 : return 30;
    case 0x00b7 : return 31;
  }
  return c;
}

static QChar identicalMap(QChar c)
{
  return c;
}

void TEWidget::fontChange(const QFont &)
{
  qDebug("TEWidget::fontChange font_w: %d", font_w);
  QFontMetrics fm(font());
  font_h = fm.height();
  font_w = fm.maxWidth();
  //sleep(3);
  font_a = fm.ascent();
//printf("font_h: %d\n",font_h);
//printf("font_w: %d\n",font_w);
//printf("font_a: %d\n",font_a);
//printf("charset: %s\n",QFont::encodingName(font().charSet()).ascii());
//printf("rawname: %s\n",font().rawName().ascii());
  fontMap =
#if QT_VERSION < 0x030000
  strcmp(QFont::encodingName(font().charSet()).ascii(),"iso10646")
          ? vt100extended
          :
#endif
  identicalMap;
  propagateSize();
  update();
}

void TEWidget::setVTFont(const QFont& f)
{
  QFrame::setFont(f);
}

QFont TEWidget::getVTFont() {
  return font();
}

#if 0
// I don't konw why to design, by descent
void TEWidget::setFont(const QFont &)
{
  // ignore font change request if not coming from konsole itself
}
#endif


/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                         Constructor / Destructor                          */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*
char * memcopy(void *des, void *src, size_t n)
{
  ch_ = new char [n];
  for (int i=0 ; i < n ; ++i)
    ch_[i]=src[i]; 
  return ch_;
}
*/

void TEWidget::get_pty_data(const char* s, int len)
{
  printf("TEWidget::get_pty_data\n");

//#if 0
  //ch_ = new char [len];
  //ch_ = memcopy(s,);
  ch_ = new char [len+1];
  //char *p=ch_;
  for (int i=0 ; i < len ; ++i)
  {
    //qDebug("len: %d", len);
    ch_[i]=s[i]; 
    //*p++=*s++;
  }
  //*p=0;
  ch_[len]=0;
  //ch_=strdup("abc");
  len_=len;
//#endif
  //ch_=strdup("ABC");
  repaint();

  write(STDOUT_FILENO,s,len);
  //write(STDOUT_FILENO,s,len);
  //for (int i=0 ; i < len ; ++i)
    //printf("%c", s[i]);
  //printf("\n");
}

TEWidget::TEWidget(QWidget *parent) : QFrame(parent)
{
  setuid(getuid()); setgid(getgid()); // drop privileges
  len_=0;
  ch_=0;


  // prompt not the same (/bin/bash, /bin/sh)
  //const char *shell="/bin/bash";
  const char *shell="/bin/sh";
  //mypty_ = new MyPty();
  //StrList str;
  //mypty_->run(shell, str, "", false);
  //screen_ = new DS::Screen();

  //connect( mypty_, SIGNAL( block_in(const char* , int ) ), this, SLOT(get_pty_data(const char* , int) ) );
  //input_text_ = new QLabel();
  input_text_ = new DS::InputMethodW(0);
  //input_text_->setGeometry(100,100,400,400);
#ifndef QT_NO_CLIPBOARD
  cb = QApplication::clipboard();
  QObject::connect( (QObject*)cb, SIGNAL(dataChanged()), this, SLOT(onClearSelection()) );
#endif
  setAttribute(Qt::WA_InputMethodEnabled);

  scrollbar = new QScrollBar(this);
  scrollbar->setCursor(Qt::ArrowCursor);
  connect(scrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollChanged(int)));
  scrollLoc = SCRRIGHT;

  blinkT   = new QTimer(this);
  connect(blinkT, SIGNAL(timeout()), this, SLOT(blinkEvent()));
  // blinking = FALSE;
  blinking = TRUE;

  resizing = FALSE;
  actSel   = 0;
  image    = 0;
  font_a   = 1;
  word_selection_mode = FALSE;

  QFontMetrics fm(font());
  font_h = fm.height();
  font_w = fm.maxWidth();

  lines = 1;
  columns = 1;

  setMouseMarks(TRUE);
  //setVTFont( QFont("fixed") );
  setColorTable(base_color_table); // init color table

  qApp->installEventFilter( this ); //FIXME: see below
//  KCursor::setAutoHideCursor( this, true );

  // Init DnD ////////////////////////////////////////////////////////////////
  currentSession = 0;
//  setAcceptDrops(true); // attempt
//  m_drop = new QPopupMenu(this);
//  m_drop->insertItem( QString("Paste"), 0);
//  m_drop->insertItem( QString("cd"),    1);
//  connect(m_drop, SIGNAL(activated(int)), SLOT(drop_menu_activated(int)));

  // we need focus so that the auto-hide cursor feature works
  setFocus();
  setFocusPolicy( WheelFocus );
}

//FIXME: make proper destructor
// Here's a start (David)
TEWidget::~TEWidget()
{
  qApp->removeEventFilter( this );
  if (image) free(image);
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                             Display Operations                            */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*
    attributed string draw primitive
*/


void TEWidget::drawAttrStr(QPainter &paint, QRect rect,
                           QString& str, ca attr, BOOL pm, BOOL clear)
{
  //qDebug("begin TEWidget::drawAttrStr");
  if (pm && color_table[attr.b].transparent)
  {
    paint.setBackgroundMode( TransparentMode );
    if (clear) 
    {
      // erase(rect);
      qDebug("org code : erase(rect);");
    }
  }
  else
  {
    if (blinking)
    {
      paint.fillRect(rect, color_table[attr.b].color); // draw cursor
      qDebug("\tdarw cursor");
    }
    else
    {
      paint.setBackgroundMode(Qt::OpaqueMode);
      //paint.setBackgroundColor( color_table[attr.b].color );
      qDebug("org code : paint.setBackgroundColor( color_table[attr.b].color );");
    }
  }


  if (color_table[attr.f].bold)
    paint.setPen(QColor( 0x8F, 0x00, 0x00 ));
  else
    paint.setPen(color_table[attr.f].color);

  paint.drawText(rect.x(),rect.y()+font_a, str);

  static int count=0;
  if (attr.r & RE_UNDERLINE)
    paint.drawLine(rect.left(), rect.y()+font_a+1, rect.right(),rect.y()+font_a+1 );
  //qDebug("\t%d  str : %s", count++,str.toStdString().c_str() );
  //qDebug("end TEWidget::drawAttrStr");
}

/*
    The image can only be set completely.

    The size of the new image may or may not match the size of the widget.
*/

void TEWidget::setImage(const ca* const newimg, int lines, int columns)
{ int y,x,len;
  const QPixmap* pm = backgroundPixmap();
  //qDebug("in TEWidget::setImage()");
  //sleep(2);
  //QPainter paint;
  setUpdatesEnabled(FALSE);
  //paint.begin( this );
HCNT("setImage");

  QPoint tL  = contentsRect().topLeft();
  int    tLx = tL.x();
  int    tLy = tL.y();
  hasBlinker = FALSE;

  int cf  = -1; // undefined
  int cb  = -1; // undefined
  int cr  = -1; // undefined

  int lins = min(this->lines,  max(0,lines  ));
  int cols = min(this->columns,max(0,columns));
  QChar *disstrU = new QChar[cols];
  bool repaint_screen=false;

//{ static int cnt = 0; printf("setImage %d\n",cnt++); }
  for (y = 0; y < lins; y++)
  {
    const ca*       lcl = &image[y*this->columns];
    const ca* const ext = &newimg[y*columns];
    if (!resizing) // not while resizing, we're expecting a paintEvent
    for (x = 0; x < cols; x++)
    {
      hasBlinker |= (ext[x].r & RE_BLINK);
      if (ext[x] != lcl[x])
      {
        repaint_screen=true;
        cr = ext[x].r;
        cb = ext[x].b;
        if (ext[x].f != cf) cf = ext[x].f;
        int lln = cols - x;
        disstrU[0] = fontMap(ext[x+0].c);
        for (len = 1; len < lln; len++)
        {
          if (ext[x+len].f != cf || ext[x+len].b != cb || ext[x+len].r != cr ||
              ext[x+len] == lcl[x+len] )
            break;
          disstrU[len] = fontMap(ext[x+len].c);
        }
        //QString unistr(disstrU,len);
        //drawAttrStr(paint, QRect(blX+tLx+font_w*x,bY+tLy+font_h*y,font_w*len,font_h), unistr, ext[x], pm != NULL, true);
        x += len - 1;
      }
      else
      {
        //qDebug("(ext[x] == lcl[x]");
      }
    }
    // finally, make `image' become `newimg'.
    memcpy((void*)lcl,(const void*)ext,cols*sizeof(ca));
  }
  //drawFrame( &paint );
  //paint.end();
  if (repaint_screen==true)
    setUpdatesEnabled(TRUE);
  if ( hasBlinker && !blinkT->isActive()) blinkT->start(1000); // 1000 ms
  if (!hasBlinker &&  blinkT->isActive()) { blinkT->stop(); blinking = FALSE; }
  delete [] disstrU;
}

// paint Event ////////////////////////////////////////////////////

/*
    The difference of this routine vs. the `setImage' is,
    that the drawing does not include a difference analysis
    between the old and the new image. Instead, the internal
    image is used and the painting bound by the PaintEvent box.
*/

void TEWidget::paintEvent( QPaintEvent* pe )
{
  QPainter p;

  qDebug("TEWidget::paintEvent");
  qDebug("lines: %d", lines);
  qDebug("columns: %d", columns);

  const deque<ca> &screen_data=screen_->get_screen_data();
  deque<ca> :: const_iterator it=screen_data.begin();
  int x=0, y=10;
  p.begin(this);
  for ( ; it != screen_data.end() ; ++it)
  {
    ca c=*it;
    char ch=c.c;
    
    if (ch == '\n' || x >= columns*font_w || ch=='\r')
    {
      y+=font_h;
      x=0;
      continue;
    }

    QChar qch(ch);
    QString qstr(1, qch);

    p.drawText(x,y, qstr);
    x+=font_w;
  }

  p.end();
#if 0
  //x=y=10;
  //QString str(ch_, len_);
  //ch_=strdup("abc");
  qDebug("ch_ : %s", ch_);
  p.drawText(x,y,ch_);
  x+=(len_*font_w);

#endif
  //p.drawText(x,y,ch_);

#if 0
  //p.drawText(10,10,"A");
  for (int i=0 ; i < len_ ; ++i)
  {
    p.drawText(x,y,ch_);
    //p.drawText(x,y,"A");
    x+=font_w;
  }
  delete ch_;
#endif
  //qDebug("TEWidget::paintEvent");
}

void TEWidget::blinkEvent()
{
  blinking = !blinking;
  //repaint(FALSE);
  repaint();
}


/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                  Resizing                                 */
/*                                                                           */
/* ------------------------------------------------------------------------- */

void TEWidget::resizeEvent(QResizeEvent* ev)
{
  qDebug("calcGeometry()");
  calcGeometry();
  //mypty_->setSize(lines, columns);
  screen_->change_size(lines, columns);
  // change pty size

  //printf("resize: %d,%d\n",ev->size().width(),ev->size().height());
  //printf("approx: %d,%d\n",ev->size().width()/font_w,ev->size().height()/font_h);
  //printf("leaves: %d,%d\n",ev->size().width()%font_w,ev->size().height()%font_h);
  //printf("curren: %d,%d\n",width(),height());
HCNT("resizeEvent");

  // see comment in `paintEvent' concerning the rounding.
  //FIXME: could make a routine here; check width(),height()
  assert(ev->size().width() == width());
  assert(ev->size().height() == height());

  propagateSize();
}


void TEWidget::propagateSize()
{
  ca* oldimg = image;
  int oldlin = lines;
  int oldcol = columns;
#if 0
  makeImage();
  // we copy the old image to reduce flicker
  int lins = min(oldlin,lines);
  int cols = min(oldcol,columns);
  if (oldimg)
  {
    for (int lin = 0; lin < lins; lin++)
      memcpy((void*)&image[columns*lin],
             (void*)&oldimg[oldcol*lin],cols*sizeof(ca));
    free(oldimg); //FIXME: try new,delete
  }
  else
    clearImage();

  //NOTE: control flows from the back through the chest right into the eye.
  //      `emu' will call back via `setImage'.

  resizing = TRUE;
  emit changedImageSizeSignal(lines, columns); // expose resizeEvent
  resizing = FALSE;
#endif
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                Scrollbar                                  */
/*                                                                           */
/* ------------------------------------------------------------------------- */

void TEWidget::scrollChanged(int)
{
  emit changedHistoryCursor(scrollbar->value()); //expose
}

void TEWidget::setScroll(int cursor, int slines)
{
  disconnect(scrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollChanged(int)));
  scrollbar->setRange(0,slines);
  //scrollbar->setSteps(1,lines);
  scrollbar->setSingleStep(lines);
  scrollbar->setValue(cursor);
  connect(scrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollChanged(int)));
}

void TEWidget::setScrollbarLocation(int loc)
{
  if (scrollLoc == loc) return; // quickly
  scrollLoc = loc;
  propagateSize();
  update();
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                   Mouse                                   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

/*?
    Three different operations can be performed using the mouse and the
    routines in this section serve all of them:

    1) The press/release events are exposed to the application
    2) Marking (press and move left button) and Pasting (press middle button)
    3) The right mouse button is used from the configuration menu

    NOTE: During the marking process we attempt to keep the cursor within
    the bounds of the text as being displayed by setting the mouse position
    whenever the mouse has left the text area.

    Two reasons to do so:
    1) QT does not allow the `grabMouse' to confine-to the TEWidget.
       Thus a `XGrapPointer' would have to be used instead.
    2) Even if so, this would not help too much, since the text area
       of the TEWidget is normally not identical with it's bounds.

    The disadvantage of the current handling is, that the mouse can visibly
    leave the bounds of the widget and is then moved back. Because of the
    current construction, and the reasons mentioned above, we cannot do better
    without changing the overall construction.
*/

/*
*/

void TEWidget::mousePressEvent(QMouseEvent* ev)
{
//printf("press [%d,%d] %d\n",ev->x()/font_w,ev->y()/font_h,ev->button());
  if ( !contentsRect().contains(ev->pos()) ) return;
  QPoint tL  = contentsRect().topLeft();
  int    tLx = tL.x();
  int    tLy = tL.y();

  word_selection_mode = FALSE;

//printf("press top left [%d,%d] by=%d\n",tLx,tLy, bY);
  if ( ev->button() == LeftButton)
  {
    QPoint pos = QPoint((ev->x()-tLx-blX)/font_w,(ev->y()-tLy-bY)/font_h);

    //if ( ev->state() & ControlButton ) preserve_line_breaks = FALSE ;
    if ( ev->modifiers() & Qt::ControlModifier ) preserve_line_breaks = FALSE ;

    if (mouse_marks || (ev->modifiers() & Qt::ShiftModifier))
    {
      emit clearSelectionSignal();
      iPntSel = pntSel = pos;
      actSel = 1; // left mouse button pressed but nothing selected yet.
      grabMouse(   /*crossCursor*/  ); // handle with care! 
    }
    else
    {
      emit mouseSignal( 0, pos.x() + 1, pos.y() + 1 ); // left button
    }
  }
  if ( ev->button() == MidButton )
  {
    emitSelection();
  }
  if ( ev->button() == RightButton ) // Configure
  {
    emit configureRequest( this, ev->modifiers()&(Qt::ShiftModifier|Qt::ControlModifier), ev->x(), ev->y() );
  }
}

void TEWidget::mouseMoveEvent(QMouseEvent* ev)
{
  // for auto-hiding the cursor, we need mouseTracking
  if (ev->modifiers() == Qt::NoButton ) return;

  if (actSel == 0) return;

 // don't extend selection while pasting
  if (ev->modifiers() & Qt::MidButton) return;

  //if ( !contentsRect().contains(ev->pos()) ) return;
  QPoint tL  = contentsRect().topLeft();
  int    tLx = tL.x();
  int    tLy = tL.y();
  int    scroll = scrollbar->value();

  // we're in the process of moving the mouse with the left button pressed
  // the mouse cursor will remain caught within the bounds of the text in
  // this widget.

  // Adjust position within text area bounds. See FIXME above.
  QPoint pos = ev->pos();
  if ( pos.x() < tLx+blX )                  pos.setX( tLx+blX );
  if ( pos.x() > tLx+blX+columns*font_w-1 ) pos.setX( tLx+blX+columns*font_w );
  if ( pos.y() < tLy+bY )                   pos.setY( tLy+bY );
  if ( pos.y() > tLy+bY+lines*font_h-1 )    pos.setY( tLy+bY+lines*font_h-1 );
  // check if we produce a mouse move event by this
  if ( pos != ev->pos() ) cursor().setPos(mapToGlobal(pos));

  if ( pos.y() == tLy+bY+lines*font_h-1 )
  {
    scrollbar->setValue(scrollbar->value()+yMouseScroll); // scrollforward
  }
  if ( pos.y() == tLy+bY )
  {
    scrollbar->setValue(scrollbar->value()-yMouseScroll); // scrollback
  }

  QPoint here = QPoint((pos.x()-tLx-blX)/font_w,(pos.y()-tLy-bY)/font_h);
  QPoint ohere;
  bool swapping = FALSE;

  if ( word_selection_mode )
  {
    // Extend to word boundaries
    int i;
    int selClass;

    bool left_not_right = ( here.y() < iPntSel.y() ||
     here.y() == iPntSel.y() && here.x() < iPntSel.x() );
    bool old_left_not_right = ( pntSel.y() < iPntSel.y() ||
     pntSel.y() == iPntSel.y() && pntSel.x() < iPntSel.x() );
    swapping = left_not_right != old_left_not_right;

    // Find left (left_not_right ? from here : from start)
    QPoint left = left_not_right ? here : iPntSel;
    i = loc(left.x(),left.y());
    selClass = charClass(image[i].c);
    while ( left.x() > 0 && charClass(image[i-1].c) == selClass )
    { i--; left.rx()--; }

    // Find left (left_not_right ? from start : from here)
    QPoint right = left_not_right ? iPntSel : here;
    i = loc(right.x(),right.y());
    selClass = charClass(image[i].c);
    while ( right.x() < columns-1 && charClass(image[i+1].c) == selClass )
    { i++; right.rx()++; }

    // Pick which is start (ohere) and which is extension (here)
    if ( left_not_right )
    {
      here = left; ohere = right;
    }
    else
    {
      here = right; ohere = left;
    }
  }

  if (here == pntSel && scroll == scrollbar->value()) return; // not moved

  if ( word_selection_mode ) {
    if ( actSel < 2 || swapping ) {
      emit beginSelectionSignal( ohere.x(), ohere.y() );
    }
  } else if ( actSel < 2 ) {
    emit beginSelectionSignal( pntSel.x(), pntSel.y() );
  }

  actSel = 2; // within selection
  pntSel = here;
  emit extendSelectionSignal( here.x(), here.y() );
}

void TEWidget::mouseReleaseEvent(QMouseEvent* ev)
{
//printf("release [%d,%d] %d\n",ev->x()/font_w,ev->y()/font_h,ev->button());
  if ( ev->button() == LeftButton)
  {
    if ( actSel > 1 ) emit endSelectionSignal(preserve_line_breaks);
    preserve_line_breaks = TRUE;
    actSel = 0;

    //FIXME: emits a release event even if the mouse is
    //       outside the range. The procedure used in `mouseMoveEvent'
    //       applies here, too.

    QPoint tL  = contentsRect().topLeft();
    int    tLx = tL.x();
    int    tLy = tL.y();

    if (!mouse_marks && !(ev->modifiers() & Qt::ShiftModifier))
      emit mouseSignal( 3, // release
                        (ev->x()-tLx-blX)/font_w + 1,
                        (ev->y()-tLy-bY)/font_h + 1 );
    releaseMouse();
  }
}

void TEWidget::mouseDoubleClickEvent(QMouseEvent* ev)
{
  if ( ev->button() != Qt::LeftButton) return;

  QPoint tL  = contentsRect().topLeft();
  int    tLx = tL.x();
  int    tLy = tL.y();
  QPoint pos = QPoint((ev->x()-tLx-blX)/font_w,(ev->y()-tLy-bY)/font_h);

  // pass on double click as two clicks.
  if (!mouse_marks && !(ev->modifiers() & Qt::ShiftModifier))
  {
    emit mouseSignal( 0, pos.x()+1, pos.y()+1 ); // left button
    emit mouseSignal( 3, pos.x()+1, pos.y()+1 ); // release
    emit mouseSignal( 0, pos.x()+1, pos.y()+1 ); // left button
    return;
  }


  emit clearSelectionSignal();
  QPoint bgnSel = pos;
  QPoint endSel = QPoint((ev->x()-tLx-blX)/font_w,(ev->y()-tLy-bY)/font_h);
  int i = loc(bgnSel.x(),bgnSel.y());
  iPntSel = bgnSel;

  word_selection_mode = TRUE;

  // find word boundaries...
  int selClass = charClass(image[i].c);
  {
    // set the start...
     int x = bgnSel.x();
     while ( x > 0 && charClass(image[i-1].c) == selClass )
     { i--; x--; }
     bgnSel.setX(x);
     emit beginSelectionSignal( bgnSel.x(), bgnSel.y() );

     // set the end...
     i = loc( endSel.x(), endSel.y() );
     x = endSel.x();
     while( x < columns-1 && charClass(image[i+1].c) == selClass )
     { i++; x++ ; }
     endSel.setX(x);
     actSel = 2; // within selection
     emit extendSelectionSignal( endSel.x(), endSel.y() );
     emit endSelectionSignal(preserve_line_breaks);
     preserve_line_breaks = TRUE;
   }
}

void TEWidget::focusInEvent( QFocusEvent * )
{
    // do nothing, to prevent repainting
}


void TEWidget::focusOutEvent( QFocusEvent * )
{
    // do nothing, to prevent repainting
}

bool TEWidget::focusNextPrevChild( bool next )
{
  if (next)
    return false; // This disables changing the active part in konqueror
                  // when pressing Tab
  return QFrame::focusNextPrevChild( next );
}


int TEWidget::charClass(char ch) const
{
    // This might seem like overkill, but imagine if ch was a Unicode
    // character (Qt 2.0 QChar) - it might then be sensible to separate
    // the different language ranges, etc.

    if ( isspace(ch) ) return ' ';

    static const char *word_characters = ":@-./_~";
    if ( isalnum(ch) || strchr(word_characters, ch) )
    return 'a';

    // Everything else is weird
    return 1;
}

void TEWidget::setMouseMarks(bool on)
{
  mouse_marks = on;
  setCursor( mouse_marks ? Qt::IBeamCursor : Qt::ArrowCursor );
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                               Clipboard                                   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#undef KeyPress

void TEWidget::emitSelection()
// Paste Clipboard by simulating keypress events
{
#ifndef QT_NO_CLIPBOARD
  QString text = QApplication::clipboard()->text();
  if ( ! text.isNull() )
  {
    text.replace(QRegExp("\n"), "\r");
    //QKeyEvent e(QEvent::KeyPress, 0, -1, 0, text);
    QKeyEvent e(QEvent::KeyPress, 0, Qt::NoModifier, text);
    emit keyPressedSignal(&e); // expose as a big fat keypress event
    emit clearSelectionSignal();
  }
#endif
}

void TEWidget::emitText(QString text)
{
  QKeyEvent e(QEvent::KeyPress, 0, Qt::NoModifier, text);
  emit keyPressedSignal(&e); // expose as a big fat keypress event
}

void TEWidget::pasteClipboard( )
{
  emitSelection();
}

static bool weSetSelection = FALSE;

void TEWidget::setSelection(const QString& t)
{
#ifndef QT_NO_CLIPBOARD
  // this disconnection doesn't work at we receive the event after we reconnect
  
  // Disconnect signal while WE set the clipboard
//  QObject *cb = QApplication::clipboard();
//  QObject::disconnect( cb, SIGNAL(dataChanged()),
//                     this, SLOT(onClearSelection()) );

  weSetSelection = TRUE;
  QApplication::clipboard()->setText(t);

//  QObject::connect( cb, SIGNAL(dataChanged()),
//                     this, SLOT(onClearSelection()) );
#endif
}

void TEWidget::onClearSelection()
{
  if ( weSetSelection ) {
    weSetSelection = FALSE;
    return;
  }
  emit clearSelectionSignal();
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                Keyboard                                   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

//FIXME: an `eventFilter' has been installed instead of a `keyPressEvent'
//       due to a bug in `QT' or the ignorance of the author to prevent
//       repaint events being emitted to the screen whenever one leaves
//       or reenters the screen to/from another application.
//
//   Troll says one needs to change focusInEvent() and focusOutEvent(),
//   which would also let you have an in-focus cursor and an out-focus
//   cursor like xterm does.

// for the auto-hide cursor feature, I added empty focusInEvent() and
// focusOutEvent() so that update() isn't called.
// For auto-hide, we need to get keypress-events, but we only get them when
// we have focus.

void TEWidget::doScroll(int lines)
{
  scrollbar->setValue(scrollbar->value()+lines);
}

bool TEWidget::eventFilter( QObject *obj, QEvent *e )
{
#if 0
// because QT4 using  QShortcut, so I diable this function
  if ( (e->type() == QEvent::Accel ||
       e->type() == QEvent::AccelAvailable ) && qApp->focusWidget() == this )
  {
      static_cast<QKeyEvent *>( e )->ignore();
      return true;
  }
#endif
  if ( obj != this /* when embedded */ && obj != parent() /* when standalone */ )
      return FALSE; // not us
  if ( e->type() == QEvent::Wheel)
  {
    QApplication::sendEvent(scrollbar, e);
  }

#ifdef FAKE_CTRL_AND_ALT
    static bool control = FALSE;
    static bool alt = FALSE;
    // Has a keyboard with no CTRL and ALT keys, but we fake it:
    bool dele=FALSE;
    if ( e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease ) {
  QKeyEvent* ke = (QKeyEvent*)e;
  bool keydown = e->type() == QEvent::KeyPress || ke->isAutoRepeat();
  switch (ke->key()) {
      case Key_F9: // let this be "Control"
    control = keydown;
    e = new QKeyEvent(QEvent::KeyPress, Key_Control, 0, ke->state());
    dele=TRUE;
    break;
      case Key_F13: // let this be "Alt"
    alt = keydown;
    e = new QKeyEvent(QEvent::KeyPress, Key_Alt, 0, ke->state());
    dele=TRUE;
    break;
      default:
    if ( control ) {
        int a = toupper(ke->ascii())-64;
        if ( a >= 0 && a < ' ' ) {
      e = new QKeyEvent(e->type(), ke->key(),
        a, ke->state()|ControlButton, QChar(a,0));
      dele=TRUE;
        }
    }
    if ( alt ) {
        e = new QKeyEvent(e->type(), ke->key(),
        ke->ascii(), ke->state()|AltButton, ke->text());
        dele=TRUE;
    }
  }
    }
#endif

  if ( e->type() == QEvent::KeyPress )
  {
    //qDebug("QEvent::KeyPress");
    QKeyEvent* ke = (QKeyEvent*)e;
    //mypty_->send_bytes(ke->text().toStdString().c_str(),ke->text().length() );

    const char *str=ke->text().toStdString().c_str();
    screen_->key_press(str[0]);
#if 0
    //mypty_->send_bytes("ls\n",3);
    if (ke->key()>0)
    { 
      unsigned char c[1];
      c[0] = ke->key();
      qDebug("ke->key(): %x", ke->key());
      if (c[0]==Qt::Key_Enter)
      {
        qDebug("Qt::Key_Enter");

      }
      if (c[0]==Qt::Key_Return)
      {
        qDebug("Qt::Key_Return");
        c[0]='\n';
        sleep(5);
      }
      screen_->key_press(c[0]);
      //mypty_->send_bytes((char*)c,1);
      //mypty_->send_bytes((char*)c,1);
      //emit sndBlock((char*)c,1);
    }
    else
    {
      qDebug("ke->key()<=0");

    }

#endif
    actSel=0; // Key stroke implies a screen update, so TEWidget won't
              // know where the current selection is.

    emit keyPressedSignal(ke); // expose
    ke->accept();
#ifdef FAKE_CTRL_AND_ALT
    if ( dele ) delete e;
#endif
    return true;               // stop the event
  }
  if ( e->type() == QEvent::Enter )
  {
    QObject::disconnect( (QObject*)cb, SIGNAL(dataChanged()),
      this, SLOT(onClearSelection()) );
  }
  if ( e->type() == QEvent::Leave )
  {
    QObject::connect( (QObject*)cb, SIGNAL(dataChanged()),
      this, SLOT(onClearSelection()) );
  }
  return QFrame::eventFilter( obj, e );
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                  Frame                                    */
/*                                                                           */
/* ------------------------------------------------------------------------- */

void TEWidget::frameChanged()
{
  propagateSize();
  update();
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                   Sound                                   */
/*                                                                           */
/* ------------------------------------------------------------------------- */

void TEWidget::Bell()
{
  QApplication::beep();
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                 Auxiluary                                 */
/*                                                                           */
/* ------------------------------------------------------------------------- */

void TEWidget::clearImage()
// initialize the image
// for internal use only
{
  for (int y = 0; y < lines; y++)
  for (int x = 0; x < columns; x++)
  {
    image[loc(x,y)].c = 0xff; //' ';
    image[loc(x,y)].f = 0xff; //DEFAULT_FORE_COLOR;
    image[loc(x,y)].b = 0xff; //DEFAULT_BACK_COLOR;
    image[loc(x,y)].r = 0xff; //DEFAULT_RENDITION;
  }
}

// Create Image ///////////////////////////////////////////////////////

void TEWidget::calcGeometry()
{
  //FIXME: set rimX == rimY == 0 when running in full screen mode.

//  scrollbar->resize(QApplication::style().scrollBarExtent().width(), contentsRect().height());
// qt3/qt4 resize method ref from kdebase-3.4.3 konsole source code
scrollbar->resize(QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent), contentsRect().height());
  int s_w=scrollbar->width();
//  must use new api but I don't know which one
  switch(scrollLoc)
  {
    case SCRNONE :
     columns = ( contentsRect().width() - 2 * rimX ) / font_w;
     blX = (contentsRect().width() - (columns*font_w) ) / 2;
     brX = blX;
     scrollbar->hide();
     break;
    case SCRLEFT :
     columns = ( contentsRect().width() - 2 * rimX - scrollbar->width()) / font_w;
     brX = (contentsRect().width() - (columns*font_w) - scrollbar->width() ) / 2;
     blX = brX + scrollbar->width();
     scrollbar->move(contentsRect().topLeft());
     scrollbar->show();
     break;
    case SCRRIGHT:
     columns = ( contentsRect().width()  - 2 * rimX - scrollbar->width()) / font_w;
     blX = (contentsRect().width() - (columns*font_w) - scrollbar->width() ) / 2;
     qDebug("blX : %d", blX);
     //sleep(5);
     brX = blX;
     scrollbar->move(contentsRect().topRight() - QPoint(scrollbar->width()-1,0));
     scrollbar->show();
     break;
  }
  //FIXME: support 'rounding' styles
  lines   = ( contentsRect().height() - 2 * rimY  ) / font_h;
  bY = (contentsRect().height() - (lines  *font_h)) / 2;
}

void TEWidget::makeImage()
//FIXME: rename 'calcGeometry?
{
  calcGeometry();
  image = (ca*) malloc(lines*columns*sizeof(ca));
  clearImage();
}

// calculate the needed size
QSize TEWidget::calcSize(int cols, int lins) const
{
  int frw = width() - contentsRect().width();
  int frh = height() - contentsRect().height();
  int scw = (scrollLoc==SCRNONE?0:scrollbar->width());
  return QSize( font_w*cols + 2*rimX + frw + scw, font_h*lins + 2*rimY + frh );
}

QSize TEWidget::sizeHint() const
{
   return size();
}

void TEWidget::styleChange(QStyle &)
{
    propagateSize();
}


#ifndef QT_NO_DRAGANDDROP

/* --------------------------------------------------------------------- */
/*                                                                       */
/* Drag & Drop                                                           */
/*                                                                       */
/* --------------------------------------------------------------------- */


void TEWidget::dragEnterEvent(QDragEnterEvent* e)
{
#if 0
  e->accept(QTextDrag::canDecode(e) ||
      QUriDrag::canDecode(e));
#endif
}

void TEWidget::dropEvent(QDropEvent* event)
{
#if 0
    // The current behaviour when URL(s) are dropped is:
    // * if there is only ONE URL and if it's a LOCAL one, ask for paste or cd
    // * in all other cases, just paste
    //   (for non-local ones, or for a list of URLs, 'cd' is nonsense)
  QStrList strlist;
  int file_count = 0;
  dropText = "";
  bool bPopup = true;

  if(QUriDrag::decode(event, strlist)) {
    if (strlist.count()) {
      for(const char* p = strlist.first(); p; p = strlist.next()) {
        if(file_count++ > 0) {
          dropText += " ";
          bPopup = false; // more than one file, don't popup
        }

/*
        KURL url(p);
        if (url.isLocalFile()) {
          dropText += url.path(); // local URL : remove protocol
        }
        else {
          dropText += url.prettyURL();
          bPopup = false; // a non-local file, don't popup
        }
*/

      }

      if (bPopup)
        // m_drop->popup(pos() + event->pos());
  //m_drop->popup(mapToGlobal(event->pos())); // reserve
      else
  {
    if (currentSession) {
      currentSession->getEmulation()->sendString(dropText.local8Bit());
    }
//    kdDebug() << "Drop:" << dropText.local8Bit() << "\n";
  }
    }
  }
  else if(QTextDrag::decode(event, dropText)) {
//    kdDebug() << "Drop:" << dropText.local8Bit() << "\n";
    if (currentSession) {
      currentSession->getEmulation()->sendString(dropText.local8Bit());
    }
    // Paste it
  }
#endif
}


void TEWidget::drop_menu_activated(int item)
{
#if 0
#ifndef QT_NO_DRAGANDDROP
  switch (item)
  {
    case 0: // paste
      currentSession->getEmulation()->sendString(dropText.local8Bit());
//    KWM::activate((Window)this->winId());
      break;
    case 1: // cd ...
      currentSession->getEmulation()->sendString("cd ");
      struct stat statbuf;
      if ( ::stat( QFile::encodeName( dropText ), &statbuf ) == 0 )
      {
        if ( !S_ISDIR(statbuf.st_mode) )
        {
/*
          KURL url;
          url.setPath( dropText );
          dropText = url.directory( true, false ); // remove filename
*/
        }
      }
      dropText.replace(QRegExp(" "), "\\ "); // escape spaces
      currentSession->getEmulation()->sendString(dropText.local8Bit());
      currentSession->getEmulation()->sendString("\n");
//    KWM::activate((Window)this->winId());
      break;
  }
#else
  Q_UNUSED(item);
#endif

#endif
}



#endif // if 0
