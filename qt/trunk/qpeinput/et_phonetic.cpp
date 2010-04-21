/*
 * $Revision: 1.1.1.1 $
 * $Id: et_phonetic.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * $Author: incoming $
 * function: 倚天注音輸入法
 */

#include "et_phonetic.h"

// debug 用，用來印出某些訊息
//#include <iostream>
#include <algorithm>

using namespace std;

namespace
{
 std::string phonetic[]={"ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ","ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄧ","ㄨ","ㄩ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ","ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","˙","ˊ","ˇ","ˋ"};

 DS::PhoneticKind phonetic_kind[]={DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::MIDS,DS::MIDS,DS::MIDS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::OTHERS,DS::OTHERS,DS::OTHERS,DS::OTHERS};

 // 倚天聲母編碼 1-21
 // 倚天介音編碼 1-3
 // 倚天韻母編碼 1-13
 // 倚天聲符編碼 1-4
 int phonetic_value[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,1,2,3,1,2,3,4,5,6,7,8,9,10,11,12,13,1,2,3,4};
}

DS::ETPhonetic::ETPhonetic(std::string fn):_fn(fn)
{
 _value[0]= _value[1]= _value[2]= _value[3]=0;
 _etphonetic_file.open(_fn.c_str());// 開啟倚天注音參考檔
 if (!_etphonetic_file)
 {
  //std::cerr << "open file error" << endl;
  exit(-1);
 }
 //cout << sizeof(phonetic) / sizeof(std::string) << endl;
 //cout << sizeof(phonetic_kind) / sizeof(DS::PhoneticKind) << endl;
 //cout << sizeof(phonetic_value) / sizeof(int) << endl;
 for (size_t i=0 ; i < sizeof(phonetic_value) / sizeof(int) ; ++i)
  _phonetic_map.insert(std::make_pair(phonetic[i],std::make_pair(phonetic_kind[i],phonetic_value[i])));
}

// 定義大千鍵盤排列的按鍵
DS::Phonetic42keys _ph[]=
{
 // 聲母
 {"ㄅ",0,1},
 {"ㄆ",0,2},
 {"ㄇ",0,3},
 {"ㄈ",0,4},
 {"ㄉ",0,5},
 {"ㄊ",0,6},
 {"ㄋ",0,7},
 {"ㄌ",0,8},
 {"ㄍ",0,9},
 {"ㄎ",0,10},
 {"ㄏ",0,11},
 {"ㄐ",0,12},
 {"ㄑ",0,13},
 {"ㄒ",0,14},
 {"ㄓ",0,15},
 {"ㄔ",0,16},
 {"ㄕ",0,17},
 {"ㄖ",0,18},
 {"ㄗ",0,19},
 {"ㄘ",0,20},
 {"ㄙ",0,21},
 // 介音
 {"一",1,1}, // 這是數字一，不是注音的ㄧ
 {"ㄨ",1,2},
 {"ㄩ",1,3},
 // 韻母
 {"ㄚ",2,1},
 {"ㄛ",2,2},
 {"ㄜ",2,3},
 {"ㄝ",2,4},
 {"ㄞ",2,5},
 {"ㄟ",2,6},
 {"ㄠ",2,7},
 {"ㄡ",2,8},
 {"ㄢ",2,9},
 {"ㄣ",2,10},
 {"ㄤ",2,11},
 {"ㄥ",2,12},
 {"ㄦ",2,13},
 // 聲符
 {" ",3,0},
 {"˙",3,1},
 {"ˊ",3,2},
 {"ˇ",3,3},
 {"ˋ",3,4},
}; // end _ph


DS::ETPhonetic & DS::ETPhonetic::operator<<(const std::string &s)
{
 for (size_t i=0; i < sizeof(_ph) /sizeof(DS::Phonetic42keys) ; i++)
 {
  if (s==_ph[i]._glyph)
  {
   _value[_ph[i]._id]=_ph[i]._value;
   /*
   std::cout << _value[0] << endl;
   std::cout << _value[1] << endl;
   std::cout << _value[2] << endl;
   std::cout << _value[3] << endl;
   */
   break;
  }
 }
 return *this;
}

int DS::ETPhonetic::get_file_offset()
{
 // 聲母編碼 X 5 + 聲符編碼
 return _value[0]*5+_value[3];
}

int DS::ETPhonetic::get_check_code()
{
 // 介音編碼 X 16 + 韻母編碼
 return _value[1]*16+_value[2];
}

// 讓倒退鍵使用，使其刪除一個注音符號
void DS::ETPhonetic::del_ph()
{
 for(int i=3; i>=0; --i)
 {
  if (_value[i]!=0)
  {
   _value[i]=0;
   return;
  }
 }
}

void DS::ETPhonetic::del_all_ph()
{
 _value[0]= _value[1]= _value[2]= _value[3]=0;
 _phonetic[0]= _phonetic[1]= _phonetic[2]= _phonetic[3]="";
}

// 將兩個 byte 轉成一個整數 ex: 0x00,0xed 轉成 0x00ed
// static member function [private]
int DS::ETPhonetic::bytes_to_int(unsigned char byte1,unsigned char byte2)
{
 if (byte1==0x00)
  return (static_cast<int>(byte2) & 0x00ff);
 signed int i=0;
 i= byte1 << 8;
 i&=0xff00;
 i+=byte2;
 return i;
}

