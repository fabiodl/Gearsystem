#ifndef _SYMBOLWINDOW_H_
#define _SYMBOLWINDOW_H_

#define Uses_TWindow
#define Uses_TListBox
#include <tv.h>
#include "Symbols.h"
#include "TrackedObject.h"

class SymbolWindow:public TWindow,public Tracked<SymbolWindow>{
public:
  SymbolWindow(const TRect& bounds,Symbols& sy);
  void handleEvent( TEvent& event );
  TPalette& getPalette() const;
private:
  Symbols& sy;
  void refresh();
  TListBox* listBox;
};


#endif
