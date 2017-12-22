#ifndef _TURBOGEAR_H_
#define _TURBOGEAR_H_

#include <TurboZ.h>
#include "TGSystem.h"
#include "VDPView.h"
#include "SDLInput.h"



class TurboGear:public TurboZ{
public:
  TurboGear(TGSystem& tgs);
  void idle();
private:
  VDPView vdpView;
  SDLInput sdlInput;
  FrameBuffer& frameBuffer;
  InputBuffer& inputBuffer;
};



#endif
