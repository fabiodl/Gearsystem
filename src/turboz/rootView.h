#ifndef _ROOTVIEW_H_
#define _ROOTVIEW_H_



inline TView* getRootView(TView* v){
  while(v->owner!=NULL){
    //std::cout<<"going to owner "<<v<<std::endl;
    v=v->owner;
  }
  return v;
}


#endif
