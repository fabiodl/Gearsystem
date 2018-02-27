#include <fstream>
#include <sstream>
#include "Symbols.h"
#include "cistring.h"
#include <stdexcept>
#include "AddressFinder.h"

Symbols::Symbols():
  maxLength(0){
}

void Symbols::loadFromFile(const std::string& filename){
  std::ifstream in(filename.c_str());
  std::string line;
  maxLength=0;
  while ( std::getline(in, line) )
{


  size_t comment=line.find(';');
  if (comment!=std::string::npos){
    line=line.substr(0,comment);
  }
  
  if (line.empty())
    continue;
  std::stringstream ss(line);
  int bank;int addr; std::string name;
  char colon;
  ss>>std::hex>>bank>>colon>>addr>>name;

  if (name.find("_sizeof_")!=std::string::npos){
    continue;
  }

  if (sym.find(addr)==sym.end()){
    sym[addr]=name;
  }else{
    if (name.length()<sym[addr].length()){
      sym[addr]=name;
    }
  }
  

  if (name.length()>maxLength){
    maxLength=name.length();
  }
  
}

}

size_t Symbols::getLabelMaxLength() const{
  return maxLength;
}


const std::string* Symbols::getLabel(uint16_t addr) const{
  auto it=sym.find(addr);
  if (it==sym.end()){
    return NULL;
  }else{
    return &it->second;
  }  
}





std::vector<std::pair<uint16_t,const std::string*> > Symbols::getSubstringMatches(const std::string& s) const{
  std::vector<std::pair<uint16_t,const std::string*> > matches;

  for (auto it=sym.begin();
       it!=sym.end();++it){
    if (istring(it->second).find(istring(s.c_str()))!=std::string::npos){
      matches.push_back(std::make_pair(it->first,&it->second));
    }
  }
  

  return matches;

}


void Symbols::getSubstringMatches(const char* s,void (*cb)(const char*,void*),void* userData) const{
   for (auto it=sym.begin();it!=sym.end();++it){
    if (istring(it->second).find(istring(s))!=std::string::npos){
      (*cb)(it->second.c_str(),userData);
    }
   }//for
}


bool Symbols::isAvailable(const std::string& s) const{
  for (auto it=sym.begin();it!=sym.end();++it){
    if (it->second==s){
      return true;
    }
  }
  return false;
}

uint16_t Symbols::getAddress(const std::string& s)const{
  for (std::map<uint16_t,std::string>::const_iterator it=sym.begin();
       it!=sym.end();++it){
    if (it->second==s){
      return it->first;
    }
  }
  throw std::invalid_argument(std::string("Symbol ")+s+" not found");
}

uint32_t Symbols::getAddressExtended(const std::string& s)const {
  for (std::map<uint16_t,std::string>::const_iterator it=sym.begin();
       it!=sym.end();++it){
    if (it->second==s){
      return it->first;
    }
  }
  return  AddressFinder::INVALIDADDR;
}





void Symbols::clear(){
  maxLength=0;
  sym.clear();
}
