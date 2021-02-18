/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                   XMS.h                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                                                             |
|                           Constants                         |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_XMS
# define XYAG_XMS

/*------------------------------------------------------------\
|                                                             |
|                            Types                            |
|                                                             |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                                                             |
|                          Variables                          |
|                                                             |
\------------------------------------------------------------*/

  extern XyagMenuItem  XyagSetupMenu[];
  extern XyagPanelItem XyagSetupInformationsPanel;

  extern int XyagSetupInformationsDefaultValues[ 5 ];

/*------------------------------------------------------------\
|                                                             |
|                          Functions                          |
|                                                             |
\------------------------------------------------------------*/

  extern void CallbackSetupInformations();
  extern void CallbackSetupLoadConfig();
  extern void CallbackSetupSaveConfig();
  extern void CallbackSetupDefaultConfig();

  extern void XyagLoadConfig();
  extern void XyagSaveConfig();
  extern void XyagLoadTopLevelConfig();

# endif
