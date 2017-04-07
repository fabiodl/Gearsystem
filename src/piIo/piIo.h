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
  
#ifdef __cplusplus
}
#endif

#endif
