#ifndef _ADDRESSFINDER_H_
#define _ADDRESSFINDER_H_

#include "Symbols.h"
#include "ProcessorSymbols.h"

class AddressFinder{
public:
  static const uint32_t INVALIDADDR=0xFFFFFFFF;
  AddressFinder(Symbols& syms,Processor& proc);
  uint32_t getAddressExtended(const char* name);
  static uint32_t  getHex(const char* name);
  Symbols& syms;
private:

  ProcessorSymbols procsyms;
};
                  




#endif
