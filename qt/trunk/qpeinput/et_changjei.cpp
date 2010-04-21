/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Id: et_changjei.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * function: 倚天倉頡輸入法
 */


#include "et_changjei.h"

//#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

namespace
{
 std::string changjei[]={" ","日","月","金","木","水","火","土","竹","戈","十","大","中","一","弓","人","心","手","口","尸","廿","山","女","田","難","卜","重"};
 int changjei_value[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
}

/*
unsigned int DS::ser2big5(unsigned int sercode)
{
 Big5Code big5;

 if ( 0x8001 <= sercode && sercode <= 0x805e)
 {
  big5.code[0]=(sercode-0x8001)/0x9d + 0xc6;
  big5.code[1]=(sercode-0x8001)%0x9d + 0xa1;
  return big5.num;
 }

 unsigned serbase,big5base;
 if ( 0x8400 <= sercode && sercode <= 0x8597)
 {
  serbase=0x8400;
  big5base=0xa1;
 }
 else
 {
  if ( 0x8800 <= sercode && sercode <= 0x9d18)
  {
   serbase=0x8800;
   big5base=0xa4;
  }
  else
  {
   if ( 0x9d19 <= sercode && sercode <= 0xbb25)
   {
    serbase=0x9d19;
    big5base=0xc9;
   }
   else
   {
    if ( 0x805f <= sercode && sercode <= 0x616d)
    {
     serbase=0x805f;
     big5base=0xc7;
    }
    else
     return 0 ; // not found big5 code
   }
  }
 }

 big5.code[0]=(sercode-serbase)/0x9d + big5base;
 big5.code[1]=(sercode-serbase)%0x9d;

 if (big5.code[1] < 0x3f)
 {
  big5.code[1]+=0x40;
 }
 else
 {
  big5.code[1]=big5.code[1]-0x3f + 0xa1;
 }
 //std::swap(big5.code[0],big5.code[1]);
 big5.code[2]=big5.code[3]=0;
 return big5.num;
}
*/

//DS::ETChangjei::ETChangjei(std::string fn):_calc_value(5),_fn(fn)
DS::ETChangjei::ETChangjei(std::string fn):_fn(fn)
{
 // 初始化倉頡字根與編碼值的對應
 for (int i=0 ; i < sizeof(changjei_value) / sizeof(int) ; i++)
  _changjei_map.insert(std::make_pair(changjei[i],changjei_value[i]));
 _sercode=0x8800;
 _calc_value[0]= _calc_value[1]= _calc_value[2]= _calc_value[3]= _calc_value[4]==0;
 //_etchangjei_nums=0;
}

std::vector<DS::Big5Code> DS::ETChangjei::del_changjei() // 刪除最後一個倉頡字根
{
 if (!_changjei_char.empty())
 {
  _changjei_char.pop_back();
  calc_encoding_value();
  find_match_words();
 }
 return _match_words;
}

std::vector<DS::Big5Code> DS::ETChangjei::add_changjei(std::string changjei) // 讀進一個倉頡字根
{
 //++_etchangjei_nums;
 if(_changjei_char.size() < 5 )
 {
  _changjei_char.push_back(changjei);
  calc_encoding_value();
  find_match_words();
 }
 return _match_words;
}

void DS::ETChangjei::calc_encoding_value()
{
 //_changjei_value;

 //_calc_value.clear();
 _calc_value[0]=_calc_value[1]=_calc_value[2]=_calc_value[3]=_calc_value[4]=0;	
 int i=0;	
 // 這裡可以改成抓最後輸入的倉頡字根即可, 不要每次都從頭抓一次
 for (ChangjeiCharIt it=_changjei_char.begin() ; it != _changjei_char.end() ; ++it,++i)
 {
  int v=changjei2value(*it);
  if (v!=-1) 
   _calc_value[i]=v;
  else // v==-1 不是倉頡字根
   return;
 }

 // 將編碼的順序對調
 unsigned int calc_value[5];
 turn_changjei_value(calc_value,5);

 //_etchangjei_value._encoding_value = 27*27*27*27*_calc_value[0] + 27*27*27*_calc_value[1] + 27*27*_calc_value[2] + 27* _calc_value[3] + _calc_value[4];
 _etchangjei_value._encoding_value = 27*27*27*27*calc_value[0] + 27*27*27*calc_value[1] + 27*27*calc_value[2] + 27* calc_value[3] + calc_value[4];

 /*
 unsigned int changjei_value=0;
 changjei_value+=_calc_value[0]*27*27*27*27;
 changjei_value+=_calc_value[1]*27*27*27;
 changjei_value+=_calc_value[2]*27*27;
 changjei_value+=_calc_value[3]*27;
 changjei_value+=_calc_value[4];
 _etchangjei_value._encoding_value=changjei_value;
 cout << "calc_value[0] : " << dec << calc_value[0] << endl;
 cout << "calc_value[1] : " << calc_value[1] << endl;
 cout << "calc_value[2] : " << calc_value[2] << endl;
 cout << "calc_value[3] : " << calc_value[3] << endl;
 cout << "calc_value[4] : " << calc_value[4] << endl;
 cout << "changjei encoding value : " << _etchangjei_value._encoding_value << endl;
 cout << "_etchangjei_value.ch[0] : " << hex << (int)_etchangjei_value._byte3[0] << endl;
 cout << "_etchangjei_value.ch[1] : " << (int)_etchangjei_value._byte3[1] << endl;
 cout << "_etchangjei_value.ch[2] : " << (int)_etchangjei_value._byte3[2] << endl;
 */

}

// 將編碼的順序對調
void DS::ETChangjei::turn_changjei_value(unsigned int *value,int size) 
{
 value[0]= value[1]= value[2]= value[3]= value[4]=0;
 value[0]=_calc_value[0];
 value[1]=_calc_value[1];
 value[2]=_calc_value[2];
 value[3]=_calc_value[3];
 value[4]=_calc_value[4];
 if (_changjei_char.size() >= 3 )
 { // 將編碼的順序對調
  std::swap(value[1],value[_changjei_char.size()-1]);
 }
}

int DS::ETChangjei::changjei2value(std::string changjei) // 將倉頡字根轉成編碼值
{
 std::map<std::string,int> :: iterator it;
 it=_changjei_map.find(changjei);
 if (it==_changjei_map.end()) // not found or space
  return -1;
 else
  return it->second;
}

void DS::ETChangjei::find_match_words()
{
 _match_words.clear();
 _sercode=0x8800;
 std::ifstream et_changjei_file(_fn.c_str());
 if (!et_changjei_file)
 {
  //std::cerr << "open file error " << endl; 
  exit(-1);
 }
 //std::ios::pos_type pos=0;
 et_changjei_file.seekg(0,std::ios::beg);
 while(!et_changjei_file.eof())
 {
  for(int i=0 ; i < 3 ; ++i)
  {
   char ch;
   et_changjei_file.get(ch);
   unsigned uch=static_cast<unsigned char>(ch);
   if (uch==_etchangjei_value._byte3[i])
   {
    if (i==2) // 比對 3 個 byte 完全符合
    {
     //cout << "sercode : " << _sercode << endl;
     DS::Big5Code g5;
     g5=DS::Big5Codec::ser2big5(_sercode);
     //cout << "big5 code : " << g5.big5num() << endl;
     //cout << (int)g5.hi() << (int)g5.lo() << endl;
     char s[]="宋";
     s[0]=g5.hi();
     s[1]=g5.lo();
     //cout << s << endl;
     _match_words.push_back(DS::Big5Code(g5.hi(),g5.lo()) );
     //_sercode=0x8800;
    }
   }
   else
   {
    et_changjei_file.seekg(2-i,std::ios::cur);
    break;
   }
  } // end for(int i=0 ; i < 3 ; ++i)
  ++_sercode;
 } // end while(!et_changjei_file.eof())

}

