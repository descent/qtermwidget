/*
 * $Revision: 1.1.1.1 $
 * $Author: incoming $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: etphonetic_input.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function: ���ѭʤѪ`������J��k, gui part
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
   void draw_composite_area(); // �e�X�զr��
   void draw_select_area(); // �e�X��r��
   void draw_select_area(int paragraph); // �e�X��r��
   DS::QtKey do_with_pos(int x,int y); // �B�z�Q�I�諸�y�Ц�m
   //std::string english_to_phonetic41(int ch); // �N�^�ƫ��������j�d�`��
   QString english_to_phonetic41(int ch); // �N�^�ƫ��������j�d�`��
   std::string english_to_phonetic41_string(int ch); // �N�^�ƫ��������j�d�`��
   void change_size();
  private:
   // ��ҿ�ܪ��r
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
   QRect calc_status_bar(); // �p�� status bar ���j�p
   std::map<int,std::string> _english_to_phonetic41;
   QString _phonetic[4]; // �զr�Ϫ��`���Ÿ�
   DS::ETPhonetic _etphonetic;
   std::vector<DS::Big5Code> _match_words; // �ŦX�`����J���r
   int _paragraph; // �O����r�ϥثe�O���@�q�n��ܥX��
   int _max_paragraph; // �O����r�ϳ̦h���X�q
   bool _draw_select_area,_draw_keys,_draw_composite_area;
   std::vector<std::string> _composite_words; // �ثe�����Ǫ`���Q��J

 };

} // end namespace DS


#endif
