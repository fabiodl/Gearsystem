#include "DisassemblyWindow.h"
#include "turbozPalette.h"
#include "TDynamicScrollBar.h"
#define Uses_TScrollBar
#define Uses_TScroller
#define Uses_TPalette
#define Uses_TFrame
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TCheckBoxes
#define Uses_TSItem
#define Uses_TButton
#include <tv.h>
#include "Placer.h"
#include "commands.h"
#include "GoToDialog.h"
#include "rootView.h"

class DisassemblyScroller:public TScroller{
public:
  DisassemblyScroller( const TRect& bounds, TScrollBar *aHScrollBar,TScrollBar *aVScrollBar );
  virtual void draw();              
  TPalette& getPalette() const;
  void handleEvent(TEvent& event);
private:
  void showGoToDialog();
  DisassemblyWindow* getWindow();
};




DisassemblyScroller:: DisassemblyScroller( const TRect& bounds, TScrollBar *aHScrollBar,TScrollBar *aVScrollBar ):
  TScroller( bounds, aHScrollBar, aVScrollBar )
{
    growMode = gfGrowHiX | gfGrowHiY;
    options = options | ofFramed;
    setLimit( 80,0x10000);
}


void DisassemblyWindow::updatePosition(uint16_t addr){
  sprintf(const_cast<char*>(title)+strlen(title)-4,"%04X",addr);
  frame->drawView();
}

/*void DisassemblyWindow::handleEvent(TEvent& event){
  if (event.what==evBroadcast&&event.message.command==cmScrollBarChanged){
    updateTitle(12);
  }
  TWindow::handleEvent(event); 
  }*/


void DisassemblyWindow::scrollTo(uint16_t addr){  
  vScrollBar->setValue(addr);
}


void DisassemblyWindow::showGoToDialog(){
  
  GoToDialog *pd = new GoToDialog(Placer::center(owner->getBounds(),24,11),
                                  sys.symbols,
                                  sys.processor
                                  );
  if( pd ){
    ushort control=static_cast<TGroup*>(owner)->execView( pd );
    if( control != cmCancel ){
      scrollTo(pd->getChoice());
    }
    CLY_destroy( pd );
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
    case cmGoTo:
      showGoToDialog();
      clearEvent( event );
      break;
    case cmOptionDialog:
      showOptionsDialog();
      clearEvent(event);
    }
  }
  TWindow::handleEvent(event);  
}

void DisassemblyWindow::setState( ushort aState, Boolean enable){
  if (aState&sfSelected){
    if (enable){
      enableCommand(cmGoTo);
      enableCommand(cmOptionDialog);
    }else{
      disableCommand(cmGoTo);
      enableCommand(cmOptionDialog);
    }
  }
   
  
  TWindow::setState(aState,enable);
}

DisassemblyWindow* DisassemblyScroller::getWindow(){
  return static_cast<DisassemblyWindow*>(owner);
}

void DisassemblyScroller::handleEvent(TEvent& event){
 if (event.what==evBroadcast){
    switch(event.message.command){
    case cmScrollBarChanged:
      getWindow()-> updatePosition(vScrollBar->value);
      break;
    case cmRefreshState:
      //std::cout<<"Disassembly window refresh"<<std::endl;
      if (getWindow()->isFollowPC()){
        getWindow()->scrollTo((std::max)(0,getWindow()->getPC()-size.y/2));
      }
      draw();
      break;
    }//switch
  }//if evBroadcast
 

 TScroller::handleEvent(event);
}
 

DisassemblyWindow::DisassemblyWindow(const TRect& bounds,System& _sys):
   TWindowInit( &DisassemblyWindow::initFrame ),
   TWindow( bounds,"Disasm 0000", 0),
   sys(_sys),
   followPC(false)
{
   
  TRect r = getExtent();
  r = TRect( r.b.x-1, r.a.y+1, r.b.x, r.b.y-1 );
  insert( vScrollBar = new TDynamicScrollBar(r) );
  vScrollBar->options |= ofPostProcess;

  TScrollBar *hScrollBar =
     standardScrollBar( sbHorizontal |  sbHandleKeyboard );

   r = getClipRect();    // get exposed view bounds
   r.grow( -1, -1 );           // shrink to fit inside window frame
   DisassemblyScroller* scroller=new DisassemblyScroller( r, hScrollBar, vScrollBar );
   insert(scroller);
}


class ClippedString{
public:
  char* s;
  int offset;
  inline void set(char* s,int deltax,int sizex);  
};




void  ClippedString::set(char* buff,int deltax,int sizex){
  int len=strlen(buff);

  if (deltax>len || deltax+sizex < 0 ){
    s=buff;
    offset=0;
    s[0]=0;
  }else if (deltax>0){
    s=buff+deltax;
    offset=0;
  }else{
    s=buff;
    offset=-deltax;
  }
}



TPalette& DisassemblyScroller::getPalette() const{
  static TPalette passthrough(0,0);
  return passthrough;
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
  
void DisassemblyScroller::draw(){ 
  static const int BUFFERSIZE=256;
  static const int INFOLENGTH=8;
  
  static LineColor lineColor;
  char buffer[BUFFERSIZE];
  Disassembly& disassembly(static_cast<DisassemblyWindow*>(owner)->getDisassembly());
  Symbols& sym=disassembly.getSymbols();
  
  int labelMaxLength=sym.getLabelMaxLength();
  
  int labelLength;
  ClippedString cs;
  std::string* labelString;
  for( int i = 0; i < size.y; i++ ){   
    int addr = delta.y + i;       // delta is scroller offset          
    TDrawBuffer b;
    b.moveChar( 0, ' ', getColor(1), size.x );// fill line buffer with spaces
    labelString=sym.getLabel(addr);
    if (labelString){
      labelLength=labelString->length();
      memcpy(buffer,labelString->c_str(),std::min(BUFFERSIZE,labelLength));
      memcpy(buffer+labelLength," ",2);
      labelLength++;
    }else{
      labelLength=0;
    }
    sprintf(buffer+labelLength,"%04X %02X ",addr,disassembly.getData(addr));
    labelLength+=INFOLENGTH;
    
      
    Disassembly::State state=disassembly.getState(addr);
    if (state==Disassembly::ConfirmedHead||state==Disassembly::PredictedHead){
      disassembly.disassembleWithSymbols(buffer+labelLength,BUFFERSIZE-labelLength,addr);
    }
    short color;
    static TView* rootView=getRootView(this);
    //std::cout<<"addr is"<<std::hex<<addr<<" and PC is"<<
    //      static_cast<DisassemblyWindow*>(owner)->getPC()<<std::endl;

    bool isPC=static_cast<DisassemblyWindow*>(owner)->getPC()==addr;
    bool isBreakpoint=static_cast<DisassemblyWindow*>(owner)->isBreakpoint(addr);

    color=rootView->getColor(lineColor.getPaletteId(isPC,isBreakpoint,state));
    cs.set(buffer,-(labelMaxLength+INFOLENGTH-labelLength) +delta.x,size.x);    
    b.moveStr( cs.offset, cs.s, color );      
    writeLine( 0, i, size.x, 1, b);
  }
}

