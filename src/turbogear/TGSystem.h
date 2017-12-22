#ifndef _TGSYSTEM_H_
#define _TGSYSTEM_H_

#include <memory>
#include <Video.h>
#include <Audio.h>
#include <Input.h>
#include "System.h"
#include "FrameBuffer.h"


class TGSystem:public System{
public:
enum SystemType{GAMEGEAR,MASTERSYSTEM};
  Audio audio;
  Video video;
  Input input;
  std::unique_ptr<IOPorts> ioports;
  FrameBuffer frameBuffer;
  TGSystem(SystemType systemType);
  virtual unsigned int Tick();
};

#endif
