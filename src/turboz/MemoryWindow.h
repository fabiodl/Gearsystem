
#include "AddressableWindow.h"
#include "TrackedObject.h"
#include "MemoryInterface.h"

class MemoryWindow:public AddressableWindow{
public:
  MemoryWindow(const TRect& bounds,const std::string& _title,MemoryInterface& mem,AddressFinder& addrFind);
  void cycleFormat();
private:
  virtual void updateTitle(size_t addr);
  virtual void generateContent(TView& sink,TPoint& delta,TPoint& size);
  virtual int addrToScroll(size_t addr);
  virtual size_t scrollToAddr(int scroll);
  enum Format{Hex,Dec,Ascii,FORMATS};
  Format format;
  void handleEvent(TEvent& event);
  MemoryInterface& mem;
  AddressFinder& addrFind;
  std::string titlePrefix;
};


template <int ID> class TrackedMemoryWindow:public MemoryWindow,public Tracked<TrackedMemoryWindow<ID>,ID>{
public:
  TrackedMemoryWindow(const TRect& bounds,const std::string& _title,MemoryInterface& mem,AddressFinder& addrFind):
    TWindowInit( &MemoryWindow::initFrame ),
    MemoryWindow(bounds,_title,mem,addrFind)
  {}
};
