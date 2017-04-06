#ifndef _FLEXMEMORYRULE_H_
#define	_FLEXMEMORYRULE_H_

#include "MemoryRule.h"

class FlexMemoryRule : public MemoryRule
{
public:
  FlexMemoryRule(Memory* pMemory, Cartridge* pCartridge);
  virtual u8 PerformRead(u16 address);
  virtual void PerformWrite(u16 address, u8 value);
  virtual void Reset();

private:
  uint8_t ffSlot[3];
  bool ffRam[3];
  uint8_t flex;
  uint8_t mt;
  bool masterLock;
  
  
  uint8_t sRam[2][16*1024];
  bool getMiso();
  void writeOuts(uint8_t);
  void expandAddress(uint16_t addr);
  bool sramEn,romEn;
  uint8_t romAddr;
  bool highRom;
  bool sram_haddr;
};

#endif

