/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: input_handler.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: 所有的輸入法 class 的 base class
 */


#include <qapplication.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h> 

#include "input_handler.h"
#include "qpe_input_w.h"
#include "im_png.h"

//#define QT_VERSION 232

namespace
{

 DS::Key keys[]=
 {
  {DS::CHANGE_INPUT_METHOD,2,1}, // 改變輸入法的按鍵
  {'`',2,1},
  {'1',2,1},
  {'2',2,1},
  {'3',2,1},
  {'4',2,1},
  {'5',2,1},
  {'6',2,1},
  {'7',2,1},
  {'8',2,1},
  {'9',2,1},
  {'0',2,1},
  {'-',2,1},
  {'=',2,1},
  {DS::BACKSPACE,2,1},// <- symbol


  {DS::TAB,3,1},
  {'q',2,1},
  {'w',2,1},
  {'e',2,1},
  {'r',2,1},
  {'t',2,1},
  {'y',2,1},
  {'u',2,1},
  {'i',2,1},
  {'o',2,1},
  {'p',2,1},
  {'[',2,1},
  {']',2,1},
  {'\\',3,1},

  {DS::CAPS_LOCK,4,1},
  {'a',2,1},
  {'s',2,1},
  {'d',2,1},
  {'f',2,1},
  {'g',2,1},
  {'h',2,1},
  {'j',2,1},
  {'k',2,1},
  {'l',2,1},
  {';',2,1},
  {'\'',2,1},
  {DS::ENTER,4,1},

  {DS::SHIFT,5,1},
  {'z',2,1},
  {'x',2,1},
  {'c',2,1},
  {'v',2,1},
  {'b',2,1},
  {'n',2,1},
  {'m',2,1},
  {',',2,1},
  {'.',2,1},
  {'/',2,1},
  {DS::SHIFT,5,1},

  {DS::SPACE,30,1},

 };
}

void DS::BaseInput::change_size()
{
}

DS::BaseInput::BaseInput(QPEInputMethod *parent):_parent(parent)
{
 // 設定第一列到第五列的按鍵
 for (int i=0 ; i < 15 ; ++i)
 {
  _lines[0].push_back(&keys[i]);
  //qDebug("_ch : %s",keys[i]._ch);
 }
 for (int i=0+15 ; i < 15+14 ; ++i)
  _lines[1].push_back(&keys[i]);
 for (int i=0+15+14 ; i < 15+14+13 ; ++i)
  _lines[2].push_back(&keys[i]);
 for (int i=0+15+14+13 ; i < 15+14+13+12 ; ++i)
  _lines[3].push_back(&keys[i]);

 _lines[4].push_back(&keys[15+14+13+12]); // Space key

 _keyboard_mode=NORMAL;
 // 這是 member function pointer
 _mfp[NORMAL]=&DS::BaseInput::normal_mode;
 _mfp[CAPS]=&DS::BaseInput::caps_mode;
 _mfp[SHIFT]=&DS::BaseInput::shift_mode;
 //int a=0;
 // member function point 的用法
 //(this->*(_mfp[NORMAL]))(9);
}

int DS::BaseInput::normal_mode(int c)
{
 return c;
}
int DS::BaseInput::caps_mode(int c)
{
 // 將英文字母轉大寫
 return toupper(c);
}

int DS::BaseInput::shift_mode(int c)
{
	/*
 std::string bs[]={"`","1","2","3","4","5","6","7","8","9","0","-","=","[","]","\\",";","\'",",",".","/"}; // bs is before shift
 std::string as[]={"~","!","@","#","$","%","^","&","*","(",")","_","+","{","}","|",":","\"","<",">","?"}; // as is after shift
 */
 int bs[]={'`','1','2','3','4','5','6','7','8','9','0','-','=','[',']','\\',';','\'',',','.','/'}; // bs is before shift
 int as[]={'~','!','@','#','$','%','^','&','*','(',')','_','+','{','}','|',':','\"','<','>','?'}; // as is after shift
 std::map <int,int> shift_map; // 當 shift 被按下之後要傳回的按鍵
 for (size_t i=0 ; i < sizeof(bs) /sizeof(int) ; ++i)
  shift_map.insert(std::make_pair(bs[i],as[i]));
 std::map <int,int> :: iterator it=shift_map.find(c);
 if (it == shift_map.end()) // not found
 {
  return toupper(c);
 }
 else
 {
  qDebug("c : %d",c);
  qDebug("it->first : %d",it->first);
  qDebug("it->second : %d",it->second);
  return it->second;
 }
}

void DS::BaseInput::get_key_matrix(const DS::Key *key,int &w,int &h)
{
 w=get_parent()->key_unit_width() * key->_w_unit;
 h=get_parent()->key_unit_height()* key->_h_unit;
}

DS::EnglishInput::EnglishInput(QPEInputMethod *parent):BaseInput(parent)
{
 set_capital(false); // 將按鍵初始化為小寫字母
 //set_capital(true); // 將按鍵初始化為大寫字母

}


