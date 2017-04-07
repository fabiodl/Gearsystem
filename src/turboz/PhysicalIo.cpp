#include "PhysicalIo.h"

#ifdef __arm__
#include "piIo.h"

enum {MOSI,CLK,CS,ROMWR,OUT_PIN_N};

static const uint8_t opins[OUT_PIN_N]={
  SPI0_MOSI,SPI0_SCLK,SPI0_CE0_N,GPIO22
};

static const uint8_t ipin=SPI0_MISO;

class IoMap{
public:
  IoMap();
  uint32_t mask;
  uint32_t outs[1<<OUT_PIN_N];  
};


IoMap::IoMap(){
  io_setup();

  mask=0;
  for (int b=0;b<OUT_PIN_N;b++){
    mask|=(1<<opins[b]);
  }
  for (int i=0;i<(1<<OUT_PIN_N);i++){
    outs[i]=0;
    for (int b=0;b<OUT_PIN_N;b++){
      if (i&(1<<b)){
	outs[i]|=(1<<opins[b]);
      }
    }    
  }
}

static IoMap ioMap;

void PhysicalIo::write(uint8_t v){
  io_bus_setVal(ioMap.mask,ioMap.outs[v]);
}

bool PhysicalIo::read(){
  return io_pin_get(ipin);
}


#else

void PhysicalIo::write(uint8_t v){}
bool PhysicalIo::read(){return false;}


#endif
