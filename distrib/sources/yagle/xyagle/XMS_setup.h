/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                  Setup.h                          |
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

# ifndef XYAG_SETUP
# define XYAG_SETUP

/*------------------------------------------------------------\
|                                                             |
|                           Constants                         |
|                                                             |
\------------------------------------------------------------*/

# define XYAG_TOPLEVEL_TRANSLATE_X     5
# define XYAG_TOPLEVEL_TRANSLATE_Y     24
# define XYAG_PANEL_TRANSLATE_X        5 
# define XYAG_PANEL_TRANSLATE_Y        24

# define XYAG_TOPLEVEL_X                50
# define XYAG_TOPLEVEL_Y                50
# define XYAG_TOPLEVEL_WIDTH          1024
# define XYAG_TOPLEVEL_HEIGHT          768

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
/*------------------------------------------------------------\
|                                                             |
|                          Functions                          |
|                                                             |
\------------------------------------------------------------*/

extern void XyagDefaultConfig();
extern void XyagLoadConfig();
extern void XyagSaveConfig();

# endif 
