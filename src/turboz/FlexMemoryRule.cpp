#include "FlexMemoryRule.h"
#include "Memory.h"
#include "Cartridge.h"

FlexMemoryRule::FlexMemoryRule(Memory* pMemory, Cartridge* pCartridge):
  MemoryRule(pMemory, pCartridge)
{
  
}

u8 FlexMemoryRule::PerformRead(u16 address){
  if (address<0xC000&&address<m_pCartridge->GetROMSize()){   
    return m_pCartridge->GetROM()[address];
  }else{
    return m_pMemory->Retrieve(address);
  }
}

void FlexMemoryRule::PerformWrite(u16 address, u8 value){
  m_pMemory->Load(address,value);  
}

void FlexMemoryRule::Reset(){  
}
