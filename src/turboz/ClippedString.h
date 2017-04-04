#ifndef _CLIPPEDSTRING_H_
#define _CLIPPEDSTRING_H_

class ClippedString{
public:
  char* s;
  int offset;
  inline void set(char* s,int deltax,int sizex);  
};




inline void ClippedString::set(char* buff,int deltax,int sizex){
  int len=strlen(buff);  
  if (deltax>len || deltax+sizex < 0 ){
    s=buff;
    offset=0;
    s[0]=0;
  }else if (deltax>0){
    s=buff+deltax;
    offset=0;
  }else{
    s=buff;
    offset=-deltax;
  }
}



#endif
