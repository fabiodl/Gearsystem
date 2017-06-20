#include "ProcessorSymbols.h"
#include "cistring.h"
#include "AddressFinder.h"

ProcessorSymbols::ProcessorSymbols(Processor& p){
  regs["BC"]=&p.BC;
  regs["DE"]=&p.DE;
  regs["HL"]=&p.HL;
  regs["IX"]=&p.IX;
  regs["IY"]=&p.IY;
  regs["SP"]=&p.SP;
  regs["PC"]=&p.PC;
}

bool ProcessorSymbols::isSymbol(const char* s){
  return regs.find(istring(s))!=regs.end();
}

uint16_t ProcessorSymbols::getAddress(const char* s){
  return regs[istring(s)]->GetValue();
}

uint32_t ProcessorSymbols::getAddressExtended(const char* s){
  auto it=regs.find(istring(s));
  if (it==regs.end()){
    return AddressFinder::INVALIDADDR;
  }else{
    return it->second->GetValue();
  }
    
}
