#include "FlexMemoryRule.h"
#include "Memory.h"
#include "Cartridge.h"


enum RamCfg{BANKSELECT,SLOT2RAM_EN,SLOT3RAM_EN};
enum MapperTypes{SEGA,CODEMASTERS,KOREAN};      
enum Outs{MOSI,CLK,CS,ROMWR};
enum Slots{SLOT0,SLOT1,SLOT2,SLOT3};

inline uint8_t getSlot(uint16_t addr){
  return (addr&0xC000)>>14;
}

inline uint8_t lowAddr(uint16_t addr){
  return addr& ((1<<14) -1);
}
     
void readLogic(bool &ramEn,bool& romEn, uint8_t& romAddr,uint16_t addr,uint8_t* bank,bool* ramConfig){
  uint8_t slot=getSlot(addr);
  ramEn=(ramConfig[SLOT2RAM_EN]&&(slot==SLOT2))||(ramConfig[SLOT3RAM_EN]&&(slot==SLOT3));
  romEn=(slot==SLOT0)|| (slot==SLOT1) || ((slot==SLOT2) && !ramConfig[SLOT2RAM_EN]);
  romAddr=(slot==SLOT3||addr<0x400)?0:
    slot==SLOT0?bank[0]:
    slot==SLOT1?bank[1]:
    bank[2];
  //std::cout<<std::hex<<addr<<" slot "<<(int)slot<<"romEn "<<romEn<<std::endl;
}


void writeLogic(bool* setSlot,uint8_t mt,uint16_t addr){
  setSlot[0]=(mt==SEGA && addr==0xFFFD) || (mt==CODEMASTERS && addr==0x0000);
  setSlot[1]=(mt==SEGA && addr==0xFFFE) || (mt==CODEMASTERS && addr==0x4000);
  setSlot[2]=(mt==SEGA && addr==0xFFFF) || (mt==CODEMASTERS && addr==0x8000) || (mt==KOREAN && addr==0xA000);
  setSlot[3]=(mt==SEGA && addr==0xFFFC);
}



FlexMemoryRule::FlexMemoryRule(Memory* pMemory, Cartridge* pCartridge):
  MemoryRule(pMemory, pCartridge)
{
  Reset();
}

void FlexMemoryRule::expandAddress(uint16_t addr){
  sram_haddr=(ffRam[BANKSELECT] && (getSlot(addr)==SLOT2)) || (!masterLock && getMiso());
  readLogic(sramEn,romEn,romAddr,addr,ffSlot,ffRam);
}

u8 FlexMemoryRule::PerformRead(u16 addr){
  expandAddress(addr);
  if (sramEn){
    //std::cout<<std::hex<<addr<<" sram"<<std::endl;
    return sRam[sram_haddr][lowAddr(addr)];
  }
  if (romEn){
    //std::cout<<std::hex<<addr<<" rom"<<std::endl;
    auto size=m_pCartridge->GetROMSize();
    if (!size) return 0;    
    return m_pCartridge->GetROM()[(lowAddr(addr)+(romAddr<<14))%size];
  }
  //std::cout<<std::hex<<addr<<" ram"<<std::endl;
  return m_pMemory->Retrieve(addr);
  
}

void FlexMemoryRule::PerformWrite(u16 addr, u8 value){
  expandAddress(addr);
  bool masterWrite=addr==0x0000 && !masterLock;

  if (masterWrite){
    masterLock=value&0x01;
    mt=(value>>1)&0x03;
    highRom=(value>>3)&0x01;
  }
  bool setSlot[4];
  writeLogic(setSlot,mt,addr);
  for (int i=0;i<3;i++){
    if (setSlot[i]){
      ffSlot[i]=value;
    }    
  }
  if (setSlot[3]){
    for(int i=0;i<3;i++){
      ffRam[i]=value&(1<<(i+2));
    }
  }

  if (sramEn){
    sRam[sram_haddr][lowAddr(addr)]=value;
  }else if(!romEn){
    m_pMemory->Load(addr,value);
  }
  uint8_t outs=ffSlot[2]&0x0F;
  if (masterLock){
    outs|=(1<<ROMWR)|(1<<CS)|(1<<MOSI);        
    outs&=~(1<<CLK);
  }
  writeOuts(outs);
}

void FlexMemoryRule::Reset(){  
  masterLock=false;
}


#ifdef __arm__


#else
bool FlexMemoryRule::getMiso(){
  return false;
}


void FlexMemoryRule::writeOuts(uint8_t v){  
}
#endif
