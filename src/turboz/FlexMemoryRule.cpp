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
Vtop m;
VtopPrinter mprinter(m);

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
  sram(32*1024),ioram(32*1024)
{
  
}



class CheckMultiwrite{
  const std::string* firstName;

  CheckMultiwrite():firstName(nullptr){}
public:
  void add(const std::string& name,bool v){
    if (v){
      if (firstName==nullptr){
        firstName=&name;
      }else{
        std::cerr<<"Multiple write! data bus"<<*firstName<<" and "<<name<<std::endl;
      }
    }
  }
};




void FlexMemoryRule::sanityCheck(){
  if (m.isBusDataOut){
    data=m.busDataOut;
  }

  if (m.bufferCe){
    CheckMultiwrite mw;
    mw.add("smsram",0xC000<=address && address<=0XFFFF&&!m.killmem);
    mw.add("sram",!m.sram_ce&&!m.ram_rd);
    mw.add("flash",!m.flash_ce&&!m.flash_rd);
    mw.add("cpld",isBusDataOut);
    mw.add("cpu",!m._wr);
    if (m.isBankAddrOut){
      std::cerr<"Multiple write! address bus"<<std::endl;
    }
  }

  
  if (m.isIoramDataOut&&ioram_rd){
      std::cerr<"Multiple write! ioram data bus"<<std::endl;
  }
  

}

void FlexMemoryRule::exchange(u16 address,u8& data){
  std::lock_guard<std::mutex> lock(access);

  
  uint16_t bankAddr=address&((1<<14)-1);
  uint8_t slotAddr=address>>14;
  uint8_t ioramData;
  
  m.slotAddr=slotAddr;
  m.bankAddrIn=bankAddr;

  m.miso=PhysicalIo::read();  

  m.eval();//fix the ioramAddr

  ioram.eval(m.ioramAddr,ioramData,true,m.ioram_wr,m.ioram_rd);

  m.eval();

  PhysicalIo::write((m.mosi<<0)+
                    (m.clk<<1)+
                    (m.cs<<2)
                    );
  
  if (m.isBankAddrOut){
    bankAddr=m.bankAddrOut;
  }

  if (m.isBusDataOut){
    data=busDataOut;
  }

  uint32_t flashFullAddr=
    (m.flashUHaddr<<(14+6))+
    (m.flashHaddr<<14)+
    bankAddr;
  
  flash->eval(flashFullAddr,data,m.flash_ce,m.flash_wr,m._rd);

  uint32_t sramFullAddr=(m.sramHaddr<<14)+bankAddr;

  sram.eval(sramFullAddr,data,m.sram_ce,m._wr,m._rd);
  ioram.eval(m.ioramAddr,ioramData,true,m.ioram_wr,m.ioram_rd);


  sanityCheck();

}


void FlexMemoryRule::defaultInputs(){
  m._mreq=0;
  m._ce=0;
  m._unlockOnPause=1;

}


u8 FlexMemoryRule::PerformRead(u16 address){
  uint8_t data;
  m._rd=0;
  m._wr=1;
  exchange(address,data);
  return data;

  

  
  if (!m.sram_ce && !m.flash_ce){
    std::cerr<<"SRAM-ROM CONFLICT!"<<std::endl;
  }
  if (!m.sram_ce && !m.killmem && isBank3){
    std::cerr<<"SRAM-RAM CONFLICT!"<<std::endl;
  }
  if (!m.sram_ce){ //assume rd
    return sRam[m.sramHaddr][bankAddr];
  }else if (!m.flash_ce){
    if (!flash->GetROMSize()){
      //std::cout<<"Empty cartridge"<<std::endl;
      return 0;
    }
    int idx=(m.flashUHaddr<<(14+6))+
      (m.flashHaddr<<14)+
      bankAddr;
    //std::cout<<"rom addr"<<idx<<std::endl;
    return flash->GetROM()[idx%flash->GetROMSize()];
  }else if (isBank3){
    return m_pMemory->Retrieve(address);
  }
  std::cerr<<"reading floating bus!"<<std::endl;
  return 0;
 
}//performRead
  
void FlexMemoryRule::PerformWrite(u16 address, u8 value){
    std::lock_guard<std::mutex> lock(access);
    uint16_t bankAddr=address&((1<<14)-1);
    bool isBank3=0xC000<=address && address<=0XFFFF;
    defaultInputs();
    m.slotAddr=address>>14;
    m.bankAddr=bankAddr;
    m.data=value;
    m._wr=0;
    m.eval();
    cout<<"wrote "<<hex<<(int)value<<" to "<<hex<<address<<dec<<endl;
    std::cout<<"bankAddr"<<hex<<bankAddr<<dec<<endl;
    std::cout<<describe(-1);

    //printState();


    //std::cout<<"write "<<std::hex<<address<<std::dec<<" ramCe"<<(bool)m.ramCe<<"romCe"<<(bool)m.romCe<<"isBank3"<<isBank3<<"killMem"<<(bool)m.killmem<<std::endl;
    if (!m.sram_ce && !m._wr){
      sRam[m.sramHaddr][bankAddr]=value;
    }
    if (!m.flash_ce && !m.flash_wr){
      std::cout<<"Flash write not yet implemented"<<std::endl;
    }
    if (isBank3 && !m.killmem){
      m_pMemory->Load(address,value);
    }
    /*uint8_t val=
      (m.mosi<<0)|(m.clk<<1)|(m.cs<<2);
    PhysicalIo::write(val);
    */
  }

void FlexMemoryRule::Reset(){
}

void FlexMemoryRule::Tick(){
  std::lock_guard<std::mutex> lock(access);
  m.cpldClk=~m.cpldClk;
  m.eval();
}



FlexMemoryRule::Sram::Sram(size_t size):mem(size){}

void FlexMemoryRule::Sram::eval(uint32_t addr,uint8_t &data,bool _ce,bool _we,bool _oe){
  if (_ce){
    return;
  }
  if (!_oe && !_we){
    std::cerr<<"SRAM memory has simulataneously _oe and _we low"<<std::endl;
  }
  if (!_we){
    mem[addr]=data;
  }
  if (!_oe){
    data=mem[addr];
  }
      
}

#endif
