/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: etphonetic_input.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: 提供倚天注音的輸入方法
 */

#include <algorithm>
//#include <iostream>

#include <qtextcodec.h>
#include <qapplication.h>

#include "etphonetic_input.h"
#include "qpe_input_w.h" // nOo include
#include "im_png.h"
#include "big5str_to_qstr.h"

using namespace std;

namespace 
{
 int english[]={'1','q','a','z','2','w','s','x','e','d','c','r','f','v','5','t','g','b','y','h','n','u','j','m','8','i','k',',','9','o','l','.','0','p',';','/','-','7','6','3','4'};
 std::string phonetic[]={"ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ","ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄧ","ㄨ","ㄩ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ","ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","˙","ˊ","ˇ","ˋ"};
 int kind[]={};
}

DS::EtPhoneticInput::EtPhoneticInput(QPEInputMethod *parent,const std::string &fn):BaseInput(parent),_etphonetic(fn)
{
 get_parent()->set_row(6);
 get_parent()->set_col(15);
 for (size_t i=0 ; i < sizeof(english) / sizeof(int) ; i++)
  _english_to_phonetic41.insert(make_pair(english[i],phonetic[i]));
 _max_paragraph=_paragraph=0;
 _draw_select_area=_draw_keys=_draw_composite_area=true;
}

void DS::EtPhoneticInput::draw_keys()
{
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
   //++iter;
   if (((*iter)->_ch) == DS::CHANGE_INPUT_METHOD)
   {
    //get_parent()->draw_icon(x,y+h,w,h,new QPixmap("keyboard_icon/ph.png"));
    get_parent()->draw_icon(x,y+h,w,h,&QPixmap(qembed_findData("ph.png")));
   }
   else
   {
    //get_parent()->draw_text(x,y,*iter);
    QString q_str=english_to_phonetic41((*iter)->_ch);
    //qDebug("phonetic : %s",q_str.latin1());
    //q_str=get_qstring((*iter)->_ch);
    get_parent()->draw_text(x,y+h,w,h,q_str);
    //get_parent()->draw_text(x,y,w,h,q_str);
   }

   x+=w;
   // 畫直線
   get_parent()->draw_line(x,y+h,x,y+h+h);
  }
  y+=h;
  x=0;

 }
 //get_parent()->show();
 // 畫出選字區及組字區
 draw_composite_area();
 //if (_draw_select_area==true)
 draw_select_area(_paragraph); // 畫出選字區
}

void DS::EtPhoneticInput::draw_composite_area() // 畫出組字區
{
 int w,h;
 get_key_matrix(_lines[0][0],w,h);
 int x,y;
 x=y=0;
 // 目前有那些注音被輸入
 std::vector<std::string> :: iterator it=_composite_words.begin(); 
 for ( ; it != _composite_words.end() ; ++it)
 {
  get_parent()->draw_text(x,y,w,h,*it);
  x+=w;
 }
 
 /*
 for (int i=0 ; i<4 ; i++)
 {
  //_phonetic[i]="a";
  get_parent()->draw_text(x,y,w,h,_phonetic[i]);
  x+=w;
 }
 */
}


void DS::EtPhoneticInput::add_phonetic(int ch)
{
 struct PhoneticKind
 {
  int _ph;
  int kind;
 }; 
 // 這裡考慮搬到 constructor
 std::map<int,int> pk;

 for (int i=0 ; i < 21 ; i++)
  pk.insert(make_pair(english[i],0));

 for (int i=0+21 ; i < 21+3 ; i++)
  pk.insert(make_pair(english[i],1));

 for (int i=0+21+3 ; i < 21+3+13 ; i++)
  pk.insert(make_pair(english[i],2));

 for (int i=0+21+3+13 ; i < 21+3+13+4 ; i++)
  pk.insert(make_pair(english[i],3));

 //qDebug("DS::EtPhoneticInput::add_phonetic()");
 _phonetic[pk[ch]]=english_to_phonetic41(ch);
 //_phonetic[0]="b";


}

