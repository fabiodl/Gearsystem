#include "FlexMemoryRule.h"
#include <mapper/obj_dir/Vmapper.h>
#include "PhysicalIo.h"
#include "Cartridge.h"
#include "Memory.h"

Vmapper m;


FlexMemoryRule::FlexMemoryRule(Memory* pMemory, Cartridge* pCartridge):
  MemoryRule(pMemory,pCartridge)
{
  
}

void FlexMemoryRule::defaultInputs(){
  m.reset=0;
  m.mreq=0;
  m.ce=0;
  m.miso=PhysicalIo::read();  
}


u8 FlexMemoryRule::PerformRead(u16 address){
  std::lock_guard<std::mutex> lock(access);
  defaultInputs();
  m.addr=address;
  m.wr=0;
  uint16_t lowAddr=address&((1<<14)-1);
  bool isBank3=0xC000<=address && address<=0XFFFF;
  m.eval();
  if (!m.ramCe && !m.romCe){
    std::cerr<<"SRAM-ROM CONFLICT!"<<std::endl;
  }
  if (!m.ramCe && !m.killmem && isBank3){
    std::cerr<<"SRAM-RAM CONFLICT!"<<std::endl;
  }
  if (m.ramCe){ //assume rd
    return sRam[m.ram_haddr][lowAddr];
  }else if (m.romCe){
    if (!m_pCartridge->GetROMSize()) return 0;
    int idx=(m.rom_uhaddr<<(14+6))+
      (m.rom_haddr<<14)+
      lowAddr;
    return m_pCartridge->GetROM()[idx%m_pCartridge->GetROMSize()];
  }else if (isBank3){
    return m_pMemory->Retrieve(address);
  }else{
    std::cerr<<"reading floating bus!"<<std::endl;
    return 0;
  }
    
}//performRead
  
void FlexMemoryRule::PerformWrite(u16 address, u8 value){
    std::lock_guard<std::mutex> lock(access);
    defaultInputs();
    m.addr=address;
    m.data=value;
    m.wr=0;
    m.eval();
    uint16_t lowAddr=address&((1<<14)-1);
    bool isBank3=0xC000<=address && address<=0XFFFF;
    if (!m.ramCe && !m.ramWr){
      sRam[m.ram_haddr][lowAddr]=value;
    }
    if (!m.romCe && !m.romWr){
      std::cout<<"Flash write not yet implemented"<<std::endl;
    }
    if (isBank3 && !m.killmem){
      m_pMemory->Load(address,value);
    }
    uint8_t val=
      (m.mosi<<0)|(m.clk<<1)|(m.cs<<2);
    PhysicalIo::write(val);
      
  }

void FlexMemoryRule::Reset(){
  m.reset=0;
  m.eval();
  m.reset=1;
}
