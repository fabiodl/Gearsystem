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
  void setAddress();
  void eval();
  void sanityCheck();
  uint8_t inspectRead(size_t addr);
  std::mutex access;
  FlashMemory* flash;
  Sram sram,ioram,onboardRam;
  class Bus{
  public:
    uint16_t bankAddr;
    uint8_t slotAddr;
    uint8_t ioramData;
    uint8_t data;
    void setAddress(uint16_t addr);
  };
  Bus bus;
  bool inspecting;
public:
  RawMemoryInterface sramIf,ioramIf,onboardIf;
  LambdaMemoryInterface flashIf;
  AddrspaceInterface addrIf;
  WriteTracker onboardTracker;
};

#endif

#endif

