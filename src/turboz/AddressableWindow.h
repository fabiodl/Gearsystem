#ifndef _ADDRESSABLEWINDOW_H_
#define _ADDRESSABLEWINDOW_H_


#define Uses_TWindow
#define Uses_TCommandSet
#define Uses_TScroller
#include <tv.h>
#include "AddressFinder.h"
#include "TDynamicScrollBar.h"

class AddressableWindow:public TWindow{
public:
  AddressableWindow(const TRect& bounds,const char* title,AddressFinder& addrFind);
  virtual void updateTitle(uint16_t addr);
  void scrollTo(uint16_t addr);
  void setState(ushort aState,Boolean enable);
  void showGoToDialog();
  void handleEvent(TEvent& event);
  virtual void generateContent(TView& sink,TPoint& delta,TPoint& size)=0;
protected:
  TCommandSet windowCommands;
  AddressFinder& addrFind;
  virtual int addrToScroll(uint16_t addr);
  virtual uint16_t scrollToAddr(int scroll);
  TScroller* scroller;
private:
  TDynamicScrollBar* vScrollBar;
};



#endif
