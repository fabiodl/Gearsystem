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
  if (m->dataBufferCe){
    CheckMultiwrite mw;
    mw.add("onboardram",m->slotAddr==3&&!m->killmem&&!m->_rd);
    mw.add("sram",!m->sram_ce&&!m->_rd);
    mw.add("flash",!m->flash_ce&&!bus.flash_rd);
    mw.add("cpld",m->isBusDataOut);
    mw.add("cpu",!m->_wr);
  }else{
    if (!m->flash_wr&&!m->isBusDataOut){
      cerr<<"Writing to flash with a floating bus"<<endl;      
    }    
    if (!m->_rd&&!m->_mreq&&m->slotAddr!=3&&!inspecting){
      cerr<<"Reading from floating bus"<<endl;
      cout<<describe(-1)<<endl;
    }
  }
  if (m->addrBufferCe&&m->isBankAddrOut){
      cerr<<"Multiple write! address bus"<<endl;
  }
  
  if (m->isIoramDataOut&&!m->ioram_rd){
    cerr<<"Multiple write! ioram data bus"<<endl;
  }
  

}

void FlexMemoryRule::Bus::setAddress(uint16_t address){
  systemBankAddr=address&((1<<14)-1);
  systemSlotAddr=address>>14; 
}

void FlexMemoryRule::Bus::systemToLocal(bool addrEn,bool dataEn){
  if (addrEn){
    localBankAddr=systemBankAddr;
  }
  if (dataEn){
    localData=systemData;
  }
}


void FlexMemoryRule::Bus::localToSystem(bool addrEn,bool dataEn){
  /*
    address shold never go to the system bus, handled in sanityCheck
   */
  if (dataEn){
    systemData=localData;
  }
}



void FlexMemoryRule::eval(){

  
  bus.systemToLocal(m->addrBufferCe,m->dataBufferCe);
                    
  m->slotAddr=bus.systemSlotAddr;
  m->bankAddrIn=bus.localBankAddr;
  m->busDataIn=bus.localData;
  if (!inspecting){
    m->miso=PhysicalIo::read();
    //std::cout<<"miso="<<(int)m->miso<<std::endl;
  }
  m->flashBusy=flash->isBusy();
  m->eval();

  
  if (!inspecting){
    PhysicalIo::write((m->mosi<<0)+
                      (m->clk<<1)+
                      (m->cs<<2)
                      );
  }
  if (m->isBankAddrOut){
    bus.localBankAddr=m->bankAddrOut;
  }

  if (m->isBusDataOut){
    bus.localData=m->busDataOut;
  }

  if(m->isIoramDataOut){
    bus.ioramData=m->ioramDataOut;
  }

  uint32_t flashFullAddr=
    (m->flashUHaddr<<(14+6))+
    (m->flashHaddr<<14)+
    bus.localBankAddr;

  bus.flash_rd=m->addrBufferCe?m->_rd:true; //assumes a pull-up after the buffer
  
  flash->eval(flashFullAddr,bus.localData,m->flash_ce,m->flash_wr,bus.flash_rd);

  uint32_t sramFullAddr=(m->sramHaddr<<14)+bus.localBankAddr;

  sram.eval(sramFullAddr,bus.localData,m->sram_ce,m->_wr,m->_rd);
  ioram.eval(m->ioramAddr,bus.ioramData,0,m->ioram_wr,m->ioram_rd);

  bus.localToSystem(m->addrBufferCe,m->dataBufferCe);
  
  bool isOnboardRam=bus.systemSlotAddr==3&&!m->killmem;
  onboardRam.eval(bus.systemBankAddr,bus.systemData,!isOnboardRam,m->_wr,m->_rd);

  uint16_t zaddress=(bus.systemSlotAddr<<14)|bus.systemBankAddr;
  onboardTracker.eval(zaddress,bus.systemData,!isOnboardRam,m->_wr);
  
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
  return bus.systemData; 
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
  return bus.systemData; 
}


void FlexMemoryRule::PerformWrite(u16 address, u8 value){
  std::lock_guard<std::mutex> lock(access);
  bus.setAddress(address);
  bus.systemData=value;
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
  flash->tick();
}


void FlexMemoryRule::ToggleMenu(){
  std::lock_guard<std::mutex> lock(access);
  m->_unlockOnPause=!m->_unlockOnPause;
}

#endif
