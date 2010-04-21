/*
 * This is input method widget for display input info
 *
 */
#ifndef INPUT_METHOD_W_H
#define INPUT_METHOD_W_H

#include <QLabel>

using namespace Qt;

namespace DS
{
  class InputMethodW : public QWidget
  {
    Q_OBJECT
    public:
      InputMethodW(QWidget *parent);
      void clear()
      {
        input_method_text_->clear();
      }
      void set_text(const QString &str)
      {
        input_method_text_->setText(str);
	//fm.width(str);
        //input_method_text_->resize();
      }
    private:
      QLabel *input_method_text_;
  };

} // end namespace DS


#endif
