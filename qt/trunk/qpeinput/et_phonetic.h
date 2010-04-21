/*
 * $Revision: 1.1.1.1 $
 * $Id: et_phonetic.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * $Author: incoming $
 * function: 倚天注音輸入法, non-gui part
 */

// 要撰寫一個新的輸入法時需要提供兩個部份，一是輸入法本身一是按鍵
//

#ifndef PHONETIC_H
#define PHONETIC_H

#include "big5.h"
//#include "input_base.h"

#include <fstream>
#include <string>
#include <map>

namespace DS
{
 // NOS 代表i no symbol,
 // TOPS 代表聲母, 0,BOTTOMS 韻母, MIDS 介音,
 // OTHERS 聲符, S 為 symbol 之意
 enum PhoneticKind {NOS=-1,TOPS=0,MIDS,BOTTOMS,OTHERS};
 enum InputStatus {DEL,ADD}; // 目前是增加一個注音還是刪除一個注音

  struct Phonetic42keys
  {
   std::string _glyph; // 注音符號
   int _id; // 代表 0 聲母, 1 韻母, 2 介音, 3 聲符
   int _value; // 該注音符號的編碼值
  };

 class ETPhonetic
 {
  public:
   // open 輸入法參考檔，傳入對應的鍵盤 class
   ETPhonetic(std::string fn);
   // 加入一個注音
   std::vector<DS::Big5Code> add_phonetic(const std::string &ph);
   // 刪除最後一個注音符號, ex: ㄅㄧ, 將 ㄧ 刪除
   std::vector<DS::Big5Code> del_phonetic();
   // 刪除所有以輸入的注音符號
   // ex: ㄅㄧ 
   // 將 ㄅㄧ 全刪除
   void del_all_ph();
   ETPhonetic & operator<<(const std::string &s);
   // 讓倒退鍵使用，使其刪除一個注音符號
   // ex: ㄅㄧ 
   // 將 ㄧ 刪除
   void del_ph();
   //std::string get_phonetic_glyph();
   // 傳回組字的注音符號
   std::vector<std::string> get_composite_words();
  private:
   void find_match_words();
   //void calc_encoding_value(); // 計算比對倚天注音參考檔案時所需要的數值
   int get_file_offset();
   int get_check_code();
   // 找出倚天注音的編碼值注音的種類的 pair, ex: ㄅ (TOPS,1)
   std::pair<DS::PhoneticKind,int> lookup_etphonetic_pair(const std::string &str);
   // 找出倚天注音的編碼值, ex: ㄅ是 1
   int lookup_etphonetic_value(const std::string &str);
   // 找出注音的種類, ex: ㄅ是 TOPS (聲母)
   DS::PhoneticKind lookup_etphonetic_kind(const std::string &str);
   int _file_offset,_check_code;
   // 將兩個 byte 轉成一個整數 ex: 0x00,0xed 轉成 0x00ed
   static int bytes_to_int(unsigned char byte1,unsigned char byte2); 
   int _value[4]; // 聲母、介音、韻母、聲符的編碼值
   std::string _fn;
   std::ifstream _etphonetic_file;
   std::vector<DS::Big5Code> _big5_codes; // 符合的字
   // 將輸入的注音符號存起來, 最多 4 碼
   std::string _phonetic[4]; 
   // 找出注音符號的符號種類及編碼值
   std::map<std::string,std::pair<DS::PhoneticKind,int> > _phonetic_map;
   // 用來紀錄最後一次輸入的是那一種注音符號, 聲母還是韻母等等 ...
   //DS::PhoneticKind _new_pk; 
   //InputStatus _input_status;
 };
}


#endif
