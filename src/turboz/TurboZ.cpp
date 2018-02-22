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

#include "LoadRamDialog.h"



static const char* CFGFILENAME="turboz.cfg";
static const char* CFG_LASTOPENED="lastopened";
static const char* CFG_LASTRAMLOADOPENED="lastRamLoadOpened";
static const char* CFG_LASTRAMSAVEOPENED="lastRamSaveOpened";
static const char* CFG_LASTBREAKPOINTS="breakpoints";
static const char* CFG_PALETTE="palette";



template<typename Window> class WindowFactory{
public:
  static void addWindow(TGroup* g,TurboZ* turboz);
  static void showWindow(TGroup* g,TurboZ* turboz);
  static TRect initPos;
protected:
  static Window* getWindow(TurboZ* turboz);
  static TRect getPos();
};


enum MemoryWindows{
  ADDRSPACE,
  SRAM,
  IORAM,
  ONBOARD,
  FLASH
};


Placer winPlacer(0,0);

template <int ID> using MemWin = TrackedMemoryWindow<ID>;

template<> TRect WindowFactory<ProcessorWindow>::initPos(winPlacer.place(15,12,true));
template<> TRect WindowFactory<ExecutionWindow>::initPos(winPlacer.spaceAndPlace(0,1,21,14));
template<> TRect WindowFactory<DisassemblyWindow>::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,60,20),5,0) );

template<> TRect WindowFactory<MemWin<ADDRSPACE> >::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,70,20),7,2) );
template<> TRect WindowFactory<MemWin<SRAM> >::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,70,20),9,2) );

template<> TRect WindowFactory<MemWin<IORAM> >::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,70,20),11,2) );
template<> TRect WindowFactory<MemWin<ONBOARD> >::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,70,20),13,2) );
template<> TRect WindowFactory<MemWin<FLASH> >::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,70,20),15,2) );


template<> TRect WindowFactory<SymbolWindow>::initPos(Placer::move(Placer::rightOf(WindowFactory<ProcessorWindow>::initPos,20,20),9,4) );


template<typename Window>
void WindowFactory<Window>::addWindow(TGroup* g,TurboZ* turboz){  
  Window * win=getWindow(turboz);
  g->insert(win);
}

