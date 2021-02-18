/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                   View.h                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                                                             |
|                         Include Files                       |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_VIEW
# define XYAG_VIEW

/*------------------------------------------------------------\
|                                                             |
|                           Constants                         |
|                                                             |
\------------------------------------------------------------*/

# define XYAG_PERCENT_ZOOM      30
# define XYAG_PERCENT_ZOOM_MIN   5
# define XYAG_PERCENT_ZOOM_MAX  95

# define XYAG_PERCENT_MOVE      30
# define XYAG_PERCENT_MOVE_MIN   5
# define XYAG_PERCENT_MOVE_MAX  95

/*------------------------------------------------------------\
|                                                             |
|                            Types                            |
|                                                             |
\------------------------------------------------------------*/

  typedef struct xyagzoom
   {
     struct xyagzoom *NEXT;

     long              X;
     long              Y;
     long             DX;
     long             DY;

   } xyagzoom;

/*------------------------------------------------------------\
|                                                             |
|                          Variables                          |
|                                                             |
\------------------------------------------------------------*/

  extern int            XyagPercentZoom;
  extern int            XyagPercentMove;
  extern int XyagDepthOfDep;

/*------------------------------------------------------------\
|                                                             |
|                          Functions                          |
|                                                             |
\------------------------------------------------------------*/

  extern void XyagZoomUndo();
  extern void XyagZoomRefresh();

  extern void XyagZoomLeft();
  extern void XyagZoomRight();
  extern void XyagZoomUp();
  extern void XyagZoomDown();

  extern void XyagZoomMore();
  extern void XyagZoomLess();

  extern void XyagZoomFit();

  extern void XyagInitializeZoom();
  extern void XyagInitializeUnitGrid();
void XyagThruTree(long X1, long Y1);
xyagzoom *XyagZoomSave();
     void XyagZoomRestore(xyagzoom *zoom);
# endif 
