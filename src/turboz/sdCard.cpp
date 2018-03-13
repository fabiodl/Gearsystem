#include "sdCard.h"
using namespace std;
#include <iostream>

SDCard::SDCard():
  oldClk(false),
  currMiso(false),
  inByte(0xFF),
  bitCnt(0),byteCnt(0),mode(FFMODE),outSector(f,outBytes)
  //mode(CMDMODE)
{
}

SDCard::VList::VList():
  cnt(0)
{  
}

void SDCard::VList::pop(){
  cnt++;
  if (cnt>=data.size()){
    cnt=0;
    data.clear();
  }
}


uint8_t SDCard::VList::front(){
  if (cnt<data.size()){
    return data[cnt];
  }else{
    return 0xFF;
  }
}

void SDCard::VList::push_back(uint8_t x){
  data.push_back(x);
}

bool SDCard::pullBit(){
  //std::cout<<"pulling "<<std::dec<<(int)bitCnt<<" from "<<std::hex<<(int)outBytes.front()<<std::endl;
  return outBytes.front() & (1<<(7-bitCnt));
}

void SDCard::eval(bool mosi,bool clk,bool cs){  
  //std::cout<<"cs:"<<cs<<"mosi:"<<mosi<<"clk:"<<clk<<std::endl;
  if (!cs && clk && !oldClk){
    pushBit(mosi);
  }
  if (!cs && !clk && oldClk){
    currMiso=pullBit();
  }
  if (!cs){
    oldClk=clk;
  }else{
    mode=FFMODE;
    bitCnt=0;
  }
}

bool SDCard::getMiso(){
  return currMiso;
}


void SDCard::pushBit(bool b){
  //std::cout<<"bit"<<b<<std::endl;  
  if (!b&&mode==FFMODE){
    mode=CMDMODE;
  }
  if (mode==CMDMODE || mode ==REPLYMODE || mode == WRITEMODE){
    inByte=(inByte<<1) | (b?1:0);
    bitCnt++;
    if (bitCnt==8){
      pushByte(inByte);
      bitCnt=0;
    }
  }
  
}



void SDCard::loadFile(const std::string& fname){
  //f.close();
  f.open(fname.c_str(),std::ifstream::ate |std::ifstream::binary| ios::in | ios::out);
  if (f.fail()){
    std::cerr<<"Error opening "<<fname<<std::endl;
  }
  fileSize=f.tellg();
  //std::cout<<fname<<" is "<<fileSize<<" bytes"<<std::endl;
}



#define CMD(x) (0x40+x)



uint64_t getBlock(uint8_t* cmdBuffer){
  return (static_cast<uint64_t>(cmdBuffer[1])<<24)+
    (static_cast<uint64_t>(cmdBuffer[2])<<16)+
    (static_cast<uint64_t>(cmdBuffer[3])<<8)+
    (static_cast<uint64_t>(cmdBuffer[4])<<0);	
}

void SDCard::execCmd(){
  switch(cmdBuffer[0]){
  case 0xFF:
    break;
  case CMD(0):
    //cout<<"reset"<<endl;
    outBytes.data.clear();
    outBytes.data.push_back(0x01);
    break;
  case CMD(8):
    //cout<<"cmd8"<<endl;
    outBytes.push_back(0x01);
    outBytes.push_back(0x00);//buff 0
    outBytes.push_back(0x00);//buff 1
    outBytes.push_back(0x01);//buff 2
    outBytes.push_back(0xAA);//buff 3
    break;
  case CMD(17):
    outBytes.data.resize(2+512+2);
    {
      uint64_t block=getBlock(cmdBuffer);
	
      if (static_cast<std::ifstream::pos_type>((block+1)*512)>fileSize){
	std::cerr<<"reading past the file end"<<std::endl;
      }
      std::cout<<"reading block "<<block<<std::endl;
      f.seekg(512*block);
      f.read(reinterpret_cast<char*>(&outBytes.data[2]),512);
    }
    outBytes.data[0]=0x00;
    outBytes.data[1]=0xFE;    
    outBytes.data[2+512+0]=0x00;//crc 1
    outBytes.data[2+512+1]=0x00;//crc 2
    break;
  case CMD(41):
    outBytes.push_back(0x00);
    break;
  case CMD(58):
    outBytes.push_back(0x00);
    outBytes.push_back(1<<6);//buff 0
    outBytes.push_back(0x00);//buff 1
    outBytes.push_back(0x00);//buff 2
    outBytes.push_back(0x00);//buff 3
    break;
  case CMD(24):
    {
      uint64_t block=getBlock(cmdBuffer);
      outBytes.push_back(0x00);
      if (static_cast<std::ifstream::pos_type>((block+1)*512)>fileSize){
        std::cerr<<"writing past the file end"<<std::endl;
      }
      std::cout<<"writing block "<<block<<std::endl;
      mode=WRITEMODE;
      outSector.init(block);
    }
    break;    
  }
}


void SDCard::pushByte(uint8_t b){    
  //std::cout<<"byte"<<std::hex<<(int)b<<std::dec<<"is "<<(int)byteCnt<<" th"<<std::endl;
  outBytes.pop();
  //cout<<"front is"<<hex<<(int)outBytes.front()<<dec<<endl;
  if (mode==CMDMODE){    
    cmdBuffer[byteCnt]=b;
    byteCnt++;
    if (byteCnt==6){
      execCmd();
      if (mode==CMDMODE){
        mode=REPLYMODE;
      }
      byteCnt=0;
    }    
  }//mode==cmdmode
  else if (mode==WRITEMODE){
    if (outSector.pushByte(b)){
      mode=REPLYMODE;
    }
  }
}


SDCard::OutSector::OutSector(fstream& _f,VList &_outBytes):f(_f),outBytes(_outBytes){}

void SDCard::OutSector::init(uint64_t _block){
  block=_block;
  cnt=0;
  f.seekp(512*block);
}

uint8_t expected[3]={0xFF,0xFF,0xFE};

bool SDCard::OutSector::pushByte(uint8_t b){
  if (cnt<3){
    if (expected[cnt]!=b){
      //cerr<<"Received "<<hex<<(int)b<<" expected "<<dec<<(int)b<<endl;
    }else{
      //cout<<"received header "<<cnt<<endl;
    }
  }else if(cnt<3+512){
    //cout<<dec<<(cnt-3)<<"th byte written"<<endl;
    f.write(reinterpret_cast<char*>(&b),1);
  }
  cnt++;  
  if (cnt==3+512+2){
    f.flush();
    outBytes.push_back(0x05);
     outBytes.push_back(0xFF);//wait some time to simulate write completion
     outBytes.push_back(0xFF);
     outBytes.push_back(0xFF);
     outBytes.push_back(0x00);
  }
  return (cnt>=3+512+5);
}
