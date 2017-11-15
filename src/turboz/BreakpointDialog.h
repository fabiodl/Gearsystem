#ifndef _BREAKPOINTDIALOG_H_
#define _BREAKPOINTDIALOG_H_
#include "GoToDialog.h"
#include "Breakpoints.h"
#define Uses_TStringCollection
#include <tv.h>

class BreakpointDialog:public GoToDialog{
  TStringCollection existingBpList;
public:
  BreakpointDialog(const TRect& pos,const char* title,AddressFinder& addrFind,Breakpoints& bp);
  ~BreakpointDialog();
  
};

#endif
