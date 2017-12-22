#ifndef _SDLINPUT_H_
#define _SDLINPUT_H_

#include <SDL.h>
#include "Input.h"
#include <mutex>
#include <vector>
#include <map>

class SDLInput{
public:
  SDLInput();
  void update();
  void apply(Input& input);
  std::mutex mutex;
private:
  enum KeyState{DOWN,UP,STICKY};
  std::vector<KeyState> state[2];//one for each joypad
  std::map<SDL_Keycode,std::pair<GS_Joypads,GS_Keys> > keyMap;
  void push(SDL_Keycode sym,uint16_t mod);
  void release(SDL_Keycode sym,uint16_t mod);
  SDL_Keycode globalRelease;
};



#endif
