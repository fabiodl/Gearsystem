#ifndef _GOTODIALOG_H_
#define _GOTODIALOG_H_

#define Uses_TDialog

#include <tv.h>
#include "Symbols.h"
#include "AddressFinder.h"

class TSearchResults;
class TListBox;
class TInputLine;
class GoToDialog:public TDialog{
public:  
  GoToDialog(const TRect& pos,const char* title,AddressFinder& addrFind);
  void handleEvent(TEvent& event);
  virtual Boolean valid( ushort command );  
  uint16_t getChoice();//assumes valid returned true
private:
  const char* getInputLine();
  void setInputLine(const char* s);
  void updateChoices(const char* s);
  TSearchResults* list;
  TInputLine* inputLine;
  TListBox* listBox;
  AddressFinder& addrFind;
};


#endif
