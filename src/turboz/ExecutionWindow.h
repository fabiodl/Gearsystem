#ifndef _EXECUTIONWINDOW_
#define _EXECUTIONWINDOW_


#define Uses_TDialog
#define Uses_TButton
#include <tv.h>
#include "TrackedObject.h"
#include "System.h"
#include "Spinner.h"
#include "TurboZ.h"
#include <functional>


class TCheckBoxes;
class TInputLine;
class ExecutionWindow:public TDialog,public Tracked<ExecutionWindow>{
public:
  class HaltCondition{
  public:
    HaltCondition();
    std::function<void()> init;
    Spinner::HaltCondition check;
  };
  ExecutionWindow(const TRect& bounds,System& sys,TurboZ* turboz);
  ~ExecutionWindow();
  void setHaltCondition(HaltCondition* haltCondition);
  void requestRun();
  void requestHalt();
  void handleEvent( TEvent& event );
private:
  System& sys;
  Spinner::Work fastWork,realtimeWork;
  
  uint32_t freq;
  TCheckBoxes* realtimeCheck;
  TInputLine* freqInputLine;

  std::vector<HaltCondition*> haltConditions;
  HaltCondition* haltCondition;//the current one

  TurboZ* turboz;
};


#endif



