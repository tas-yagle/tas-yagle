/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                    XSB.h                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_XSB 
# define XYAG_XSB 

# define XYAG_TRUE   1
# define XYAG_FALSE  0

# define XYAG_ERROR  1
# define XYAG_OK     0

# define XYAG_DEFAULT_FIGURE_NAME  "NONAME"

/*------------------------------------------------------\
|                        Layer                          |
\------------------------------------------------------*/

# define XYAG_MAX_LAYER    5

# define XYAG_CGVBOX_LAYER        0
# define XYAG_CGVCONIN_LAYER      1
# define XYAG_CGVCONOUT_LAYER     2
# define XYAG_CGVNET_LAYER        3
# define XYAG_CONSTRUCTION_LAYER  4

/*------------------------------------------------------\
|                        Active Name                    |
\------------------------------------------------------*/

# define XYAG_CONNECTOR_NAME        0
# define XYAG_INSTANCE_NAME         1
# define XYAG_MAX_ACTIVE_NAME       2

/*------------------------------------------------------------\
|                           Edit Mode                         |
\------------------------------------------------------------*/

# define XYAG_EDIT_MEASURE           (char)0x00
# define XYAG_EDIT_IDENTIFY          (char)0x01
# define XYAG_EDIT_SELECT            (char)0x02
# define XYAG_EDIT_BEHAVIOUR         (char)0x03
# define XYAG_EDIT_BACKWARDEXTRACT   (char)0x04  
# define XYAG_EDIT_FORWARDEXTRACT    (char)0x05
# define XYAG_EDIT_BOTHEXTRACT       (char)0x06
# define XYAG_EDIT_BACKWARDHILITE    (char)0x07  
# define XYAG_EDIT_FORWARDHILITE     (char)0x08
# define XYAG_EDIT_BOTHHILITE        (char)0x09
# define XYAG_EDIT_TREE              (char)0x0a

# define XYAG_ZOOM_CENTER            (char)0x80
# define XYAG_ZOOM_IN                (char)0x81
# define XYAG_ZOOM_PAN               (char)0x82

# define XYAG_ZOOM_MARK              (char)0x80

/*------------------------------------------------------------\
|                           Input Mode                        |
\------------------------------------------------------------*/

# define XYAG_INPUT_POINT     0
# define XYAG_INPUT_ORTHO     1
# define XYAG_INPUT_LINE      2
# define XYAG_INPUT_LSTRING   3
# define XYAG_INPUT_BOX       4
# define XYAG_INPUT_SORTHO    5
# define XYAG_INPUT_HALF_BOX  6

/*------------------------------------------------------------\
|                            Types                            |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                          Variables                          |
\------------------------------------------------------------*/

  extern char   XYAG_ACTIVE_LAYER_TABLE[ XYAG_MAX_LAYER ];
  extern char   XYAG_ACTIVE_NAME_TABLE[ XYAG_MAX_ACTIVE_NAME ];

  extern char  *XYAG_CURSOR_COLOR_NAME;
  extern char  *XYAG_BACKGROUND_COLOR_NAME;
  extern char  *XYAG_FOREGROUND_COLOR_NAME;
  extern char  *XYAG_ACCEPT_COLOR_NAME;
  extern char  *XYAG_CONNECT_COLOR_NAME;

  extern long   XYAG_CURSOR_SIZE;
  extern long   XYAG_UNIT;
  extern char  *XYAG_LAYER_NAME_TABLE[ XYAG_MAX_LAYER ][ 3 ];
  extern char  *XYAGLE_PARAM_NAME;

  extern char  *XYAG_XMS_FILE_NAME;
  extern char  *XyagFirstFileName;
  extern char   XYAGLE_IN_CGV[];
  extern int    XyagCnsMode;

//  extern char   XYAGLE_NAME[]; 
  extern char   XYAG_SWITCH_COLOR_MAP;
  extern char   XYAG_FORCE_DISPLAY;
  extern char   XYAG_XOR_CURSOR;

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/

  extern char *XyagPostTreatString();
  extern void  XyagLoadParameters();
  extern void  Xyaggetenv();
  extern void  XyagLoadColors();
 
# endif
