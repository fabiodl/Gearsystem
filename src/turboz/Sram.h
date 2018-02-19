#ifndef _SRAM_H_
#define _SRAM_H_

#include <vector>
#include <stdint.h>
#include <cstddef>

  class Sram{
    std::vector<uint8_t> mem;
  public:
    uint8_t* getRaw();
    size_t size();
    Sram(size_t size);
    void eval(uint32_t addr,uint8_t &data,bool _ce,bool _we,bool _oe);
  };


#endif
