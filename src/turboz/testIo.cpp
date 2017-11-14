#include "PhysicalIo.h"
#include <iostream>


void testOut(PhysicalIo& io){
  uint16_t x;
  while(true){
    std::cout<<">"<<std::flush;
    std::cin>>std::hex>>x;
    io.write((uint8_t)x);
    std::cout<<"wrote "<<std::hex<<x<<std::endl;
  }
}

void testIn(PhysicalIo& io){
  while(true){
    std::cout<<io.read()<<std::endl;
  }
   
}

int main(){
  PhysicalIo io;
  testIn(io);
  //testOut(io);
}
