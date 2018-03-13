#ifndef _TRACKER_H_
#define _TRACKER_H_

#include <map>
#include <list>
#include <stdint.h>

class WriteTracker{
public:
  WriteTracker();
  void setEnable(bool b);
  void eval(uint32_t addr,uint8_t &data,bool _ce,bool _we);
  void reset();
  void printLast();
  void printRanges();  
private:
  std::map<uint32_t,std::list<uint8_t> > writes;
  bool enabled;
};



#endif
