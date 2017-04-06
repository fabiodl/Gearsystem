#include "System.h"
#include "ProcessorWindow.h"
#include "ExecutionWindow.h"
#include "DisassemblyWindow.h"
#include "MemoryWindow.h"
#include "SymbolWindow.h"
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
#include "TExistingFileDialog.h"
#include "fileUtils.h"

static const char* CFGFILENAME="turboz.cfg";
static const char* CFG_LASTOPENED="lastopened";
static const char* CFG_PALETTE="palette";

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
template<> TRect WindowFactory<ExecutionWindow>::initPos(winPlacer.spaceAndPlace(0,1,21,14));
template<> TRect WindowFactory<DisassemblyWindow>::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,60,20),5,0) );

template<> TRect WindowFactory<MemoryWindow>::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,70,20),7,2) );

template<> TRect WindowFactory<SymbolWindow>::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,20,20),9,4) );


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
  if (ObjectTracker<DisassemblyWindow>::objs.size()==1){
    win->setFollowPC(true);
  }
  return win;
}


template<> MemoryWindow* WindowFactory<MemoryWindow>::getWindow(TurboZ* turboz){
  return new MemoryWindow(getPos(),turboz->system);
}


template<> SymbolWindow* WindowFactory<SymbolWindow>::getWindow(TurboZ* turboz){
  return new SymbolWindow(getPos(),turboz->system.symbols);
}


void metalPaletteCore(TPalette& palette,int background,int menuback,int buttonback){
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

  palette[TDialog_StaticText]=background*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_LabelNormal]=background*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_LabelHighlight]=background*palette::BACKGROUND+palette::CYAN+palette::LIGHT;

  palette[TDialog_InputLineNormal]=palette::BLACK*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_InputLineSelected]=palette::CYAN*palette::BACKGROUND+palette::WHITE;  
  palette[TDialog_ButtonShadow]=background*palette::BACKGROUND+palette::BLACK;

  palette[TDialog_ButtonNormal]=buttonback*palette::BACKGROUND+palette::BLUE;
  palette[TDialog_ButtonDefault]=buttonback*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_ButtonSelected]=buttonback*palette::BACKGROUND+palette::WHITE;
  palette[TDialog_ButtonDisabled]=palette::BLACK*palette::BACKGROUND+palette::DARKGRAY;
  palette[TDialog_ButtonShortcut]=buttonback*palette::BACKGROUND+palette::CYAN+palette::LIGHT;

  palette[TDialog_ClusterNormal]=palette[TDialog_LabelNormal];
  palette[TDialog_ClusterSelected]=palette[TDialog_LabelHighlight];
  palette[TDialog_ClusterShortcut]=background*palette::BACKGROUND+palette::BLACK;  

}

void metalPaletteExtension(TPalette& palette,int background){
  palette[palette::DEBUG]=palette::YELLOW*palette::BACKGROUND+palette::BLUE;
  palette[palette::DISASM_PREDICTED_HEAD]=background*palette::BACKGROUND+palette::CYAN;
  palette[palette::DISASM_PREDICTED_TAIL]=background*palette::BACKGROUND+palette::BLUE;
  palette[palette::DISASM_CONFIRMED_HEAD]=background*palette::BACKGROUND+palette::WHITE;
  palette[palette::DISASM_CONFIRMED_TAIL]=background*palette::BACKGROUND+palette::GRAY;
  palette[palette::DISASM_CURRENT_PC_LINE]=palette::CYAN*palette::BACKGROUND+background;
  palette[palette::DISASM_BREAKPOINT]=palette::RED*palette::BACKGROUND+palette::YELLOW;
  palette[palette::DISASM_PC_AT_BREAKPOINT]=palette::MAGENTA*palette::BACKGROUND+background;
}

