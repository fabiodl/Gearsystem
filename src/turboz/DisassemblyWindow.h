#ifndef _DISASSEMBLYWINDOW_H_
#define _DISASSEMBLYWINDOW_H_


#include "TrackedObject.h"
#include "AddressableWindow.h"
#include "System.h"

class DisassemblyWindow: public AddressableWindow,public Tracked<DisassemblyWindow>{
public:
  DisassemblyWindow(const TRect& bounds,System& system);  
  void handleEvent(TEvent& event);
  void setFollowPC(bool b){followPC=b;}
  bool isFollowPC(){return followPC;}
private:
  void generateContent(TView& sink,TPoint& delta,TPoint& size);
  void showOptionsDialog();
  void showBreakpointDialog();

  bool isBreakpoint(uint16_t addr){return sys.breakpoints.isBreakpoint(addr);}
  bool followPC;
  System& sys;
};


#endif
