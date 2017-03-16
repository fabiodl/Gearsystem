#include <chrono>

#include "System.h"

System::System():
  rule(&memory,&cartridge),
  processor(&memory),
  disassembly(&memory,&symbols)
{
  //add exceptions when it returns false
  cartridge.LoadFromFile("/home/fabio/dev/s/workspace/mapper/mapper.sg");
  symbols.loadFromFile("/home/fabio/dev/s/workspace/mapper/mapper.sym");
  memory.Init();
  memory.SetCurrentRule(&rule);
  processor.SetIOPorts(&dummyIO);
  disassembly.enter(0);
  disassembly.enter(0x66);
  disassembly.enter(0x28);
  breakpoints.add(0x12);
}


unsigned int System::Tick(){
  disassembly.enter(processor.GetPC());
  //std::cout<<"tick"<<std::endl;
  return processor.Tick();
}


void System::Reset(){
  processor.SetPC(0);
}
