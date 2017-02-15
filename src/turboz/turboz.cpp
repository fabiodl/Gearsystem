#include "System.h"
#include "ProcessorWindow.h"
#include "ExecutionWindow.h"
#include "DisassemblyWindow.h"
#include "commands.h"
#include "Placer.h"
#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TKeys
#define Uses_TMenuBar
#define Uses_TStatusDef
#define Uses_TStatusLine
#define Uses_TStatusItem
#include <tv.h>
#include <algorithm>


template<typename Window> class WindowFactory{
public:
  static void addWindow(TGroup* g,System& system);
  static void showWindow(TGroup* g,System& system);
private:
  static Window* getWindow(System& system);
  static TRect initPos;
};





Placer winPlacer(0,1);
template<> TRect WindowFactory<ProcessorWindow>::initPos(winPlacer.place(20,12));
template<> TRect WindowFactory<ExecutionWindow>::initPos(winPlacer.place(20,5));
template<> TRect WindowFactory<DisassemblyWindow>::initPos(winPlacer.place(20,30));


template<typename Window>
void WindowFactory<Window>::addWindow(TGroup* g,System& system){
  Window* win=getWindow(system);
  g->insert(win);
  ObjectTracker<Window>::objs.push_back(win);
  initPos.move(1,1);
}



template<typename Window>
void WindowFactory<Window>::showWindow(TGroup* g,System& system){
  if (ObjectTracker<Window>::objs.size()){
    g->setCurrent(ObjectTracker<Window>::objs.back(),TGroup::normalSelect);
  }else{
    addWindow(g,system);
  }
}


template<> ProcessorWindow* WindowFactory<ProcessorWindow>::getWindow(System& system){
  return new ProcessorWindow(initPos,system.processor);
}

template<> ExecutionWindow* WindowFactory<ExecutionWindow>::getWindow(System& system){
  return new ExecutionWindow(initPos);
}

template<> DisassemblyWindow* WindowFactory<DisassemblyWindow>::getWindow(System& system){
  return new DisassemblyWindow(initPos,system.disassembly);
}




class TurboZ : public TApplication
{
public:
  TurboZ(System& system);
  static TMenuBar * initMenuBar( TRect r );
  static TStatusLine *initStatusLine(TRect r);
  void handleEvent(TEvent& event);
  void remove(TView* view);
private:
  template<typename WindowType> void showWindow();
  template<typename WindowType> void addWindow();
  System& system;
};

TurboZ::TurboZ(System& _system) :
  TProgInit( &TurboZ::initStatusLine,
             &TurboZ::initMenuBar,
             &TurboZ::initDeskTop
             ),
  system(_system)
{
  //RegisterWindow *regWin = new RegisterWindow(r);
  //newDialog();
  //deskTop->insert(new RegDlg());



}


void TurboZ::handleEvent(TEvent& event){
  TApplication::handleEvent(event); 
  if (event.what==evCommand){
    switch(event.message.command){
    case cmShowProcessorWindow:
      showWindow<ProcessorWindow>();
      clearEvent(event);
      break;
    case cmShowExecutionWindow:
      showWindow<ExecutionWindow>();
      clearEvent(event);
      break;
    case cmAddDisassemblyWindow:
      addWindow<DisassemblyWindow>();
      clearEvent(event);
      break;


    case cmStep:
      system.processor.Tick();
      message(this,evBroadcast,cmRefresh,NULL);
      clearEvent(event);
      break;
    }
  }
};

template<typename Window> void TurboZ::showWindow(){
  WindowFactory<Window>::showWindow(deskTop,system);
}

template<typename Window> void TurboZ::addWindow(){
  WindowFactory<Window>::addWindow(deskTop,system);
}




TMenuBar *TurboZ::initMenuBar( TRect r )
{

  r.b.y = r.a.y + 1;    // set bottom line 1 line below top line
  return new TMenuBar
    (
     r,
     *new TSubMenu( "~F~ile", kbAltF )+
     //*new TMenuItem( "~O~pen", cmOpen, kbF3, hcNoContext, "F3" )+
     newLine()+
     *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "" )+
     *new TSubMenu( "~V~iew", kbAltW )+
     *new TMenuItem( "~R~egisters", cmShowProcessorWindow,  cmShowProcessorWindow, hcNoContext, "" )
     );
}



TStatusLine *TurboZ::initStatusLine(TRect r)
{
  r.a.y = r.b.y - 1;     // move top to 1 line above bottom
  return new TStatusLine
    (
     r,
     *new TStatusDef( 0, 0xFFFF ) +
     // set range of help contexts
     *new TStatusItem( 0, kbF10, cmMenu )+
     *new TStatusItem( "~R~eg", kbAltR, cmShowProcessorWindow)+
     *new TStatusItem( "~E~xec", kbAltE, cmShowExecutionWindow)+
     *new TStatusItem( "~D~isasm", kbAltD, cmAddDisassemblyWindow)+
     *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )
     // define an item
     );
}



 

int main()
{
  System system;
  TurboZ turboz(system);
  turboz.run();
  return 0;
}
