/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                   XFS.h                           |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                                                             |
| Date    :                   01.06.96                        |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_XFS
# define XYAG_XFS 
#include <setjmp.h>

/*------------------------------------------------------------\
|                           Constants                         |
\------------------------------------------------------------*/

# define XYAG_OBJECT_CIRCLE      0x0000
# define XYAG_OBJECT_LINE        0x0001
# define XYAG_OBJECT_RECTANGLE   0x0002
# define XYAG_OBJECT_ARROW       0x0003
# define XYAG_OBJECT_HEXAGON     0x0004
# define XYAG_OBJECT_TEXT_CENTER 0x0005
# define XYAG_OBJECT_TEXT_LEFT   0x0006
# define XYAG_OBJECT_TEXT_RIGHT  0x0007
# define XYAG_OBJECT_TRIANGLE    0x0008
# define XYAG_OBJECT_SLIB        0x0009
# define XYAG_OBJECT_CELL        0x000A
# define XYAG_OBJECT_TYPE        0x000F

# define XYAG_USER_OBJECT_CGV    0x1234

# define XYAG_OBJECT_CGVCON      0x0010
# define XYAG_OBJECT_CGVNET      0x0020
# define XYAG_OBJECT_CGVBOX      0x0040

# define XYAG_OBJECT_LINE_DOWN   0x0100
# define XYAG_OBJECT_LINE_LEFT   0x0200

# define XYAG_OBJECT_SELECT      0x0400
# define XYAG_OBJECT_ACCEPT      0x0800
# define XYAG_OBJECT_CONNECT     0x1000

/*------------------------------------------------------------\
|                            Macros                           |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                           Object                            |
\------------------------------------------------------------*/


# define GetXyagObjectType( X ) ( ( X )->TYPE & XYAG_OBJECT_TYPE )

# define IsXyagLineDown( X )  ( ( X )->TYPE & XYAG_OBJECT_LINE_DOWN )
# define IsXyagLineLeft( X )  ( ( X )->TYPE & XYAG_OBJECT_LINE_LEFT )
# define IsXyagCgvCon( X )    ( ( X )->TYPE & XYAG_OBJECT_CGVCON    )
# define IsXyagCgvNet( X )    ( ( X )->TYPE & XYAG_OBJECT_CGVNET    )
# define IsXyagCgvBox( X )    ( ( X )->TYPE & XYAG_OBJECT_CGVBOX    )

# define SetXyagLineDown( X )  ( ( X )->TYPE |= XYAG_OBJECT_LINE_DOWN )
# define SetXyagLineLeft( X )  ( ( X )->TYPE |= XYAG_OBJECT_LINE_LEFT )
# define SetXyagCgvCon( X )    ( ( X )->TYPE |= XYAG_OBJECT_CGVCON    )
# define SetXyagCgvNet( X )    ( ( X )->TYPE |= XYAG_OBJECT_CGVNET    )
# define SetXyagCgvBox( X )    ( ( X )->TYPE |= XYAG_OBJECT_CGVBOX    )

# define ClearXyagLineDown( X )  ( ( X )->TYPE &= ~XYAG_OBJECT_LINE_DOWN )
# define ClearXyagLineLeft( X )  ( ( X )->TYPE &= ~XYAG_OBJECT_LINE_LEFT )
# define ClearXyagCgvCon( X )    ( ( X )->TYPE &= ~XYAG_OBJECT_CGVCON    )
# define ClearXyagCgvNet( X )    ( ( X )->TYPE &= ~XYAG_OBJECT_CGVNET    )
# define ClearXyagCgvBox( X )    ( ( X )->TYPE &= ~XYAG_OBJECT_CGVBOX    )

/*------------------------------------------------------------\
|                           Select                            |
\------------------------------------------------------------*/

# define IsXyagSelect( X )    ( ( X )->TYPE & XYAG_OBJECT_SELECT   )
# define SetXyagSelect( X )   ( ( X )->TYPE |= XYAG_OBJECT_SELECT  )
# define ClearXyagSelect( X ) ( ( X )->TYPE &= ~XYAG_OBJECT_SELECT )

/*------------------------------------------------------------\
|                           Accept                            |
\------------------------------------------------------------*/

# define IsXyagAccept( X )    ( ( X )->TYPE & XYAG_OBJECT_ACCEPT   )
# define SetXyagAccept( X )   ( ( X )->TYPE |= XYAG_OBJECT_ACCEPT  )
# define ClearXyagAccept( X ) ( ( X )->TYPE &= ~XYAG_OBJECT_ACCEPT )

/*------------------------------------------------------------\
|                           Connect                           |
\------------------------------------------------------------*/

# define IsXyagConnect( X )    ( ( X )->TYPE & XYAG_OBJECT_CONNECT   )
# define SetXyagConnect( X )   ( ( X )->TYPE |= XYAG_OBJECT_CONNECT  )
# define ClearXyagConnect( X ) ( ( X )->TYPE &= ~XYAG_OBJECT_CONNECT )

