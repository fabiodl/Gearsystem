#ifndef _FLASHCARTRIDGE_H_
#define _FLASHCARTRIDGE_H_

#include "FlashMemory.h"
#include "Cartridge.h"


class FlashCartridge:public Cartridge,public FlashMemory{

public:
  
  bool LoadFromFile(const char* path){
    //Cartridge::LoadFromFile(path);
    FlashMemory::load(path);
    return true;
  }

};

#endif
