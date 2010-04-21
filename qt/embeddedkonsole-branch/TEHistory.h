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
#ifndef TEHISTORY_H
#define TEHISTORY_H

#include "TECommon.h"

class HistoryScrollData;

class HistoryScroll
{
public:
    HistoryScroll();
    ~HistoryScroll();

public:
    void setScroll(bool on);
    bool hasScroll() const;

public:
    int  getLines() const;
    int  getLineLen(int lineno) const;
    void getCells(int lineno, int colno, int count, ca res[]) const;
    ca   getCell(int lineno, int colno) const;

public:
    void addLine(ca a[], int count);

private:
    HistoryScrollData* d;
};

#endif // TEHISTORY_H
