#ifndef _SDCARD_H_
#define _SDCARD_H_
#include <fstream>
#include <stdint.h>
#include <vector>

class SDCard{
public:
  void loadFile(const std::string& file);
  void eval(bool mosi,bool clk,bool cs);
  bool getMiso();
  SDCard();
private:
  void pushBit(bool b);
  void pushByte(uint8_t b);
  bool pullBit();
  void execCmd();
  std::ifstream f;
  std::ifstream::pos_type fileSize;
  bool oldClk,currMiso;
  uint8_t inByte;

  class VList{
  public:
    std::vector<uint8_t> data;
    VList();
    void pop();          
    void push_back(uint8_t x);
    uint8_t front();
  private:    
    size_t cnt;
  };
  
  VList outBytes;
  uint8_t bitCnt,byteCnt;
  enum Mode{FFMODE,CMDMODE,REPLYMODE};
  Mode mode;
  uint8_t cmdBuffer[6];
};



#endif
