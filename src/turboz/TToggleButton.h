#ifndef _TTOGGLEBUTTON_H_
#define _TTOGGLEBUTTON_H_


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
    enableCommand(
  }
  title=state_down?dTitle.c_str():uTitle.c_str();
  drawView();
}



#endif
