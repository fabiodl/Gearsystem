#ifndef _TURBOZ_H_
#define _TURBOZ_H_

#define Uses_TApplication
#define Uses_TPalette

#include <tv.h>
#include "Spinner.h"
#include <stdint.h>
#include <atomic>
#include <libconfig.h++>
#include "System.h"


class TurboZ : public TApplication
{
public:
  TurboZ(System& system);
  static TMenuBar * initMenuBar( TRect r );
  static TStatusLine *initStatusLine(TRect r);
  void handleEvent(TEvent& event);
  void remove(TView* view);
  TPalette& getPalette() const;
  void idle();
  void requestRun(Spinner::Work* work,Spinner::HaltCondition* haltCondition);
  void requestHalt();
  void onExit();
  ~TurboZ();
private:
  template<typename WindowType> void showWindow();
  template<typename WindowType> void addWindow();
  template<typename Window>  friend class WindowFactory;
  System& system;
  TPalette palette;
  Spinner spinner;
  void refreshState();
  void refreshSymbols();
  void openFile( const char *fileSpec);
  void reload();
  std::atomic_bool spinnerHalted;
  libconfig::Config cfg;
  enum LogType{WARNING,OK};
  void log(LogType,const std::string& s);
  void setPalette();
  void loadRam();
  void saveRam();
  std::string getLastDirectory(const char* property);
  void storeFilename(const char* property,const char* filename);
  void loadBreakpoints();
  void storeBreakpoints();
  void storeConfig();
};

#endif
