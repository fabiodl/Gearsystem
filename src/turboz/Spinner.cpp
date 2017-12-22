#include "Spinner.h"
#include <iostream>

Spinner::Spinner():
  state(idle),
  spinner([this]{waitForWork();})
{
  
}

bool Spinner::isRunning(){
  return state==working;
}

bool Spinner::isIdle(){
  return state==idle;
}

Spinner::~Spinner(){
  {
    std::lock_guard<std::mutex> lock(workm);
    state=destroyRequested;
    cv.notify_all();
  }  
  spinner.join();
}

void Spinner::blockingHalt(){
  std::unique_lock<std::mutex> lock(workm);
  while(state!=idle && state!= destroyRequested){
    if (state==working){
      state=haltRequested;
    }
    cv.wait(lock);
  }
}

void Spinner::nonblockingHalt(){
  std::unique_lock<std::mutex> lock(workm);
  if (state==working||state==workRequested){
    state=haltRequested;
  }
      
}

void Spinner::setWork(Work* _work,const HaltCondition* _haltCondition){
  //std::cout<<"setting work"<<std::endl;
  std::unique_lock<std::mutex> lock(workm);
  while(state!=idle && state!= destroyRequested){
    if (state==working){
      state=haltRequested;
    }
    cv.wait(lock);
  }
  if (state==destroyRequested){
    return;
  }

  work=_work;
  haltCondition=_haltCondition;
  state=workRequested;
  cv.notify_all();
}

template<typename T>
class unlock_guard{
  T & m;
public:
  unlock_guard(T & _m):m(_m){
    m.unlock();
  }


  ~unlock_guard(){
    m.lock();
  }
  
};

void Spinner::waitForWork(){
  std::unique_lock<std::mutex> lock(workm);
  while(state!=destroyRequested){
    if (state==workRequested){
      state=working;      
      //std::cout<<"work init"<<std::endl;
      work->init();
      //std::cout<<"working"<<std::endl;
      while(state==working){ //i.e. not haltRequested or destroyRequested
        unlock_guard<std::unique_lock<std::mutex> > unlocker(lock);
        try{
          if (haltCondition->preWork()){
            break;
          }
          work->job();
          if (haltCondition->postWork()){
            break;
          }
          //std::cout<<"work called"<<std::endl;
        }catch(...){
          work->teptr = std::current_exception();                  
        }        
      }//while state==working
      for (auto c:onHalt){
        c();
      }
      if (state==working){
	state=idle;
      }
      cv.notify_all();
      //std::cout<<"now idle"<<std::endl;
    }//if state==workRequested
   
    if (state!=destroyRequested){
      cv.wait(lock);
    }
  }
}
