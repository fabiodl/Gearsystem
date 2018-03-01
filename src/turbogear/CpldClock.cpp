#include "CpldClock.h"
#include <iostream>
using namespace std;

void CpldClock::Tick(unsigned int cpuCycles){
  size_t cycles=cpuCycles*3300000/3579540;
  //cout<<"running "<<cycles<<" cycles"<<endl;
  for (size_t i=0;i<cycles;i++){
    onTick();
  }
}
