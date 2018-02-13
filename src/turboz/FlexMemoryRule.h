//#define MANUAL_FLEXMEMORYRULE

#ifdef MANUAL_FLEXMEMORYRULE
#include "Manual_FlexMemoryRule.h"
#else

#ifndef _FLEXMEMORYRULE_H_
#define	_FLEXMEMORYRULE_H_


#include "MemoryRule.h"
#include "FlashCartridge.h"
#include <mutex>

class FlexMemoryRule : public MemoryRule
{
public:
  FlexMemoryRule(Memory* pMemory, FlashCartridge* pCartridge);
  virtual u8 PerformRead(u16 address);
  virtual void PerformWrite(u16 address, u8 value);
  virtual void Reset();  
  void Tick();
  std::string describe(int level) const;
private:
  void defaultInputs();
  void exchange(u16 address,u8& data);
  void sanityCheck();
  class Sram{
    std::vector<uint8_t> mem;
  public:
    Sram(size_t size);
    void eval(uint32_t addr,uint8_t &data,bool _ce,bool _we,bool _oe);
  };

  Sram sram,ioram;

  std::mutex access;
  FlashMemory* flash;
};

#endif

#endif

