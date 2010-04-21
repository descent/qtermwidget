/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Id: et_changjei.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * function: 倚天倉頡輸入法
 */

#ifndef ET_CHANGJEI_H
#define ET_CHANGJEI_H

#include <string>
#include <stack>
#include <map>
#include <vector>

#include "big5.h"

namespace DS
{

 //unsigned int ser2big5(unsigned int sercode);

 /*
 union Big5Code
 {
  unsigned char code[4];
  unsigned int num;
 };
 */

 union ETChangjeiValue
 {
  unsigned int _encoding_value;
  // 這裡有硬體平台相依性, big endian or little endian
  unsigned char _byte3[3]; // 當要用 _bytes 做比對資料時要注意
 };
 class ETChangjei
 {
  public:
   ETChangjei(std::string fn);
   // 傳回組字的倉頡字根
   std::vector<std::string> get_composite_words(){ return _changjei_char;}
   std::vector<DS::Big5Code> add_changjei(std::string changjei); // 讀進一個倉頡字根
   std::vector<DS::Big5Code> del_changjei(); // 刪除最後一個倉頡字根
   void del_all_changjei() // 刪除所有倉頡字根
   {
    while(!_changjei_char.empty())
     _changjei_char.pop_back();
   }
   /*
   std::vector<DS::Big5Code> get_match_words()
   {
    return _match_words;
   }
   */
  private:
   void find_match_words();
   void calc_encoding_value(); // 計算倉頡編碼值並轉成 3 byte 的資料存入 _byte3
   void turn_changjei_value(unsigned int *value,int size); // 將編碼的順序對調
   int changjei2value(std::string changjei); // 將倉頡字根轉成編碼值
   std::string _fn; // 倉頡輸入法參考檔檔名
   unsigned int _encoding_value; // 倉頡編碼值
   // 將倉頡編碼值轉成 3 byte 的資料, 用來和倉頡輸入法參考檔做比對
   unsigned char _byte3[3];
   // 用來存放倉頡字根的編碼值以供計算, 最多 5 碼
   //std::vector<unsigned int> _calc_value; 
   unsigned int _calc_value[5]; 
   // 用來存放個倉頡字根最多 5 碼 (big5 編碼)
   std::vector<std::string> _changjei_char; 
   typedef std::vector<std::string> :: iterator ChangjeiCharIt;
   std::map<std::string,int> _changjei_map; // 倉頡字根與編碼值的對應
   ETChangjeiValue _etchangjei_value;
   int _sercode; // 序列碼
   std::vector<DS::Big5Code> _match_words;
   //int _etchangjei_nums; // 紀錄目前輸入多少個倉頡字根

 };

} // end namespace DS

#endif
