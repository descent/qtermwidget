#ifndef TEMULATION_PARSER_H
#define TEMULATION_PARSER_H

//#include "observer.h"

namespace DS
{
  class TEmulationParser 
  {
    public:
      virtual ~TEmulationParser()=0;
    private:
  };

  // don't pasre any terminal escape sequense
  class TEmulationNoParser : public TEmulationParser
  {
    public:
    private:
  };
}

#endif
