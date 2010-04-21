/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Id: etchangjei_input.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * function: 倚天倉頡輸入法在 QPE input method 的 class
 */


#include "etchangjei_input.h"

#include <qpixmap.h>

#include <algorithm>

#include "big5str_to_qstr.h"
#include "im_png.h"

namespace
{
 int english[]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
 std::string changjei[]={"日","月","金","木","水","火","土","竹","戈","十","大","中","一","弓","人","心","手","口","尸","廿","山","女","田","難","卜","重"};
}

DS::EtChangjeiInput::EtChangjeiInput(QPEInputMethod *parent,const std::string &fn):BaseInput(parent),_et_changjei(fn)
//DS::EtChangjeiInput::EtChangjeiInput(QPEInputMethod *parent):BaseInput(parent)
{
 for (size_t i=0 ; i < sizeof(english) / sizeof(int) ; ++i)
 {
  _english_to_changjei.insert(std::make_pair(english[i],changjei[i]));
 }
 _paragraph=_max_paragraph=0;
}
void DS::EtChangjeiInput::draw_keys()
{
 //qDebug("in EtChangjeiInput::draw_keys()");
 int x,y,w,h;
 w=h=x=y=0;
 // keyboard 總共有 KEYBOARD_ROWS 這麼多行
 for (int i=0 ; i < KEYBOARD_ROWS ; ++i)
 {
  // 畫橫線
  get_key_matrix(_lines[0][0],w,h);// 為了抓 w,h
  get_parent()->draw_line(0,y+h,15*w,y+h);
  // 畫出每一行的 key
  for (Line_Iter iter=_lines[i].begin() ; iter!=_lines[i].end() ; ++iter)
  {
   get_key_matrix(*iter,w,h);
   if (((*iter)->_ch) == DS::CHANGE_INPUT_METHOD)
   {
    //get_parent()->draw_icon(x,y+h,w,h,QPixmap("keyboard_icon/c0.png"));
    get_parent()->draw_icon(x,y+h,w,h,&QPixmap(qembed_findData("c0.png")));
   }
   else
   {
    std::string str=english_to_changjei((*iter)->_ch);
    get_parent()->draw_text(x,y+h,w,h, str);
   }
   x+=w;
   // 畫直線
   get_parent()->draw_line(x,y+h,x,y+h+h);
  } // end for (Line_Iter iter=_lines[i].begin() ; iter!=_lines[i].end() ; ++iter)
  y+=h;
  x=0;
 } // end for (int i=0 ; i < KEYBOARD_ROWS ; ++i)
 draw_composite_area();
 draw_select_area(_match_words);
}

void print_it (DS::Big5Code &big5_code)
{
 //cout << big5_code.big5_string() << endl;
}

