#include "ProcessorWindow.h"
#include "commands.h"

#define Uses_TEvent
#define Uses_TView
#define Uses_TKeys
#include <tv.h>
#include <iostream>
#include <sstream>

class TLightInputLine:public TInputLine{
public:
  TLightInputLine( const TRect& bounds, int aMaxLen);
  void handleEvent( TEvent& event ); 
  virtual void set(int v);
  virtual int get();
  void setOwner(ProcessorWindow::Register* _owner);
protected:
  std::string formatString;    
  friend class TFlagInputLine;
  ProcessorWindow::Register* owner;
};

class TFlagInputLine:public TLightInputLine{
public:
  TFlagInputLine( const TRect& bounds,TLightInputLine* numerical);
  virtual void set(int v);
  virtual int get();
private:
  std::string highformatString;
  TLightInputLine* numerical;
};


TLightInputLine::TLightInputLine( const TRect& bounds, int aMaxLen)  
    :TInputLine(bounds,aMaxLen),
     owner(NULL)
  {
    std::stringstream ss;    
    ss<<"%0"<<(aMaxLen-1)<<"X";
    formatString=ss.str();
    //std::cout<<"new inputline at"<<this<<std::endl;
  }


void TLightInputLine::handleEvent( TEvent& event ){
    if (state& sfSelected){
      ushort key;
      switch(event.what){
      case  evKeyDown:
        key = ctrlToArrow(event.keyDown.keyCode);
        switch(key){
        case kbEnter:
        case kbTab:        
          owner->toCpu(get());
          owner->cpuToDisplay();
          break;
        case kbEsc:
          owner->cpuToDisplay();
          break;
        }//switch key
      }//switch event.what
    }//switch state
    TInputLine::handleEvent(event);
}//handleEvent

void TLightInputLine::set(int v){//assumes len(str)>=maxlen
    sprintf(data,formatString.c_str(),v);
    writeStr(1,0,data,1);
}

int TLightInputLine::get(){
  int r=0;
  sscanf(data,formatString.c_str(),&r);
  return r;
}

TFlagInputLine::TFlagInputLine(const TRect& bounds,TLightInputLine* _numerical):  
  TLightInputLine(bounds,9),
  numerical(_numerical)
{
  formatString="szyhxpnc";
  highformatString="SZYHXPNC";
  setOwner(_numerical->owner);
  data[8]=0;
  set(0);
}

void TFlagInputLine::set(int v){
  for (int i=0;i<8;i++){
    data[i]=v&(1<<(7-i))?highformatString[i]:formatString[i];
  }  
  writeStr(1,0,data,1);
  numerical->set(v);  
}

int TFlagInputLine::get(){
  int v=0;
  for (int i=0;i<8;i++){
    if (data[i]==highformatString[i]){
      v|=(1<<(7-i));
    }
  }
  return v;
}


void TLightInputLine::setOwner(ProcessorWindow::Register* _owner){
  owner=_owner;
  //std::cout<<"setting owner of "<<this<<" to "<<owner<<std::endl;
}


ProcessorWindow::Register::Register(EightBitRegister* _h,EightBitRegister* _l,TLightInputLine* _input):
  l(_l),h(_h),input(_input)
{
  //std::cout<<"new register at "<<this<<"pointing to"<<h<<" "<<l<<",il "<<input<<std::endl;
}

void ProcessorWindow::Register::cpuToDisplay(){
  //std::cout<<"popping data from "<<input<<std::endl;
  if (h){
    uint16_t v=(static_cast<uint16_t>(h->GetValue())<<8)+l->GetValue();   
    input->set(v);
    //std::cout<<"v is"<<v<<std::endl;
  }else{
    uint8_t v=l->GetValue();
    input->set(v);
  }
}

void ProcessorWindow::Register::setOwnership(){
  input->setOwner(this);
}



void ProcessorWindow::Register::toCpu(int v){
  //std::cout<<"popping data from "<<input<<std::endl;
  if (h){
    h->SetValue((v>>8)%256);
  }
  l->SetValue(v%256);
}




void ProcessorWindow::addRegister(Placer& placer,Monitor::Register h,Monitor::Register l,const char* label){
  bool single=h==NOREGISTER;
  TRect labelbb=placer.place(single?2:3,1);
  TRect inputbb=placer.place(single?4:6,1);

  TLightInputLine* il=new TLightInputLine(inputbb,single?3:5);
  
  registers.push_back
    (
     Register(
              single?NULL:processor.getRegister(h),
              processor.getRegister(l),
              il
              )
     );
  insert(il);
  insert(new TLabel(labelbb,label,il));
  registers.back().cpuToDisplay();
}

ProcessorWindow::ProcessorWindow(const TRect& bounds,Processor& _processor):
  TWindowInit( &TDialog::initFrame ),
  TDialog(bounds,"CPU"),
  processor(_processor)
{
  //TWindow( bounds,"Registerm window", 1){  
  Placer p(1,1);
  addRegister(p,NOREGISTER,Monitor::A,"A");
  addRegister(p,NOREGISTER,Monitor::F,"F");  
  p.newLine();
  addRegister(p,NOREGISTER,Monitor::B,"B");
  addRegister(p,NOREGISTER,Monitor::C,"C");
  p.newLine();
  addRegister(p,NOREGISTER,Monitor::D,"D");
  addRegister(p,NOREGISTER,Monitor::E,"E");
  p.newLine();
  addRegister(p,NOREGISTER,Monitor::H,"H");
  addRegister(p,NOREGISTER,Monitor::L,"L");
  p.newLine();
  addRegister(p,Monitor::IX_H,Monitor::IX_L,"IX");
  p.newLine();
  addRegister(p,Monitor::IY_H,Monitor::IY_L,"IY");
  p.newLine();
  addRegister(p,Monitor::SP_H,Monitor::SP_L,"SP");
  p.newLine();
  addRegister(p,Monitor::PC_H,Monitor::PC_L,"PC");
  p.newLine();
  for (auto&& r:registers){
    r.setOwnership();
  }
  TFlagInputLine* fil=new TFlagInputLine(p.spaceAndPlace(1,1,10,1),registers[1].input);
  registers[1].input=fil;
  insert(fil);
  selectNext(false);
}


void ProcessorWindow::handleEvent(TEvent& event){
  if (event.what==evCommand){
    switch(event.message.command){
    case cmClose:
      registers.clear();
      break;  
    }
  }else if (event.what==evBroadcast&&event.message.command==cmRefreshState){
      update();
  }
  TDialog::handleEvent(event);
}

void ProcessorWindow::update(){
  for (auto  r=registers.begin();r!=registers.end();++r){
    r->cpuToDisplay();
  }       
}



void ProcessorWindow::draw(){
  //std::cout<<"proc win"<<std::endl;
  update();
  TDialog::draw();
}
