/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   XME.h                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_XME
# define XYAG_XME 


#define XYAG_BACKWARD 0
#define XYAG_FORWARD  1
#define XYAG_BOTH     2

/*------------------------------------------------------------\
|                          Variables                          |
\------------------------------------------------------------*/

  extern XyagMenuItem  XyagEditMenu[];

  extern XyagPanelItem XyagEditBehPanel;
  extern XyagPanelItem XyagEditIdentifyPanel;

  extern int XyagEditIdentifyDefaultValues[ 5 ];
  extern int XyagEditBehDefaultValues[ 5 ];

  extern int XyagDependencyMode;

  extern char *XyagTempBehFile;

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

  extern void XyagBuildEditDialog();

  extern void XyagEditSearch();
  extern void XyagEditIdentify();
  extern void XyagEditSelect();
  extern void XyagEditUnselectAll();
  extern void XyagEditBehaviour();
  
  extern void XyagDisplayEditIdentify();
  extern void XyagDisplayEditBeh();

# endif
