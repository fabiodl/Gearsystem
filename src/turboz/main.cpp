#include <csignal>
#include "TurboZ.h"

TurboZ* tzInstance=nullptr;
void finalize(int signal){
  tzInstance->onExit();
   exit(0);
}

int main(){
  System system;
  TurboZ turboz(system);  
  tzInstance=&turboz;
  std::signal(SIGINT,finalize);
  turboz.run();
  return 0;
}
