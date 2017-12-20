#ifndef _VDPVIEW_H_
#define _VDPVIEW_H_

#include <definitions.h>
#include "SDL.h"
#include <memory>

class VDPView{  
public:
  VDPView(int width=GS_SMS_WIDTH,int height=GS_SMS_HEIGHT);
  ~VDPView();
  void draw(GS_Color* FrameBuffer);
private:

  template<typename T,void (*df)(T*)> class SDLPointer{
  public:
    SDLPointer():p(nullptr){}
    SDLPointer(const SDLPointer& )=delete;
    void operator=(T* t){p=t;}
    operator T*(){return p;}
    ~SDLPointer(){
      if (p) df(p);
    }

    T* p;
  };
  
  int width,height;
  SDLPointer<SDL_Window,&SDL_DestroyWindow> window;
  SDLPointer<SDL_Renderer,&SDL_DestroyRenderer> renderer;
  SDLPointer<SDL_Texture,&SDL_DestroyTexture> texture;
};


#endif
