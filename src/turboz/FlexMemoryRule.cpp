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
  sramIf(sram.getRaw(),sram.size()),
  ioramIf(ioram.getRaw(),ioram.size()),
  onboardIf(onboardRam.getRaw(),onboardRam.size()),
  flashIf([this](){return flash->GetROM();},[this](){return flash->GetROMSize();}),
  addrIf(this)
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

void FlexMemoryRule::eval(u16 address,u8& data,u8& ioramData){

  uint16_t bankAddr=address&((1<<14)-1);
  uint8_t slotAddr=address>>14;
  
  m->slotAddr=slotAddr;
  m->bankAddrIn=bankAddr;
  m->busDataIn=data;
  m->miso=PhysicalIo::read();  

  m->eval();

  PhysicalIo::write((m->mosi<<0)+
                    (m->clk<<1)+
                    (m->cs<<2)
                    );
  
  if (m->isBankAddrOut){
    bankAddr=m->bankAddrOut;
  }

  if (m->isBusDataOut){
    data=m->busDataOut;
  }

  if(m->isIoramDataOut){
    ioramData=m->ioramDataOut;
  }

  uint32_t flashFullAddr=
    (m->flashUHaddr<<(14+6))+
    (m->flashHaddr<<14)+
    bankAddr;
  
  flash->eval(flashFullAddr,data,m->flash_ce,m->flash_wr,m->_rd);

  uint32_t sramFullAddr=(m->sramHaddr<<14)+bankAddr;

  sram.eval(sramFullAddr,data,m->sram_ce,m->_wr,m->_rd);
  ioram.eval(m->ioramAddr,ioramData,0,m->ioram_wr,m->ioram_rd);

  bool isOnboardRam=0xC000<=address && address<=0XFFFF&&!m->killmem;
  onboardRam.eval(address,data,!isOnboardRam,m->_wr,m->_rd);

  onboardTracker.eval(address,data,!isOnboardRam,m->_wr);
  
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
  uint8_t data,ioramData;
  eval(address,data,ioramData);
  m->_rd=0;
  eval(address,data,ioramData);  
  m->_rd=1;
  return data; 
}//performRead




void FlexMemoryRule::PerformWrite(u16 address, u8 value){
  std::lock_guard<std::mutex> lock(access);
  uint8_t ioramData;
  m->_wr=0;
  eval(address,value,ioramData);
  m->_wr=1;
}

void FlexMemoryRule::Reset(){
  delete m;
  m=new Vtop();
  defaultInputs();
}

void FlexMemoryRule::Tick(){
  std::lock_guard<std::mutex> lock(access);
  m->cpldClk=~m->cpldClk;
  m->eval();
}


void FlexMemoryRule::ToggleMenu(){
  std::lock_guard<std::mutex> lock(access);
  m->_unlockOnPause=!m->_unlockOnPause;
}

#endif
