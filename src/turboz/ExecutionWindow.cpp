#include "ExecutionWindow.h"
#include "Placer.h"
#include "commands.h"

#include <iostream>
#include <functional>
#include "TurboZ.h"
#include <chrono>
#include <thread>

#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_TInputLine
#define Uses_TStaticText
#define Uses_TButton
#include <tv.h>
#include "cistring.h"
#include "rootView.h"

static const int FREQCHARS=10;


class TCondButton:public TButton{
public:  
  TCondButton(const TRect& bounds,
              const char *aTitle,
              ushort aCommand,
              ushort aFlags,
              ExecutionWindow* win,
              ExecutionWindow::HaltCondition* cond);
  //void handleEvent( TEvent& event );
  void press();
private:
  ExecutionWindow* win;
  ExecutionWindow::HaltCondition* cond;
};

TCondButton::TCondButton(const TRect& bounds,
                         const char *aTitle,
                         ushort aCommand,
                         ushort aFlags,
                         ExecutionWindow* _win,
                         ExecutionWindow::HaltCondition* _cond):
  TButton(bounds,aTitle,aCommand,aFlags),
  win(_win),
  cond(_cond)
{
}


void TCondButton::press(){
  win->setHaltCondition(cond);
  TButton::press();
}


void ExecutionWindow::handleEvent( TEvent& event ){
  if (event.what==evCommand){
    switch(event.message.command){
    case cmStep:
      sys.Tick();
      clearEvent(event);
      message( owner, evBroadcast, cmRefreshState, this );
      break;
    case cmRun:
      disableCommand(cmRun);
      enableCommand(cmHalt);
      requestRun();
      clearEvent(event);
      break;
    case cmHalt:      
      disableCommand(cmHalt);
      requestHalt();
      enableCommand(cmRun);
      clearEvent(event);
      break;
    }
  }
  TDialog::handleEvent(event);
}







ExecutionWindow::HaltCondition::HaltCondition(){
  init=[]{};
  check=[]{return true;};
}


void ExecutionWindow::setHaltCondition(ExecutionWindow::HaltCondition* hc){
  haltCondition=hc;
}

ExecutionWindow::ExecutionWindow(const TRect& bounds,System& _sys,TurboZ* _turboz):
  TWindowInit( &TDialog::initFrame ),
  TDialog(bounds,"Execution"),
  sys(_sys),
  turboz(_turboz)
  
{
  haltCondition=&noRun;
  runFoverver.check=[]{return false;};
  
  auto captureNextPc=[this]{
    //std::cout<<"initingCondition"<<std::endl;
    uint16_t addr=sys.processor.GetPC();    
    haltInitCond.nextPC=addr+sys.disassembly.getInstructionLength(addr);
    //std::cout<<"target"<<haltInitCond.nextPC<<std::endl;
  };
  
  runOver.init=captureNextPc;
  runOver.check=[this]{
    //std::cout<<"checking if pc=="<<haltInitCond.nextPC<<std::endl;
    return sys.processor.GetPC()==haltInitCond.nextPC;
  };
  

  
  Placer p(1,1);

  realtimeCheck= new TCheckBoxes( p.place(14,1,true),
                              new TSItem( "~F~ixedFreq",0)
                              );
  insert( realtimeCheck );

  freqInputLine=new TInputLine(p.spaceAndPlace(4,0,FREQCHARS,1),FREQCHARS);
  insert(freqInputLine);
  freqInputLine->setData(const_cast<char*>("NTSC"));
  //insert(new TStaticText(p.place(3,1),"Hz"));
  p.newLine(2);
        
  
  insert( new TButton( p.place(8,2), "~S~tep", cmStep,bfDefault ));
  insert( new TButton( p.place(8,2,true), "~H~alt", cmHalt,bfNormal ));
  insert( new TCondButton( p.place(8,2), "~R~un", cmRun,bfNormal,this,&runFoverver));
  insert( new TCondButton( p.place(8,2), "~O~ver", cmRun,bfNormal,this,&runOver));
  
  
  
  fastWork.init=[this]{haltCondition->init();};
  fastWork.job=[this]{
    sys.Tick();
  };
  static std::chrono::time_point<std::chrono::high_resolution_clock>  start;
  static int ticks=0;
  realtimeWork.init=[this]{
    haltCondition->init();
    start= std::chrono::high_resolution_clock::now();
    ticks=0;
  };
  realtimeWork.job=[this]{
    ticks+=sys.Tick();
    
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime=start+
    std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>
    (
     std::chrono::duration<float>(float(ticks)/freq)
     );
    
    std::this_thread::sleep_until
    (
     endTime    
     );
    
    
    //std::this_thread::sleep_for(waitTime);
  };
  freq=1;
  
}



void ExecutionWindow::requestRun(){
  ushort isRealtime;
  realtimeCheck->getData(&isRealtime);
  if (isRealtime){
    char buffer[FREQCHARS];
    freqInputLine->getData(buffer);
    if (istring(buffer)=="NTSC"){
      freq=3579540;
    }else if (istring(buffer)=="PAL"){
      freq=3546893;
    }else{
      bool invalid=false;
      try{
        freq=std::stoi(buffer);
      }catch(std::invalid_argument &){
        invalid=true;
      }catch(std::out_of_range &){
        invalid=true;
      }        
      if (invalid||freq<=0){
        freq=1;
      }
      sprintf(buffer,"%d",freq);
      freqInputLine->setData(buffer);
    }
  }

  turboz->requestRun(isRealtime?&realtimeWork:&fastWork,&haltCondition->check);  
}

 void ExecutionWindow::requestHalt(){
   turboz->requestHalt();
 }


 void printTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& p){

  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());
  std::cout<<ms.count()<<std::endl;
}
