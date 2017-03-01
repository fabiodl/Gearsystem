#ifndef _SPINNER_
#define _SPINNER_

#include <stdint.h>
#include <thread>
#include <mutex>
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
  typedef std::function<bool()> HaltCondition;
  Spinner();
  ~Spinner();
  void setWork(Work* work,const HaltCondition* haltCondition);
  bool isRunning();
  bool isIdle();
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
