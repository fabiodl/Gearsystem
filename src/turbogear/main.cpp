#include <csignal>
#include "TurboGear.h"


TurboZ* tzInstance=nullptr;
void finalize(int signal){
  tzInstance->onExit();
   exit(0);
}

int main(){    
  TGSystem tgSystem(TGSystem::MASTERSYSTEM);  
  TurboGear turboz(tgSystem);  
  tzInstance=&turboz;
  std::signal(SIGINT,finalize);
  
  turboz.run();
  return 0;
}





