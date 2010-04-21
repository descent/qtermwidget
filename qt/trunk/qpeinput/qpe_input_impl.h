/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: qpe_input_impl.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: QPE input method ����w
 */
#ifndef QPE_INPUT_IMPL_H
#define QPE_INPUT_IMPL_H

#include <qpe/inputmethodinterface.h>

class QPEInputMethod;
class QPixmap;

//namespace DS
//{
 class QPEInputMethodImpl : public InputMethodInterface
 {
  public:
   QPEInputMethodImpl();
   virtual ~QPEInputMethodImpl();
   #ifndef QT_NO_COMPONENT
    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface);
    Q_REFCOUNT
   #endif

   virtual QWidget *inputMethod( QWidget *parent, Qt::WFlags f );
   virtual void resetState();
   virtual QPixmap *icon();
   virtual QString name();
   virtual void onKeyPress( QObject *receiver, const char *slot );
  private:
   QPEInputMethod *input;
   QPixmap *icn;
   ulong ref; // �o���ܼƤ���ç�W�r, compile �|���L, �ڲq�O�M Q_REFCOUNT MACRO ����

 };
//} // end namespace DS


#endif
