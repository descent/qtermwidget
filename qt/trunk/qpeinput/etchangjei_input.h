/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Id: etchangjei_input.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * function: 倚天倉頡輸入法在 QPE input method 的 class
 */

#ifndef ETCHANGJEI_INPUT_H
#define ETCHANGJEI_INPUT_H

#include "input_handler.h"
#include "qpe_input_w.h"
#include "et_changjei.h"

#include <map>
#include <vector>
#include <string>

#include <qtextcodec.h>

namespace DS
{
 class EtChangjeiInput : public BaseInput
 {
  public:
   EtChangjeiInput(QPEInputMethod *parent,const std::string &fn);
   void draw_keys();
   DS::QtKey do_with_pos(int x,int y);
   void change_size()
   {
    get_parent()->set_row(6);
   }
  private:
   QRect calc_status_bar(); // 計算 status bar 的大小
   QString get_select_word(int paragraph,int index);
   // 畫出倉頡字根組字區
   void draw_composite_area(); 
   // 畫出倉頡選字區
   void draw_select_area(std::vector<DS::Big5Code> &match_words);
   std::string english_to_changjei(int ch);
   // 將英文符號對應到倉頡字根
   std::map<int,std::string> _english_to_changjei;
   DS::ETChangjei _et_changjei;
   std::vector<DS::Big5Code> _match_words;
   std::vector<std::string> _composite_words;
   enum {MAX_WORDS=7}; // 在選字區最多一次可秀出的字
   int _max_paragraph,_paragraph;
 };

}


#endif
