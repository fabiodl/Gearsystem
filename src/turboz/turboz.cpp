#include "System.h"
#include "ProcessorWindow.h"
#include "ExecutionWindow.h"
#include "DisassemblyWindow.h"
#include "commands.h"
#include "Placer.h"

#define Uses_TDeskTop
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TKeys
#define Uses_TMenuBar
#define Uses_TStatusDef
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TPalette
#define Uses_RProgram
#define Uses_MsgBox
#include <tv.h>
#include <algorithm>
#include "turbozPalette.h"
#include "TurboZ.h"

template<typename Window> class WindowFactory{
public:
  static void addWindow(TGroup* g,TurboZ* turboz);
  static void showWindow(TGroup* g,TurboZ* turboz);
  static TRect initPos;
private:
  static Window* getWindow(TurboZ* turboz);
  static TRect getPos();
};





Placer winPlacer(0,0);
template<> TRect WindowFactory<ProcessorWindow>::initPos(winPlacer.place(15,12,true));
template<> TRect WindowFactory<ExecutionWindow>::initPos(winPlacer.spaceAndPlace(0,1,20,10));
template<> TRect WindowFactory<DisassemblyWindow>::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,60,20),5,0) );


template<typename Window>
void WindowFactory<Window>::addWindow(TGroup* g,TurboZ* turboz){
  Window* win=getWindow(turboz);
  g->insert(win);
  ObjectTracker<Window>::objs.push_back(win);
}

template<typename Window>
TRect WindowFactory<Window>::getPos(){
  TRect p(initPos);
  p.move(ObjectTracker<Window>::objs.size(),ObjectTracker<Window>::objs.size());
  return p;
}


template<typename Window>
void WindowFactory<Window>::showWindow(TGroup* g,TurboZ* turboz){
  if (ObjectTracker<Window>::objs.size()){
    TView* win=ObjectTracker<Window>::objs.back();
    g->setCurrent(win,TGroup::normalSelect);
    win->putInFrontOf(g->first());
  }else{
    addWindow(g,turboz);
  }
}


template<> ProcessorWindow* WindowFactory<ProcessorWindow>::getWindow(TurboZ* turboz){
  return new ProcessorWindow(getPos(),turboz->system.processor);
}

template<> ExecutionWindow* WindowFactory<ExecutionWindow>::getWindow(TurboZ* turboz){
  return new ExecutionWindow(getPos(),turboz->system,turboz);
}

template<> DisassemblyWindow* WindowFactory<DisassemblyWindow>::getWindow(TurboZ* turboz){
  DisassemblyWindow* win=new DisassemblyWindow(getPos(),turboz->system);
  if (ObjectTracker<DisassemblyWindow>::objs.size()==0){
    win->setFollowPC(true);
  }
  return win;
}







void loadPalette(TPalette& palette){
  int background=palette::DARKGRAY;
  int menuback=palette::BLACK;
  palette[TBackground]=palette::BLACK*palette::BACKGROUND+palette::DARKGRAY;
  palette[TMenuView_TextNormal]=menuback*palette::BACKGROUND+palette::CYAN;
  palette[TMenuView_TextDisabled]=menuback*palette::BACKGROUND+palette::DARKGRAY;
  palette[TMenuView_TextShortcut]=menuback*palette::BACKGROUND+palette::CYAN+palette::LIGHT;
  palette[TMenuView_SelectedNormal]=palette::CYAN*palette::BACKGROUND+palette::WHITE;
  palette[TMenuView_SelectedDisabled]=palette::CYAN*palette::BACKGROUND+palette::DARKGRAY;
  palette[TMenuView_SelectedShortcut]=palette::CYAN*palette::BACKGROUND+palette::GRAY; 
  palette[TWindowBlue_FramePassive]=background*palette::BACKGROUND+palette::GRAY;
  palette[TWindowBlue_FrameActive]=background*palette::BACKGROUND+palette::WHITE;
  palette[TWindowBlue_FrameIcon]=background*palette::BACKGROUND+palette::WHITE;
  palette[TWindowBlue_ScrollBarPage]=background*palette::BACKGROUND+palette::GRAY;

  palette[TDialog_FramePassive]= palette[TWindowBlue_FramePassive];
  palette[TDialog_FrameActive]=palette[TWindowBlue_FrameActive];
  palette[TDialog_FrameIcon]=palette[TWindowBlue_FrameIcon];
  palette[TDialog_ScrollBarPage]=palette[TWindowBlue_ScrollBarPage];

  palette[TDialog_LabelNormal]=background*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_LabelHighlight]=background*palette::BACKGROUND+palette::CYAN+palette::LIGHT;

  palette[TDialog_InputLineNormal]=palette::BLACK*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_InputLineSelected]=palette::CYAN*palette::BACKGROUND+palette::WHITE;
  
  palette[TDialog_ButtonShadow]=background*palette::BACKGROUND+palette::BLACK;

  int buttonback=palette::CYAN;
  
  palette[TDialog_ButtonNormal]=buttonback*palette::BACKGROUND+palette::BLUE;
  palette[TDialog_ButtonDefault]=buttonback*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_ButtonSelected]=buttonback*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_ButtonDisabled]=palette::BLACK*palette::BACKGROUND+palette::DARKGRAY;
  palette[TDialog_ButtonShortcut]=buttonback*palette::BACKGROUND+palette::CYAN+palette::LIGHT;


  palette[TDialog_ClusterNormal]=palette[TDialog_LabelNormal];
  palette[TDialog_ClusterSelected]=palette[TDialog_LabelHighlight];
  palette[TDialog_ClusterShortcut]=background*palette::BACKGROUND+palette::BLACK;  

  palette[palette::DEBUG]=palette::YELLOW*palette::BACKGROUND+palette::BLUE;
  palette[palette::DISASM_PREDICTED_HEAD]=background*palette::BACKGROUND+palette::CYAN;
  palette[palette::DISASM_PREDICTED_TAIL]=background*palette::BACKGROUND+palette::BLUE;
  palette[palette::DISASM_CONFIRMED_HEAD]=background*palette::BACKGROUND+palette::WHITE;
  palette[palette::DISASM_CONFIRMED_TAIL]=background*palette::BACKGROUND+palette::GRAY;
  palette[palette::DISASM_CURRENT_PC_LINE]=palette::RED*palette::BACKGROUND+palette::WHITE;
}


