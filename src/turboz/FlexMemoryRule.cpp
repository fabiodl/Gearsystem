#include "FlexMemoryRule.h"
#include "Memory.h"
#include "Cartridge.h"
#include "PhysicalIo.h"
#include <sstream>
#include "HardwareCondition.h"

static const uint16_t MASTER_ADDR=0xA000;

enum RamCfg{BANKSELECT,SLOT2RAM_EN,SLOT3RAM_EN};
enum MapperTypes{SEGA,CODEMASTERS,KOREAN};      
enum Outs{MOSI,CLK,CS,ROMWR};
enum Slots{SLOT0,SLOT1,SLOT2,SLOT3};

inline uint8_t getSlot(uint16_t addr){
  return (addr&0xC000)>>14;
}

inline uint16_t lowAddr(uint16_t addr){
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
  sram_haddr=(getSlot(addr)==SLOT2) && (ffRam[BANKSELECT] || (!masterLock && getMiso()) );
  readLogic(sramEn,romEn,romAddr,addr,ffSlot,ffRam);
}

std::string FlexMemoryRule::describe(){
  std::lock_guard<std::mutex> lock(access);
  std::stringstream ss;
  ss<<std::hex;
  for (int i=0;i<3;i++){
    ss<<"ffSlot["<<i<<"]="<<(int)ffSlot[i]<<std::endl;
  }
  for (int i=0;i<3;i++){
    ss<<"ffRam["<<i<<"]="<<ffRam[i]<<std::endl;
  }
  ss<<"mt="<<(int)mt<<std::endl;
  ss<<"masterLock="<<masterLock<<std::endl;
  ss<<"sramEn="<<sramEn<<" romEn="<<romEn<<" highRom="<<highRom<<" sram_haddr="<<sram_haddr<<std::endl;
  ss<<"romAddr="<<(int)romAddr<<std::endl;
  return ss.str();
}

u8 FlexMemoryRule::PerformRead(u16 addr){
    std::lock_guard<std::mutex> lock(access);
    expandAddress(addr);
    if (sramEn){
      //if (!lowAddr(addr))
      //std::cout<<"reading "<<addr<<" sram["<<sram_haddr<<"]["<<lowAddr(addr)<<"]"<<std::endl;
      return sRam[sram_haddr][lowAddr(addr)];
    }
    if (romEn){
      auto size=m_pCartridge->GetROMSize();
      if (!size) return 0;    
      //std::cout<<std::hex<<addr<<" rom addr"<<((lowAddr(addr)+(romAddr<<14))%size)<<std::endl;
      
      
      return m_pCartridge->GetROM()[(lowAddr(addr)+(romAddr<<14))%size];
    }
    //std::cout<<std::hex<<addr<<" ram"<<std::endl;
    return m_pMemory->Retrieve(addr);
    
}

void FlexMemoryRule::PerformWrite(u16 addr, u8 value){
  std::lock_guard<std::mutex> lock(access);
  expandAddress(addr);
  bool masterWrite=addr==MASTER_ADDR && !masterLock;

  if (masterWrite){
    masterLock=value&0x01;
    mt=(value>>1)&0x03;
    std::cout<<"mt="<<(int)mt<<std::endl;
    if (mt==2){
      std::cout<<"wrote masterValue"<<(int)value<<std::endl;
    }
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
  bool sram_wr_en=! (!masterLock && mt==CODEMASTERS);
  if (sramEn&&sram_wr_en){
    //std::cout<<"writing "<<addr<<" sram["<<sram_haddr<<"]["<<lowAddr(addr)<<"]="<<value<<std::endl;
    sRam[sram_haddr][lowAddr(addr)]=value;
  }else if(!(romEn||sramEn) ){
    m_pMemory->Load(addr,value);
  }
  uint8_t outs=ffSlot[2]&0x0F;
  if (masterLock){
    outs|=(1<<ROMWR)|(1<<CS)|(1<<MOSI);        
    outs&=~(1<<CLK);
  }
  writeOuts(outs);
}


void FlexMemoryRule::writeOuts(uint8_t v){  
  PhysicalIo::write(v);
}

bool FlexMemoryRule::getMiso(){
  return  PhysicalIo::read();
}


void FlexMemoryRule::Reset(){
  for (int i=0;i<3;i++){
    ffSlot[i]=i;
  }
  ffSlot[3]=0;
  for (int i=0;i<3;i++){
    ffRam[i]=false;
  }    
  mt=0;
  masterLock=false;
  memset(sRam,0,2*16*1024);
}
