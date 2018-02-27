#ifndef _MEMORYINTERFACE_H_
#define _MEMORYINTERFACE_H_

#include "Memory.h"
#include <functional>

class MemoryInterface:public Memory{
public:
  virtual size_t size()=0;
  virtual uint8_t inspectRead(size_t addr)=0;
  virtual void inspectWrite(size_t addr,uint8_t val)=0;
};

class AddrspaceInterface:public MemoryInterface{
public:
  AddrspaceInterface(MemoryRule* rule,
                     std::function<uint8_t(size_t)> readf,
                     std::function<void(size_t,uint8_t)> writef);
  size_t size();
  uint8_t inspectRead(size_t addr);
  void inspectWrite(size_t addr,uint8_t val);
private:
  std::function<uint8_t(size_t)> readf;
  std::function<void(size_t,uint8_t)> writef;
};

class RawMemoryInterface:public MemoryInterface,public MemoryRule{
public:
  RawMemoryInterface(uint8_t* p,size_t s);
  size_t size();
  uint8_t inspectRead(size_t addr);
  void inspectWrite(size_t addr,uint8_t val);
  

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
  uint8_t inspectRead(size_t addr);
  void inspectWrite(size_t addr,uint8_t val);


  u8 PerformRead(u16 address);
  void PerformWrite(u16 address, u8 value);
  void Reset();
private:
  std::function<size_t()> sizef;
  std::function<uint8_t*()> accessf;
  
};

#endif
