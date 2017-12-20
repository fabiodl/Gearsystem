#include "VDPView.h"
#include <stdexcept>

class SDLException:public std::runtime_error{
public:
  SDLException(const std::string& prefix):
    std::runtime_error(prefix+":"+SDL_GetError()){}
};


  VDPView::VDPView(int _width,int _height):
    width(_width),height(_height)
  {   
  if (SDL_Init(SDL_INIT_VIDEO)<0){
    throw SDLException("SLD Init error");
  }

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "VDP View",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        width,                               // width, in pixels
        height,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL) {
      throw SDLException("SDL CreateWindow error");
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer,
				SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET,
				width,height);
    // The window is open: could enter program loop here (see SDL_PollEvent())
    //SDL_Delay(3000);  // Pause execution for 3000 milliseconds, for example

}

void VDPView::draw(GS_Color* FrameBuffer){  
  SDL_SetRenderTarget(renderer, texture);
  SDL_UpdateTexture(texture,NULL,FrameBuffer,width*4);
  /*SDL_Rect r;
  r.x=rand()%500;
  r.y=rand()%500;
  SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
  SDL_RenderClear(renderer);
  SDL_RenderDrawRect(renderer,&r);
  SDL_SetRenderDrawColor(renderer, 0xCC, 0xFF, 0x00, 0x00);
  SDL_RenderFillRect(renderer, &r);*/

  SDL_SetRenderTarget(renderer, NULL);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

}


VDPView::~VDPView(){
  SDL_Quit();
}
