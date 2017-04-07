#ifndef _PIIO_H_
#define _PIIO_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

  void io_setup();
  void io_pin_set(uint8_t pin);
  void io_pin_clr(uint8_t pin);
  void io_pin_setVal(uint8_t pin,bool v);
  bool io_pin_get(uint8_t pin);
  void io_pin_setDirIn(uint8_t pin);
  void io_pin_setDirOut(uint8_t pin);
  void io_bus_setVal(uint32_t mask,uint32_t val);
  uint32_t io_bus_get();

  enum GpioOtherFunc{
    SDA1=2,
    SCL1=3,
    GPIO4=4,
    GPIO17=17,
    GPIO27=27,
    GPIO22=22,
    SPI0_MOSI=10,
    SPI0_MISO=9,
    SPI0_SCLK=11,
    GPIO5=5,
    GPIO6=6,
    GPIO13=13,
    GPIO19=19,
    GPIO26=26,
    UART0_TXD=14,
    UART0_RXD=15,
    PCM_CLK=18,
    GPIO23=23,
    GPIO24=24,
    GPIO25=25,
    SPI0_CE0_N=8,
    SPI0_CE1_N=7,
    GPIO12=12,
    GPIO16=16,
    GPIO20=20,
    GPIO21=21
  };

  
#ifdef __cplusplus
}
#endif

#endif
