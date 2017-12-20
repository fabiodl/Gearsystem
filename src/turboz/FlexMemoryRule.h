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
private:
  void defaultInputs();
  uint8_t sRam[2][16*1024]; 
  std::mutex access;
  FlashMemory* flash;
};

#endif

#endif

