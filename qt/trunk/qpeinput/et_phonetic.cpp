/*
 * $Revision: 1.1.1.1 $
 * $Id: et_phonetic.cpp,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * $Date: 2003/08/11 07:47:47 $
 * $Author: incoming $
 * function: �ʤѪ`����J�k
 */

#include "et_phonetic.h"

// debug �ΡA�ΨӦL�X�Y�ǰT��
//#include <iostream>
#include <algorithm>

using namespace std;

namespace
{
 std::string phonetic[]={"�t","�u","�v","�w","�x","�y","�z","�{","�|","�}","�~","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��"};

 DS::PhoneticKind phonetic_kind[]={DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::TOPS,DS::MIDS,DS::MIDS,DS::MIDS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::BOTTOMS,DS::OTHERS,DS::OTHERS,DS::OTHERS,DS::OTHERS};

 // �ʤ��n���s�X 1-21
 // �ʤѤ����s�X 1-3
 // �ʤ������s�X 1-13
 // �ʤ��n�Žs�X 1-4
 int phonetic_value[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,1,2,3,1,2,3,4,5,6,7,8,9,10,11,12,13,1,2,3,4};
}

DS::ETPhonetic::ETPhonetic(std::string fn):_fn(fn)
{
 _value[0]= _value[1]= _value[2]= _value[3]=0;
 _etphonetic_file.open(_fn.c_str());// �}�ҭʤѪ`���Ѧ���
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

// �w�q�j�d��L�ƦC������
DS::Phonetic42keys _ph[]=
{
 // �n��
 {"�t",0,1},
 {"�u",0,2},
 {"�v",0,3},
 {"�w",0,4},
 {"�x",0,5},
 {"�y",0,6},
 {"�z",0,7},
 {"�{",0,8},
 {"�|",0,9},
 {"�}",0,10},
 {"�~",0,11},
 {"��",0,12},
 {"��",0,13},
 {"��",0,14},
 {"��",0,15},
 {"��",0,16},
 {"��",0,17},
 {"��",0,18},
 {"��",0,19},
 {"��",0,20},
 {"��",0,21},
 // ����
 {"�@",1,1}, // �o�O�Ʀr�@�A���O�`������
 {"��",1,2},
 {"��",1,3},
 // ����
 {"��",2,1},
 {"��",2,2},
 {"��",2,3},
 {"��",2,4},
 {"��",2,5},
 {"��",2,6},
 {"��",2,7},
 {"��",2,8},
 {"��",2,9},
 {"��",2,10},
 {"��",2,11},
 {"��",2,12},
 {"��",2,13},
 // �n��
 {" ",3,0},
 {"��",3,1},
 {"��",3,2},
 {"��",3,3},
 {"��",3,4},
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
 // �n���s�X X 5 + �n�Žs�X
 return _value[0]*5+_value[3];
}

int DS::ETPhonetic::get_check_code()
{
 // �����s�X X 16 + �����s�X
 return _value[1]*16+_value[2];
}

// ���˰h��ϥΡA�Ϩ�R���@�Ӫ`���Ÿ�
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

// �N��� byte �ন�@�Ӿ�� ex: 0x00,0xed �ন 0x00ed
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

// ��X�ʤѪ`�����s�X��, ex: �t�O 1
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
 if (pk==DS::NOS) // �S�����`���Ÿ�
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

// ��X�ʤѪ`�����s�X�Ȫ`���������� pair, ex: �t (TOPS,1)
std::pair<DS::PhoneticKind,int> DS::ETPhonetic::lookup_etphonetic_pair(const std::string &str)
{
 //std::map<std::string,std::pair<DS::PhoneticKind,int> > :: iterator it;

 return _phonetic_map.find(str)->second;

}

/*
// �p����ʤѪ`���Ѧ��ɮ׮ɩһݭn���ƭ�
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
 // �o�� offset �O�Ψӧ�X�ŦX���Ϭq�O�b�o�ӰѦ��ɪ����@�q
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
 bool found=false; // �O�_���b�������Ϭq���ŦX���r
 //cout << "offset_begin : " << offset_begin << endl;
 //cout << "offset_end : " << offset_end << endl;
 // offset_end ���Ӧ�}���Τ��
 for (int i=offset_begin; i < offset_end; i++)
 {
  int c = _etphonetic_file.get();
  if (c==check_code)
  {
   found=true;
   break;
  }
 }
 // ��X�ŦX���r
 if (found==true)
 {
  char c1;
  if (!_etphonetic_file.eof())
   _etphonetic_file.get(c1);
  while(static_cast<unsigned char>(c1)>128) // ��ܬ� big5 code, �A���U��@ byte
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
 // �n�O�ثe�� stream ���A�O�b eof �������򥲶��I�s member function clear()
 // �N���A�M���A�H�קK�U�@�������� stream �ާ@�|����
 _etphonetic_file.clear();

}

// �Ǧ^�զr���`���Ÿ�
std::vector<std::string> DS::ETPhonetic::get_composite_words()
{
 std::vector<std::string> composite_words;
 for (int i=0 ; i < 4 ; ++i)
  composite_words.push_back(_phonetic[i]);
 return composite_words;
}
