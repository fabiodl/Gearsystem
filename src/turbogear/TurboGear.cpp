#include <csignal>
#include <TurboZ.h>
#include <GameGearIOPorts.h>
#include <Video.h>
#include <Audio.h>
#include <Input.h>

TurboZ* tzInstance=nullptr;
void finalize(int signal){
  tzInstance->onExit();
   exit(0);
}


class TurboGear{ 
public:
  TurboGear();    
  System system;
private:
  Audio audio;
  Video video;
  Input input;
  GameGearIOPorts ioports;
};

TurboGear::TurboGear():
  video(&system.memory,&system.processor),
  input(&system.processor),//for requesting NMI
  ioports(&audio,&video,&input,&system.cartridge)
{
  system.processor.SetIOPorts(&ioports);
}


int main(){
  
  
  TurboGear turboGear;
  
  TurboZ turboz(turboGear.system);  
  tzInstance=&turboz;
  std::signal(SIGINT,finalize);
  turboz.run();
  return 0;
}




