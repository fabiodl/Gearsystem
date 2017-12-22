#include "InputBuffer.h"



InputBuffer::InputBuffer(){
  for (int j=0;j<2;j++){
    state[j].resize(Key_Start+1,UP);
  }
}

InputBuffer::InputState& InputBuffer::getState(){
  mutex.lock();
  return state;
}


void InputBuffer::releaseState(){
  mutex.unlock();
}


void InputBuffer::set(GS_Joypads pad,GS_Keys btn,KeyState ks){
  mutex.lock();
  state[pad][btn]=ks;
  mutex.unlock();
}

void InputBuffer::copyTo(Input& input){
  mutex.lock();
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
  mutex.unlock();
}//apply
  