DS::QtKey DS::EtChangjeiInput::do_with_pos(int x,int y)
{
 int cell_x=0,cell_y;

 // 目前 cell height 均為 1 單位
 // 算出目前在第幾列
 cell_y = y / (get_parent()->key_unit_height() * 1);
 if (cell_y==0) // 組字區刪除倉頡字根的功能
 {
  cell_x = x / (get_parent()->key_unit_width() * 2); // 每一個 key 是 2 個單位
  //qDebug("Cell x : %d",cell_x);
  switch (cell_x)
  {
   case 0: // 組字區
   case 1: // 組字區
   case 2: // 組字區
   case 3: // 組字區
   case 4: // 組字區
   {
    DS::QtKey qt_key;
    qt_key._unicode=DS::NOP;
    qt_key._qt_keycode=DS::NOP;
    return qt_key;
   }
   case 5: // 組字區的刪除鍵
   {
    _match_words=_et_changjei.del_changjei();
    std::vector<std::string> composite_words=_et_changjei.get_composite_words();
    _composite_words=composite_words;
    break;
   }
   case 6: // 選字區顯示上一段符合的字
   {
    if (_paragraph > 0)
     --_paragraph;
    break;
   }
   case 14: // 選字區顯示下一段符合的字
   {
    ++_paragraph;
    if (_paragraph > _max_paragraph)
     --_paragraph;
    break;
   }
   default: // 送出所選擇的字
   {
  //qDebug("send a chinese word");
    DS::QtKey qt_key;
    if (_match_words.size()!=0)
    {
     QString s=get_select_word(_paragraph,cell_x-7);
               //qDebug("you select : %s",s.latin1());
     int unicode=s[0].unicode();
     int qkeycode=0;
     int modifiers=0;
     qt_key._unicode=unicode;
     qt_key._qt_keycode=DS::CHAR;
     //emit get_parent()->keyPress( unicode, qkeycode, modifiers, true, false );
     _et_changjei.del_all_changjei();
     _composite_words=_et_changjei.get_composite_words();
     _match_words.clear();
     //emit get_parent()->repaint();
    // emit get_parent()->update();
     emit get_parent()->update(calc_status_bar());
    }
    else
    {
     qt_key._unicode=DS::NOP;
     qt_key._qt_keycode=DS::NOP;
    }
    return qt_key;
   }
  } // end switch
   //qDebug("in changjei status bar");
   //emit get_parent()->repaint();
    // emit get_parent()->update();
   emit get_parent()->update(calc_status_bar());
   DS::QtKey qt_key;
   qt_key._unicode=DS::NOP;
   qt_key._qt_keycode=DS::NOP;
   return qt_key;
  }

  Line_Iter iter;
  // cell_y-1 減 1 的原因是因為第一列是空的
  for (iter=_lines[cell_y-1].begin() ; x > 0 ; ++iter)
  {
   //qDebug("_w_unit : %d",(*iter)->_w_unit);
   x-=((*iter)->_w_unit * get_parent()->key_unit_width());
   ++cell_x;
  }
  --iter;
  switch ((*iter)->_ch) // 特殊按鍵
  {
   case DS::CHANGE_INPUT_METHOD:
   case DS::BACKSPACE:
   case DS::TAB:
   case DS::CAPS_LOCK:
   case DS::ENTER:
   case DS::SHIFT:
   case DS::SPACE:
   // 若不是倉頡字根按鍵直接送出該英文按鍵
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
   case '0':
   case '-':
   case '~':
   case '=':
   case '[':
   case ']':
   case '\\':
   case ';':
   case '\'':
   case ',':
   case '.':
   case '/':
   {
    return get_qt_key((*iter)->_ch);
   }
   default: // 倉頡字根按鍵
   {
    //_match_words.clear();
    _match_words=_et_changjei.add_changjei(english_to_changjei((*iter)->_ch));
    _composite_words=_et_changjei.get_composite_words();
    _paragraph = _match_words.size() / MAX_WORDS;
    /*
    std::vector<DS::Big5Code> :: iterator it;
    for (it=_match_words.begin() ; it != _match_words.end() ; ++it)
    {
     cout << it->big5_string() << endl;
    }
    */
    //for_each(_match_words.begin(),_match_words.end(),print_it);
    //emit get_parent()->repaint();

    //if (_composite_words.size < 4)
    emit get_parent()->update(calc_status_bar());

    
    DS::QtKey qt_key;
    qt_key._unicode=DS::NOP;
    qt_key._qt_keycode=DS::NOP;
    return qt_key;
   }
  }
}

QRect DS::EtChangjeiInput::calc_status_bar() // 計算 status bar 的大小
{
 int w,h;
 get_key_matrix(_lines[0][0],w,h);// 為了抓 w,h
 return QRect (0,0,15*w,h);
}


// 將英文符號對應到倉頡字根
std::string DS::EtChangjeiInput::english_to_changjei(int ch)
{
 std::map<int,std::string> :: iterator it;
 it=_english_to_changjei.find(ch);
 if (it==_english_to_changjei.end())  // not found
 {
  return get_string(ch);
 }
 else
 {
  return it->second;
 }
}

// 畫出倉頡字根組字區
void DS::EtChangjeiInput::draw_composite_area()
{
 int w,h;
 get_key_matrix(_lines[0][0],w,h);
 int x=0,y=0;

 std::vector<std::string> :: iterator it;
 for(it=_composite_words.begin() ; it!=_composite_words.end() ; ++it)
 {
  get_parent()->draw_text(x,y,w,h, *it);
  x+=w;
 }
}

void DS::EtChangjeiInput::draw_select_area(std::vector<DS::Big5Code> &match_words)
{
 int w,h;
 get_key_matrix(_lines[0][0],w,h);
 int x=7*w,y=0;

 std::vector<DS::Big5Code> :: iterator it;
 for(it=match_words.begin() ; it!=match_words.end() ; ++it)
 {
  get_parent()->draw_text(x,y,w,h,it->big5_string());
  x+=w;
 }
}

QString DS::EtChangjeiInput::get_select_word(int paragraph,int index)
{
  size_t pos = MAX_WORDS * paragraph + index;
  QString qstr;
  DS::big5str_to_qstr(_match_words[pos].big5_string(), qstr); // maybe check return value is fail

 return qstr;
}