void TurboZ::setPalette(){
  palette.data=new uint8_t[palette::PALETTE_LENGTH+1];
  palette.data[0]=palette::PALETTE_LENGTH;
  memcpy(palette.data+1,TProgram::getPalette().data+1,TProgram::getPalette().data[0]);
  memset(palette.data+1+TProgram::getPalette().data[0],palette::MAGENTA,palette::PALETTE_LENGTH-TProgram::getPalette().data[0]);

  
  if (!cfg.exists(CFG_PALETTE)){
    cfg.getRoot().add(CFG_PALETTE,libconfig::Setting::TypeString)="metal";
    cfg.writeFile(CFGFILENAME);
  }
  
  std::string paletteId=cfg.getRoot()[CFG_PALETTE];

  if (paletteId=="borland"){
    int background=palette::BLUE;
    metalPaletteExtension(palette,background);
  }else if (paletteId=="metal"){
    int background=palette::DARKGRAY;
    int menuback=palette::BLACK;
    int buttonback=palette::CYAN;
    metalPaletteCore(palette,background,menuback,buttonback);
    metalPaletteExtension(palette,background);
  }else{
    log(WARNING,std::string("Unknown palette "+paletteId));
    int background=palette::DARKGRAY;
    int menuback=palette::BLACK;
    int buttonback=palette::CYAN;
    metalPaletteCore(palette,background,menuback,buttonback);
    metalPaletteExtension(palette,background); 
  }
        
  

  

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
  if (file_exists(CFGFILENAME)){
    cfg.readFile(CFGFILENAME);
  }

  setPalette();
  
  showWindow<ProcessorWindow>();
  addWindow<DisassemblyWindow>();
  showWindow<ExecutionWindow>();
  //std::cout<<"turboz is "<<((TView*)(this))<<std::endl;
  //std::cout<<"desktop is"<<(TView*)deskTop<<std::endl;
  disableCommand(cmOptionDialog);
  disableCommand(cmBreakpointDialog);
  disableCommand(cmGoTo);
  disableCommand(cmCycleFormat);
  
  disableCommand(cmHalt);
  spinner.onHalt.push_back([this]{
      spinnerHalted=true;      
    });
  if (cfg.exists(CFG_LASTOPENED)){
    enableCommand(cmReload);    
  }else{
    disableCommand(cmReload);
  }
}

void TurboZ::handleEvent(TEvent& event){
  try{
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
      case cmAddMemoryWindow:
        addWindow<MemoryWindow>();
        clearEvent(event);
        break;
      case cmShowSymbolWindow:
        showWindow<SymbolWindow>();
        clearEvent(event);
        break;
      case cmQuit:
        if (messageBox("Quit, are you sure?",mfYesButton|mfCancelButton)==cmCancel){
          clearEvent(event);
        }
        break;
      case cmOpen:
        openFile("*.*");        
        clearEvent(event);
        refreshState();
        refreshSymbols();
        break;
      case cmReload:
        reload();
        clearEvent(event);
        refreshState();
        refreshSymbols();
        break;
      }
    }
    TApplication::handleEvent(event);
  }catch(const std::runtime_error& e){
    messageBox(e.what(),mfOKButton);
    clearEvent(event);
  }
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
     *new TMenuItem( "~O~pen", cmOpen, kbF3, hcNoContext, "F3" )+
     *new TMenuItem( "~R~eload", cmReload, kbF5, hcNoContext, "F5" )+
     newLine()+
     *new TMenuItem( "E~x~it", cmQuit, kbAltX, hcNoContext, "" )+
     *new TSubMenu( "~V~iew", kbAltW )+
     *new TMenuItem( "~C~pu", cmShowProcessorWindow,  kbAltC, hcNoContext, "" )+
     *new TMenuItem( "~D~isassembly", cmAddDisassemblyWindow,  kbAltD, hcNoContext, "" )+
     *new TMenuItem( "~M~em", cmAddMemoryWindow,  kbAltM, hcNoContext, "" )+
     *new TMenuItem( "~E~xecution", cmShowExecutionWindow,  kbAltE, hcNoContext, "" )

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
     *new TStatusItem( "~M~em", kbAltM, cmAddMemoryWindow)+
     *new TStatusItem( "S~y~mbol", kbAltY, cmShowSymbolWindow)+
     *new TStatusItem( "~E~xec", kbAltE, cmShowExecutionWindow)+
     *new TStatusItem( "~F3~ Open", kbF3, cmOpen )+
     *new TStatusItem( "~Alt-F3~ Close", kbAltF3, cmClose )+
     *new TStatusItem( "~F5~ Reload", kbF5, cmReload )+
     *new TStatusItem("~G~o to", kbAltG, cmGoTo )+
     *new TStatusItem( "~O~ptions", kbAltO, cmOptionDialog )+
     *new TStatusItem( "~B~reakpt", kbAltB, cmBreakpointDialog )+
     *new TStatusItem( "~H~ex", kbAltH, cmCycleFormat )
     // define an item
     );
  

}


