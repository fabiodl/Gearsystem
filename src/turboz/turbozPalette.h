
#ifndef _TURBOZPALETTE_H_
#define _TURBOZPALETTE_H_


/* ---------------------------------------------------------------------- */
/*      class TProgram                                                    */
/*                                                                        */
/*      Palette layout                                                    */
/*          1 = TBackground                                               */
/*       2- 7 = TMenuView and TStatusLine                                 */
/*       8-15 = TWindow(Blue)                                             */
/*      16-23 = TWindow(Cyan)                                             */
/*      24-31 = TWindow(Gray)                                             */
/*      32-63 = TDialog                                                   */
/* ---------------------------------------------------------------------- */


enum TVPalette{
  TBackground=1,
  TMenuView_TextNormal=2,
  TMenuView_TextDisabled=3,
  TMenuView_TextShortcut=4,
  TMenuView_SelectedNormal=5,
  TMenuView_SelectedDisabled=6,
  TMenuView_SelectedShortcut=7,
  TWindowBlue_FramePassive=8,
  TWindowBlue_FrameActive=9,
  TWindowBlue_FrameIcon=10,
  TWindowBlue_ScrollBarPage=11,
  TWindowBlue_ScrollBarReserved=12,
  TWindowBlue_ScrollerNormalText=13,
  TWindowBlue_ScrollerSelectedText=14,
  TWindowBlue_Reserved=15,
  TWindowCyan_FramePassive=16,
  TWindowCyan_FrameActive=17,
  TWindowCyan_FrameIcon=18,
  TWindowCyan_ScrollBarPage=19,
  TWindowCyan_ScrollBarReserved=20,
  TWindowCyan_ScrollerNormalText=21,
  TWindowCyan_ScrollerSelectedText=22,
  TWindowCyan_Reserved=23,
  TWindowGray_FramePassive=24,
  TWindowGray_FrameActive=25,
  TWindowGray_FrameIcon=26,
  TWindowGray_ScrollBarPage=27,
  TWindowGray_ScrollBarReserved=28,
  TWindowGray_ScrollerNormalText=29,
  TWindowGray_ScrollerSelectedText=30,
  TWindowGray_Reserved=31,
  TDialog_FramePassive=32,
  TDialog_FrameActive=33,
  TDialog_FrameIcon=34,
  TDialog_ScrollBarPage=35,
  TDialog_ScrollBarControls=36,
  TDialog_StaticText=37,
  TDialog_LabelNormal=38,
  TDialog_LabelHighlight=39,
  TDialog_LabelShortcut=40,
  TDialog_ButtonNormal=41,
  TDialog_ButtonDefault=42,
  TDialog_ButtonSelected=43,
  TDialog_ButtonDisabled=44,
  TDialog_ButtonShortcut=45,
  TDialog_ButtonShadow=46,
  TDialog_ClusterNormal=47,
  TDialog_ClusterSelected=48,
  TDialog_ClusterShortcut=49,
  TDialog_InputLineNormal=50,
  TDialog_InputLineSelected=51,
  TDialog_InputLineArrows=52,
  TDialog_HistoryArrow=53,
  TDialog_HistorySides=54,
  TDialog_HistoryWindowScrollBarPage=55,
  TDialog_HistoryWindowScrollBarControls=56,
  TDialog_ListViewerNormal=57,
  TDialog_ListViewerFocused=58,
  TDialog_ListViewerSelected=59,
  TDialog_ListViewerDivider=60,
  TDialog_InfoPane=61,
  TDialog_Reserved1=62,
  TDialog_Reserved2=63
};


namespace palette{
  enum Color{
    BLACK,BLUE,GREEN,CYAN,RED,MAGENTA,BROWN,GRAY,LIGHT,
    DARKGRAY=LIGHT+BLACK,
    WHITE=LIGHT+GRAY,
    YELLOW=LIGHT+BROWN,
    BACKGROUND=16,
  };

  enum TurboZ{        
    DEBUG=64,
    DISASM_CURRENT_PC_LINE=65,
    DISASM_BREAKPOINT,
    DISASM_PC_AT_BREAKPOINT,
    DISASM_PREDICTED_HEAD,
    DISASM_PREDICTED_TAIL,
    DISASM_CONFIRMED_HEAD,
    DISASM_CONFIRMED_TAIL,
    PALETTE_LENGTH
  };
  
}


#endif
