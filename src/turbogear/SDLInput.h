#ifndef _SDLINPUT_H_
#define _SDLINPUT_H_

#include <SDL.h>
#include <vector>
#include <map>
#include "InputBuffer.h"

class SDLInput{
public:
  SDLInput();
  void get(InputBuffer& buffer);
 

private:
  std::map<SDL_Keycode,std::pair<GS_Joypads,GS_Keys> > keyMap;
  void push(InputBuffer& buffer,SDL_Keycode sym,uint16_t mod);
  void release(InputBuffer& buffer,SDL_Keycode sym,uint16_t mod);
  SDL_Keycode globalRelease;
};



#endif
