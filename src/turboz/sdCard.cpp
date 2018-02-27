#include "sdCard.h"
using namespace std;
#include <iostream>

SDCard::SDCard():
  oldClk(false),
  currMiso(false),
  inByte(0xFF),
  bitCnt(0),byteCnt(0),mode(FFMODE)
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
  std::cout<<"cs:"<<cs<<"mosi:"<<mosi<<"clk:"<<clk<<std::endl;
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
  std::cout<<"bit"<<b<<std::endl;  
  if (!b&&mode==FFMODE){
    mode=CMDMODE;
  }
  if (mode==CMDMODE || mode ==REPLYMODE){
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
  f.open(fname.c_str(),std::ifstream::ate |std::ifstream::binary);
  fileSize=f.tellg();
  //std::cout<<fname<<" is "<<fileSize<<" bytes"<<std::endl;
}



#define CMD(x) (0x40+x)

void SDCard::execCmd(){
  switch(cmdBuffer[0]){
  case 0xFF:
    break;
  case CMD(0):
    //cout<<"reset"<<endl;
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
      uint64_t block=
	(static_cast<uint64_t>(cmdBuffer[1])<<24)+
	(static_cast<uint64_t>(cmdBuffer[2])<<16)+
	(static_cast<uint64_t>(cmdBuffer[3])<<8)+
	(static_cast<uint64_t>(cmdBuffer[4])<<0);	
      if (static_cast<std::ifstream::pos_type>((block+1)*512)>fileSize){
	std::cerr<<"reading past the file end"<<std::endl;
      }
      //std::cout<<"reading block "<<block<<std::endl;
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
  }
}


void SDCard::pushByte(uint8_t b){    
  //std::cout<<"byte"<<std::hex<<(int)b<<std::dec<<"is "<<(int)byteCnt<<" th"<<std::endl;
  outBytes.pop();  
  if (mode==CMDMODE){    
    cmdBuffer[byteCnt]=b;
    byteCnt++;
    if (byteCnt==6){
      execCmd();
      mode=REPLYMODE;
      byteCnt=0;
    }    
  }//mode==cmdmode
}
