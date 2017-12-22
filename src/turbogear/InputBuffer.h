#ifndef _INPUTBUFFER_H_
#define _INPUTBUFFER_H_

#include "Input.h"
#include <mutex>
#include <vector>

class InputBuffer{
public:
  InputBuffer();
  InputBuffer(const InputBuffer&)=delete;
  enum KeyState{DOWN,UP,STICKY};
  typedef std::vector<KeyState> InputState[2];//one for each joypad
  void copyTo(Input& input);
  void set(GS_Joypads pad,GS_Keys btn,KeyState state);

  InputState& getState();
  void releaseState();
private:
  std::mutex mutex;
  InputState state;
};


#endif
