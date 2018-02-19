#include "MemoryInterface.h"
RawMemoryInterface::RawMemoryInterface(uint8_t* p,size_t s):
  MemoryRule(this,nullptr),
  data(p),dataSize(s)
{
  SetCurrentRule(this);
}

AddrspaceInterface::AddrspaceInterface(MemoryRule* rule){
  SetCurrentRule(rule);
}

size_t AddrspaceInterface::size(){
  return 0x10000;
}


u8 RawMemoryInterface::PerformRead(u16 addr){
  addr=addr%dataSize;
  return data[addr];
}


void RawMemoryInterface::PerformWrite(u16 addr, u8 value){
  addr=addr%dataSize;
  data[addr]=value;
}


void RawMemoryInterface::Reset(){
}

size_t RawMemoryInterface::size(){
  return dataSize;
}


LambdaMemoryInterface::LambdaMemoryInterface(std::function<uint8_t*()> _accessf,std::function<size_t()> _sizef):
  MemoryRule(this,nullptr),
  sizef(_sizef),accessf(_accessf)
{
    SetCurrentRule(this);
}

size_t LambdaMemoryInterface::size(){
  return sizef();
}

u8 LambdaMemoryInterface::PerformRead(u16 addr){
  if (!size()) return 0;
  addr=addr%size();
  return accessf()[addr];
}


void LambdaMemoryInterface::PerformWrite(u16 addr, u8 value){
  if (!size()) return;
  addr=addr%size();
  accessf()[addr]=value;
}

void LambdaMemoryInterface::Reset(){}