void DS::EnglishInput::draw_keys()
{
 //get_parent()->resize(get_parent()->width(),50);
 //qDebug("resize h : %d", get_parent()->height());
 //get_parent()->set_row(6);
 //get_parent()->resize(get_parent()->width(),50);
 //get_parent()->move(100,100);
 //get_parent()->show();
 int x,y,w,h;
 w=h=x=y=0;
 // keyboard 總共有 KEYBOARD_ROWS 這麼多行

 // 畫橫線
 get_key_matrix(_lines[0][0],w,h);// 為了抓 w,h
 get_parent()->draw_line(0,y,15*w,y);
 for (int i=0 ; i < KEYBOARD_ROWS ; ++i)
 {
  // 畫橫線
  get_key_matrix(_lines[0][0],w,h);// 為了抓 w,h
  get_parent()->draw_line(0,y+h,15*w,y+h);

  // 畫出每一行的 key
  for (Line_Iter iter=_lines[i].begin() ; iter!=_lines[i].end() ; ++iter)
  {
   //++iter;
   get_key_matrix(*iter,w,h);
   //get_parent()->draw_text(x,y,*iter);
   if (((*iter)->_ch) == DS::CHANGE_INPUT_METHOD)
   {
    //get_parent()->draw_icon(x,y,w,h,new QPixmap("keyboard_icon/en.png"));
    get_parent()->draw_icon(x,y,w,h,&QPixmap(qembed_findData("en.png")));
   }
   else
   {
    get_parent()->draw_text(x,y,w,h,get_string( (this->*(_mfp[_keyboard_mode]))((*iter)->_ch)));
   }
   x+=w;
   // 畫直線
   get_parent()->draw_line(x,y,x,y+h);
  }
  y+=h;
  x=0;
 }
 // 畫橫線
 //get_key_matrix(_lines[0][0],w,h);// 為了抓 w,h
 //get_parent()->draw_line(0,y+h,15*w,y+h);
}

DS::QtKey DS::EnglishInput::do_with_pos(int x,int y) // 處理被點選的座標位置
{
 int cell_x=0,cell_y;

 // 目前 cell height 均為 1 單位
 // 算出目前在第幾列
 cell_y = y / (get_parent()->key_unit_height() * 1); 
 //qDebug("%d line",cell_y);
 Line_Iter iter;
 for (iter=_lines[cell_y].begin() ; x > 0 ; ++iter)
 {
  //qDebug("_w_unit : %d",(*iter)->_w_unit);
  x-=((*iter)->_w_unit * get_parent()->key_unit_width());
  ++cell_x;
 }
 //qDebug("%d col",cell_x);
 --iter;
 DS::QtKey qt_key;
 //qDebug("(*iter)->_ch : %d",(*iter)->_ch);
 switch ((*iter)->_ch)
 {
  case DS::CAPS_LOCK:
  {
   if (_keyboard_mode==CAPS)
    _keyboard_mode=NORMAL;
   else
    _keyboard_mode=CAPS;
   qt_key._unicode=DS::NOP;
   qt_key._qt_keycode=DS::NOP;
   //emit get_parent()->repaint();
   emit get_parent()->update();
   return qt_key;
   break;
  }
  case DS::SHIFT:
  {
   _keyboard_mode=SHIFT;
   qt_key._unicode=DS::NOP;
   qt_key._qt_keycode=DS::NOP;
   //emit get_parent()->repaint();
   emit get_parent()->update();
   return qt_key;
   break;
  }
  /*
  default:
  {
   _keyboard_mode=NORMAL;
   qt_key._unicode=DS::NOP;
   qt_key._qt_keycode=DS::NOP;
   emit get_parent()->repaint();
   return qt_key;
   break;
  }
  */

 }
 qt_key=get_qt_key( (this->*(_mfp[_keyboard_mode]))((*iter)->_ch) );
 if (_keyboard_mode==SHIFT)
 {
  set_keyboard_mode(NORMAL);
  //emit get_parent()->repaint();
  emit get_parent()->update();
 }

 //qDebug("(this->*(_mfp[_keyboard_mode]))((*iter)->_ch) : %c" ,(this->*(_mfp[_keyboard_mode]))((*iter)->_ch));
 return qt_key;
 /*
 if (_capital==true)
  return get_qt_key(toupper((*iter)->_ch));
 else
  return get_qt_key((*iter)->_ch);
  */
 //QString q_str=get_qstring((*iter)->_ch);
 //qDebug("You press %s",q_str.latin1());
}

void DS::EnglishInput::change_size()
{
 //DS::BaseInput::change_size();

 qDebug("in DS::EnglishInput::change_size()");
	/*
 if (get_parent()->isVisible())
  get_parent()->hide();
 */
 get_parent()->set_row(5);
 /*
 int height = min( get_parent()->sizeHint().height(), 134 );
 get_parent()->resize( qApp->desktop()->width()/2, height );
 get_parent()->move( 0, 621 - height );
 */
 //get_parent()->show();
} 
