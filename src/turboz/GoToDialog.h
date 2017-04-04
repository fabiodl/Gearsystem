#ifndef _GOTODIALOG_H_
#define _GOTODIALOG_H_

#define Uses_TDialog

#include <tv.h>
#include "Symbols.h"
#include "Processor.h"
#include "ProcessorSymbols.h"

class TSearchResults;
class TListBox;
class TInputLine;
class GoToDialog:public TDialog{
public:  
  GoToDialog(const TRect& pos,const char* title,Symbols& symbols,Processor& processor);
  void handleEvent(TEvent& event);
  virtual Boolean valid( ushort command );  
  uint16_t getChoice();//assumes valid returned true
private:
  const char* getInputLine();
  void setInputLine(const char* s);
  void updateChoices(const char* s);
  int32_t getHex(); //returns -1 when invalid
  TSearchResults* list;
  TInputLine* inputLine;
  TListBox* listBox;
  Symbols& symbols;
  ProcessorSymbols processorSymbols;
};


#endif
