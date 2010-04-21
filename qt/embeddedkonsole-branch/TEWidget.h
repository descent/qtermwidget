/****************
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
/* [te_widget.h]           Terminal Emulation Widget                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>         */
/*                                                                         */
/* This file is part of Konsole - an X terminal for KDE                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                        */
/* Konsole ported to Qt/Embedded by Trolltech                              */
/*                        */
/* ----------------------------------------------------------------------- */

#ifndef TE_WIDGET_H
#define TE_WIDGET_H


#include <QLabel>
//#include <qwidget.h>
#include <QWidget>
#include "input_method_w.h"

//#include <qlabel.h>
#include <QLabel>

//#include <qtimer.h>
#include <QTimer>

//#include <qcolor.h>
#include <QColor>



//#include <qkeycode.h>

//#include <qscrollbar.h>
#include <QScrollBar>

//#include <qpopupmenu.h>
#include <QMenu> // qpopupmenu replace by qmenu in qt4

#include <QApplication>

#include "TECommon.h"
#include "screen.h"

using namespace Qt;
using namespace std;

extern unsigned short vt100_graphics[32];

class TESession;

// class Konsole;


//class MyPty;


class TEWidget : public QFrame
// a widget representing attributed text
{ Q_OBJECT

//  friend class Konsole;

public:

    TEWidget(QWidget *parent=0);
    virtual ~TEWidget();

  protected:
    void inputMethodEvent( QInputMethodEvent * e) ; // is a virtual function
public:
    QColor getDefaultBackColor();

    const ColorEntry* getColorTable() const;
    const ColorEntry* getdefaultColorTable() const;
    void              setColorTable(const ColorEntry table[]);

    void setScrollbarLocation(int loc);
    enum { SCRNONE=0, SCRLEFT=1, SCRRIGHT=2 };

    void setScroll(int cursor, int lines);
    void doScroll(int lines);

    void emitSelection();

public:

    void setImage(const ca* const newimg, int lines, int columns);

    int  Lines()   { return lines;   }
    int  Columns() { return columns; }

    void calcGeometry();
    void propagateSize();
    QSize calcSize(int cols, int lins) const;

    QSize sizeHint() const;

public:

    void Bell();
    void emitText(QString text);
    void pasteClipboard();

signals:

    void keyPressedSignal(QKeyEvent *e);
    void mouseSignal(int cb, int cx, int cy);
    void changedImageSizeSignal(int lines, int columns);
    void changedHistoryCursor(int value);
    void configureRequest( TEWidget*, int state, int x, int y );

    void clearSelectionSignal();
    void beginSelectionSignal( const int x, const int y );
    void extendSelectionSignal( const int x, const int y );
    void endSelectionSignal(const BOOL preserve_line_breaks);


protected:

    virtual void styleChange( QStyle& );

    bool eventFilter( QObject *, QEvent * );

    void drawAttrStr(QPainter &paint, QRect rect,
                     QString& str, ca attr, BOOL pm, BOOL clear);
    void paintEvent( QPaintEvent * );

    void resizeEvent(QResizeEvent*);

    void fontChange(const QFont &font);
    void frameChanged();

    void mouseDoubleClickEvent(QMouseEvent* ev);
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );

    void focusInEvent( QFocusEvent * );
    void focusOutEvent( QFocusEvent * );
    bool focusNextPrevChild( bool next );

#ifndef QT_NO_DRAGANDDROP
    // Dnd
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
#endif

    virtual int charClass(char) const;

    void clearImage();

public:
    const QPixmap *backgroundPixmap();
    
    void setSelection(const QString &t);

    //virtual void setFont(const QFont &);
    void setVTFont(const QFont &);
    QFont getVTFont();

    void setMouseMarks(bool on);

public slots:

    void onClearSelection();

protected slots:

    void scrollChanged(int value);
    void blinkEvent();

private:

    QChar (*fontMap)(QChar); // possible vt100 font extention

    DS::InputMethodW *input_text_;

    bool fixed_font; // has fixed pitch
    int  font_h;     // height
    int  font_w;     // width
    int  font_a;     // ascend

    int blX;    // actual offset (left)
    int brX;    // actual offset (right)
    int bY;     // actual offset

    int lines;
    int columns;
    ca *image; // [lines][columns]

    ColorEntry color_table[TABLE_COLORS];

    BOOL resizing;
    bool mouse_marks;

    void makeImage();

    QPoint iPntSel; // initial selection point
    QPoint pntSel; // current selection point
    int    actSel; // selection state
    BOOL    word_selection_mode;
    BOOL    preserve_line_breaks;

    QClipboard*    cb;
    QScrollBar* scrollbar;
    int         scrollLoc;

//#define SCRNONE  0
//#define SCRLEFT  1
//#define SCRRIGHT 2

    int encoding_;
    int input_encoding_; // input method encoding
    bool blinking;   // hide text in paintEvent
    bool hasBlinker; // has characters to blink
    QTimer* blinkT;  // active when hasBlinker
    //QPopupMenu* m_drop;
    QString dropText;
 public:
    void set_screen(DS::Screen *screen){screen_=screen;}
    void set_encoding(int enc){encoding_=enc;}
    int get_encoding(){return encoding_;}
    void set_input_encoding(int enc){input_encoding_=enc;}
    int get_input_encoding(){return input_encoding_;}

    // current session in this widget
    TESession *currentSession;
private slots:
    void get_pty_data(const char* s, int len);


    void drop_menu_activated(int item);

  private:
    //MyPty *mypty_;
    DS::Screen *screen_;
    char *ch_;
    int len_;
};

#endif // TE_WIDGET_H
