#include "big5.h"

// ¶Ç¦^ big5 ½s½Xªº std::string
std::string DS::Big5Code::big5_string()
{
 char s[]="§º";
 s[0]=hi();
 s[1]=lo();
 return std::string(s);
}

DS::Big5Code DS::Big5Codec::ser2big5(unsigned int sercode)
{
 aBig5Code big5;
 big5.code[0] = big5.code[1] = big5.code[2] = big5.code[3]=0;

  if ( 0x8001 <= sercode && sercode <= 0x805e)
  {
   big5.code[0]=(sercode-0x8001)/0x9d + 0xc6;
   big5.code[1]=(sercode-0x8001)%0x9d + 0xa1;
   return big5.num;
  }

  unsigned serbase,big5base;
  if ( 0x8400 <= sercode && sercode <= 0x8597)
  {
   serbase=0x8400;
   big5base=0xa1;
  }
  else
  {
   if ( 0x8800 <= sercode && sercode <= 0x9d18)
   {
    serbase=0x8800;
    big5base=0xa4;
   }
   else
   {
    if ( 0x9d19 <= sercode && sercode <= 0xbb25)
    {
     serbase=0x9d19;
     big5base=0xc9;
    }
    else
   {
    if ( 0x805f <= sercode && sercode <= 0x616d)
    {
     serbase=0x805f;
     big5base=0xc7;
    }
    else
     return 0 ; // not found big5 code
   }
  }
 }

 big5.code[0]=(sercode-serbase)/0x9d + big5base;
 big5.code[1]=(sercode-serbase)%0x9d;

 if (big5.code[1] < 0x3f)
 {
  big5.code[1]+=0x40;
 }
 else
 {
  big5.code[1]=big5.code[1]-0x3f + 0xa1;
 }
 //std::swap(big5.code[0],big5.code[1]);
 big5.code[2]=big5.code[3]=0;
 return DS::Big5Code(big5.code[0],big5.code[1]);
}
