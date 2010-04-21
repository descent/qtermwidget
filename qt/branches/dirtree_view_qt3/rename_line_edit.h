#ifndef RENAME_LINE_EDIT_H
#define RENAME_LINE_EDIT_H

#include <qlineedit.h>

class RenameLineEdit : public QLineEdit
{
  Q_OBJECT
    
  public:
    RenameLineEdit( QWidget * parent = 0, const char * name = 0 );

  signals:
    void lost_focus();

  protected:
    void focusOutEvent( QFocusEvent * );
};

#endif