// 找出倚天注音的編碼值, ex: ㄅ是 1
int DS::ETPhonetic::lookup_etphonetic_value(const std::string &str)
{
 if (str.empty())
  return 0;
 std::map<std::string,std::pair<DS::PhoneticKind,int> > :: iterator it;

 it=_phonetic_map.find(str);
 if (it==_phonetic_map.end()) // not found
 {
  return -1;
 }
 else
 {
  return it->second.second;
 }

}

DS::PhoneticKind DS::ETPhonetic::lookup_etphonetic_kind(const std::string &str)
{
 std::map<std::string,std::pair<DS::PhoneticKind,int> > :: iterator it;

 it=_phonetic_map.find(str);
 if (it==_phonetic_map.end()) // not found
 {
  return DS::NOS;
 }
 else
 {
  return it->second.first;
 }
}

std::vector<DS::Big5Code> DS::ETPhonetic::del_phonetic()
{
 //_input_status=DS::DEL;
 for(int pk=3 ; pk >=0  ; --pk)
 {
  if (!_phonetic[pk].empty())
  {
   _phonetic[pk]="";
   _value[pk]=0;
   break;
  }
 }
 //calc_encoding_value();
 find_match_words();
 return _big5_codes;
}

std::vector<DS::Big5Code> DS::ETPhonetic::add_phonetic(const std::string &ph)
{
 //_input_status=DS::ADD;
 DS::PhoneticKind pk=lookup_etphonetic_kind(ph);
 if (pk==DS::NOS) // 沒有此注音符號
  return _big5_codes;
 _phonetic[lookup_etphonetic_kind(ph)]=ph; 
 _value[pk]=lookup_etphonetic_value(_phonetic[pk]);
 /*
 cout << "_value[0] : " << _value[0] << endl;
 cout << "_value[1] : " << _value[1] << endl;
 cout << "_value[2] : " << _value[2] << endl;
 cout << "_value[3] : " << _value[3] << endl;
 */
 //calc_encoding_value();
 find_match_words();
 return _big5_codes;
}

// 找出倚天注音的編碼值注音的種類的 pair, ex: ㄅ (TOPS,1)
std::pair<DS::PhoneticKind,int> DS::ETPhonetic::lookup_etphonetic_pair(const std::string &str)
{
 //std::map<std::string,std::pair<DS::PhoneticKind,int> > :: iterator it;

 return _phonetic_map.find(str)->second;

}

/*
// 計算比對倚天注音參考檔案時所需要的數值
void DS::ETPhonetic::calc_encoding_value()
{
 //_value[0] = _value[1] = _value[2] = _value[3] = 0;
 _file_offset=get_file_offset();
 _check_code=get_check_code();
 cout << "_file_offset : " << _file_offset << endl;
 cout << "_check_code : " << _check_code << endl;
}
*/

void DS::ETPhonetic::find_match_words()
{
 _big5_codes.clear();
 // 這個 offset 是用來找出符合的區段是在這個參考檔的那一段
 int offset=get_file_offset();
 int check_code=get_check_code();

 //std::cout << "offset : " << offset << endl;
 //std::cout << "check_code : " << check_code << endl;
 //_etphonetic_file.seekg(0,std::ios::beg);
 _etphonetic_file.seekg(offset*2,std::ios::beg);
 char bytes[2];
 _etphonetic_file.get(bytes[0]);
 _etphonetic_file.get(bytes[1]);
 //cout << "byte[0] :" << bytes[0] << endl;
 //cout << "byte[1] :" << bytes[1] << endl;
 int offset_begin,offset_end;
 offset_begin=bytes_to_int(bytes[1],bytes[0]);
 _etphonetic_file.get(bytes[0]);
 _etphonetic_file.get(bytes[1]);
 offset_end=bytes_to_int(bytes[1],bytes[0]);
 //cout << hex << offset_begin << " " << offset_end << endl;
 _etphonetic_file.seekg(offset_begin,std::ios::beg);
 bool found=false; // 是否找到在對應的區段有符合的字
 //cout << "offset_begin : " << offset_begin << endl;
 //cout << "offset_end : " << offset_end << endl;
 // offset_end 那個位址不用比對
 for (int i=offset_begin; i < offset_end; i++)
 {
  int c = _etphonetic_file.get();
  if (c==check_code)
  {
   found=true;
   break;
  }
 }
 // 找出符合的字
 if (found==true)
 {
  char c1;
  if (!_etphonetic_file.eof())
   _etphonetic_file.get(c1);
  while(static_cast<unsigned char>(c1)>128) // 表示為 big5 code, 再往下抓一 byte
  {
   if (_etphonetic_file.eof())
    break;
   char c2;
   _etphonetic_file.get(c2);
   if (_etphonetic_file.eof())
    break;
   _big5_codes.push_back(DS::Big5Code(c1,c2));
   //cout << hex << (int)c << "," << (int)c2 << endl;
   _etphonetic_file.get(c1);
  }
 }
 // 要是目前的 stream 狀態是在 eof 之內那麼必須呼叫 member function clear()
 // 將狀態清除，以避免下一次有關的 stream 操作會失敗
 _etphonetic_file.clear();

}

// 傳回組字的注音符號
std::vector<std::string> DS::ETPhonetic::get_composite_words()
{
 std::vector<std::string> composite_words;
 for (int i=0 ; i < 4 ; ++i)
  composite_words.push_back(_phonetic[i]);
 return composite_words;
}
