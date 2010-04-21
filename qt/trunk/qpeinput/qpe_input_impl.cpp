/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: qpe_input_impl.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: QPE input method ªº¨ó©w
 */

#include <qpixmap.h>
#include <qapplication.h>

#include "qpe_input_impl.h"
#include "qpe_input_w.h"

#include "im_png.h"

QPEInputMethodImpl::QPEInputMethodImpl():input(0),icn(0),ref(0)
{
}

QPEInputMethodImpl::~QPEInputMethodImpl()
{
 delete input;
 delete icn;
}


QWidget *QPEInputMethodImpl::inputMethod(QWidget *parent,Qt::WFlags f )
{
 if ( !input )
  input = new QPEInputMethod( parent, "QPEInputMethodImpl", f );
 return input;
}

QString QPEInputMethodImpl::name()
{
 return qApp->translate( "InputMethods", "QPEInputMethodImpl" );
}


QPixmap *QPEInputMethodImpl::icon()
{
 if ( !icn )
  icn = new QPixmap(qembed_findData("k.png"));
 return icn;
}

void QPEInputMethodImpl::resetState()
{
 //input->hide(); 
}

void QPEInputMethodImpl::onKeyPress( QObject *receiver, const char *slot )
{
 if ( input )
  QObject::connect( input, SIGNAL(keyPress(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
QRESULT QPEInputMethodImpl::queryInterface( const QUuid &uuid, QUnknownInterface** iface)

{
 *iface = 0;
 if ( uuid == IID_QUnknown )
  *iface = this;
 else 
  if ( uuid == IID_InputMethod )
   *iface = this;

 if ( *iface )
  (*iface)->addRef();
 return QS_OK;
}
Q_EXPORT_INTERFACE()
{
 Q_CREATE_INSTANCE( QPEInputMethodImpl )
}
#endif

