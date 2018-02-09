# ifndef _VTOP_DESCRIBER_H_
# define _VTOP_DESCRIBER_H_

#include <iostream>
#include <string>

class GenericPrinter{
public:
  virtual void print(std::ostream& o) const=0;
  virtual ~GenericPrinter(){}
};

template<typename T> class Printer:public GenericPrinter{
  const std::string name;
  const T& t;
public:
  Printer(const std::string& _name,const T& _t):name(_name),t(_t){}  
  void print(std::ostream& o) const{
    o<<name<<" "<<std::hex<<(int)t<<std::dec<<std::endl;
  }
};


template<int N,typename T> class Printer<T[N]>:public GenericPrinter{
  const std::string name;
  const T* t;
public:
  Printer(const std::string& _name,const T* _t):name(_name),t(_t){}  
  void print(std::ostream& o) const{    
    o<<name;
    for (int i=0;i<N;i++){
      o<<" "<<std::hex<<(int)t[i];
    }
    o<<std::dec<<std::endl;          
  }
};

class BulkPrinter:public GenericPrinter{
  std::vector<GenericPrinter*> p;

public:

  ~BulkPrinter(){
    for (auto i:p){
      delete i;
    }
  }

  template<typename T>
  void push_back(const std::string& name,T& t){
    p.push_back(new Printer<T>(name,t));
  }

  void print(std::ostream& o) const{
    for (auto i:p){
      i->print(o);
    }
  }
  
};

inline std::ostream& operator<<(std::ostream& o,const BulkPrinter& bp){
  bp.print(o);
  return o;
}

# endif
