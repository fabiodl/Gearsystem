#ifndef _EXECUTIONWINDOW_
#define _EXECUTIONWINDOW_


#define Uses_TDialog
#define Uses_TButton
#include <tv.h>
#include "TrackedObject.h"
#include "System.h"
#include "Spinner.h"
#include <functional>


class ToggleButton;
class TCheckBoxes;
class TInputLine;
class ExecutionWindow:public TDialog,public Tracked<ExecutionWindow>{
public:
  ExecutionWindow(const TRect& bounds,System& sys);
  void requestRun(const Spinner::HaltCondition* haltCondition);
private:
  System& sys;
  Spinner::Work fastWork,realtimeWork;
  uint32_t freq;
  TCheckBoxes* realtimeCheck;
  TInputLine* freqInputLine;
};


#endif



