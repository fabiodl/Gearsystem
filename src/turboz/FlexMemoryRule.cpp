#include "FlexMemoryRule.h"

#ifdef MANUAL_FLEXMEMORYRULE
#include "Manual_FlexMemoryRule.cpp"
#else

#include <mapper/obj_dir/Vmapper.h>
#include "PhysicalIo.h"
#include "Cartridge.h"
#include "Memory.h"
#include <sstream>

using namespace std;
Vmapper m;


template<typename T> void printHex(const std::string& prefix,T& val,std::ostream& out=cout){
  out<<prefix<<hex<<(int)val<<dec<<endl;
}

std::string FlexMemoryRule::describe() const{
  std::stringstream ss;
  ss<<"{------------"<<endl;
  printHex("addr",m.addr,ss);
  printHex("data",m.data,ss);  
  printHex("setSlot",m.v__DOT__setSlot,ss);
  printHex("ROM HADDR",m.rom_haddr,ss);
  for (int i=0;i<4;i++){
    printHex("banks[" "]",m.v__DOT__banks[i],ss);
  }
  ss<<"------------}"<<endl;
  return ss.str();
}



FlexMemoryRule::FlexMemoryRule(Memory* pMemory, FlashCartridge* pCartridge):
  MemoryRule(pMemory,pCartridge),
  flash(pCartridge)
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
  m.wr=1;
  m.eval();
  //cout<<"read from "<<hex<<address<<dec<<endl;
  //  printState();
  
  uint16_t lowAddr=address&((1<<14)-1);
  bool isBank3=0xC000<=address && address<=0XFFFF;

  
  if (!m.ramCe && !m.romCe){
    std::cerr<<"SRAM-ROM CONFLICT!"<<std::endl;
  }
  if (!m.ramCe && !m.killmem && isBank3){
    std::cerr<<"SRAM-RAM CONFLICT!"<<std::endl;
  }
  if (!m.ramCe){ //assume rd
    return sRam[m.ram_haddr][lowAddr];
  }else if (!m.romCe){
    if (!flash->GetROMSize()){
      //std::cout<<"Empty cartridge"<<std::endl;
      return 0;
    }
    int idx=(m.rom_uhaddr<<(14+6))+
      (m.rom_haddr<<14)+
      lowAddr;
    //std::cout<<"rom addr"<<idx<<std::endl;
    return flash->GetROM()[idx%flash->GetROMSize()];
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
    //cout<<"wrote "<<hex<<(int)value<<" to "<<hex<<address<<dec<<endl;
    //printState();
    uint16_t lowAddr=address&((1<<14)-1);
    bool isBank3=0xC000<=address && address<=0XFFFF;
    //std::cout<<"write "<<std::hex<<address<<std::dec<<" ramCe"<<(bool)m.ramCe<<"romCe"<<(bool)m.romCe<<"isBank3"<<isBank3<<"killMem"<<(bool)m.killmem<<std::endl;
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

void FlexMemoryRule::Tick(){
  m.clk=~m.clk();
  m.eval();
}

#endif
