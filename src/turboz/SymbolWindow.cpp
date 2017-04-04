#include "SymbolWindow.h"
#include "Placer.h"
#include "commands.h"
#define Uses_TScrollBar
#define Uses_TStringCollection
#define Uses_TPalette
#define Uses_TApplication
#include <tv.h>
#include <cstring>

#define cpDialog "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"\
                 "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"


SymbolWindow::SymbolWindow(const TRect& bounds,Symbols& _sy):
  TWindowInit( &SymbolWindow::initFrame ),
  TWindow(bounds,"Symbols",0),
  sy(_sy)
{
  int width= bounds.b.x-bounds.a.x;
  int height=bounds.b.y-bounds.a.y;
  TScrollBar * scrollb=new TScrollBar
    (
     TRect(width-1,1,width,height-1)
     );
  
  scrollb->options|=ofPostProcess;    
  insert(scrollb);
  listBox=new TListBox(TRect(1,1,width-1,height-1),
                                 1,scrollb);
  listBox->growMode=gfGrowHiX |gfGrowHiY;
  insert(listBox);
  refresh();
}
void SymbolWindow::refresh(){  
  auto list = new TStringCollection(0,1);
  for (auto s:sy.getSymbols()){
    char* buffer=new char[s.second.length()+1+10];
    sprintf(buffer,"%04x %s",s.first,s.second.c_str());
    list->insert(buffer);
  }
  listBox->newList(list);
  redraw();
}


void  SymbolWindow::handleEvent( TEvent& event ){
  if (event.what==evBroadcast){
    switch(event.message.command){
    case cmRefreshSymbols:
      refresh();
      clearEvent(event);
      break;
    }
  }
  TWindow::handleEvent(event);
}


TPalette& SymbolWindow::getPalette() const{
    static TPalette palette1( cpDialog, sizeof( cpDialog )-1 );
    static TPalette palette2( 0, 0 );

    // Avoid returning the palette if the dialog isn't inserted on the
    // desktop or the application. Example: a dialog inside another.
    if ( (owner == (TGroup*)TApplication::deskTop) ||
         (owner == (TGroup*)TApplication::application) 
       ) return palette1;
    return palette2;
}
