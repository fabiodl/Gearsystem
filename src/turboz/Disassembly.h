#ifndef __DISASSEMBLY__
#define __DISASSEMBLY__

#include <set>
#include <stdint.h>
#include <Memory.h>
#include "Symbols.h"

class Disassembly{

public:
  Disassembly(Memory* mem,Symbols* sym);

  enum State{
    Unknown,
    PredictedHead,
    PredictedTail,
    ConfirmedHead,
    ConfirmedTail
  };    

  void invalidate(uint16_t addr);
  void enter(uint16_t addr);

    
  class Info{
  public:
    int t_states,t_states2;
    int length;
  };

  class Format{
  public:
    char immediate;
    Format();
  };
  
  Info disassemble(char* dis, int disMaxLength, uint16_t addr);
  Info disassembleWithSymbols(char* dis, int disMaxLength, uint16_t addr);
  
  State getState(uint16_t addr);//if unknown it computes it and returns the prediction
  uint8_t getData(uint16_t addr);
  Symbols& getSymbols(){return sym;}
  void setFormat(Format& format);
  int getInstructionLength(uint16_t addr);
private:
    
  static const size_t MEMORYSIZE=64*1024;
  State state[MEMORYSIZE];
  Memory* memory;
  Symbols& sym;
  Format format;
};



#endif
