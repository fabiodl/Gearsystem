#ifndef _TURBOZPALETTE_H_
#define _TURBOZPALETTE_H_




namespace palette{
  enum Color{
    BLACK,BLUE,GREEN,CYAN,RED,MAGENTA,BROWN,GRAY,LIGHT,
    DARKGRAY=LIGHT+BLACK,
    WHITE=LIGHT+GRAY,
    YELLOW=LIGHT+BROWN,
    BACKGROUND=16,
  };

  enum TurboZ{
    CURRENT_PC_LINE=RED*BACKGROUND+YELLOW
  };
  
}


#endif
