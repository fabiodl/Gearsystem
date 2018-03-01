#include "FlexMemoryRule.h"

#ifdef MANUAL_FLEXMEMORYRULE
#include "Manual_FlexMemoryRule.cpp"
#else

#include <mapper/obj_dir/Vtop.h>
#include <mapper/obj_dir/VtopPrinter.h>
#include "PhysicalIo.h"
#include "Cartridge.h"
#include "Memory.h"
#include <sstream>

using namespace std;



Vtop *m=new Vtop();
VtopPrinter mprinter(*m);

template<typename T> void printHex(const std::string& prefix,T& val,std::ostream& out=cout){
  out<<prefix<<hex<<(int)val<<dec<<endl;
}

std::string FlexMemoryRule::describe(int level) const{
  std::stringstream ss;
  static int cnt=0;
  ss<<"{------------"<<cnt++<<endl;

  if (level>0){
    for (int i=0;i<=level;i++){
      if (mprinter.level.find(i)!=mprinter.level.end()){
        ss<<mprinter.level[i];
      }else{
        std::cout<<"no level "<<i<<std::endl;
      }
      
    }
  }else{
    for (auto const& l:mprinter.level){
        ss<<l.second;
    }
  }
  ss<<"------------}"<<endl;
  return ss.str();
}



FlexMemoryRule::FlexMemoryRule(Memory* pMemory, FlashCartridge* pCartridge):
  MemoryRule(pMemory,pCartridge),
  flash(pCartridge),
  sram(32*1024),ioram(32*1024),onboardRam(8*1024),
  inspecting(false),
  sramIf(sram.getRaw(),sram.size()),
  ioramIf(ioram.getRaw(),ioram.size()),
  onboardIf(onboardRam.getRaw(),onboardRam.size()),
  flashIf([this](){return flash->GetROM();},[this](){return flash->GetROMSize();}),
  addrIf(this,[this](size_t addr){return inspectRead(addr);},[](size_t,uint8_t){cerr<<"Unimplemented"<<endl;})
{
  Reset();
}



class CheckMultiwrite{
  const char* firstName;

public:
  CheckMultiwrite():firstName(nullptr){}
  void add(const char* name,bool v){
    if (v){
      if (firstName==nullptr){
        firstName=name;
      }else{
        cerr<<"Multiple write! data bus "<<firstName<<" and "<<name<<endl;
      }
    }
  }
};




void FlexMemoryRule::sanityCheck(){
  if (m->bufferCe){
    CheckMultiwrite mw;
    uint16_t address=(m->slotAddr<<14)+m->bankAddrIn;
    mw.add("smsram",0xC000<=address && address<=0XFFFF&&!m->killmem&&!m->_rd);
    mw.add("sram",!m->sram_ce&&!m->_rd);
    mw.add("flash",!m->flash_ce&&!m->_rd);
    mw.add("cpld",m->isBusDataOut);
    mw.add("cpu",!m->_wr);
    if (m->isBankAddrOut){
      cerr<<"Multiple write! address bus"<<endl;
    }
  }

  
  if (m->isIoramDataOut&&!m->ioram_rd){
    cerr<<"Multiple write! ioram data bus"<<endl;
  }
  

}

void FlexMemoryRule::Bus::setAddress(uint16_t address){
  bankAddr=address&((1<<14)-1);
  slotAddr=address>>14;
 
}


void FlexMemoryRule::eval(){

   
  m->slotAddr=bus.slotAddr;
  m->bankAddrIn=bus.bankAddr;
  m->busDataIn=bus.data;
  if (!inspecting){
    m->miso=PhysicalIo::read();
    //std::cout<<"miso="<<(int)m->miso<<std::endl;
  }

  m->eval();

  if (!inspecting){
    PhysicalIo::write((m->mosi<<0)+
                      (m->clk<<1)+
                      (m->cs<<2)
                      );
  }
  if (m->isBankAddrOut){
    bus.bankAddr=m->bankAddrOut;
  }

  if (m->isBusDataOut){
    bus.data=m->busDataOut;
  }

  if(m->isIoramDataOut){
    bus.ioramData=m->ioramDataOut;
  }

  uint32_t flashFullAddr=
    (m->flashUHaddr<<(14+6))+
    (m->flashHaddr<<14)+
    bus.bankAddr;
  
  flash->eval(flashFullAddr,bus.data,m->flash_ce,m->flash_wr,m->_rd);

  uint32_t sramFullAddr=(m->sramHaddr<<14)+bus.bankAddr;

  sram.eval(sramFullAddr,bus.data,m->sram_ce,m->_wr,m->_rd);
  ioram.eval(m->ioramAddr,bus.ioramData,0,m->ioram_wr,m->ioram_rd);


  bool isOnboardRam=bus.slotAddr==3&&!m->killmem;
  onboardRam.eval(bus.bankAddr,bus.data,!isOnboardRam,m->_wr,m->_rd);

  uint16_t zaddress=(bus.slotAddr<<14)|bus.bankAddr;
  onboardTracker.eval(zaddress,bus.data,!isOnboardRam,m->_wr);
  
  sanityCheck();

}


void FlexMemoryRule::defaultInputs(){
  m->_rd=1;
  m->_wr=1;
  m->_mreq=0;
  m->_ce=0;
  m->_unlockOnPause=1;
  m->flashBusy=0;
  m->cpldClk=0;
  m->_ioWr=1;
  m->_ioRd=1;
}


u8 FlexMemoryRule::PerformRead(u16 address){
  std::lock_guard<std::mutex> lock(access);
  bus.setAddress(address);
  eval();
  m->_rd=0;
  eval();  
  m->_rd=1;
  return bus.data; 
}//performRead



u8 FlexMemoryRule::inspectRead(size_t address){
  std::lock_guard<std::mutex> lock(access);
  inspecting=true;
  bus.setAddress(address);
  eval();
  m->_rd=0;
  eval();  
  m->_rd=1;
  inspecting=false;
  return bus.data; 
}


void FlexMemoryRule::PerformWrite(u16 address, u8 value){
  std::lock_guard<std::mutex> lock(access);
  bus.setAddress(address);
  bus.data=value;
  m->_wr=0;
  eval();
  m->_wr=1;
}

void FlexMemoryRule::Reset(){
  delete m;
  m=new Vtop();
  defaultInputs();
}

void FlexMemoryRule::Tick(){
  std::lock_guard<std::mutex> lock(access);
  //std::cout<<"tick"<<std::endl;
  m->cpldClk=~m->cpldClk;
  eval();
}


void FlexMemoryRule::ToggleMenu(){
  std::lock_guard<std::mutex> lock(access);
  m->_unlockOnPause=!m->_unlockOnPause;
}

#endif
