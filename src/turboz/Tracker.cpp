#include "Tracker.h"
#include <iostream>

using namespace std;


WriteTracker::WriteTracker():enabled(false){
  
};


void WriteTracker::setEnable(bool b){
  enabled=b;
}

void WriteTracker::eval(uint32_t addr,uint8_t &data,bool _ce,bool _we){
  if (enabled&&!_ce&&!_we){
    writes[addr].push_back(data);
  }
}


void WriteTracker::printLast(){
  for (auto& p:writes){
    cout<<std::hex<<p.first<<"="<<(int)p.second.back()<<"("<<p.second.back()<<")"<<std::dec<<endl;
  }
}


void WriteTracker::printRanges(){
  for (auto& p:writes){
    bool isHead=writes.find(p.first-1)==writes.end();
    bool isTail=writes.find(p.first+1)==writes.end();
    cout<<std::hex;
    if (isHead){
      cout<<"[";
    }    
    if (isHead||isTail){
      cout<<p.first;
    }
    if (isHead&&!isTail){
      cout<<"~";
    }
    if (isTail){
      cout<<"]"<<std::endl;
    }
    
  }
  
}


void WriteTracker::reset(){
  writes.clear();
}
