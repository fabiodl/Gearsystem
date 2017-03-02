#include "DisassemblyWindow.h"
#include "turbozPalette.h"
#include "TDynamicScrollBar.h"
#define Uses_TScrollBar
#define Uses_TScroller
#define Uses_TPalette
#define Uses_TFrame
#define Uses_TEvent
#define Uses_TKeys
#include <tv.h>
#include "Placer.h"
#include "commands.h"
#include "GoToDialog.h"

class DisassemblyScroller:public TScroller{
public:
  DisassemblyScroller( const TRect& bounds, TScrollBar *aHScrollBar,TScrollBar *aVScrollBar );
  virtual void draw();              
  TPalette& getPalette() const;
  void handleEvent(TEvent& event);
private:
  void showGoToDialog();
  bool followPC;
  DisassemblyWindow* getWindow();
};



TView* getRoot(TView* v){
  while(v->owner!=NULL){
    std::cout<<"going to owner "<<v<<std::endl;
    v=v->owner;
  }
  return v;
}

DisassemblyScroller:: DisassemblyScroller( const TRect& bounds, TScrollBar *aHScrollBar,TScrollBar *aVScrollBar ):
  TScroller( bounds, aHScrollBar, aVScrollBar ),
  followPC(true)
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


void DisassemblyWindow::handleEvent(TEvent& event){
  if (event.what== evKeyDown  && event.keyDown.keyCode ==kbAltG){
    showGoToDialog();
    clearEvent( event );
  }
  TWindow::handleEvent(event);  
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
      if (followPC){
        getWindow()->scrollTo((std::max)(0,getWindow()->getPC()-size.y/2));
      }
      draw();
      break;
    }//switch
  }//if evCommand


 TScroller::handleEvent(event);
}
 

DisassemblyWindow::DisassemblyWindow(const TRect& bounds,System& _sys):
   TWindowInit( &DisassemblyWindow::initFrame ),
   TWindow( bounds,"Disasm 0000", 0),
   sys(_sys)  
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




void DisassemblyScroller::draw(){ 
  static const int BUFFERSIZE=256;
  static const int INFOLENGTH=8;
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
    static TView* rootView=getRoot(this);
    //std::cout<<"addr is"<<std::hex<<addr<<" and PC is"<<
    //      static_cast<DisassemblyWindow*>(owner)->getPC()<<std::endl;
    if (static_cast<DisassemblyWindow*>(owner)->getPC()==addr){
      color=rootView->getColor(palette::DISASM_CURRENT_PC_LINE);
    }else{   
      switch(state){
      case Disassembly::PredictedHead:
        //std::cout<<"root view is"<<rootView<<std::endl;
        color=rootView->getColor(palette::DISASM_PREDICTED_HEAD);
        break;
      case Disassembly::PredictedTail:
        color=rootView->getColor(palette::DISASM_PREDICTED_TAIL);
        break;
      case Disassembly::ConfirmedHead:
        color=rootView->getColor(palette::DISASM_CONFIRMED_HEAD);
        break;
      case Disassembly::ConfirmedTail:
        color=rootView->getColor(palette::DISASM_CONFIRMED_TAIL);
        break;
      case Disassembly::Unknown:
        color=rootView->getColor(palette::DEBUG);
      }
    }
    cs.set(buffer,-(labelMaxLength+INFOLENGTH-labelLength) +delta.x,size.x);    
    b.moveStr( cs.offset, cs.s, color );      
    writeLine( 0, i, size.x, 1, b);
  }
}

