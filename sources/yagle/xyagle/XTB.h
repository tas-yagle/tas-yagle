/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                           |
|                                                             |
| File    :                   XTB.h                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_XTB
# define XYAG_XTB

# define XYAG_NORMAL_CURSOR     0
# define XYAG_WATCH_CURSOR      1
# define XYAG_PIRATE_CURSOR     2 
# define XYAG_CROSS_CURSOR      3 
# define XYAG_NO_CURSOR         4

# define XYAG_MAX_CURSOR        5

/*------------------------------------------------------------\
|                         XyagMenuItem                       |
\------------------------------------------------------------*/

  typedef struct XyagMenuItem

  {
    char                 *LABEL;
    char                  MNEMONIC; 
    char                 *SHORT_KEY;
    char                 *SHORT_KEY_TEXT;
    WidgetClass          *CLASS; 
    Boolean               TEAROFF; 
    Boolean               SEPARATOR; 
    Boolean               TITLE;  
    Boolean               HELP;
    void                  (*CALLBACK)(); 
    XtPointer             CALLDATA;
    Widget                BUTTON;
    Widget                MENU;
    struct XyagMenuItem  *NEXT;

  } XyagMenuItem;

/*------------------------------------------------------------\
|                      XyagPanelButtonItem                   |
\------------------------------------------------------------*/

  typedef struct XyagPanelButtonItem 

  {
    char                       **LABEL;
    char                        *TEXT;
    char                        *BITMAP;
    int                          WIDTH;
    int                          HEIGHT;
    char                       **FOREGROUND;
    char                       **BACKGROUND;
    unsigned char                X;
    unsigned char                Y;
    unsigned char                DX;
    unsigned char                DY;
    void                         (*CALLBACK)(); 
    XtPointer                    CALLDATA;
    Widget                       BUTTON;

  } XyagPanelButtonItem;

/*------------------------------------------------------------\
|                          XyagPanelItem                     |
\------------------------------------------------------------*/

  typedef struct XyagPanelItem

  {
    char                  *TITLE;
    char                   COMPUTE;
    char                   MANAGED;
    int                    X;
    int                    Y;
    int                    WIDTH;
    int                    HEIGHT;
    unsigned char          COLUMN;
    unsigned char          ROW;
    Widget                 PANEL;
    Widget                 PANEL_FORM;
    Widget                 FRAME;
    Widget                 FORM;
    XyagPanelButtonItem  *LIST; 

  } XyagPanelItem;

/*------------------------------------------------------------\
|                          Variables                          |
\------------------------------------------------------------*/

 extern Widget  XyagFileSelectDialog;

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

  extern void   XyagDestroyDialogCallback();
  extern void   XyagExitDialogCallback();

  extern void   XyagSetMouseCursor();
  extern Pixmap XyagCreatePixmap();
  extern Pixmap XyagCreateColorPixmap();
  extern void   XyagSetIcon();
  extern void   XyagSetColorIcon();
  extern void   XyagBuildMenus();
  extern void   XyagBuildPanel();
  extern void   XyagEnterPanel();
  extern void   XyagExitPanel();
  extern void   XyagBuildDialog();
  extern void   XyagEnterDialog();
  extern void   XyagReEnterDialog();
  extern void   XyagExitDialog();
  extern void   XyagWarningMessage();
  extern void   XyagErrorMessage();
  extern Widget XyagWorkingDialog();
  extern void   XyagConfirmQuit();
  extern void   XyagZoomSet();
  extern void   XyagDepthSet();
  extern void   XyagFind();
  extern void   XyagFileSelect();
  extern void   XyagLimitedLoop();
  extern void   XyagLeaveLimitedLoop();

# endif