DS::QtKey DS::EtPhoneticInput::do_with_pos(int x,int y) // 處理被點選的座標位置
{
 int cell_x=0,cell_y;

  // 目前 cell height 均為 1 單位
  // 算出目前在第幾列
  cell_y = y / (get_parent()->key_unit_height() * 1);
  if (cell_y==0) // status bar
  {
   cell_x = x / (get_parent()->key_unit_width() * 2); // 每一個 key 是 2 個單位
   //qDebug("Cell x : %d",cell_x);
   switch (cell_x)
   {
    case 0: // 組字區
    case 1: // 組字區
    case 2: // 組字區
    case 3: // 組字區
    {
     DS::QtKey qt_key;
     qt_key._unicode=DS::NOP;
     qt_key._qt_keycode=DS::NOP;
     return qt_key;
    }
    case 4: // 組字區的刪除鍵
    {
     _paragraph=0;
     _match_words=_etphonetic.del_phonetic();
     _composite_words=_etphonetic.get_composite_words();
     _max_paragraph=_match_words.size() / MAX_WORDS;
     break;
    }
    case 5: // 選字區顯示上一段符合的字
    {
     if (_paragraph > 0)
      --_paragraph;
     else // _paragraph <= 0 
     {
      // 不用 repaint
      DS::QtKey qt_key;
      qt_key._unicode=DS::NOP;
      qt_key._qt_keycode=DS::NOP;
      return qt_key;
     }
     //draw_select_area(_paragraph); // 畫出選字區
     break;
    }
    case 14: // 選字區顯示下一段符合的字
    {
     ++_paragraph;
     // >= 是必要的條件
     if (_paragraph >= _max_paragraph)
     {
      --_paragraph;
      // 不用 repaint
      DS::QtKey qt_key;
      qt_key._unicode=DS::NOP;
      qt_key._qt_keycode=DS::NOP;
      return qt_key;
     }
     //draw_select_area(_paragraph); // 畫出選字區
     break;
    }
    default: // 送出所選擇的字
    {
     //qDebug("send a chinese word");
     DS::QtKey qt_key;
     if (_match_words.size()!=0)
     { // 送出所選的字
      QString s=get_select_word(_paragraph,cell_x-6);
      //qDebug("you select : %s",s.latin1());
      int unicode=s[0].unicode();
      //int qkeycode=0;
      //int modifiers=0;
      qt_key._unicode=unicode;
      qt_key._qt_keycode=DS::CHAR;
      _etphonetic.del_all_ph();
      _composite_words=_etphonetic.get_composite_words();
      _match_words.clear();
      //emit get_parent()->repaint();
      //emit get_parent()->update();
      emit get_parent()->update(calc_status_bar());
     }
     else
     {
      qt_key._unicode=DS::NOP;
      qt_key._qt_keycode=DS::NOP;
     }
     return qt_key;
    }
   }
   //emit get_parent()->repaint();
   emit get_parent()->update(calc_status_bar());
   DS::QtKey qt_key;
   qt_key._unicode=DS::NOP;
   qt_key._qt_keycode=DS::NOP;
   return qt_key;
  }
  // return qt_key; 	    
  //qDebug("%d line",cell_y);
  Line_Iter iter;
  // cell_y-1 減 1 的原因是因為第一列是空的
  for (iter=_lines[cell_y-1].begin() ; x > 0 ; ++iter)
  {
   //qDebug("_w_unit : %d",(*iter)->_w_unit);
   x-=((*iter)->_w_unit * get_parent()->key_unit_width());
   ++cell_x;
  }
  --iter;
 //qDebug("You press %c",(*iter)->_ch);
 // 點在特殊按鍵以及選字區才送出 qt_key
 // 點在特殊按鍵可以呼叫 BaseInput :: get_qt_key 來送出按鍵
 switch ((*iter)->_ch) // 特殊按鍵
 {
  case DS::CHANGE_INPUT_METHOD:
  case DS::BACKSPACE:
  case DS::TAB:
  case DS::CAPS_LOCK:
  case DS::ENTER:
  case DS::SHIFT:
  case DS::SPACE:
  // 若不是注音按鍵直接送出該英文按鍵
  // 以下的按鍵並沒有對應到注音, 所以直接送出按鍵值
  case '~':
  case '=':
  case '[':
  case ']':
  case '\\':
  case '\'':
  {
   return get_qt_key((*iter)->_ch);
  }
  default: // 注音按鍵
  {
   _paragraph=0;
   // 處理選字區和組字區
   add_phonetic((*iter)->_ch);
   // std::string(1,(char)(*iter)->_ch) 這種 constructor 有點怪
   //_etphonetic << english_to_phonetic41_string((*iter)->_ch);
   _match_words=_etphonetic.add_phonetic(english_to_phonetic41_string((*iter)->_ch));
   // 抓出組字區所需要的字
   _composite_words=_etphonetic.get_composite_words();
   //_match_words=_etphonetic.get_match_words();
   _max_paragraph=_match_words.size() / MAX_WORDS;
   /*
   std::vector<DS::Big5Code> :: iterator it;
   for (it=_match_words.begin() ; it != _match_words.end() ; ++it)
   {
    cout << it->hi() << it->lo() << endl;
   }
   */

   //qDebug("in draw_composite_area();");
   //emit get_parent()->repaint();


   // 只要 update status bar 即可, 避免閃爍
   emit get_parent()->update(calc_status_bar());
   DS::QtKey qt_key;
   qt_key._unicode=DS::NOP;
   qt_key._qt_keycode=DS::NOP;
   return qt_key;
  }
 }
}

