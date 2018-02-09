#include "CpldClock.h"

void CpldClock::Tick(unsigned int cpuCycles){
  size_t cycles=cpuCycles*3300000/
    3579540;
  for (size_t i=0;i<cycles;i++){
    onTick();
  }
}
