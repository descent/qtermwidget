/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: etphonetic_input.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: 提供倚天注音的輸入方法, gui part
 */

#ifndef ETPHONETIC_INPUT_H
#define ETPHONETIC_INPUT_H

#include <string>
#include <map>

#include "input_handler.h"
#include "et_phonetic.h"

namespace DS
{
 class EtPhoneticInput : public BaseInput
 {
  public:
   EtPhoneticInput(QPEInputMethod *parent,const std::string &fn);
   void draw_keys();
   void draw_composite_area(); // 畫出組字區
   void draw_select_area(); // 畫出選字區
   void draw_select_area(int paragraph); // 畫出選字區
   DS::QtKey do_with_pos(int x,int y); // 處理被點選的座標位置
   //std::string english_to_phonetic41(int ch); // 將英數按鍵對應到大千注音
   QString english_to_phonetic41(int ch); // 將英數按鍵對應到大千注音
   std::string english_to_phonetic41_string(int ch); // 將英數按鍵對應到大千注音
   void change_size();
  private:
   // 抓所選擇的字
   QString get_select_word(int paragraph,int index);
   enum {MAX_WORDS=8};	
   //void press_phonetic();
   void add_phonetic(int ch);
   void del_phonetic()
   {
    for (int i=3 ; i >=0 ; --i)
    {
     if (!_phonetic[i].isEmpty())
     {
      _phonetic[i]="";
      return;
     }
    }
   }
   QRect calc_status_bar(); // 計算 status bar 的大小
   std::map<int,std::string> _english_to_phonetic41;
   QString _phonetic[4]; // 組字區的注音符號
   DS::ETPhonetic _etphonetic;
   std::vector<DS::Big5Code> _match_words; // 符合注音輸入的字
   int _paragraph; // 記錄選字區目前是那一段要顯示出來
   int _max_paragraph; // 記錄選字區最多有幾段
   bool _draw_select_area,_draw_keys,_draw_composite_area;
   std::vector<std::string> _composite_words; // 目前有那些注音被輸入

 };

} // end namespace DS


#endif
