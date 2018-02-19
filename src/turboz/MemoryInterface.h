#ifndef _MEMORYINTERFACE_H_
#define _MEMORYINTERFACE_H_

#include "Memory.h"
#include <functional>

class MemoryInterface:public Memory{
public:
  virtual size_t size()=0;
};

class AddrspaceInterface:public MemoryInterface{
public:
  AddrspaceInterface(MemoryRule* rule);
  size_t size();
};

class RawMemoryInterface:public MemoryInterface,public MemoryRule{
public:
  RawMemoryInterface(uint8_t* p,size_t s);
  size_t size();
  u8 PerformRead(u16 address);
  void PerformWrite(u16 address, u8 value);
  void Reset();
private:
  uint8_t* data;
  size_t dataSize;
};

class LambdaMemoryInterface:public MemoryInterface,public MemoryRule{
  public:
  LambdaMemoryInterface(std::function<uint8_t*()> accessf,
                        std::function<size_t()> sizef
                     );
  size_t size();
  u8 PerformRead(u16 address);
  void PerformWrite(u16 address, u8 value);
  void Reset();
private:
  std::function<size_t()> sizef;
  std::function<uint8_t*()> accessf;
  
};

#endif
