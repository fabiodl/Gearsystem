#ifndef _COMMANDS_H_
#define _COMMANDS_H_

enum Commands{
  cmTextEdited=200,
  cmShowProcessorWindow,  
  cmShowExecutionWindow,
  cmShowSymbolWindow,
  cmAddDisassemblyWindow,
  cmAddMemoryWindow,
  cmShowSramWindow,
  cmShowIoramWindow,
  cmShowFlashWindow,
  cmRefreshState,
  cmRefreshSymbols,
  cmOptionDialog,
  cmBreakpointDialog,
  cmCycleFormat,
  cmGoTo,
  cmStep,
  cmRun,
  cmHalt,
  cmReset,
  cmOpen,
  cmReload,
  cmLoadRam,
  cmResetRam,
  cmResetDisasm,
  cmResetCartridge,
  cmToggleCartridgeMenu,
  cmBreakpointsClear,
  cmBreakpointsReload,  
  cmDescribeCartridge
};



#endif
