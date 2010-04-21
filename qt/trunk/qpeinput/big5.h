/*
 * $Author: incoming $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/11 07:47:47 $
 * $Id: big5.h,v 1.1.1.1 2003/08/11 07:47:47 incoming Exp $
 * function : get big5 code
 */
#ifndef BIG5_H 
#define BIG5_H 

#include <vector>
#include <string>
//#include <iostream>


using std::vector;
using std::string;
//using std::cout;
//using std::endl;

namespace DS
{
   // �o�� union �̩ۨ�w�饭�x�H�ξ�ƪ��j�p, port �ɭn�`�N
   union aBig5Code
   {
    unsigned char code[4];
    unsigned int num;
   };

 class Big5Code
 {
  public:
   Big5Code(unsigned char b1,unsigned char b2)
   {
    init_big5_code();
    _big5_code.code[0]=b2;
    _big5_code.code[1]=b1;
   }
   Big5Code(int big5_num=0)
   {

    init_big5_code();
    _big5_code.num=big5_num;
   }

   unsigned char hi(){return _big5_code.code[1];}
   unsigned char lo(){return _big5_code.code[0];}
   // �Ǧ^ big5 �s�X
   unsigned int big5num(){return _big5_code.num;}
   // �Ǧ^ big5 �s�X�� std::string
   std::string big5_string();
   
  private:
   void init_big5_code()
   {
    _big5_code.code[0] = _big5_code.code[1] = _big5_code.code[2] = _big5_code.code[3]=0;
   }
   unsigned char _hi,_lo; // high byte and low byte
   aBig5Code _big5_code;

 };

 class Big5Codec
 {
  public:
   static Big5Code ser2big5(unsigned int sercode); // �ǦC�X�ন BIG5 �X
   vector<Big5Code> & get_big5code(const std::string &str)
   {
    std::string::const_iterator it;
    for (it=str.begin() ; it != str.end() ; it++)
    {
     // big5 high byte
     if ( ( 0xa1 <= static_cast<unsigned char>(*it) && static_cast<unsigned char>(*it) <= 0xfe) || 
          ( 0x8e <= static_cast<unsigned char>(*it) && static_cast<unsigned char>(*it) <= 0xa0 ) || 
	  ( 0x81 <= static_cast<unsigned char>(*it) && static_cast<unsigned char>(*it) <= 0x8d) )
     {
      //cout << std::hex << (int)*it << endl;
      //cout << (int)*++it << endl;
      unsigned char hi,lo;
      hi=*it;
      ++it;
      lo=*it;

      _big5_codes.push_back(Big5Code(hi,lo));
      //_big5_codes.push_back(Big5Code(*it,*++it));

       
     }
     else
     {
      // 32 - 126 seeable ASCII
      if ( 32 <= static_cast<unsigned char>(*it) && static_cast<unsigned char>(*it) <= 126)
      {
       _big5_codes.push_back(Big5Code(0x00,*it));
      }

     }

    }

    return _big5_codes;
   }
   /*
   const vector<Big5Code> & get_big5code()
   {
    return _big5_codes;
   }
   */


  private:
   vector<Big5Code> _big5_codes;


 };

} // end namespace DS


#endif