TurboZ::TurboZ(System& _system) :
  TProgInit( &TurboZ::initStatusLine,
             &TurboZ::initMenuBar,
             &TurboZ::initDeskTop
             ),
  system(_system),
  palette(nullptr,0),
  spinnerHalted(true)//to trigger a refresh
{
  palette.data=new uint8_t[palette::PALETTE_LENGTH+1];
  palette.data[0]=palette::PALETTE_LENGTH;
  memcpy(palette.data+1,TProgram::getPalette().data+1,TProgram::getPalette().data[0]);
  memset(palette.data+1+TProgram::getPalette().data[0],palette::MAGENTA,palette::PALETTE_LENGTH-TProgram::getPalette().data[0]);
  loadPalette(palette);
         
  showWindow<ProcessorWindow>();
  addWindow<DisassemblyWindow>();
  showWindow<ExecutionWindow>();
  //std::cout<<"turboz is "<<((TView*)(this))<<std::endl;
  //std::cout<<"desktop is"<<(TView*)deskTop<<std::endl;
  disableCommand(cmOptionDialog);
  disableCommand(cmGoTo);
  disableCommand(cmHalt);
  spinner.onHalt.push_back([this]{
      spinnerHalted=true;      
    });
}

void TurboZ::handleEvent(TEvent& event){
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
    case cmQuit:
      if (messageBox("Quit, are you sure?",mfYesButton|mfCancelButton)==cmCancel){
        clearEvent(event);
      }
    }
  }
  TApplication::handleEvent(event); 
};

template<typename Window> void TurboZ::showWindow(){
  WindowFactory<Window>::showWindow(deskTop,this);
}

template<typename Window> void TurboZ::addWindow(){
  WindowFactory<Window>::addWindow(deskTop,this);
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
     *new TMenuItem( "E~x~it", cmQuit, kbAltX, hcNoContext, "" )+
     *new TSubMenu( "~V~iew", kbAltW )+
     *new TMenuItem( "~R~egisters", cmShowProcessorWindow,  kbNoKey, hcNoContext, "" )
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
     *new TStatusItem( "~C~pu", kbAltC, cmShowProcessorWindow)+
     *new TStatusItem( "~D~isasm", kbAltD, cmAddDisassemblyWindow)+
     *new TStatusItem( "~E~xec", kbAltE, cmShowExecutionWindow)+

     *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )+
     *new TStatusItem( "~Alt-G~ Go to", kbAltG, cmGoTo )+
     *new TStatusItem( "~Alt-O~ Options", kbAltO, cmOptionDialog )

     // define an item
     );
  

}


TPalette& TurboZ::getPalette() const{ 
 return const_cast<TPalette&>(palette);
};



void TurboZ::refresh(){
  message(this,evBroadcast,cmRefreshState,NULL);
}


void TurboZ::idle(){    

  if (!spinner.isIdle()){
    refresh();    
  }

  if(spinnerHalted.exchange(false)){
    enableCommand(cmRun);
    disableCommand(cmHalt);    
    refresh();    
  }
  
  TApplication::idle();
  static bool firstRun=true;
  if (firstRun){
    redraw();
    deskTop->redraw();
    firstRun=false;      
  }
    
}

void TurboZ::requestRun(Spinner::Work* work,Spinner::HaltCondition* haltCondition){
  spinner.setWork(work,haltCondition);
}

void TurboZ::requestHalt(){
  spinner.halt();
}


int main()
{

  System system;
  TurboZ turboz(system);  
  turboz.run();
  return 0;
}
