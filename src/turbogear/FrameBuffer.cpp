#include "FrameBuffer.h"

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

