#include "AddressFinder.h"



AddressFinder::AddressFinder(Symbols& _syms,Processor& _proc):
  syms(_syms),procsyms(_proc)
{
}

uint32_t AddressFinder::getHex(const char* s){
  char* end;
  long int res=strtol(s,&end,16);
  if (*end==0){
    return res;
  }else{
    return AddressFinder::INVALIDADDR;
  }
}


uint32_t AddressFinder::getAddressExtended(const char* name){
  uint32_t addr=syms.getAddressExtended(name);
  if (addr!=INVALIDADDR){
    //std::cout<<name<<" is a symbol"<<std::endl;
    return addr;
  }
  addr=procsyms.getAddressExtended(name);
  if (addr!=INVALIDADDR){
    //std::cout<<name<<" is a proc symbol"<<std::endl;
    return addr;
  }
  return getHex(name);
}
