#include "SDLInput.h"

using namespace std;

SDLInput::SDLInput(){
  globalRelease=SDLK_r;

  keyMap[SDLK_UP]=make_pair(Joypad_1,Key_Up);
  keyMap[SDLK_DOWN]=make_pair(Joypad_1,Key_Down);
  keyMap[SDLK_LEFT]=make_pair(Joypad_1,Key_Left);
  keyMap[SDLK_RIGHT]=make_pair(Joypad_1,Key_Right);
  keyMap[SDLK_x]=make_pair(Joypad_1,Key_1);
  keyMap[SDLK_c]=make_pair(Joypad_1,Key_2);
  keyMap[SDLK_s]=make_pair(Joypad_1,Key_Start);
  keyMap[SDLK_p]=make_pair(Joypad_1,Key_Start);
  
}


void SDLInput::get(InputBuffer& buffer){
  SDL_Event event;
  while (SDL_PollEvent( &event )){
    switch( event.type ){
    case SDL_KEYDOWN:
      push(buffer,event.key.keysym.sym,event.key.keysym.mod);
      break;      
    case SDL_KEYUP:
      release(buffer,event.key.keysym.sym,event.key.keysym.mod);
      break;
      
    default:
      break;
    }    
  }
}



void SDLInput::push(InputBuffer& buffer,SDL_Keycode sym,uint16_t mod){
  auto it=keyMap.find(sym);
  if ( it !=keyMap.end()){
    buffer.set(it->second.first,it->second.second,
	       mod&KMOD_SHIFT?InputBuffer::STICKY:InputBuffer::DOWN);

  }
  if (sym==globalRelease){
    InputBuffer::InputState& state=buffer.getState();
    for (int j=0;j<2;j++){
      for (auto& s:state[j]){
	s=InputBuffer::UP;
      }
    }
    buffer.releaseState();
  }  
}


void SDLInput::release(InputBuffer& buffer,SDL_Keycode sym,uint16_t mod){
  auto it=keyMap.find(sym);
  if ( it !=keyMap.end()){
    InputBuffer::InputState& state=buffer.getState();
    InputBuffer::KeyState& s=state[it->second.first][it->second.second];
    if (s==InputBuffer::DOWN&& !(mod& KMOD_SHIFT) ){
      s=InputBuffer::UP;
    }
    buffer.releaseState();
  }
}
