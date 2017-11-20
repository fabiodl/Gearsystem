#include "DisassemblyWindow.h"
#include "turbozPalette.h"
#include "TDynamicScrollBar.h"


#define Uses_TEvent
#define Uses_TKeys
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_TButton
#include <tv.h>
#include "Placer.h"
#include "commands.h"
#include "BreakpointDialog.h"
#include "rootView.h"
#include "ClippedString.h"





void DisassemblyWindow::showBreakpointDialog(){
  
  GoToDialog *pd = new BreakpointDialog
    (
     Placer::center(owner->getBounds(),40,11),
     "Breakpoints",
     sys.addrFind,
     sys.breakpoints
     );
  if( pd ){
    ushort control=static_cast<TGroup*>(owner)->execView( pd );
    if( control != cmCancel ){
      sys.breakpoints.toggle(pd->getChoice());
    }
    CLY_destroy( pd );
    redraw();
  }

}





void DisassemblyWindow::showOptionsDialog(){
  TDialog* opt=new TDialog(Placer::center(owner->getBounds(),24,7),"Options");
  Placer placer(1,2);
  opt->insert(new TCheckBoxes( placer.place(14,1,true),
                              new TSItem( "~F~ollow PC",0)
                               ));
  placer.newLine();
  opt->insert( new TButton(placer.place(10,2), "OK", cmOK,
                    bfDefault ));
  opt->insert(new TButton(placer.place(10,2), "Cancel", cmCancel,
                              bfNormal ));
  ushort data=isFollowPC()?1:0;
  
  opt->setData(&data);
  if( opt ){
    ushort control=static_cast<TGroup*>(owner)->execView( opt );
    if( control != cmCancel ){
      ushort data;
      opt->getData(&data);
      setFollowPC(data&1);
    }
  }
}

void DisassemblyWindow::handleEvent(TEvent& event){
  if (event.what==evCommand){
    switch(event.message.command){  
    case cmOptionDialog:
      showOptionsDialog();
      clearEvent(event);
      break;
    case cmBreakpointDialog:
      showBreakpointDialog();
      clearEvent(event);
      break;
    }
  }
  if (event.what==evBroadcast){
    switch(event.message.command){
    case cmRefreshState:
      if (isFollowPC()){
        scrollTo((std::max)(0,sys.processor.GetPC()-size.y/2));
      }
      redraw();
      break;
    }
  }
  AddressableWindow::handleEvent(event);  //this handles the GoTo command
}


DisassemblyWindow::DisassemblyWindow(const TRect& bounds,System& _sys):
   TWindowInit( &DisassemblyWindow::initFrame ),
   AddressableWindow( bounds,"Disasm 0000",_sys.addrFind),
   followPC(false),
   sys(_sys)
{      
   windowCommands.enableCmd(cmOptionDialog);
   windowCommands.enableCmd(cmBreakpointDialog);   
}







class LineColor{
  palette::TurboZ colorByState[5];
public:
  LineColor(){
    colorByState[Disassembly::Unknown]=palette::DEBUG;
    colorByState[Disassembly::PredictedHead]=palette::DISASM_PREDICTED_HEAD;
    colorByState[Disassembly::PredictedTail]=palette::DISASM_PREDICTED_TAIL;
    colorByState[Disassembly::ConfirmedHead]=palette::DISASM_CONFIRMED_HEAD;
    colorByState[Disassembly::ConfirmedTail]=palette::DISASM_CONFIRMED_TAIL;
  }

  palette::TurboZ getPaletteId(bool isPc,bool isBreakpoint,Disassembly::State state){
    if (isPc&&isBreakpoint){
      return palette::DISASM_PC_AT_BREAKPOINT;
    }else if (isPc){
      return palette::DISASM_CURRENT_PC_LINE;
    }else if (isBreakpoint){
      return palette::DISASM_BREAKPOINT;
    }
    return colorByState[state];        
  }

  
};
  
void DisassemblyWindow::generateContent(TView& sink,TPoint& delta,TPoint& size){ 
  static const int BUFFERSIZE=256;
  static const int INFOLENGTH=8;
  
  static LineColor lineColor;
  char buffer[BUFFERSIZE];
  
  int labelMaxLength=sys.symbols.getLabelMaxLength();
  
  int labelLength;
  ClippedString cs;
  std::string* labelString;
  for( int i = 0; i < size.y; i++ ){   
    int addr = delta.y + i;       // delta is scroller offset          
    TDrawBuffer b;
    b.moveChar( 0, ' ', getColor(1), size.x );// fill line buffer with spaces
    labelString=sys.symbols.getLabel(addr);
    if (labelString){
      labelLength=labelString->length();
      memcpy(buffer,labelString->c_str(),std::min(BUFFERSIZE,labelLength));
      memcpy(buffer+labelLength," ",2);
      labelLength++;
    }else{
      labelLength=0;
    }
    sprintf(buffer+labelLength,"%04X %02X ",addr,sys.disassembly.getData(addr));
    labelLength+=INFOLENGTH;
    
      
    Disassembly::State state=sys.disassembly.getState(addr);
    if (state==Disassembly::ConfirmedHead||state==Disassembly::PredictedHead){
      sys.disassembly.disassembleWithSymbols(buffer+labelLength,BUFFERSIZE-labelLength,addr);
    }
    short color;
    static TView* rootView=getRootView(this);

    bool isPC=sys.processor.GetPC()==addr;
    bool isBreakpoint=sys.breakpoints.isBreakpoint(addr);

    color=rootView->getColor(lineColor.getPaletteId(isPC,isBreakpoint,state));
    cs.set(buffer,-(labelMaxLength+INFOLENGTH-labelLength) +delta.x,size.x);    
    b.moveStr( cs.offset, cs.s, color );      
    sink.writeLine( 0, i, size.x, 1, b);
  }
}

