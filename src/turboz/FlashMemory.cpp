#include "FlashMemory.h"
#include <fstream>
#include <iostream>
#include <cstring>

class SetKeys{
public:
  FlashMemory::ProgramKeys& keys;

  SetKeys(FlashMemory::ProgramKeys& _keys):keys(_keys){}
  SetKeys& add(uint32_t addr,uint32_t addrMask,uint8_t data,uint8_t dataMask){
    keys.push_back(std::make_pair(FlashMemory::Masked<uint32_t>(addr,addrMask),FlashMemory::Masked<uint8_t>(data,dataMask)));
    return *this;
  }
};



void setSectors(std::vector<FlashMemory::SectorInfo>& sectors,const std::vector<uint32_t>& sizes,uint32_t basicSize){
  uint32_t start=0;
  for (size_t block=0;block<sizes.size();block++){
    uint32_t blockSize=sizes[block];
    for (uint32_t s=0;s<blockSize/basicSize;s++){
      sectors.push_back(FlashMemory::SectorInfo(start,blockSize));
    }
    start+=blockSize;
  }
}

constexpr uint32_t operator "" _Kb (unsigned long long int x )
{ return 1024*x; }

FlashMemory::FlashMemory(uint32_t size):data(size,0xFF){
  SetKeys(detectors[ERASESECTOR].keys)
    .add(0xAAA,0xFFF,0xAA,0xFF)
    .add(0x555,0xFFF,0X55,0xFF)
    .add(0xAAA,0xFFF,0X80,0xFF)
    .add(0xAAA,0xFFF,0xAA,0xFF)
    .add(0x555,0xFFF,0x55,0xFF)
    .add(0x000,0x000,0x30,0xFF);
  SetKeys(detectors[WRITE].keys)
    .add(0xAAA,0xFFF,0xAA,0xFF)
    .add(0x555,0xFFF,0x55,0xFF)
    .add(0xAAA,0xFFF,0xA0,0xFF)
    .add(0x000,0x000,0x00,0x00);
  //if it is a  B type

  std::vector<uint32_t> sizes={16_Kb,8_Kb,8_Kb,32_Kb};
  sizes.insert(sizes.end(),31,64_Kb);  
  setSectors(sectors,sizes,8_Kb);
    
}


void FlashMemory::load(const std::string& fname){
  std::ifstream is(fname.c_str(),std::ifstream::binary);
  is.seekg(0,is.end);
  data.resize(is.tellg());
  is.seekg(0,is.beg);
  is.read(reinterpret_cast<char*>(&data[0]),data.size());
}

void FlashMemory::store(const std::string& fname){
  std::ofstream os(fname.c_str(),std::ios::binary|std::ios::out);
  os.write(reinterpret_cast<char*>(&data[0]),data.size());
}


FlashMemory::ProgramDetector::ProgramDetector(){
  matchCount=0;
}

void FlashMemory::ProgramDetector::write(uint32_t addr,uint8_t data){
  if (keys[matchCount].first.isMatch(addr) && keys[matchCount].second.isMatch(data)){
    matchCount++;
  }else{
    matchCount=0;
  }
}


bool FlashMemory::ProgramDetector::isActive() const{
  return matchCount==keys.size();
}


void FlashMemory::write(uint32_t addr,uint8_t datalines){
  if (addr>data.size()){
    std::cerr<<"warning: address "<<addr<<" > size "<<data.size()<<std::endl;
    addr=addr%data.size();
  }
  if (data[addr]!=0xFF){
    std::cerr<<"warning: writing over data"<<std::endl;
  }
  data[addr]=datalines;
}
  

void FlashMemory::eraseSector(uint32_t sector){
  SectorInfo& info=sectors[sector];
  memset(&data[info.start],0xFF,info.size);
}

void FlashMemory::eval(uint32_t addr,uint8_t& datalines,bool _ce,bool _we,bool _oe){
  if (_ce) return;
  if (!_oe && !_we){
    std::cerr<<"Flash memory has simulataneously _oe and _we low"<<std::endl;
  }
  if (!_oe){
    if (addr>data.size()){
      std::cerr<<"warning: address "<<addr<<" > size "<<data.size()<<std::endl;
      addr=addr%data.size();
    }
    datalines=data[addr];
  }
  if (!_we){
    for (int i=0;i<DETECTOR_NUM;i++){
      detectors[i].write(addr,datalines);
    }
    if (detectors[WRITE].isActive()){
      write(addr,datalines);
    }
    if (detectors[ERASESECTOR].isActive()){
      eraseSector(addr);
    }
  }

}

int FlashMemory::GetROMSize(){
  return data.size();
}

uint8_t* FlashMemory::GetROM(){
  return &data[0];
}
