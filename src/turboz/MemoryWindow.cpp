#define Uses_TDrawBuffer
#define Uses_TFrame
#define Uses_TEvent
#include <tv.h>
#include <sstream>
#include "MemoryWindow.h"
#include "ClippedString.h"
#include "commands.h"

MemoryWindow::MemoryWindow(const TRect& bounds,const std::string& _title,MemoryInterface& _mem,AddressFinder& _addrFind):
  TWindowInit( &MemoryWindow::initFrame ),
  AddressableWindow(bounds,"      000x",_addrFind),
  format(Hex),
  mem(_mem),
  addrFind(_addrFind)
{
  scroller->setLimit(80,0x10000/16+1);
  windowCommands.enableCmd(cmCycleFormat);    
  memcpy(const_cast<char*>(title),_title.c_str(),(std::min)((size_t)6,_title.length()));
}


int MemoryWindow::addrToScroll(uint16_t addr){
  return addr/16;
}

uint16_t MemoryWindow::scrollToAddr(int scroll){
  return scroll*16;
}

inline void writeLine(TView& sink,int row,char* buffer,short color,TPoint& delta,TPoint& size){
  ClippedString cs;
  cs.set(buffer,delta.x,size.x);
  TDrawBuffer b;
  b.moveChar( 0, ' ', color, size.x );// fill line buffer with spaces
  b.moveStr(cs.offset,cs.s,color);
  sink.writeLine(0,row,size.x,1,b);  
}


void MemoryWindow::cycleFormat(){
  format=(Format)((format+1)%FORMATS);
  //std::cout<<"format "<<format<<std::endl;
  redraw();
}

void MemoryWindow::generateContent(TView& sink,TPoint& delta,TPoint& size){
  static const int BUFFERSIZE=256;
  char buffer[BUFFERSIZE];
  auto color=getColor(1);


  const char* formatString=nullptr;
  uint8_t width=4;
  memset(buffer,' ',4);
  
  switch(format){
  case Hex:
    sprintf(buffer,"HEX ");
    formatString=" %02X ";
    break;
  case Dec:
    sprintf(buffer,"DEC ");
    formatString="%3d ";
    break;
  case Ascii:
    sprintf(buffer,"ASC ");
    formatString="%3c ";
    break;
  default:
    break;
  }

  for( int x = 0; x < 0x10; x++ ){
    sprintf(buffer+4+x*width,"  %X ",x);
  }
  
  ::writeLine(sink,0,buffer,color,delta,size);
  
  for( int i = 0; i < size.y-1; i++ ){   
    int addr = scrollToAddr(delta.y + i);       // delta is scroller offset          
    sprintf(buffer,"%03X ",delta.y+i);    
    for (int i=0;i<0x10;i++){
      sprintf(buffer+4+i*width,formatString,mem.Read(addr+i));
    }
    ::writeLine(sink,i+1,buffer,color,delta,size);
    
  }
  
}
void MemoryWindow::updateTitle(uint16_t addr){
  sprintf(const_cast<char*>(title)+strlen(title)-4,"%03X_",addr/16);
  frame->drawView();
}

void MemoryWindow::handleEvent(TEvent& event){
  if (event.what==evCommand){
    switch(event.message.command){  
    case cmCycleFormat:      
      cycleFormat();
      clearEvent(event);
      break;
    }
  }
  if (event.what==evBroadcast&&event.message.command==cmRefreshState){
    redraw();
  }

  AddressableWindow::handleEvent(event);  //this handles the GoTo command
}
