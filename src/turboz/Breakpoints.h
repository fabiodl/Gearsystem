#ifndef _BREAKPOINTS_H_
#define _BREAKPOINTS_H_
#include <stdint.h>
#include <set>

class Breakpoints{
public:
  void add(uint16_t);
  void remove(uint16_t);
  void toggle(uint16_t);
  bool isBreakpoint(uint16_t) const;
  const std::set<uint16_t>& getBreakpoints() const;
  void clear();
private:
  std::set<uint16_t> breakpoints;

};


#endif
