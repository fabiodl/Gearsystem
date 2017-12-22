#include "TurboGear.h"

TurboGear::TurboGear(TGSystem& tgs):
  TProgInit( &TurboZ::initStatusLine,
             &TurboZ::initMenuBar,
             &TurboZ::initDeskTop
             ),
  TurboZ(tgs),
  frameBuffer(tgs.frameBuffer),
  inputBuffer(tgs.inputBuffer)
{
  
}



void TurboGear::idle(){
  TurboZ::idle();
  if (frameBuffer.isBlanked()){
    vdpView.draw(frameBuffer.get());
    frameBuffer.release();
  }
  sdlInput.get(inputBuffer);
}




