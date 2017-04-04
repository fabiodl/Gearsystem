#ifndef _BREAKPOINTDIALOG_H_
#define _BREAKPOINTDIALOG_H_
#include "GoToDialog.h"
#include "Breakpoints.h"

class BreakpointDialog:public GoToDialog{
public:
  BreakpointDialog(const TRect& pos,const char* title,Symbols& symbols,Processor& processor,Breakpoints& bp);
  
};

#endif
