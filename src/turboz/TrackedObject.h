#ifndef _TRACKEDOBJECT_H_
#define _TRACKEDOBJECT_H_


#include<algorithm>
#include <vector>

template<typename Object,int id=0> class Tracked;

template<typename Object,int id=0> class ObjectTracker{
public:
  static std::vector<Object*> objs;
};

template<typename Object,int id>
std::vector<Object*> ObjectTracker<Object,id>::objs;




template<typename Object,int id> class Tracked{
public:
  Tracked(){
    ObjectTracker<Object,id>::objs.push_back(static_cast<Object*>(this));
  }
  
  ~Tracked(){
    std::vector<Object*>& objs( ObjectTracker<Object,id>::objs);
    objs.erase(std::remove(objs.begin(),objs.end(),static_cast<Object*>(this)),objs.end());
  }
};


#endif
