#ifndef _TGSYSTEM_H_
#define _TGSYSTEM_H_

#include <memory>
#include <Video.h>
#include <Audio.h>
#include <Input.h>
#include "System.h"
#include "FrameBuffer.h"
#include "InputBuffer.h"
#include "CpldClock.h"

class TGSystem:public System{
public:
enum SystemType{GAMEGEAR,MASTERSYSTEM};
  Audio audio;
  Video video;
  Input input;
  CpldClock cpldClock;
  std::unique_ptr<IOPorts> ioports;
  FrameBuffer frameBuffer;
  InputBuffer inputBuffer;
  TGSystem(SystemType systemType);
  virtual unsigned int Tick();
};

#endif
