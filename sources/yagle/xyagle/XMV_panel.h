/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                  Panel.h                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

#ifndef XYAG_VIEW_PANEL
#define XYAG_VIEW_PANEL

/*------------------------------------------------------------\
|                       Default Panel Locations               |
\------------------------------------------------------------*/

#define XYAG_VIEW_MESSAGE_X   330
#define XYAG_VIEW_MESSAGE_Y   280

#define XYAG_VIEW_ARROWS_X     990
#define XYAG_VIEW_ARROWS_Y      76
#define XYAG_VIEW_ZOOM_X       990
#define XYAG_VIEW_ZOOM_Y       230
#define XYAG_VIEW_LAYER_X      430
#define XYAG_VIEW_LAYER_Y      190

/*------------------------------------------------------------\
|                     View Panels Callbacks                   |
\------------------------------------------------------------*/

void            CallbackViewCloseMessage();

void            CallbackLeftArrow();
void            CallbackRightArrow();
void            CallbackUpArrow();
void            CallbackDownArrow();
void            CallbackMoveSet();
void            CallbackCloseArrows();

void            CallbackZoomRefresh();
void            CallbackZoomPrevious();
void            CallbackZoomLess();
void            CallbackZoomMore();
void            CallbackZoomSet();
void            CallbackZoomIn();
void            CallbackZoomFit();
void            CallbackZoomCenter();
void            CallbackZoomGoto();
void            CallbackZoomPan();
void            CallbackCloseZoom();

void            CallbackLayerAllVisible();
void            CallbackLayerAllInvisible();
void            CallbackLayerVisible();
void            CallbackLayerInvisible();
void            CallbackNameVisible();
void            CallbackNameInvisible();
void            CallbackLayerCursor();
void            CallbackLayerForceDisplay();
void            CallbackLayerApply();
void            CallbackCloseLayer();

void            XyagChangeForceDisplay();
void            XyagChangeCursor();

void            XyagSetLayerVisible();
void            XyagSetLayerInvisible();
void            XyagSetNameVisible();
void            XyagSetNameInvisible();
void            XyagLayerAllVisible();
void            XyagLayerAllInvisible();
void            XyagDisplayMessage();

#endif
