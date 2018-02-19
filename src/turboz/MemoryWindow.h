
#include "AddressableWindow.h"
#include "TrackedObject.h"
#include "MemoryInterface.h"

class MemoryWindow:public AddressableWindow,public Tracked<MemoryWindow>{
public:
  MemoryWindow(const TRect& bounds,const std::string& _title,MemoryInterface& mem,AddressFinder& addrFind);
  void cycleFormat();
private:
  virtual void updateTitle(uint16_t addr);
  virtual void generateContent(TView& sink,TPoint& delta,TPoint& size);
  virtual int addrToScroll(uint16_t addr);
  virtual uint16_t scrollToAddr(int scroll);
  enum Format{Hex,Dec,Ascii,FORMATS};
  Format format;
  void handleEvent(TEvent& event);
  Memory& mem;
  AddressFinder& addrFind;
};
