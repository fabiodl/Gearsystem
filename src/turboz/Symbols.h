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
  const std::string* getLabel(uint16_t addr) const;//NULL for non existing
  size_t getLabelMaxLength() const;
  std::vector<std::pair<uint16_t,const std::string*> > getSubstringMatches(const std::string& s) const;
  void getSubstringMatches(const char* s,void (*cb)(const char*,void*),void* userData=NULL) const;
  bool isAvailable(const std::string& s) const;
  uint16_t getAddress(const std::string& s) const;
  uint32_t getAddressExtended(const std::string& s) const ;
  void clear();
  const std::map<uint16_t,std::string>& getSymbols(){return sym;};  
private:
  size_t maxLength;
  std::map<uint16_t,std::string> sym;  
  
};

#endif
