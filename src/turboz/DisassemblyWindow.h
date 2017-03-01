#ifndef _DISASSEMBLYWINDOW_H_
#define _DISASSEMBLYWINDOW_H_


#define Uses_TWindow
#include <tv.h>
#include "System.h"
#include "TrackedObject.h"
#include "TDynamicScrollBar.h"

class DisassemblyScroller;

class DisassemblyWindow: public TWindow,public Tracked<DisassemblyWindow>{
public:
  DisassemblyWindow(const TRect& bounds,System& system);
  void updatePosition(uint16_t addr);
  Disassembly& getDisassembly(){return sys.disassembly;}
  uint16_t getPC(){return sys.processor.GetPC();}
  void handleEvent(TEvent& event);
  void scrollTo(uint16_t addr);
private:
  System& sys;
  void showGoToDialog();
  TDynamicScrollBar* vScrollBar;
};


#endif
