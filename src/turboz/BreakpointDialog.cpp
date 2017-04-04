#include "BreakpointDialog.h"

#define Uses_TScrollBar
#define Uses_TListBox
#define Uses_TCollection
#define Uses_TStringCollection
#include <tv.h>
#include "Placer.h"




BreakpointDialog::BreakpointDialog(const TRect& pos,const char* title,Symbols& symbols,Processor& processor,Breakpoints& bp):
  TWindowInit( &TDialog::initFrame ),
  GoToDialog(pos,title,symbols,processor)
{
  Placer placer(23,1);
  TRect listRect(placer.place(14,6));
  TScrollBar * scrollb=new TScrollBar(placer.place(1,6,true));
  scrollb->options|=ofPostProcess;
  insert(scrollb);
  TListBox* listBox=new TListBox(listRect,1,scrollb);
  listBox->setState(sfDisabled,true);
  auto list = new TStringCollection(0,1);
  for (auto addr:bp.getBreakpoints()){
    std::string label;    
    std::string* isLabel=symbols.getLabel(addr);
    if (isLabel){
      label=*isLabel;
      if (label.length()>100){
        label=label.substr(0,100);
      }
    }
    char* buffer=new char[256];
    sprintf(buffer,"%04X %s",addr,label.c_str());
    list->insert(buffer);
  }

  listBox->newList(list);
  insert(listBox);
  selectNext(false);

}
