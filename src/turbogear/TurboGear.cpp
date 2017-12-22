#include <csignal>
#include <TurboZ.h>
#include <GameGearIOPorts.h>
#include <SmsIOPorts.h>
#include <Video.h>
#include <Audio.h>
#include <Input.h>
#include "VDPView.h"
#include <mutex>
#include <atomic>
#include <memory>

enum SystemType{GAMEGEAR,MASTERSYSTEM};

TurboZ* tzInstance=nullptr;
void finalize(int signal){
  tzInstance->onExit();
   exit(0);
}

class FrameBuffer{
public:
  GS_Color* get();
  void release();
  FrameBuffer();
  void setBlanked();
  bool isBlanked();
private:  
  std::mutex mutex;
  std::vector<GS_Color> frameBuffer;
  std::atomic_bool blanked;
};


class TGSystem:public System{
public:
  virtual unsigned int Tick();
  Audio audio;
  Video video;
  Input input;
  std::unique_ptr<IOPorts> ioports;
  FrameBuffer frameBuffer;
  TGSystem(SystemType systemType);
   
};


class TurboGear:public TurboZ{
public:
  TurboGear(TGSystem& tgs);
  void idle();
private:
  VDPView vdpView;
  FrameBuffer& frameBuffer;
};


static const GS_Color black={0,0,0,0};

FrameBuffer::FrameBuffer():
  frameBuffer(GS_SMS_WIDTH*GS_SMS_HEIGHT,black)
{
}

void FrameBuffer::setBlanked(){
  blanked=true;
}
  
bool FrameBuffer::isBlanked(){
  return blanked.exchange(false);
}


GS_Color* FrameBuffer::get(){
  mutex.lock();
  return frameBuffer.data();
}

void FrameBuffer::release(){
  mutex.unlock();
}

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

TurboGear::TurboGear(TGSystem& tgs):
  TProgInit( &TurboZ::initStatusLine,
             &TurboZ::initMenuBar,
             &TurboZ::initDeskTop
             ),
  TurboZ(tgs),
  frameBuffer(tgs.frameBuffer)
{
  
}


void TurboGear::idle(){
  TurboZ::idle();
  if (frameBuffer.isBlanked()){
    vdpView.draw(frameBuffer.get());
    frameBuffer.release();
  }
    
}



int main(){    
  TGSystem tgSystem(MASTERSYSTEM);  
  TurboGear turboz(tgSystem);  
  tzInstance=&turboz;
  std::signal(SIGINT,finalize);
  
  turboz.run();
  return 0;
}




