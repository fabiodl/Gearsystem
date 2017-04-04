#include "Breakpoints.h"

void Breakpoints::add(uint16_t addr){
  breakpoints.insert(addr);
}

void Breakpoints::remove(uint16_t addr){
  breakpoints.erase(addr);
}

bool Breakpoints::isBreakpoint(uint16_t addr){
  return breakpoints.find(addr)!=breakpoints.end();
}

void Breakpoints::toggle(uint16_t addr){
  if (isBreakpoint(addr)){
    remove(addr);
  }else{
    add(addr);
  }
}

const std::set<uint16_t>& Breakpoints::getBreakpoints(){
  return breakpoints;
}

void Breakpoints::clear(){
  breakpoints.clear();
}
