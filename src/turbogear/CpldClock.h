#ifndef _CPLDCLOCK_H_
#define _CPLDCLOCK_H_
#include <functional>

class CpldClock{
public:
  std::function<void()> onTick;
  void Tick(unsigned int cpuCycles);
};


#endif
