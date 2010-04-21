/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: input_handler.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: 所有的輸入法 class 的 base class, 以及英數輸入的功能
 */

#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <vector>
#include <map>
#include <string>

#include <qstring.h>
#include <qnamespace.h>
#include <qtextcodec.h>

#include <cctype> // for toupper

//#include "qpe_input_w.h"

class QPEInputMethod;

namespace DS
{
 // NOP 表示該按鍵沒反應
 enum KeyKind {NOP=-2,CHANGE_INPUT_METHOD=-1,TAB,CAPS_LOCK,BACKSPACE,ENTER,SHIFT,SPACE,CHAR};
 struct Key
 {
  //const char * _ch;
  const int _ch;
  //int _send_key; // 要送到 QT/E AP 的按鍵值，這要使用 unicode 的值 ex:a = 0x65
  const int _w_unit;
  const int _h_unit;
 };

 typedef std::vector<Key *> Line;
 typedef std::vector<Key *>::iterator Line_Iter;

 // 要送出一個 QT key 要有以下的資料
 struct QtKey
 {
  int _unicode;
  int _qt_keycode;
 };

 class BaseInput
 {
  protected:
   enum KeyBoard{KEYBOARD_ROWS=5};
   enum KeyboardMode {NORMAL=0,CAPS,SHIFT,LAST};
   KeyboardMode _keyboard_mode;
   Line _lines[KEYBOARD_ROWS];
   // 由於每一個輸入法的 widget 大小不同, 所以藉由此 member function 來改變 widget 大小
  public:
   int normal_mode(int c);
   int caps_mode(int c);
   int shift_mode(int c);
   typedef int (BaseInput::*MFp)(int c);
   MFp _mfp[LAST];
   virtual void change_size();
   virtual void draw_keys()=0;
   virtual DS::QtKey do_with_pos(int x,int y)=0; // 處理被點選的座標位置
   BaseInput(QPEInputMethod *parent);
   virtual ~BaseInput(){}
   void get_key_matrix(const DS::Key *key,int &w,int &h);

   DS::QtKey get_qt_key(int ch)
   {
    QtKey qt_key;
    switch (ch)
    {
     case DS::CHANGE_INPUT_METHOD:
     {
      qt_key._unicode=0;
      qt_key._qt_keycode=DS::CHANGE_INPUT_METHOD;
      return qt_key;
     }
     case DS::BACKSPACE:
     {
      qt_key._unicode=0;
      qt_key._qt_keycode=Qt::Key_Backspace;
      return qt_key;
     }
     case DS::TAB:
     {
      qt_key._unicode=9;
      qt_key._qt_keycode=Qt::Key_Tab;
      return qt_key;
     }
     case DS::CAPS_LOCK:
     {
      qt_key._unicode=0;
      qt_key._qt_keycode=DS::CAPS_LOCK;
      return qt_key;
     }
     case DS::ENTER:
     {
      qt_key._unicode=0;
      qt_key._qt_keycode=Qt::Key_Enter;
      return qt_key;
     }
     case DS::SHIFT:
     {
      qt_key._unicode=0;
      qt_key._qt_keycode=DS::SHIFT;
      return qt_key;
     }
     case DS::SPACE:
     {
      qt_key._unicode=0x20; // 空白的 unicode ' '
      qt_key._qt_keycode=0;
      return qt_key;
     }
     default: // 處理 ASCII 按鍵
     {
      qt_key._unicode=ch;
      qt_key._qt_keycode=0;
      return qt_key;
     }
    }
   }

   std::string get_string(int ch,KeyboardMode keyboard_mode=NORMAL)
   {
    switch (ch)
    {
     case DS::CHANGE_INPUT_METHOD:
     {
      return std::string("CH");
      break;
     }
     case DS::BACKSPACE:
     {
      return std::string("<-");
      break;
     }
     case DS::TAB:
     {
      return std::string("Tab");
      break;
     }
     case DS::CAPS_LOCK:
     {
      return std::string("Caps");
      break;
     }
     case DS::ENTER:
     {
      return std::string("Enter");
      break;
     }
     case DS::SHIFT:
     {
      return std::string("Shift");
      break;
     }
     case DS::SPACE:
     {
      return std::string("SPACE");
      break;
     }
     default:
     {
      if (keyboard_mode==CAPS)
       return std::string(1,static_cast<char>(toupper(ch)));
      return std::string(1,static_cast<char>(ch));
     }
    }
   } // end get_string

   QString get_qstring(int ch)
   {
    switch (ch)
    {
     case DS::CHANGE_INPUT_METHOD:
     {
      return QString("CH");
      break;
     }
     case DS::BACKSPACE:
     {
      return QString("<-");
      break;
     }
     case DS::TAB:
     {
      return QString("Tab");
      break;
     }
     case DS::CAPS_LOCK:
     {
      return QString("Caps");
      break;
     }
     case DS::ENTER:
     {
      return QString("Enter");
      break;
     }
     case DS::SHIFT:
     {
      return QString("Shift");
      break;
     }
     case DS::SPACE:
     {
      return QString("SPACE");
      break;
     }
     default:
      return QChar(ch);
    }
   }

   QPEInputMethod *get_parent()
   {
    return _parent;
   }
   virtual void reverse_case() // 將大寫轉小寫，小寫轉大寫
   {
   }
   void set_keyboard_mode(KeyboardMode keyboard_mode)
   {
    _keyboard_mode=keyboard_mode;
   }
  private:
   QPEInputMethod *_parent;

 };

 class EnglishInput : public BaseInput
 {
  public:
   void change_size();
   EnglishInput(QPEInputMethod *parent);
   virtual void draw_keys();
   virtual DS::QtKey do_with_pos(int x,int y); // 處理被點選的座標位置
   void set_capital(bool f){_capital=f;}
   void reverse_case() // 將大寫轉小寫，小寫轉大寫
   {
    set_capital(!_capital);
   }
  private: 
   bool _capital; // 若設為 true 表示秀出大寫英文字母

 };

} // end namespace DS


#endif

