#include "rename_line_edit.h"
RenameLineEdit::RenameLineEdit( QWidget * parent, const char * name ) : QLineEdit( parent, name )
{
}

void RenameLineEdit::focusOutEvent( QFocusEvent * )
{
    emit lost_focus();
}