TPalette& TurboZ::getPalette() const{ 
 return const_cast<TPalette&>(palette);
};



void TurboZ::refreshState(){
  message(this,evBroadcast,cmRefreshState,NULL);
}

void TurboZ::refreshSymbols(){
  message(this,evBroadcast,cmRefreshSymbols,NULL);
}



void TurboZ::idle(){    

  if (!spinner.isIdle()){
    refreshState();    
  }

  if(spinnerHalted.exchange(false)){
    enableCommand(cmRun);
    enableCommand(cmStep);
    enableCommand(cmReset);
    disableCommand(cmHalt);    
    refreshState();    
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
  disableCommand(cmRun);
  disableCommand(cmStep);
  disableCommand(cmReset);
  enableCommand(cmHalt);
  spinner.setWork(work,haltCondition);
}

void TurboZ::requestHalt(){
  disableCommand(cmHalt);
  spinner.nonblockingHalt();
}


void TurboZ::openFile( const char *fileSpec)
{

  std::string directory;
  if (cfg.exists(CFG_LASTOPENED)){
    std::string lastOpened;
    cfg.lookupValue(CFG_LASTOPENED,lastOpened);
    size_t lastSlash=lastOpened.find_last_of("/");
    if (lastSlash!=std::string::npos){
      directory=lastOpened.substr(0,lastSlash+1);
    }    
  }

  
  TExistingFileDialog *d= (TExistingFileDialog *)validView
  (
   new TExistingFileDialog(fileSpec, "Select ROM", "~N~ame", fdOpenButton, 100 )
   );
  d->setDirectory(directory.c_str());
  if( d != 0 && deskTop->execView( d ) != cmCancel )
    {
      char fileName[PATH_MAX];
      d->getFileName( fileName );
      libconfig::Setting& root=cfg.getRoot();
      if (!cfg.exists(CFG_LASTOPENED)){
        root.add(CFG_LASTOPENED,libconfig::Setting::TypeString);
      }
      root[CFG_LASTOPENED]=fileName;
      enableCommand(cmReload);    
      try{
        cfg.writeFile(CFGFILENAME);
      }catch(const libconfig::FileIOException &fioex){
        log(WARNING,std::string("Cannot write to ")+CFGFILENAME);
      }
      system.loadCartridge(fileName);
      system.breakpoints.clear();
    }
  CLY_destroy(d);
}


void TurboZ::reload(){
  auto breakpoints=system.breakpoints.getBreakpoints();
  std::vector<std::string> labels;
  std::vector<uint16_t> addrs;
  for (auto addr:breakpoints){
    std::string* thisLabel;
    if ((thisLabel=system.symbols.getLabel(addr))){
      labels.push_back(*thisLabel);
    }else{
      addrs.push_back(addr);
    }
  }
  system.loadCartridge(cfg.getRoot()[CFG_LASTOPENED]);
  system.breakpoints.clear();
  for (auto addr:addrs){
    system.breakpoints.add(addr);
  }
  for (auto label:labels){
    if (system.symbols.isAvailable(label)){
      system.breakpoints.add(system.symbols.getAddress(label));
    }
  }
}

void TurboZ::log(TurboZ::LogType,const std::string& ){
  //add to a log window...
}


int main()
{

  System system;
  TurboZ turboz(system);  
  turboz.run();
  return 0;
}
