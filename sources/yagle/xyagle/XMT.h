/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   XMT.h                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

#ifndef XYAG_XMT
#define XYAG_XMT

/*------------------------------------------------------------\
|                          Variables                          |
\------------------------------------------------------------*/

extern XyagMenuItem XyagWindowsMenu[];
extern XyagPanelItem XyagToolsHilitePanel;
extern XyagPanelItem XyagToolsExtractPanel;

extern int      XyagToolsExtractDefaultValues[5];
extern int      XyagToolsHiliteDefaultValues[5];

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

void            XyagBuildToolsDepDialog();

#endif
