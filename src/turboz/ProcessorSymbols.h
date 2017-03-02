#ifndef _PROCESSORSYMBOLS_H_
#define _PROCESSORSYMBOLS_H_

#include <Processor.h>
#include <map>
#include "cistring.h"

class ProcessorSymbols{
public:
  ProcessorSymbols(Processor&);
  bool isSymbol(const char* s);
  uint16_t getValue(const char* s);
  int32_t getValueExtended(const char* s); //returns -1 when invalid
  bool getMatches(const char* s,void (*cb)(const char*,void*),void* userData=NULL);
private:
  struct caseInsensitive{
  };
  std::map<cistring,SixteenBitRegister*> regs;

  
};




#endif