QRect DS::EtPhoneticInput::calc_status_bar() // 計算 status bar 的大小
{
 int w,h;
 get_key_matrix(_lines[0][0],w,h);// 為了抓 w,h
 return QRect (0,0,15*w,h);
}

// 將英數按鍵對應到大千注音
QString DS::EtPhoneticInput::english_to_phonetic41(int ch) 
{
 std::map<int,std::string> :: iterator it = _english_to_phonetic41.find(ch);
 if (it==_english_to_phonetic41.end()) // 找不到
 {
  return get_qstring(ch);
 }
 else
 {
  QString qstr;
  DS::big5str_to_qstr(it->second, qstr);
  return qstr;

  //QTextCodec *codec=QTextCodec::codecForName("big5");
  //return QString(codec->toUnicode((it->second).c_str()));
  
 }
}

void DS::EtPhoneticInput::change_size()
{

 qDebug("in DS::EtPhoneticInput::change_size()");
 //get_parent()->hide();
 // set_row() 會間接改到 sizeHint()。
 get_parent()->set_row(6);
}

std::string DS::EtPhoneticInput::english_to_phonetic41_string(int ch) // 將英數按鍵對應到大千注音
{
 std::map<int,std::string> :: iterator it = _english_to_phonetic41.find(ch);
 if (it==_english_to_phonetic41.end()) // 找不到
 {
  return get_string(ch);
 }
 else
 {
  return it->second;
  
 }
}

void DS::EtPhoneticInput::draw_select_area() // 畫出選字區
{
 // 目前最多一次畫出 8 個符合的字
 int begin=0,len=_match_words.size();
 len < MAX_WORDS ? (len) : (len=MAX_WORDS);
 int w,h;
 get_key_matrix(_lines[0][0],w,h);
 int x=6*w,y=0;
 for (int i=begin ; i < len ; i++)
 {
  char big5_ch[]="宋";
  big5_ch[0]=_match_words[i].hi();
  big5_ch[1]=_match_words[i].lo();

  //QTextCodec *codec=QTextCodec::codecForName("big5");
  //QString qs(codec->toUnicode(big5_ch));
  QString qs;
  DS::big5str_to_qstr(big5_ch,qs);
  get_parent()->draw_text(x,y,w,h,qs);
  x+=w;
 }
}

void DS::EtPhoneticInput::draw_select_area(int paragraph) // 畫出選字區
{
 //const size_t MAX_WORDS=8;	
 size_t begin=MAX_WORDS*paragraph,len=MAX_WORDS*(paragraph+1);
 // begin 的範圍也要檢查
 if (begin >= _match_words.size())
 {
  return;
 }

 _match_words.size() < len ? (len=_match_words.size()) : (len);
 int w,h;
 get_key_matrix(_lines[0][0],w,h);
 int x=6*w,y=0;
 for (size_t i=begin ; i < len ; i++)
 {
  char big5_ch[]="宋";
  big5_ch[0]=_match_words[i].hi();
  big5_ch[1]=_match_words[i].lo();

  //QTextCodec *codec=QTextCodec::codecForName("big5");
  //QString qs(codec->toUnicode(big5_ch));
  QString qs;
  DS::big5str_to_qstr(big5_ch,qs);
  get_parent()->draw_text(x,y,w,h,qs);
  x+=w;
 }
}

// 抓所選擇的字
QString DS::EtPhoneticInput::get_select_word(int paragraph,int index)
{
 size_t pos=MAX_WORDS*paragraph+index;

 char big5_ch[]="宋";
 big5_ch[0]=_match_words[pos].hi();
 big5_ch[1]=_match_words[pos].lo();

 QString qs;
 DS::big5str_to_qstr(big5_ch,qs);
 return qs;
}
