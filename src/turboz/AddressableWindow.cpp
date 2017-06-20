#include "AddressableWindow.h"
#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_TPalette
#define Uses_TKeys
#define Uses_TFrame
#define Uses_TEvent
#include <tv.h>
#include "GoToDialog.h"
#include "commands.h"
#include "Placer.h"


static const int MAXCOLS=80;
static const int MAXROWS=0x10000;

class AddressScroller:public TScroller{
public:
  AddressScroller( const TRect& bounds, TScrollBar *aHScrollBar,TScrollBar *aVScrollBar,AddressableWindow* window );
  virtual void draw();              
  TPalette& getPalette() const;
  //void handleEvent(TEvent& event);
private:
  void showGoToDialog();
  AddressableWindow* window;
};

TPalette& AddressScroller::getPalette() const{
  static TPalette passthrough(0,0);
  return passthrough;
}



AddressScroller:: AddressScroller( const TRect& bounds, TScrollBar *aHScrollBar,TScrollBar *aVScrollBar,AddressableWindow* _window ):
  TScroller( bounds, aHScrollBar, aVScrollBar ),
  window(_window)
{
    growMode = gfGrowHiX | gfGrowHiY;
    options = options | ofFramed;
    setLimit(MAXCOLS,MAXROWS);
}

void AddressScroller::draw(){
  window->generateContent(*this,delta,size);  
}





AddressableWindow:: AddressableWindow(const TRect& bounds,const char* title,AddressFinder& _addrFind):
  TWindowInit( &AddressableWindow::initFrame ),
  TWindow( bounds,title, 0),
  addrFind(_addrFind)

{

  TRect r = getExtent();
  r = TRect( r.b.x-1, r.a.y+1, r.b.x, r.b.y-1 );
  insert( vScrollBar = new TDynamicScrollBar(r) );
  vScrollBar->options |= ofPostProcess;

  TScrollBar *hScrollBar =
     standardScrollBar( sbHorizontal |  sbHandleKeyboard );

   r = getClipRect();    // get exposed view bounds
   r.grow( -1, -1 );           // shrink to fit inside window frame
   scroller=new AddressScroller( r, hScrollBar, vScrollBar,this);
   insert(scroller);



  windowCommands.enableCmd(cmGoTo);
  
}

void AddressableWindow::updateTitle(uint16_t addr){
  sprintf(const_cast<char*>(title)+strlen(title)-4,"%04X",addr);
  frame->drawView();
}


int AddressableWindow::addrToScroll(uint16_t addr){
  return addr;
}

uint16_t AddressableWindow::scrollToAddr(int scroll){
  return scroll;
}

void AddressableWindow::scrollTo(uint16_t addr){  
  vScrollBar->setValue(addrToScroll(addr));
}


void AddressableWindow::showGoToDialog(){
  
  GoToDialog *pd = new GoToDialog(Placer::center(owner->getBounds(),24,11),
                                  "Go To",
                                  addrFind
                                  );
  if( pd ){
    ushort control=static_cast<TGroup*>(owner)->execView( pd );
    if( control != cmCancel ){
      scrollTo(pd->getChoice());
    }
    CLY_destroy( pd );
  }

}


void AddressableWindow:: handleEvent(TEvent& event){
  if (event.what==evCommand&&event.message.command==cmGoTo){
      showGoToDialog();
      clearEvent( event );
  }
  if (event.what==evBroadcast&&event.message.command==cmScrollBarChanged){
    updateTitle(scrollToAddr(vScrollBar->value));
  }
  
  TWindow::handleEvent(event);  
}


void AddressableWindow::setState(ushort aState,Boolean enable){
   if (aState&sfSelected){
    if (enable){
      enableCommands(windowCommands);
    }else{
      disableCommands(windowCommands);
    }
  }
   
  
  TWindow::setState(aState,enable);
}
