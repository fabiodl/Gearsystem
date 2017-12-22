#include "TGSystem.h"
#include <GameGearIOPorts.h>
#include <SmsIOPorts.h>


TGSystem::TGSystem(SystemType systemType):
  video(&memory,&processor),
  input(&processor)//for requesting NMI
{
  switch(systemType){
  case MASTERSYSTEM:
    ioports.reset(new SmsIOPorts(&audio,&video,&input,&cartridge));
    break;
  case GAMEGEAR:
    ioports.reset(new GameGearIOPorts(&audio,&video,&input,&cartridge));
    break;
  }
  processor.SetIOPorts(ioports.get());
  audio.Init();
  SDL_PauseAudio(true);
  video.Init();
  input.Init();
  frameBuffer.setBlanked();
  audio.Enable(false);
}


unsigned int TGSystem::Tick(){
  inputBuffer.copyTo(input);
  unsigned int cpuCycles = processor.Tick();
  bool vblank = video.Tick(cpuCycles,frameBuffer.get());  
  frameBuffer.release();
  audio.Tick(cpuCycles);
  input.Tick(cpuCycles);
  if (vblank){
    audio.EndFrame();
    frameBuffer.setBlanked();
  }
  return cpuCycles;
}
