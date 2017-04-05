#ifndef _PROCESSORWINDOW_H_
#define _PROCESSORWINDOW_H_

#include "MonitoredData.h"
#include "Processor.h"
#include "Placer.h"
#include "MonitoredData.h"
#include "TrackedObject.h"
#define Uses_TRect
#define Uses_TDialog
#define Uses_TInputLine
#define Uses_TLabel

#include <tv.h>
class TLightInputLine;
class ProcessorWindow: public TDialog,public Tracked<ProcessorWindow>{
public:
  ProcessorWindow(const TRect& bounds,Processor& processor);
  void draw();
  void handleEvent(TEvent& event);
 class Register{
 public:
    Register(EightBitRegister* l,EightBitRegister*h,TLightInputLine* input);
    EightBitRegister* l,*h;
   TLightInputLine* input;  
    void cpuToDisplay();
    void toCpu(int v);
   void setOwnership();
  };
private:
  void update();
  Processor& processor;

  void addRegister(Placer& p,Monitor::Register h,Monitor::Register l,const char* label);

 
  std::vector<Register> registers;
  static const Monitor::Register NOREGISTER=Monitor::REGISTERS;

};

#endif
