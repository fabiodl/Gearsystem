#include "SDLInput.h"

using namespace std;

SDLInput::SDLInput(){
  for (int j=0;j<2;j++){
    state[j].resize(Key_Start+1,UP);
  }
  globalRelease=SDLK_r;

  keyMap[SDLK_UP]=make_pair(Joypad_1,Key_Up);
  keyMap[SDLK_DOWN]=make_pair(Joypad_1,Key_Down);
  keyMap[SDLK_LEFT]=make_pair(Joypad_1,Key_Left);
  keyMap[SDLK_RIGHT]=make_pair(Joypad_1,Key_Right);
  keyMap[SDLK_x]=make_pair(Joypad_1,Key_1);
  keyMap[SDLK_c]=make_pair(Joypad_1,Key_2);
  keyMap[SDLK_s]=make_pair(Joypad_1,Key_Start);
  
}


void SDLInput::update(){
  lock_guard<std::mutex> lock(mutex);
  SDL_Event event;
  while (SDL_PollEvent( &event )){
    switch( event.type ){
    case SDL_KEYDOWN:
      push (event.key.keysym.sym,event.key.keysym.mod);
      break;      
    case SDL_KEYUP:
      push (event.key.keysym.sym,event.key.keysym.mod);
      break;
      
    default:
      break;
    }    
  }
}


void SDLInput::apply(Input& input){
  for (int j=0;j<2;j++){
    for (size_t b=0;b<state[j].size();b++){
      switch(state[j][b]){
      case DOWN:
      case STICKY:
	input.KeyPressed(static_cast<GS_Joypads>(j),
			 static_cast<GS_Keys>(b)
			 );
	break;
      case UP:
	input.KeyReleased(static_cast<GS_Joypads>(j),
			  static_cast<GS_Keys>(b)
			  );
      }
    }//for b
  }//for j
}//apply
  
void SDLInput::push(SDL_Keycode sym,uint16_t mod){
  auto it=keyMap.find(sym);
  if ( it !=keyMap.end()){
    state[it->second.first][it->second.second] =mod&KMOD_SHIFT?STICKY:DOWN;
  }
  if (sym==globalRelease){
    for (int j=0;j<2;j++){
      for (auto& s:state[j]){
	s=UP;
      }
    }
  }  
}


void SDLInput::release(SDL_Keycode sym,uint16_t mod){
  auto it=keyMap.find(sym);
  if ( it !=keyMap.end()){
    KeyState& s=state[it->second.first][it->second.second];
    if (s==DOWN&& !(mod& KMOD_SHIFT) ){
      s=UP;
    }
  }
}
