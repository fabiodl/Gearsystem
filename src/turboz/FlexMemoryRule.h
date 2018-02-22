//#define MANUAL_FLEXMEMORYRULE

#ifdef MANUAL_FLEXMEMORYRULE
#include "Manual_FlexMemoryRule.h"
#else

#ifndef _FLEXMEMORYRULE_H_
#define	_FLEXMEMORYRULE_H_
#include <mutex>
#include "MemoryInterface.h"
#include "MemoryRule.h"
#include "FlashCartridge.h"
#include "Sram.h"
#include "Tracker.h"



class FlexMemoryRule : public MemoryRule
{
public:
  FlexMemoryRule(Memory* pMemory, FlashCartridge* pCartridge);
  virtual u8 PerformRead(u16 address);
  virtual void PerformWrite(u16 address, u8 value);
  virtual void Reset();  
  void Tick();
  std::string describe(int level) const;
  void ToggleMenu();
private:
  void defaultInputs();
  void eval(u16 address,u8& data,u8& ioramData);
  void sanityCheck();

  std::mutex access;
  FlashMemory* flash;
  Sram sram,ioram,onboardRam;
public:
  RawMemoryInterface sramIf,ioramIf,onboardIf;
  LambdaMemoryInterface flashIf;
  AddrspaceInterface addrIf;
  WriteTracker onboardTracker;
};

#endif

#endif

