/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                  Cursor.h                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_CURSOR
# define XYAG_CURSOR

  extern Position XyagCursorX;
  extern Position XyagCursorY;

  extern long     XyagUnitCursorX;
  extern long     XyagUnitCursorY;
  extern long     XyagPixelCursorX;
  extern long     XyagPixelCursorY;

/*------------------------------------------------------------\
|                                                             |
|                          Functions                          |
|                                                             |
\------------------------------------------------------------*/

  extern void XyagComputeCursor();
  extern void XyagChangeCursorType();
  extern void XyagPointCursor();
  extern void XyagResetCursor();
  extern void XyagDisplayCoordinates();
  
# endif 
