/*
 * $Revision: 1.1.1.1 $
 * $Id: et_phonetic.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * $Author: incoming $
 * function: �ʤѪ`����J�k, non-gui part
 */

// �n���g�@�ӷs����J�k�ɻݭn���Ѩ�ӳ����A�@�O��J�k�����@�O����
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
 // NOS �N��i no symbol,
 // TOPS �N���n��, 0,BOTTOMS ����, MIDS ����,
 // OTHERS �n��, S �� symbol ���N
 enum PhoneticKind {NOS=-1,TOPS=0,MIDS,BOTTOMS,OTHERS};
 enum InputStatus {DEL,ADD}; // �ثe�O�W�[�@�Ӫ`���٬O�R���@�Ӫ`��

  struct Phonetic42keys
  {
   std::string _glyph; // �`���Ÿ�
   int _id; // �N�� 0 �n��, 1 ����, 2 ����, 3 �n��
   int _value; // �Ӫ`���Ÿ����s�X��
  };

 class ETPhonetic
 {
  public:
   // open ��J�k�Ѧ��ɡA�ǤJ��������L class
   ETPhonetic(std::string fn);
   // �[�J�@�Ӫ`��
   std::vector<DS::Big5Code> add_phonetic(const std::string &ph);
   // �R���̫�@�Ӫ`���Ÿ�, ex: �t��, �N �� �R��
   std::vector<DS::Big5Code> del_phonetic();
   // �R���Ҧ��H��J���`���Ÿ�
   // ex: �t�� 
   // �N �t�� ���R��
   void del_all_ph();
   ETPhonetic & operator<<(const std::string &s);
   // ���˰h��ϥΡA�Ϩ�R���@�Ӫ`���Ÿ�
   // ex: �t�� 
   // �N �� �R��
   void del_ph();
   //std::string get_phonetic_glyph();
   // �Ǧ^�զr���`���Ÿ�
   std::vector<std::string> get_composite_words();
  private:
   void find_match_words();
   //void calc_encoding_value(); // �p����ʤѪ`���Ѧ��ɮ׮ɩһݭn���ƭ�
   int get_file_offset();
   int get_check_code();
   // ��X�ʤѪ`�����s�X�Ȫ`���������� pair, ex: �t (TOPS,1)
   std::pair<DS::PhoneticKind,int> lookup_etphonetic_pair(const std::string &str);
   // ��X�ʤѪ`�����s�X��, ex: �t�O 1
   int lookup_etphonetic_value(const std::string &str);
   // ��X�`��������, ex: �t�O TOPS (�n��)
   DS::PhoneticKind lookup_etphonetic_kind(const std::string &str);
   int _file_offset,_check_code;
   // �N��� byte �ন�@�Ӿ�� ex: 0x00,0xed �ন 0x00ed
   static int bytes_to_int(unsigned char byte1,unsigned char byte2); 
   int _value[4]; // �n���B�����B�����B�n�Ū��s�X��
   std::string _fn;
   std::ifstream _etphonetic_file;
   std::vector<DS::Big5Code> _big5_codes; // �ŦX���r
   // �N��J���`���Ÿ��s�_��, �̦h 4 �X
   std::string _phonetic[4]; 
   // ��X�`���Ÿ����Ÿ������νs�X��
   std::map<std::string,std::pair<DS::PhoneticKind,int> > _phonetic_map;
   // �ΨӬ����̫�@����J���O���@�ت`���Ÿ�, �n���٬O�������� ...
   //DS::PhoneticKind _new_pk; 
   //InputStatus _input_status;
 };
}


#endif
