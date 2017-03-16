#ifndef _SPINNER_
#define _SPINNER_

#include <stdint.h>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
class Spinner{

public:

  class Work{
  public:
    std::function<void()> init;
    std::function<void()> job;
    std::exception_ptr teptr;
    Work():teptr(nullptr){
    }
  };
  class HaltCondition{
  public:
    std::function<bool()> preWork,postWork;
  };    
  Spinner();
  ~Spinner();
  void blockingHalt();
  void nonblockingHalt();
  void setWork(Work* work,const HaltCondition* haltCondition);
  bool isRunning();
  bool isIdle();
  std::vector<std::function<void()> > onHalt;
private:
  enum State{
    idle,workRequested,working,haltRequested,destroyRequested
  };
  void waitForWork(); 
  std::mutex workm; 
  volatile State state;
  std::condition_variable cv;
  Work* work;
  const HaltCondition* haltCondition;
  std::thread spinner;
};

#endif
