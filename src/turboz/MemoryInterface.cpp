#include "MemoryInterface.h"

AddrspaceInterface::AddrspaceInterface(MemoryRule* rule,
                                       std::function<uint8_t(size_t)> _readf,
                                       std::function<void(size_t,uint8_t)> _writef
                                       ):
  readf(_readf),writef(_writef)
{
  SetCurrentRule(rule);
}

size_t AddrspaceInterface::size(){
  return 0x10000;
}

uint8_t AddrspaceInterface::inspectRead(size_t addr){
  return readf(addr);
}

void AddrspaceInterface::inspectWrite(size_t addr,uint8_t val){
  writef(addr,val);
}


RawMemoryInterface::RawMemoryInterface(uint8_t* p,size_t s):
  MemoryRule(this,nullptr),
  data(p),dataSize(s)
{
  //SetCurrentRule(this);
}


uint8_t RawMemoryInterface::inspectRead(size_t addr){
  addr=addr%dataSize;
  return data[addr];
}
                                       
void RawMemoryInterface::inspectWrite(size_t addr,uint8_t val){
  addr=addr%dataSize;
  data[addr]=val;  
}


u8 RawMemoryInterface::PerformRead(u16 addr){
  return inspectRead(addr);
}


void RawMemoryInterface::PerformWrite(u16 addr, u8 value){
  inspectWrite(addr,value);
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


uint8_t LambdaMemoryInterface::inspectRead(size_t addr){
  if (!size()) return 0;
  addr=addr%size();
  return accessf()[addr];  
}

void LambdaMemoryInterface::inspectWrite(size_t addr,uint8_t value){
  if (!size()) return;
  addr=addr%size();
  accessf()[addr]=value;
}


u8 LambdaMemoryInterface::PerformRead(u16 addr){
  return inspectRead(addr);
}


void LambdaMemoryInterface::PerformWrite(u16 addr, u8 value){
  inspectWrite(addr,value);
}

void LambdaMemoryInterface::Reset(){}
