#include "TGSystem.h"
#include <GameGearIOPorts.h>
#include <SmsIOPorts.h>

template<typename T>
class NoAudio:public T{
public:
  NoAudio(Video* pVideo, Input* pInput, Cartridge* pCartridge):
    T(nullptr,pVideo,pInput,pCartridge)
  {
  }

  void DoOutput(u8 port, u8 value)
  {
    if (!((port >= 0x40) && (port < 0x80))){
      T::DoOutput(port,value);
    }
  }
  
};


TGSystem::TGSystem(SystemType systemType):
  video(&memory,&processor),
  input(&processor)//for requesting NMI
{
  switch(systemType){
  case MASTERSYSTEM:
    ioports.reset(new NoAudio<SmsIOPorts>(&video,&input,&cartridge));
    break;
  case GAMEGEAR:
    ioports.reset(new NoAudio<GameGearIOPorts>(&video,&input,&cartridge));
    break;
  }
  processor.SetIOPorts(ioports.get());
  //audio.Init();
  //SDL_PauseAudio(true);
  video.Init();
  input.Init();
  frameBuffer.setBlanked();
  //audio.Enable(false);
  cpldClock.onTick=[](){};
}


unsigned int TGSystem::Tick(){
  inputBuffer.copyTo(input);
  unsigned int cpuCycles = processor.Tick();
  bool vblank = video.Tick(cpuCycles,frameBuffer.get());  
  frameBuffer.release();
  //audio.Tick(cpuCycles);
  input.Tick(cpuCycles);
  cpldClock.Tick(cpuCycles);
  if (vblank){
    //    std::cout<<"vblank arrived"<<std::endl;
    //audio.EndFrame();
    frameBuffer.setBlanked();
  }
  return cpuCycles;
}
