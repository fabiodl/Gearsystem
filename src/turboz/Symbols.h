#ifndef _SYMBOLS_
#define _SYMBOLS_

#include <map>
#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include <string>

class Symbols{


public:
  Symbols();
  void loadFromFile(const std::string& filename);
  std::string* getLabel(uint16_t addr);//NULL for non existing
  size_t getLabelMaxLength();
  std::vector<std::pair<uint16_t,std::string*> > getSubstringMatches(const std::string& s);
  void getSubstringMatches(const char* s,void (*cb)(const char*,void*),void* userData=NULL);
  bool isAvailable(const std::string& s);
  uint16_t getAddress(const std::string& s);
  uint32_t getAddressExtended(const std::string& s);
  void clear();
  const std::map<uint16_t,std::string>& getSymbols(){return sym;};  
private:
  size_t maxLength;
  std::map<uint16_t,std::string> sym;  
  
};

#endif
