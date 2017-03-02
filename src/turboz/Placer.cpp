#include "Placer.h"

Placer::Placer(int _x,int _y):
  x(_x),y(_y),identx(_x)
  {}

TRect Placer::place(int sizex,int sizey,bool newline){
  TRect r(x,y,x+sizex,y+sizey);
  x=x+sizex;
  if (newline){
    newLine(sizey);
  }
  return r;
}


void Placer::space(int sizex,int sizey){
  x+=sizex;
  y+=sizey;
}

void Placer::newLine(int sizey){
  y=y+sizey;
  x=identx;
}

TRect  Placer::spaceAndPlace(int dx,int dy,int sizex,int sizey,bool newline){
  x+=dx;
  y+=dy;
  return place(sizex,sizey,newline);
}



TRect Placer::center(const TRect& bounds,int sizex,int sizey){
  TRect r;
  int meanx=(bounds.a.x+bounds.b.x)/2;
  int meany=(bounds.a.y+bounds.b.y)/2;
  r.a.x=meanx-sizex/2;
  r.a.y=meany-sizey/2;
  r.b.x=r.a.x+sizex;
  r.b.y=r.a.y+sizey;
  return r;    
}

void printRect(const TRect& a){
  std::cout<<a.a.x<<","<<a.a.y<<"~"<<a.b.x<<","<<a.b.y<<std::endl;
}


TRect Placer::rightOf(const TRect& a,int sizex,int sizey){
  TRect r;
  r.a.x=a.b.x;
  r.a.y=a.a.y;
  r.b.x=r.a.x+sizex;
  r.b.y=r.a.y+sizey;
  //printRect(a);
  //printRect(r);
  return r;
}

TRect Placer::move(const TRect& a,int movex,int movey){
  TRect b(a);
  b.move(movex,movey);
  return b;
}
