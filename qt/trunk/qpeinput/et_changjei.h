/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Id: et_changjei.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * function: �ʤѭܾe��J�k
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
  // �o�̦��w�饭�x�̩ۨ�, big endian or little endian
  unsigned char _byte3[3]; // ��n�� _bytes ������Ʈɭn�`�N
 };
 class ETChangjei
 {
  public:
   ETChangjei(std::string fn);
   // �Ǧ^�զr���ܾe�r��
   std::vector<std::string> get_composite_words(){ return _changjei_char;}
   std::vector<DS::Big5Code> add_changjei(std::string changjei); // Ū�i�@�ӭܾe�r��
   std::vector<DS::Big5Code> del_changjei(); // �R���̫�@�ӭܾe�r��
   void del_all_changjei() // �R���Ҧ��ܾe�r��
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
   void calc_encoding_value(); // �p��ܾe�s�X�Ȩ��ন 3 byte ����Ʀs�J _byte3
   void turn_changjei_value(unsigned int *value,int size); // �N�s�X�����ǹ��
   int changjei2value(std::string changjei); // �N�ܾe�r���ন�s�X��
   std::string _fn; // �ܾe��J�k�Ѧ����ɦW
   unsigned int _encoding_value; // �ܾe�s�X��
   // �N�ܾe�s�X���ন 3 byte �����, �ΨөM�ܾe��J�k�Ѧ��ɰ����
   unsigned char _byte3[3];
   // �ΨӦs��ܾe�r�ڪ��s�X�ȥH�ѭp��, �̦h 5 �X
   //std::vector<unsigned int> _calc_value; 
   unsigned int _calc_value[5]; 
   // �ΨӦs��ӭܾe�r�ڳ̦h 5 �X (big5 �s�X)
   std::vector<std::string> _changjei_char; 
   typedef std::vector<std::string> :: iterator ChangjeiCharIt;
   std::map<std::string,int> _changjei_map; // �ܾe�r�ڻP�s�X�Ȫ�����
   ETChangjeiValue _etchangjei_value;
   int _sercode; // �ǦC�X
   std::vector<DS::Big5Code> _match_words;
   //int _etchangjei_nums; // �����ثe��J�h�֭ӭܾe�r��

 };

} // end namespace DS

#endif
