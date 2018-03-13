#include "TurboGear.h"
#include <iostream>
using namespace std;

TurboGear::TurboGear(TGSystem& _tgs):
  TProgInit( &TurboZ::initStatusLine,
             &TurboZ::initMenuBar,
             &TurboZ::initDeskTop
             ),
  TurboZ(_tgs),
  frameBuffer(_tgs.frameBuffer),
  inputBuffer(_tgs.inputBuffer),
  tgs(_tgs)
{
   addCliActions();
}



void TurboGear::idle(){
  TurboZ::idle();
  if (frameBuffer.isBlanked()){
    vdpView.draw(frameBuffer.get());
    frameBuffer.release();
  }
  sdlInput.get(inputBuffer);
}


void TurboGear::addCliActions(){
  actions["tic"]=[this]()
    {
      tgs.totalTicks=0;
    };
  actions["tac"]=[this]()
    {
      cout<<"Elapsed "<<tgs.totalTicks<<" clocks ("<<tgs.totalTicks*1.0/3579540<<"s for NTSC, "<<tgs.totalTicks*1.0/3546893<<"s for PAL)"<<endl;
    };
}


