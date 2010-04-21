#ifndef SIMPLE_WIDGET_IM_H 
#define SIMPLE_WIDGET_IM_H 

 #include <qpe/inputmethodinterface.h>

 class SimpleInputMethod;
 class QPEInputMethod;
 class QPixmap;

 class SimpleInputMethodImpl : public InputMethodInterface
 {
  public:
  SimpleInputMethodImpl();
  virtual ~SimpleInputMethodImpl();

  #ifndef QT_NO_COMPONENT
   QRESULT queryInterface( const QUuid&, QUnknownInterface** );
   Q_REFCOUNT
  #endif

  virtual QWidget *inputMethod( QWidget *parent, Qt::WFlags f );
  virtual void resetState();
  virtual QPixmap *icon();
  virtual QString name();
  virtual void onKeyPress( QObject *receiver, const char *slot );

  private:
   //SimpleInputMethod *input;
   QPEInputMethod *input;
   QPixmap *icn;
   ulong ref;
 }; // end class SimpleInputMethodImpl

#endif
