#ifndef _FLASH_MEMORY_H_
#define _FLASH_MEMORY_H_
#include <stdint.h>
#include <string>
#include <vector>

class FlashMemory{
public:
  template<typename T> struct Masked{
    Masked(T _val,T _mask):val(_val),mask(_mask){}
    T val,mask;
    bool isMatch(T bval){return (((val^bval)&mask)==0); };
  };
  typedef  std::vector<std::pair<Masked<uint32_t>,Masked<uint8_t> > > ProgramKeys;
  class ProgramDetector{
  public:
    ProgramDetector();
    ProgramKeys keys;
    void write(uint32_t addr,uint8_t data);
    bool isActive() const;
    void reset();
  private:
    size_t matchCount;
  };

  FlashMemory(uint32_t size=0,bool topType=true);
  void load(const std::string& fname);
  void append(const std::string& fname,size_t offset);
  void store(const std::string& fname);
  void eval(uint32_t addr,uint8_t& data,bool _ce,bool _we,bool _oe);
  bool isBusy();
  void tick();
  enum DetectorType{ERASESECTOR,WRITE,DETECTOR_NUM};
  ProgramDetector detectors[DETECTOR_NUM];
  struct SectorInfo{
    uint32_t start,size;
    SectorInfo(uint32_t _start,uint32_t _size):start(_start),size(_size){}
  };
  std::vector<SectorInfo> sectors;
  uint8_t* GetROM();
  int GetROMSize();
private:
  std::vector<uint8_t> data;
  void write(uint32_t addr,uint8_t data);
  void eraseSector(uint32_t addr);
  bool prev_we;
  int busyCycles;
};



#endif  
