
#ifndef _PHYSICALIO_H_
#define _PHYSICALIO_H_

#include <stdint.h>

class PhysicalIo{
 public:
  static bool read();
  static void write(uint8_t);
};

#endif
