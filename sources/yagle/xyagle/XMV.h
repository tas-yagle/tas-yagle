/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                    XMV.h                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

#ifndef XYAG_XMV
#define XYAG_XMV

/*------------------------------------------------------------\
|                          Variables                          |
\------------------------------------------------------------*/

extern char     XyagFirstViewLayer;

extern XyagMenuItem XyagViewMenu[];

extern XyagPanelItem XyagViewMessagePanel;
extern XyagPanelItem XyagViewArrowsPanel;
extern XyagPanelItem XyagViewZoomPanel;
extern XyagPanelItem XyagViewGridPanel;
extern XyagPanelItem XyagViewLayerPanel;
extern XyagPanelItem XyagViewMapPanel;
extern XyagPanelItem XyagViewMessagePanel;

extern int      XyagPercentZoom;
extern int      XyagPercentMoveX;
extern int      XyagPercentMoveY;

extern int      XyagViewArrowsDefaultValues[5];
extern int      XyagViewZoomDefaultValues[5];
extern int      XyagViewLayerDefaultValues[5];
extern int      XyagViewGridDefaultValues[5];
extern int      XyagViewMapDefaultValues[5];

extern int      XyagDepthOfDep;
extern int      XyagViewMessageDefaultValues[5];

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

void            XyagComputeAndDisplayMap();
void            XyagInitializeZoom();
void            XyagBuildViewDialog();
void            XyagBuildPanelMap();
void            XyagInitializeMapEvent();
void            XyagViewDep();
void            XyagZoomCenter();
void            XyagZoomRefresh();
void            XyagZoomIn();
void            XyagZoomPan();
void            XyagPromptZoomIn();
void            XyagPromptZoomPan();
void            XyagPromptZoomCenter();
void            XyagInitializeLayer();
void            XyagDisplayViewMessage();
void            XyagDisplayMessage();

#endif
