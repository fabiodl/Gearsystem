
#include "GoToDialog.h"
#include "TDynamicInputLine.h"
#define Uses_TInputLine
#define Uses_TListBox
#define Uses_TButton
#define Uses_TCollection
#define Uses_TEvent
#define Uses_TScrollBar
#include <tv.h>
#include "commands.h"
#include "Placer.h"


class TSearchResults : public TCollection
{
  public:
  TSearchResults() : TCollection(0, 1) {
    shouldDelete=false;
  };
  virtual void *readItem( ipstream& ) { return 0; };
  virtual void writeItem( void *, opstream& ) {};
  void clear(){
    removeAll();
  }

  void push_back(const char* s){
    insert((void*)s);
  }

  const char* operator[](int i){
    return (const char*)at(i);
  }

  size_t size(){
    return count;
  }

};





const char* GoToDialog::getInputLine(){
  return static_cast<const char*>(inputLine->getData());
}

 Boolean GoToDialog::valid( ushort command ){
   switch(command){
   case cmCancel:
     return true;
   default:
     return list->size()>0||(addrFind.getAddressExtended(getInputLine())!=AddressFinder::INVALIDADDR);   
   }//switch
}

uint16_t GoToDialog::getChoice(){ 
  const char* name=list->size()>0?(*list)[listBox->focused]:getInputLine();
  return static_cast<uint16_t>(addrFind.getAddressExtended(name)); 
}

void GoToDialog::handleEvent(TEvent& event){
 if (event.what==evBroadcast){
   switch(event.message.command){
   case cmListItemSelected:
     setInputLine((*list)[listBox->focused]);
     break;
   case cmTextEdited:
     updateChoices(getInputLine());
   }//switch
 }//if evBroadcast
 TDialog::handleEvent(event);
}


 void GoToDialog::setInputLine(const char* s){
   int len=strlen(s);
   int oldlen=inputLine->maxLen;
   inputLine->maxLen=len;
   inputLine->setData((void*)s);
   inputLine->maxLen=oldlen;
 }


void addChoiceCb(const char* s,void* what){
  static_cast<TSearchResults*>(what)->push_back(s);    
}

void GoToDialog::updateChoices(const char* s){
 

  list->clear();
  addrFind.syms.getSubstringMatches(s,addChoiceCb,list);
  listBox->newList(list,false);
}
 
GoToDialog::GoToDialog(const TRect& pos,const char* title,AddressFinder& _addrFind):
    TWindowInit( &TDialog::initFrame ),
    TDialog( pos, title ),
    addrFind(_addrFind)
  {
    Placer placer(2,1);
    inputLine=new TDynamicInputLine(placer.place(20,1,true),256);
    insert( inputLine );          
    TRect listRect(placer.place(19,5));
    TScrollBar * scrollb=new TScrollBar(placer.place(1,5,true));
    scrollb->options|=ofPostProcess;
    insert(scrollb);
    listBox=new TListBox(listRect,1,scrollb);
    list = new TSearchResults();
    listBox->newList(list);
    insert(listBox);
    placer.newLine();   
    placer.space(-1);
    insert( new TButton(placer.place(10,2), "OK", cmOK,
                    bfDefault ));
    insert( new TButton(placer.place(10,2), "Cancel", cmCancel,
                                  bfNormal ));   
    selectNext(false);

  }