template<typename Window>
TRect WindowFactory<Window>::getPos(){
  TRect p(initPos);
  size_t delta=ObjectTracker<Window>::objs.size();
  p.move(delta,delta);
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



template<> MemWin<ADDRSPACE>* WindowFactory<MemWin<ADDRSPACE> >::getWindow(TurboZ* turboz){
  return new MemWin<ADDRSPACE>(getPos(),"Memory",turboz->system.rule.addrIf,turboz->system.addrFind);
}

template<> MemWin<SRAM>* WindowFactory<MemWin<SRAM> >::getWindow(TurboZ* turboz){
  return new MemWin<SRAM>(getPos(),"Sram",turboz->system.rule.sramIf,turboz->system.addrFind);
}

template<> MemWin<IORAM>* WindowFactory<MemWin<IORAM> >::getWindow(TurboZ* turboz){
  return new MemWin<IORAM>(getPos(),"ioram",turboz->system.rule.ioramIf,turboz->system.addrFind);
}

template<> MemWin<ONBOARD>* WindowFactory<MemWin<ONBOARD> >::getWindow(TurboZ* turboz){
  return new MemWin<ONBOARD>(getPos(),"Board",turboz->system.rule.onboardIf,turboz->system.addrFind);
}

template<> MemWin<FLASH>* WindowFactory<MemWin<FLASH> >::getWindow(TurboZ* turboz){
  return new MemWin<FLASH>(getPos(),"Flash",turboz->system.rule.flashIf,turboz->system.addrFind);
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
    storeConfig();
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

void TurboZ::loadRam(){

  LoadRamDialog* d=(LoadRamDialog*) validView
    (new LoadRamDialog(system.addrFind,"Load RAM"));
  if (d){
  
    d->setDirectory(getLastDirectory(CFG_LASTRAMLOADOPENED).c_str());
    if (deskTop->execView( d )!=cmCancel){
      LoadRamDialog::TransferData td=d->getTransferData();
      if (!td.size){
        td.size=0xFFFF-td.dst;
      }
      //std::cout<<"max load "<<td.size<<std::endl;
      storeFilename(CFG_LASTRAMLOADOPENED,td.filename.c_str());
      std::ifstream is(td.filename.c_str(),std::ifstream::binary);
      is.seekg(td.src);
      std::vector<char> buffer(td.size);
      is.read(&buffer[0],td.size);
      //std::cout<<"actually read "<<is.gcount()<<std::endl;
      for (int i=0;i<is.gcount();i++){
        system.memory.Load(td.dst+i,buffer[i]);
      }
      refreshState();
    }
  }
  
      
  CLY_destroy( d );

}



void TurboZ::saveRam(){

  LoadRamDialog* d=(LoadRamDialog*) validView
    (new LoadRamDialog(system.addrFind,"Save RAM"));
  if (d){
  
    d->setDirectory(getLastDirectory(CFG_LASTRAMSAVEOPENED).c_str());
    if (deskTop->execView( d )!=cmCancel){
      LoadRamDialog::TransferData td=d->getTransferData();
      if (!td.size){
        td.size=0xFFFF-td.dst;
      }
      //std::cout<<"max load "<<td.size<<std::endl;
      storeFilename(CFG_LASTRAMSAVEOPENED,td.filename.c_str());
      std::ofstream os(td.filename.c_str(),std::ios::binary|std::ios::out);      
      os.seekp(td.dst);
      std::vector<uint8_t> buffer(td.size);
      for (size_t i=0;i<td.size;i++){
        buffer[i]=system.memory.Retrieve(td.src+i);
      }
      os.write(reinterpret_cast<char*>(&buffer[0]),td.size);
    }
  }
  
      
  CLY_destroy( d );

}


void TurboZ::cliStub(){
  std::cout<<"Enter command:"<<std::flush;
  std::string cmd;
  std::cin>>cmd;

  std::map<std::string,std::function<void()> > actions;
  actions["on"]=[this](){system.rule.onboardTracker.setEnable(true);};
  actions["off"]=[this](){system.rule.onboardTracker.setEnable(false);};
  actions["print"]=[this](){system.rule.onboardTracker.printLast();};
  actions["ranges"]=[this](){system.rule.onboardTracker.printRanges();};
  auto l=actions.find(cmd);
  if (l==actions.end()){
    std::cout<<"Unknown command ["<<cmd<<"]"<<std::endl;
  }else{
    l->second();
    std::cout<<"ok"<<std::endl;
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
        addWindow<MemWin<ADDRSPACE> >();
        clearEvent(event);
        break;
      case cmShowSramWindow:
        showWindow<MemWin<SRAM> >();
        clearEvent(event);
        break;
      case cmShowIoramWindow:
        showWindow<MemWin<IORAM> >();
        clearEvent(event);
        break;
      case cmShowFlashWindow:
        showWindow<MemWin<FLASH> >();
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
      case cmLoadRam:
        loadRam();
        break;
      case cmResetRam:
        system.memory.Init();
        refreshState();
        break;
      case cmResetDisasm:
        system.disassembly.invalidateAll();
        refreshState();
        break;
      case cmResetCartridge:
        system.rule.Reset();
        refreshState();
        break;
      case cmToggleCartridgeMenu:
        system.rule.ToggleMenu();
        refreshState();
        break;
      case cmBreakpointsClear:
	system.breakpoints.clear();
	refreshState();
	break;
      case cmBreakpointsReload:
	loadBreakpoints();
	refreshState();
	break;
      case cmDescribeCartridge:
	std::cout<<system.rule.describe(0)<<std::endl;
	break;
      case cmShowCliWindow:
        cliStub();
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
     *new TMenuItem( "~S~ram", cmShowSramWindow,  kbNoKey, hcNoContext, "" )+
     *new TMenuItem( "~I~oram", cmShowIoramWindow,  kbNoKey, hcNoContext, "" )+
     *new TMenuItem( "~F~lash", cmShowFlashWindow,  kbNoKey, hcNoContext, "" )+
     *new TMenuItem( "~E~xecution", cmShowExecutionWindow,  kbAltE, hcNoContext, "" )+
     *new TMenuItem( "C~L~I", cmShowCliWindow,  kbF9, hcNoContext, "F9" )+
     
     *new TSubMenu("RAM",kbNoKey)+
     *new TMenuItem("Load",cmLoadRam,kbNoKey,hcNoContext,"")+
     *new TMenuItem("Reset",cmResetRam,kbNoKey,hcNoContext,"")+     
     
     *new TSubMenu("Disasm",kbNoKey)+
     *new TMenuItem("Reset",cmResetDisasm,kbNoKey,hcNoContext,"")+

     *new TSubMenu("Cartridge",kbNoKey)+
     *new TMenuItem("Reset",cmResetCartridge,kbNoKey,hcNoContext,"")+
     *new TMenuItem("ToggleMenu",cmToggleCartridgeMenu,kbNoKey,hcNoContext,"")+
     *new TMenuItem("Info",cmDescribeCartridge,kbNoKey,hcNoContext,"")+

     *new TSubMenu("Breakpt",kbNoKey)+
     *new TMenuItem("Clear",cmBreakpointsClear,kbNoKey,hcNoContext,"")+
     *new TMenuItem("Reload",cmBreakpointsReload,kbF6,hcNoContext,"")
     

     

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
    if (cfg.exists(CFG_LASTOPENED)){
      enableCommand(cmReload);    
    }else{
      disableCommand(cmReload);
    }

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
  disableCommand(cmReload);
  enableCommand(cmHalt);
  spinner.setWork(work,haltCondition);
}

void TurboZ::requestHalt(){
  disableCommand(cmHalt);
  spinner.nonblockingHalt();
}


void TurboZ::openFile( const char *fileSpec)
{  
  TExistingFileDialog *d= (TExistingFileDialog *)validView
  (
   new TExistingFileDialog(fileSpec, "Select ROM", "~N~ame", fdOpenButton, 100 )
   );

  if (d){
    d->setDirectory(getLastDirectory(CFG_LASTOPENED).c_str());
    if(deskTop->execView( d ) != cmCancel ){
      char fileName[PATH_MAX];
      d->getFileName( fileName );
      storeFilename(CFG_LASTOPENED,fileName);
      enableCommand(cmReload);    
      system.loadCartridge(fileName);
      system.memory.Init();
      system.breakpoints.clear();
    }
  }
  CLY_destroy(d);
}


class BreakpointSet{
public:
  void retrieveFrom(const System& system);
  void retrieveFrom(const libconfig::Setting& setting);
  void addTo(System& system);
  void addTo(libconfig::Setting& setting);
  std::vector<std::string> labels;
  std::vector<uint16_t> addrs;
};


void BreakpointSet::retrieveFrom(const System& system){
  auto breakpoints=system.breakpoints.getBreakpoints();

  for (auto addr:breakpoints){
    const std::string* thisLabel;
    if ((thisLabel=system.symbols.getLabel(addr))){
      labels.push_back(*thisLabel);
    }else{
      addrs.push_back(addr);
    }
  }
}

void BreakpointSet::retrieveFrom(const libconfig::Setting& bps){
  libconfig::Setting& ls=bps["labels"];
  libconfig::Setting& as=bps["addresses"];
  for (int i=0;i<ls.getLength();i++){
    labels.push_back(ls[i]);
  }
  for (int i=0;i<as.getLength();i++){
    addrs.push_back(static_cast<int>(as[i]));
  }  
}

void BreakpointSet::addTo(libconfig::Setting& bps){
  libconfig::Setting& labelStg=bps.add("labels",libconfig::Setting::TypeArray);
  libconfig::Setting& addrStg=bps.add("addresses",libconfig::Setting::TypeArray); 
  int cnt=0;
  for (auto &label:labels){
    labelStg.add(libconfig::Setting::TypeString);
    labelStg[cnt++]=label;
  }
  cnt=0;
  for (auto &addr:addrs){
    addrStg.add(libconfig::Setting::TypeInt);
    addrStg[cnt].setFormat(libconfig::Setting::FormatHex);
    addrStg[cnt++]=addr;
  }
}

void BreakpointSet::addTo(System& system){
  for (auto addr:addrs){
    system.breakpoints.add(addr);
  }
  for (auto label:labels){
    if (system.symbols.isAvailable(label)){
      system.breakpoints.add(system.symbols.getAddress(label));
    }
  }
}

void TurboZ::reload(){
  BreakpointSet bs;
  bs.retrieveFrom(system);  
  system.loadCartridge(cfg.getRoot()[CFG_LASTOPENED]);
  system.breakpoints.clear();
  bs.addTo(system);
}

void TurboZ::log(TurboZ::LogType,const std::string& ){
  //add to a log window...
}

std::string TurboZ::getLastDirectory(const char* property){
  std::string directory;
  if (cfg.exists(property)){
    std::string lastOpened;
    cfg.lookupValue(property,lastOpened);
    size_t lastSlash=lastOpened.find_last_of("/");
    if (lastSlash!=std::string::npos){
      directory=lastOpened.substr(0,lastSlash+1);
    }    
  }
  return directory;
}

void TurboZ::storeConfig(){
  try{
    cfg.writeFile(CFGFILENAME);
  }catch(const libconfig::FileIOException &fioex){
    log(WARNING,std::string("Cannot write to ")+CFGFILENAME);
  }    
}

void  TurboZ::storeFilename(const char* property,const char* filename){
  libconfig::Setting& root=cfg.getRoot();
  if (!cfg.exists(property)){
    root.add(property,libconfig::Setting::TypeString);
  }
  root[property]=filename;
  storeConfig();
}


void TurboZ::loadBreakpoints(){
  BreakpointSet bps;
  bps.retrieveFrom(cfg.getRoot()[CFG_LASTBREAKPOINTS]);
  bps.addTo(system);
}

void TurboZ::storeBreakpoints(){
  if (cfg.exists(CFG_LASTBREAKPOINTS)){
    cfg.getRoot().remove(CFG_LASTBREAKPOINTS);
  }  
  libconfig::Setting& bps=cfg.getRoot().add(CFG_LASTBREAKPOINTS,libconfig::Setting::TypeGroup);
  BreakpointSet bs;
  bs.retrieveFrom(system);
  bs.addTo(bps); 
  storeConfig();
}

void TurboZ::onExit(){
  storeBreakpoints();
}

TurboZ::~TurboZ(){
  onExit();
}



