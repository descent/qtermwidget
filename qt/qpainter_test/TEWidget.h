#ifndef TE_WIDGET_H
#define TE_WIDGET_H

#include <QFrame>

using namespace Qt;

class TEWidget : public QFrame
// a widget representing attributed text
{ Q_OBJECT

//  friend class Konsole;

public:

    TEWidget(QWidget *parent=0);
    virtual ~TEWidget(){}

protected:
     void paintEvent( QPaintEvent * );


};

#endif // TE_WIDGET_H
