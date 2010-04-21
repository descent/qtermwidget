#ifndef STR_TO_QSTR_H
#define STR_TO_QSTR_H


#include "convertencode.h"
#include "qtextcodec.h"

#include <qstring.h>

#include <string>


namespace DS
{
  inline int qstr_to_big5str(const QString &qstr, std::string &str)
  {
    DS::ConvertEncode convert_encode("big5","ucs-2le");
    //const QChar *qchar=new QChar [qstr.length()];
    //QChar qchar[qstr.length()];
    const QChar *qchar;
    qchar=qstr.unicode();

    std::vector<int> unicode;
    for (size_t i=0 ; i < qstr.length() ; ++i)
      unicode.push_back(qchar[i].unicode());
    if (convert_encode.convert(unicode,str)!=0)
      return -1;
    else
      return 0;
  }

  inline int big5str_to_qstr(const std::string &str, QString &qstr)
  {
    DS::ConvertEncode convert_encode;
    std::vector<int> unicode;
    if (convert_encode.convert(str, unicode)!=0)
      return -1;

    QChar *qchar_array = new QChar [unicode.size()];
    for (std::vector<int>::size_type i=0 ; i < unicode.size() ; ++i)
      qchar_array[i]=QChar(unicode[i]);
    //qDebug("after convert");
    qstr = QString(qchar_array, unicode.size());

    return 0;

  }


  inline QString string2qstring (const std::string &str, const char *codename="big5")
  {
    if (codename == 0)             // ASCII
    {
      return QString (str.c_str ());
    }
    else
    {
      QTextCodec *codec = QTextCodec::codecForName (codename);
      if (codec)
        return QString (codec->toUnicode (str.c_str ()));
    }
    return QString("");
  }  
}


#endif
