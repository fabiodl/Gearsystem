#include "FlashMemory.h"
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

static constexpr int WRITE_BUSY_CYCLES=33; //ceil(9us*f)
static constexpr int ERASE_BUSY_CYCLES=130;  //130 is only 36u, which is way faster than the real 0.7s


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

FlashMemory::FlashMemory(uint32_t size,bool topType):data(size,0xFF),prev_we(true),busyCycles(0){
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

  if (topType){
    std::vector<uint32_t> sizes={32_Kb,8_Kb,8_Kb,16_Kb};
    sizes.insert(sizes.begin(),31,64_Kb);  
    setSectors(sectors,sizes,8_Kb);
  }else{       
    std::vector<uint32_t> sizes={16_Kb,8_Kb,8_Kb,32_Kb};
    sizes.insert(sizes.end(),31,64_Kb);  
    setSectors(sectors,sizes,8_Kb);
  }
}




void FlashMemory::load(const std::string& fname){
  std::ifstream is(fname.c_str(),std::ifstream::binary);
  is.seekg(0,is.end);
  data.resize(is.tellg());
  is.seekg(0,is.beg);
  is.read(reinterpret_cast<char*>(&data[0]),data.size());
}

void FlashMemory::append(const std::string& fname,size_t offset){
  std::ifstream is(fname.c_str(),std::ifstream::binary);
  is.seekg(0,is.end);
  data.resize(offset+is.tellg());
  is.seekg(0,is.beg);
  is.read(reinterpret_cast<char*>(&data[offset]),data.size()-offset);
}

void FlashMemory::store(const std::string& fname){
  std::ofstream os(fname.c_str(),std::ios::binary|std::ios::out);
  os.write(reinterpret_cast<char*>(&data[0]),data.size());
}


FlashMemory::ProgramDetector::ProgramDetector(){
  matchCount=0;
}

void FlashMemory::ProgramDetector::reset(){
  matchCount=0;
}

void FlashMemory::ProgramDetector::write(uint32_t addr,uint8_t data){
  //cout<<this<<" Detector write "<<hex<<addr<<" "<<(int)data<<dec<<endl;
  if (keys[matchCount].first.isMatch(addr) && keys[matchCount].second.isMatch(data)){
    matchCount++;
    //cout<<"matchCount"<<matchCount<<endl;
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
    std::cerr<<hex<<"flash warning: writing "<<(int)datalines <<" at "<<addr<<" which contains data:"<<(int)data[addr] <<dec<<std::endl;
  }else{
    cout<<hex<<"wrote "<<(int)datalines<<" to "<<addr<<dec<<endl;
  }
  busyCycles=WRITE_BUSY_CYCLES;
  data[addr]=data[addr]&datalines;
}
  

void FlashMemory::eraseSector(uint32_t sector){
  SectorInfo& info=sectors[sector];
  std::cout<<"erasing sector "<<sector<<" starting at "<<info.start<<" size "<<info.size<<endl;
  memset(&data[info.start],0xFF,info.size);
  busyCycles=ERASE_BUSY_CYCLES;
}

void FlashMemory::eval(uint32_t addr,uint8_t& datalines,bool _ce,bool _we,bool _oe){
  if (_ce){
    prev_we=_we;
    return;
  }
  if (!_oe && !_we){
    std::cerr<<"Flash memory has simulataneously _oe and _we low"<<std::endl;
  }
  if (!_oe){
    if (!data.size()){
      datalines=0;
      return;
    }
    if (addr>=data.size()){
      std::cerr<<"warning: address "<<addr<<" > size "<<data.size()<<std::endl;
      addr=addr%data.size();
    }
    datalines=data[addr];
  }

  if (!_we&&prev_we){
    for (int i=0;i<DETECTOR_NUM;i++){
      detectors[i].write(addr,datalines);
    }
    if (detectors[WRITE].isActive()){
      detectors[WRITE].reset();
      write(addr,datalines);
    }
    if (detectors[ERASESECTOR].isActive()){
      detectors[ERASESECTOR].reset();
      eraseSector(addr/(8*1024));
    }
  }
  prev_we=_we;
}


bool FlashMemory::isBusy(){
  return busyCycles;
}


void FlashMemory::tick(){
  if (busyCycles){
    busyCycles--;
  }
}


int FlashMemory::GetROMSize(){
  return data.size();
}

uint8_t* FlashMemory::GetROM(){
  return &data[0];
}