/*------------------------------------------------------------\
|                            Types                            |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                            Object                           |
\------------------------------------------------------------*/

  typedef struct xyagobj_list
  {
    struct xyagobj_list *NEXT;
    struct xyagobj_list *LINK;
    char                *NAME;
    short                LAYER;
    long                 TYPE;
    long                 X;
    long                 Y;
    long                 DX;
    long                 DY;
    symbol_list         *SYMBOL;
    void                *USER;

  } xyagobj_list;

  typedef struct xyagfig_list
  {
    struct xyagfig_list *NEXT;
    char                *NAME;
    xyagobj_list        *OBJECT [ XYAG_MAX_LAYER ];

  } xyagfig_list;

/*------------------------------------------------------------\
|                            Select                           |
\------------------------------------------------------------*/

  typedef struct xyagselect_list 
  {
    struct xyagselect_list *NEXT;
    xyagobj_list           *OBJECT;

  } xyagselect_list;

/*------------------------------------------------------------\
|                          Variables                          |
\------------------------------------------------------------*/

  extern long            XyagBoundXmin;
  extern long            XyagBoundYmin;
  extern long            XyagBoundXmax;
  extern long            XyagBoundYmax; 
  extern char            XyagRecomputeBound;

  extern xyagfig_list    *XyagFigure;
  extern cgvfig_list     *XyagFigureCgv;
  extern cgvfig_list     *XyagFigureCgvTmp;

  extern xyagselect_list *XyagHeadSelect;
  extern xyagselect_list *XyagHeadConnect;

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/
/*------------------------------------------------------------\
|                          Add Functions                      |
\------------------------------------------------------------*/

char XyagComputeHexagon(); //long long X1, long long Y1, long long X2, long long Y2, XPoint *Points);
char XyagComputeArrow(); //long long X1, long long Y1, long long X2, long long Y2, XPoint *Points);

  extern xyagfig_list   *XyagAddFigure();
  extern xyagobj_list   *XyagAddUserDefined();
  extern xyagobj_list   *XyagAddCircle();
  extern xyagobj_list   *XyagAddLine();
  extern xyagobj_list   *XyagAddRectangle();
  extern xyagobj_list   *XyagAddTriangle();
  extern xyagobj_list   *XyagAddHexagon();
  extern xyagobj_list   *XyagAddCell();
  extern xyagobj_list   *XyagAddText();

/*------------------------------------------------------------\
|                          Del Functions                      |
\------------------------------------------------------------*/

  extern void XyagDelFigure();

/*------------------------------------------------------------\
|                   Flag Set & Clear Functions                |
\------------------------------------------------------------*/

  extern void XyagSelectObject();
  extern void XyagUnselectObject();
  extern void XyagAcceptObject();
  extern void XyagRejectObject();
  extern void XyagConnectObject();
  extern void XyagDisconnectObject();

/*------------------------------------------------------------\
|                          Select Functions                   |
\------------------------------------------------------------*/

  extern void XyagAddSelect();
  extern void XyagDelSelect();
  extern void XyagPurgeSelect();

/*------------------------------------------------------------\
|                          Connect Functions                  |
\------------------------------------------------------------*/

  extern void XyagAddConnect();
  extern void XyagDelConnect();

/*------------------------------------------------------------\
|                          Bound Compute                      |
\------------------------------------------------------------*/

  extern char XyagComputeBound();

/*------------------------------------------------------------\
|                        Load Functions                       |
\------------------------------------------------------------*/

 extern void XyagLoadFigure();

/*------------------------------------------------------------\
|                    Error Message Functions                  |
\------------------------------------------------------------*/

  extern char *XyagGetErrorMessage();
  extern void  XyagExitErrorMessage();
  extern void  XyagInitializeErrorMessage(char Debug);

/*------------------------------------------------------------\
|                    Informations Functions                   |
\------------------------------------------------------------*/

  extern char *XyagGetInformations();


void xyagSaveState();
void xyagRetreiveState();
int XyagLoadFigureByType(char *Name, int type, int silent); 
// type can be : CGV_FROM_CNS or CGV_FROM_LOFIG
int XyagFileOpenByType(char *Name, int type);

// XYAGLE INTERACTION FONCTIONS
void XyagHiLight(cgv_interaction *itr, int complete);
void XyagExtract(cgv_interaction *itr, int complete);

extern chain_list *OPEN_STACK;
void DisplayMessage(char *content);

void XyagGetWarningMess();
void XyagExit(int);
void XyagFirePasqua();
void XyagPasqua();
void XyagInitFileErr();

extern mbkContext *XYAG_ctx;
extern cnsContext *CNS_ctx;
extern void XyagExit(int exit);
extern void (* XyagCallbackFileQuitFonction)() ;
extern void (* XyagPasquaFunction)() ;
extern void (* XyagFirePasquaFunction)() ;
extern void (* XyagGetWarningFunction)() ;


extern void         XyagSetJmpEnv(sigjmp_buf*);
# endif
