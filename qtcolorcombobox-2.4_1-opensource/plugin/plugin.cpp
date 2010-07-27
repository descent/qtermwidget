/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtCore/QtPlugin>

#include "qtcolorcombobox.h"
#include "plugin.h"

QtColorComboBoxPlugin::QtColorComboBoxPlugin(QWidget *parent)
    : QObject(parent)
{
    m_initialized = false;
}

QString QtColorComboBoxPlugin::name() const
{
    return QLatin1String("QtColorComboBox");
}

QWidget *QtColorComboBoxPlugin::createWidget(QWidget *parent)
{
    QtColorComboBox *p = new QtColorComboBox(parent);
    return p;
}

QString QtColorComboBoxPlugin::includeFile() const
{
    return "qtcolorcombobox.h";
}

void QtColorComboBoxPlugin::initialize(QDesignerFormEditorInterface *core)
{
    Q_UNUSED(core);
    if(m_initialized)
        return;
    m_initialized = true;
}

bool QtColorComboBoxPlugin::isContainer() const
{
    return false;
}

bool QtColorComboBoxPlugin::isInitialized() const
{
    return m_initialized;
}

QString QtColorComboBoxPlugin::group() const
{
    return "Qt Solutions Widgets";
}

QString QtColorComboBoxPlugin::toolTip() const
{
    return "A QtColorComboBox widget";
}

QString QtColorComboBoxPlugin::whatsThis() const
{
    return "A QtColorComboBox widget";
}

QIcon QtColorComboBoxPlugin::icon() const
{
    return QIcon();
}

QString QtColorComboBoxPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QtColorComboBox\" name=\"colorcombobox\"></widget>");
}

Q_EXPORT_PLUGIN2(qtcolorcomboboxplugin,QtColorComboBoxPlugin)
