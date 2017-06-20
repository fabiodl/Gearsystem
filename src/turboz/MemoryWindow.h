
#include "AddressableWindow.h"
#include "TrackedObject.h"
#include "System.h"

class MemoryWindow:public AddressableWindow,public Tracked<MemoryWindow>{
public:
  MemoryWindow(const TRect& bounds,System& sys);
  void cycleFormat();
private:
  virtual void updateTitle(uint16_t addr);
  virtual void generateContent(TView& sink,TPoint& delta,TPoint& size);
  virtual int addrToScroll(uint16_t addr);
  virtual uint16_t scrollToAddr(int scroll);
  enum Format{Hex,Dec,Ascii,FORMATS};
  Format format;
  void handleEvent(TEvent& event);
  System& sys;
};
