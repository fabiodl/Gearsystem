#ifndef _PLACER_H_
#define _PLACER_H_

#define Uses_TRect
#include <tv.h>

class Placer{
  int x,y;
  int identx;
public:
  Placer(int startx=0,int starty=0);
  void setIdentX(int x){identx=x;}
  TRect place(int sizex,int sizey,bool newline=false);
  TRect spaceAndPlace(int dx,int dy,int sizex,int sizey,bool newline=false);
  void newLine(int sizey=1);
  void space(int sizex);
  static TRect center(const TRect& bounds,int sizex,int sizey);
  static TRect rightOf(const TRect& a,int sizex,int sizey);
  static TRect move(const TRect& a,int movex,int movey);
};



#endif
