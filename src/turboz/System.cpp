#include <chrono>

#include "System.h"
#include "fileUtils.h"
#include <stdexcept>

System::System():
  rule(&memory,&cartridge),
  processor(&memory),
  disassembly(&rule.addrIf,&symbols),
  addrFind(symbols,processor)
{
  //add exceptions when it returns false 
  //loadCartridge("/home/fabio/dev/s/workspace/mapper/mapper.sg");
  memory.Init();
  processor.Init();
  memory.SetCurrentRule(&rule);
  processor.SetIOPorts(&dummyIO);

}


void System::loadCartridge(const std::string& s){  
  symbols.clear();
  disassembly.invalidateAll();
  /*if (!cartridge.LoadFromFile(s.c_str())){
    throw std::runtime_error((std::string("Invalid ROM ")+s).c_str());
    }*/
  cartridge.load("/home/fabio/dev/s/workspace/mapper/mapper.sg");
  symbols.loadFromFile("/home/fabio/dev/s/workspace/mapper/mapper.sym");

  cartridge.append(s,0x100000);
  
  size_t lastDot=s.find_last_of(".");
  
  if (lastDot!=std::string::npos){
    std::string symName=s.substr(0,lastDot)+".sym";
    if (file_exists(symName )){
      //std::cout<<"Found symbol file"<<symName<<std::endl;
      symbols.loadFromFile(symName);
    }else{
      //std::cout<<"No symbol file"<<symName<<std::endl;
    }
  }
  //memory.Init();  
}



unsigned int System::Tick(){
  disassembly.enter(processor.GetPC());
  //std::cout<<"tick"<<std::endl;
  return processor.Tick();
}


void System::Reset(){
  processor.SetPC(0);
}
