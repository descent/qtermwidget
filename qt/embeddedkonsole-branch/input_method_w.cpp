#include "input_method_w.h"
#include <QHBoxLayout>

DS::InputMethodW::InputMethodW(QWidget *parent)
{
  input_method_text_ = new QLabel("label", this);
  QHBoxLayout *layout = new QHBoxLayout;
  setLayout(layout);
}
