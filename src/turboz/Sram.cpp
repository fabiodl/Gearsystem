#include "Sram.h"
#include <iostream>

Sram::Sram(size_t size):mem(size){}

void Sram::eval(uint32_t addr,uint8_t &data,bool _ce,bool _we,bool _oe){
  if (_ce){
    return;
  }
  if (!_oe && !_we){
    std::cerr<<"SRAM memory has simulataneously _oe and _we low"<<std::endl;
  }
  addr=addr%mem.size();
  if (!_we){
    mem[addr]=data;
  }
  if (!_oe){
    data=mem[addr];
  }
      
}
