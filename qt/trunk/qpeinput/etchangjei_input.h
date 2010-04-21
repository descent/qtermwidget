/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Id: etchangjei_input.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * function: �ʤѭܾe��J�k�b QPE input method �� class
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
   QRect calc_status_bar(); // �p�� status bar ���j�p
   QString get_select_word(int paragraph,int index);
   // �e�X�ܾe�r�ڲզr��
   void draw_composite_area(); 
   // �e�X�ܾe��r��
   void draw_select_area(std::vector<DS::Big5Code> &match_words);
   std::string english_to_changjei(int ch);
   // �N�^��Ÿ�������ܾe�r��
   std::map<int,std::string> _english_to_changjei;
   DS::ETChangjei _et_changjei;
   std::vector<DS::Big5Code> _match_words;
   std::vector<std::string> _composite_words;
   enum {MAX_WORDS=7}; // �b��r�ϳ̦h�@���i�q�X���r
   int _max_paragraph,_paragraph;
 };

}


#endif
