#include "simple_widget_im.h"
//#include "simple_widget.h"

#include "qpe_input_w.h"


#include <qapplication.h>
#include <qpixmap.h>
#include <qtextcodec.h>

//#include "s_a1.h"

SimpleInputMethodImpl::SimpleInputMethodImpl() : input(0), icn(0), ref(0)
{
}

SimpleInputMethodImpl::~SimpleInputMethodImpl()
{
 delete input;
 delete icn;
}


QWidget *SimpleInputMethodImpl::inputMethod( QWidget *parent, Qt::WFlags f )
{
 if ( !input )
  input = new QPEInputMethod( parent, "SimpleInput", f );
 return input;
}


QString SimpleInputMethodImpl::name()
{
 return qApp->translate( "InputMethods", "QPEInput" );
 /*
 QCString str="Â²³æ¿é¤Jªk";
 QTextCodec *codec=QTextCodec::codecForName("Big5");
 QString s=codec->toUnicode(str);
 return s;
 */
}

QPixmap *SimpleInputMethodImpl::icon()
{
 if ( !icn )
  icn = new QPixmap( "s_a.png" );
 return icn;
}
void SimpleInputMethodImpl::resetState()
{
}

void SimpleInputMethodImpl::onKeyPress( QObject *receiver, const char *slot )
{
 if ( input )
  QObject::connect( input, SIGNAL(keyPress(ushort,ushort,ushort,bool,bool)), receiver, slot );
}

#ifndef QT_NO_COMPONENT
//QRESULT SimpleInputMethodImpl::queryInterface( const QUuid &uuid, QUnknownInter face **iface )
QRESULT SimpleInputMethodImpl::queryInterface( const QUuid &uuid, QUnknownInterface** iface)
{
 *iface = 0;
 if ( uuid == IID_QUnknown )
  *iface = this;
 else if ( uuid == IID_InputMethod )
  *iface = this;

 if ( *iface )
  (*iface)->addRef();
  return QS_OK;
}

Q_EXPORT_INTERFACE()
{
 Q_CREATE_INSTANCE( SimpleInputMethodImpl )
}

#endif


