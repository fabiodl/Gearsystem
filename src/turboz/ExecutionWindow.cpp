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
#include <tv.h>
#include "cistring.h"

class ToggleButton:public TButtonRef{
public:

  ToggleButton(const TRect& bounds,const char *uTitle,const char* dTitle,ushort aCommand,ushort aFlags); 

  bool isStateUp(){
    return !state_down;
  }

  bool isStateDown(){
    return state_down;
  }

  
  
private:
  void press();
  std::string uTitle,dTitle;
  bool state_down;
};



ToggleButton::ToggleButton(const TRect& bounds,const char * _uTitle,const char* _dTitle,ushort aCommand,ushort aFlags):
  TButtonRef(bounds,_uTitle,aCommand,aFlags),uTitle(_uTitle),dTitle(_dTitle),state_down(false){}
  
void ToggleButton::press(){
  state_down=!state_down;
  if (state_down){
    TButton::press();
  }
  title=state_down?dTitle.c_str():uTitle.c_str();
  drawView();
}


typedef std::pair<ExecutionWindow*,ToggleButton* > ParamType;

int buttonCallback(unsigned int command,void* v){
  ParamType* param=static_cast<ParamType*>(v);
  if (param->second->isStateDown()){
    static Spinner::HaltCondition haltCondition=std::bind(&ToggleButton::isStateUp,param->second);
    param->first->requestRun(&haltCondition);
  }else{
    //the halt condition should stop the execution    
  }
  return cmValid;
}

void printTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& p){

  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(p.time_since_epoch());
  std::cout<<ms.count()<<std::endl;
}


static const int freqChars=10;

  ExecutionWindow::ExecutionWindow(const TRect& bounds,System& _sys):
                  TWindowInit( &TDialog::initFrame ),
                  TDialog(bounds,"Execution"),
                  sys(_sys)
                  
{
  Placer p(1,1);

  realtimeCheck= new TCheckBoxes( p.place(14,1,true),
                              new TSItem( "~F~ixedFreq",0)
                              );
  insert( realtimeCheck );
 
  freqInputLine=new TInputLine(p.spaceAndPlace(4,0,freqChars,1),freqChars);
  insert(freqInputLine);
  freqInputLine->setData(const_cast<char*>("NTSC"));
  //insert(new TStaticText(p.place(3,1),"Hz"));
  p.newLine(2);
        

  
  insert( new TButton( p.place(8,2), "~S~tep", cmStep,bfDefault ));
  auto run=new ToggleButton( p.place(8,2), "~R~un","S~t~op", cmRun,bfNormal);
  ParamType* param=new ParamType(this,run);
  run->setCallBack(buttonCallback,param);
  insert(run);  


  fastWork.init=[]{};
  fastWork.job=[this]{
    sys.Tick();
  };
  static std::chrono::time_point<std::chrono::high_resolution_clock>  start;
  static int ticks=0;
  realtimeWork.init=[]{
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



void ExecutionWindow::requestRun(const Spinner::HaltCondition* haltCondition){
  ushort isRealtime;
  realtimeCheck->getData(&isRealtime);
  if (isRealtime){
    char buffer[freqChars];
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
    
  TurboZ::spinner.setWork(isRealtime?&realtimeWork:&fastWork,haltCondition);  
}
