/****************************************************************************
** Hello meta object code from reading C++ file 'hello.h'
**
** Created: Tue Jul 27 16:09:04 2004
**      by: The Qt MOC ($Id: qt/src/moc/moc.y   2.3.7   edited 2002-11-27 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "hello.h"
#include <qmetaobject.h>
#include <qapplication.h>
#ifdef QWS
#include <qobjectdict.h>
#endif



const char *Hello::className() const
{
    return "Hello";
}

QMetaObject *Hello::metaObj = 0;

#ifdef QWS
static class Hello_metaObj_Unloader {
public:
    ~Hello_metaObj_Unloader()
    {
         if ( objectDict )
             objectDict->remove( "Hello" );
    }
} Hello_metaObj_unloader;
#endif

void Hello::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(QWidget::className(), "QWidget") != 0 )
	badSuperclassWarning("Hello","QWidget");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString Hello::tr(const char* s)
{
    return qApp->translate( "Hello", s, 0 );
}

QString Hello::tr(const char* s, const char * c)
{
    return qApp->translate( "Hello", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* Hello::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) QWidget::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    typedef void (Hello::*m1_t0)();
    typedef void (QObject::*om1_t0)();
    m1_t0 v1_0 = &Hello::animate;
    om1_t0 ov1_0 = (om1_t0)v1_0;
    QMetaData *slot_tbl = QMetaObject::new_metadata(1);
    QMetaData::Access *slot_tbl_access = QMetaObject::new_metaaccess(1);
    slot_tbl[0].name = "animate()";
    slot_tbl[0].ptr = (QMember)ov1_0;
    slot_tbl_access[0] = QMetaData::Private;
    typedef void (Hello::*m2_t0)();
    typedef void (QObject::*om2_t0)();
    m2_t0 v2_0 = &Hello::clicked;
    om2_t0 ov2_0 = (om2_t0)v2_0;
    QMetaData *signal_tbl = QMetaObject::new_metadata(1);
    signal_tbl[0].name = "clicked()";
    signal_tbl[0].ptr = (QMember)ov2_0;
    metaObj = QMetaObject::new_metaobject(
	"Hello", "QWidget",
	slot_tbl, 1,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}

// SIGNAL clicked
void Hello::clicked()
{
    activate_signal( "clicked()" );
}
