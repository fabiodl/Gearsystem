#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "definitions.h"

#include <mutex>
#include <atomic>
#include <vector>

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



#endif
