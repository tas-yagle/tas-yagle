/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   XMX.h                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_XMX
# define XYAG_XMX

/*------------------------------------------------------------\
|                         Graphic Context                     |
\------------------------------------------------------------*/

# define XYAG_BLACK_COLOR_NAME         "Black"

/*------------------------------------------------------------\
|                        Xyag Main Window                     |
\------------------------------------------------------------*/

  extern XtAppContext  XyagleApplication;
  extern Widget        XyagTopLevel;
  extern Widget        XyagMainWindow;

  extern Widget        XyagMenuBar;

  extern Widget        XyagGraphicWindow;
  extern Widget        XyagHScroll;
  extern Widget        XyagVScroll;
  extern Widget        XyagViewport;
  extern Display      *XyagGraphicDisplay;

/*------------------------------------------------------------\
|                       Xyag Graphic Context                  |
\------------------------------------------------------------*/

  extern GC XyagBackgroundGC;
  extern GC XyagGridGC;
  extern GC XyagXorGC;
  extern GC XyagAcceptDrawGC;
  extern GC XyagConnectDrawGC;
  extern GC XyagLayerDrawGC[ XYAG_MAX_LAYER ];
  extern GC XyagSmallTextGC;
  extern GC XyagMediumTextGC;
  extern GC XyagLargeTextGC;

  extern XFontStruct *XyagSmallTextFont;
  extern XFontStruct *XyagLargeTextFont;
  extern XFontStruct *XyagMediumTextFont;

/*------------------------------------------------------------\
|                       Xyag Graphic Pixmap                   |
\------------------------------------------------------------*/

  extern Pixmap    XyagGraphicPixmap;

/*------------------------------------------------------------\
|                       Xyag Graphic Size                     |
\------------------------------------------------------------*/

  extern Dimension XyagOldGraphicDx;
  extern Dimension XyagOldGraphicDy;
  extern Dimension XyagGraphicDx;
  extern Dimension XyagGraphicDy;
  extern Dimension XyagViewDx;
  extern Dimension XyagViewDy;

/*------------------------------------------------------------\
|                          Xyag Unit Grid                     |
\------------------------------------------------------------*/

  extern float XyagUnitGridStep;
  extern long  XyagUnitGridX;
  extern long  XyagUnitGridY;
  extern long  XyagUnitGridDx;
  extern long  XyagUnitGridDy;
  extern long  XyagPixelGridX;
  extern long  XyagPixelGridY;

/*------------------------------------------------------------\
|                          Xyag Cursor                        |
\------------------------------------------------------------*/

  extern long XyagUnitCursorX;
  extern long XyagUnitCursorY;

/*------------------------------------------------------------\
|                          Xyag Event                         |
\------------------------------------------------------------*/

  extern int  XyagCountEventZoom;
  extern int  XyagCountEventEdit;

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

  void XyagInitializeRessources();
  void XyagChangeTopLevelTitle();

  void XyagChangeEditMode();

  void XyagClearGraphicWindow();
  void XyagResizeGraphicWindow();
  void XyagRefreshGraphicWindow();

  void XyagComputeUnitGrid();
  void XyagResizeUnitGrid();

  void XyagDisplayFigure();
  void XyagDisplayObject();

  void XyagDisplayCursor();
  void XyagUndisplayCursor();

# endif 
