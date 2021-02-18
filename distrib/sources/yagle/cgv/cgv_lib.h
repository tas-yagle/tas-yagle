/*------------------------------------------------------\
|                                                       |
|  Title   :   Structures and fonctions for CGV         |
|                                                       |
|  File    :            cgv100.h                        |
|                                                       |
|                                                       |
|  Authors :         Picault Stephane                   |
|                    Miramond Benoit                    |
|                    Lester Anthony                     |
|                                                       |
\------------------------------------------------------*/

#ifndef CGV
#define CGV

#include SLB_H
#include CNS_H

#define CGV_FROM_CNS            0x0001
#define CGV_FROM_LOFIG          0x0010
#define CGV_FROM_CONE           0x0020
#define CGV_SEL_PREC_LOGIC      0x0002
#define CGV_SEL_SUCC_LOGIC      0x0004
#define CGV_SEL_REC_LOGIC       0x0008
#define CGV_FROM_CHAIN_CONE     0x1000
#define CGV_FROM_CHAIN_NAME     0x2000

#define CGV_NET_TYPE        0x90000020
#define CGV_BOX_TYPE        0x90000200
#define CGV_CON_TYPE        0x90002000
#define CGV_MARK_TYPE       0x90020000

#define CGV_BOX_TAGED           0x0001
#define CGV_BOX_RETAGED         0x0100
#define CGV_BOX_CLUSTERISED     0x0002
#define CGV_BOX_CLUSTER         0x0004
#define CGV_BOX_TRANSPARENCE    0x0008
#define CGV_BOX_PLACED          0x1000
#define CGV_BOX_HIT_FON_OUT_CON 0x2000

#define CGV_CON_IN              0x01
#define CGV_CON_OUT             0x02
#define CGV_CON_EXTERNAL        0x10
#define CGV_CON_INTERNAL        0x20

#define CGV_CON_TAGED           0x0100
#define CGV_CON_PLACED          0x0200
#define CGV_CON_FAKE            0x0400

#define CGV_NET_CANAL             0x0F

#define CGV_NET_MAXDIR            0x20
#define CGV_NET_MINDIR            0x10

#define CGV_ROOT_CON_BOX        0x0001
#define CGV_ROOT_CON_FIG        0x0002

#define CGV_SOURCE_LOSIG          0x01
#define CGV_SOURCE_LOINS          0x02
#define CGV_SOURCE_LOCON          0x03
#define CGV_SOURCE_LOFIG          0x04
#define CGV_SOURCE_CNSFIG         0x05
#define CGV_SOURCE_CNSCONE        0x06
#define CGV_SOURCE_CNSEDGE        0x07
#define CGV_SOURCE_CNSCELL        0x08
#define CGV_SOURCE_LOTRS          0x09
#define XYAG_DEFAULT_HEIGHT     0x0004
#define XYAG_DEFAULT_FACTOR     0x0003

#define CGV_CEL_BOX        0x0001
#define CGV_CEL_CON        0x0002

#define CGV_FILE_TYPE 0x22334455

/*------------------------------------------------------\
|                        Macros                         |
\------------------------------------------------------*/

#define IsCgvBoxClusterised(X)   ((X)->FLAGS & CGV_BOX_CLUSTERISED)
#define IsCgvBoxTaged(X)         ((X)->FLAGS & CGV_BOX_TAGED)
#define IsCgvBoxReTaged(X)       ((X)->FLAGS & CGV_BOX_RETAGED)
#define IsCgvBoxCluster(X)       ((X)->FLAGS & CGV_BOX_CLUSTER)
#define IsCgvBoxTransparence(X)  ((X)->FLAGS & CGV_BOX_TRANSPARENCE)
#define IsCgvBoxPlaced(X)        ((X)->FLAGS & CGV_BOX_PLACED)

#define IsCgvConIn(X)          ((X)->TYPE  & CGV_CON_IN)
#define IsCgvConOut(X)         ((X)->TYPE  & CGV_CON_OUT)
#define IsCgvConExternal(X)    ((X)->TYPE  & CGV_CON_EXTERNAL)
#define IsCgvConInternal(X)    ((X)->TYPE  & CGV_CON_INTERNAL)
#define IsCgvConTaged(X)       ((X)->FLAGS & CGV_CON_TAGED)
#define IsCgvConPlaced(X)      ((X)->FLAGS & CGV_CON_PLACED)
#define IsCgvConFake(X)        ((X)->FLAGS & CGV_CON_FAKE)

#define IsCgvNetCanal(X)        ((X)->FLAGS & CGV_NET_CANAL)

#define GetCgvNetDirType(X)     ((X)->FLAGS & (CGV_NET_MAXDIR|CGV_NET_MINDIR))

#define SetCgvNetMaxOutput(X)     ((X)->FLAGS |= CGV_NET_MAXDIR)
#define SetCgvNetMinOutput(X)     ((X)->FLAGS |= CGV_NET_MINDIR)

#define SetCgvBoxClusterised(X)  ((X)->FLAGS |= CGV_BOX_CLUSTERISED)
#define SetCgvBoxTaged(X)        ((X)->FLAGS |= CGV_BOX_TAGED)
#define SetCgvBoxUnTaged(X)      ((X)->FLAGS &= ~CGV_BOX_TAGED)
#define SetCgvBoxReTaged(X)      ((X)->FLAGS |= CGV_BOX_RETAGED)
#define SetCgvBoxCluster(X)      ((X)->FLAGS |= CGV_BOX_CLUSTER)
#define SetCgvBoxTransparence(X) ((X)->FLAGS |= CGV_BOX_TRANSPARENCE)
#define SetCgvBoxPlaced(X)       ((X)->FLAGS |= CGV_BOX_PLACED)
#define SetCgvBoxHit(X)          ((X)->FLAGS |= CGV_BOX_HIT_FON_OUT_CON)
#define IsCgvBoxHit(X)           ((X)->FLAGS & CGV_BOX_HIT_FON_OUT_CON)

#define SetCgvConExternal(X)   ((X)->TYPE  |= CGV_CON_EXTERNAL)
#define SetCgvConInternal(X)   ((X)->TYPE  |= CGV_CON_INTERNAL)
#define SetCgvConTaged(X)      ((X)->FLAGS |= CGV_CON_TAGED)
#define SetCgvConUnTaged(X)    ((X)->FLAGS &= ~CGV_CON_TAGED)
#define SetCgvConPlaced(X)     ((X)->FLAGS |= CGV_CON_PLACED)
#define SetCgvConFake(X)       ((X)->FLAGS |= CGV_CON_FAKE)

#define SetCgvNetCanal(X)       ((X)->FLAGS |= CGV_NET_CANAL)

#define ClearCgvNetCanal(X)     ((X)->FLAGS &= ~CGV_NET_CANAL)
#define ClearCgvNetDir(X)       ((X)->FLAGS &= ~(CGV_NET_MINDIR|CGV_NET_MAXDIR))

#define addcgvfigconin(F, N)  (addcgvfigcon((F), (N), CGV_CON_IN))
#define addcgvfigconout(F, N) (addcgvfigcon((F), (N), CGV_CON_OUT))

#define addcgvboxconin(F, B, N)  (addcgvboxcon((F), (B), (N), CGV_CON_IN))
#define addcgvboxconout(F, B, N) (addcgvboxcon((F), (B), (N), CGV_CON_OUT))

extern int CGV_SCALE;
#define MARGE (3*CGV_SCALE)
/*------------------------------------------------------\
|                      Structures                       |
\------------------------------------------------------*/

/*------------------------------------------------------\
|                        Wires                          |
\------------------------------------------------------*/

typedef struct cgvwir {
    struct cgvwir  *NEXT;
    struct cgvnet  *NET;
    long            X;
    long            Y;
    long            DX;
    long            DY;
    long            FLAGS;
    void           *USER;
} cgvwir_list;

/*------------------------------------------------------\
|                        Nets (Signals)                 |
\------------------------------------------------------*/

typedef struct cgvnet {
  struct cgvnet  *NEXT;
  chain_list     *CON_NET;
  struct cgvwir  *WIRE;
  void           *SOURCE;
  unsigned char   SOURCE_TYPE;
  long            FLAGS;
  long            NUMBER_IN;
  long            NUMBER_OUT;
  long            CANAL;      /* pour routage ... */
  long            YMIN;       /* pour routage ... */
  long            YMAX;       /* pour routage ... */
  void           *USER;
  char *NAME;
  struct cgvnet *UNDER;
} cgvnet_list;

/*------------------------------------------------------\
|                      Connectors                       |
\------------------------------------------------------*/

typedef struct cgvcon {
    struct cgvcon  *NEXT;
    unsigned char   TYPE;
    unsigned char   DIR;
    char           *NAME;
  char           *SUB_NAME;
    unsigned char   ROOT_TYPE;
    void           *ROOT;
    cgvnet_list    *NET;
    void           *SOURCE;
    unsigned char   SOURCE_TYPE;
    long            X_REL;
    long            Y_REL;
    long            FLAGS;
    void           *USER;
} cgvcon_list;

/*------------------------------------------------------\
|                        Box (Instance)                 |
\------------------------------------------------------*/

typedef struct cgvbox {
    struct cgvbox  *NEXT;
    char           *NAME;
    struct cgvcon  *CON_IN;
    long            NUMBER_IN;
    struct cgvcon  *CON_OUT;
    long            NUMBER_OUT;
    void           *SOURCE;
    unsigned char   SOURCE_TYPE;
    long            X;
    long            Y;
    long            DX;
    long            DY;
    long            FLAGS;
    symbol_list    *SYMBOL;
    void           *USER;
  union 
  {
    struct cgvbox *UNDER;
    int count;
  } misc;
  struct cgvbox *tmp;
} cgvbox_list;

/*------------------------------------------------------\
|                        Figure                         |
\------------------------------------------------------*/

typedef struct cgvfig {
    struct cgvfig  *NEXT;
    char           *NAME;
    struct cgvcon  *CON_IN;
    long            NUMBER_IN;
    struct cgvcon  *CON_OUT;
    long            NUMBER_OUT;
    cgvbox_list    *BOX;
    cgvnet_list    *NET;
    void           *SOURCE;
    unsigned char   SOURCE_TYPE;
    long            X;
    long            Y;
    long            FLAGS;
    library        *LIBRARY;
    long            SCALE;
    void           *USER;
  void *data0, *data1, *data2;
  void *data_ZOOM;
} cgvfig_list;

/*------------------------------------------------------\
|                        Colums                         |
\------------------------------------------------------*/

typedef struct cgvcol {
    struct cgvcol  *NEXT;
    struct cgvcel  *CELL;
    struct cgvcel  *LAST_CELL;
    long            NUMBER_CELL;
    long            MAX_DX_CELL;
  long tab[100];
} cgvcol_list;

/*------------------------------------------------------\
|                        Cells                          |
\------------------------------------------------------*/

typedef struct cgvcel {
    struct cgvcel  *NEXT;
    struct cgvcel  *PREV;
    struct cgvcol  *COL;
    void           *ROOT;
    long            TYPE;
} cgvcel_list;

typedef struct cgv_actions
{
  int type;
} cgv_actions;

typedef struct cgv_interaction
{
  struct cgv_interaction *NEXT;
  unsigned int object_type;
  char *name;
} cgv_interaction;

typedef void (*markobject)(void *data);

/*------------------------------------------------------\
|                     Global Variables                  |
\------------------------------------------------------*/

extern cgvfig_list  *HEAD_CGVFIG;
extern cgvcol_list  *HEAD_CGVCOL;
extern int           CGV_MAKE_CELLS;
extern int           CGV_WAS_PRESENT;
extern library      *CGV_LIB;

/*------------------------------------------------------\
|                        Functions                      |
\------------------------------------------------------*/

void                 cgv_hilite             (cgvfig_list     *Figure,
                                             cgvbox_list     *box,
                                             int              mode,
                                             int              depth,
                                             markobject       mark);
void                 cgv_hilite_net         (cgvfig_list     *Figure,
                                             cgvcon_list     *c,
                                             int              mode,
                                             int              depth,
                                             markobject       mark);

/*------------------------------------------------------\
|                      Free Functions                   |
\------------------------------------------------------*/

void                 freecgvfig             (cgvfig_list     *Figure);
void                 freecgvcon             (cgvcon_list     *Connector);
void                 freecgvbox             (cgvbox_list     *Box);
void                 freecgvwir             (cgvwir_list     *Wire);
void                 freecgvnet             (cgvnet_list     *Net);
void                 freecgvcol             (cgvcol_list     *Colum);
void                 freecgvcel             (cgvcel_list     *Cell);

/*------------------------------------------------------\
|                      Add Functions                    |
\------------------------------------------------------*/

cgvnet_list         *addcgvnetcon           (cgvnet_list     *Net,
                                             cgvcon_list     *Con);
cgvfig_list         *addcgvfig              (char            *Name,
                                             long             scale);
cgvcon_list         *addcgvfigcon           (cgvfig_list     *Figure,
                                             char            *Name,
                                             unsigned         char Type);
cgvbox_list         *addcgvbox              (cgvfig_list     *Figure,
                                             char            *Name);
cgvcon_list         *addcgvboxcon           (cgvfig_list     *Figure,
                                             cgvbox_list     *Box,
                                             char            *Name,
                                             unsigned         char Type);
cgvwir_list         *addcgvwir              (cgvfig_list     *Figure,
                                             cgvnet_list     *Net);
cgvnet_list         *addcgvnet              (cgvfig_list     *Figure);

cgvcel_list         *addcgvcel              (cgvcol_list     *Column);
cgvcol_list         *addcgvcol              (void);           

cgvcel_list         *addcgvboxtocgvcel      (cgvbox_list     *CgvBox,
                                             cgvcel_list     *CgvCel);
cgvcel_list         *addcgvcontocgvcel      (cgvcon_list     *CgvCon,
                                             cgvcel_list     *CgvCel);

/*------------------------------------------------------\
|                      Del Functions                    |
\------------------------------------------------------*/

void                 cgvdeltransparence     (cgvcel_list     *Cell,
                                             cgvfig_list     *Figure);
int                  delcgvfig              (char            *Name);
cgvfig_list         *rmvcgvfig              (cgvfig_list     *CgvFig);
void                 delcgvcol              (void);

/*------------------------------------------------------\
|                     View Functions                    |
\------------------------------------------------------*/

void                 viewcgvwir             (cgvwir_list     *Wire);
void                 viewcgvnet             (cgvnet_list     *Net);
void                 viewcgvcon             (cgvcon_list     *Con,
                                             char            *Blank);
void                 viewcgvbox             (cgvbox_list     *Box);
void                 viewcgvfig             (cgvfig_list     *Figure);

void                 cgv_error              (char             Error,
                                             char            *Text,
                                             char            *File,
                                             long             Line);
void                 cgv_setscale           (int              scale);

chain_list          *cgvlistcone            (cnsfig_list     *CnsFigure,
                                             chain_list      *NameList,
                                             int              Mode,
                                             int              Depth,
                                             cgvfig_list     *CgvFigure);
void                 buildcgvfig            (cgvfig_list     *Figure);

cgvfig_list         *getcgvfig              (char            *FileName,
                                             long             Type,
                                             char            *filename);
cgvfig_list         *getcgvfig_from_lofig   (char            *Name,
                                             char            *filename);
cgvfig_list         *getcgvfig_from_cnsfig  (char            *FileName,
                                             char            *filename);
cgvfig_list         *getcgvfig_from_cone    (cone_list       *cn);

void                 freecgvfigure          (cgvfig_list     *Figure);

cgvfig_list         *getcgvfile             (char            *name,
                                             int              type,
                                             int              cut,
                                             char            *filename);
void                 removecgvfiles         (char            *name,
                                             void             (*did)(cgvfig_list                                                                   *removed));
void                 removeallcgvfiles      (void             (*did)(cgvfig_list                                  *removed));
void                 addcgvfile             (char            *name,
                                             int              type,
                                             int              cut,
                                             char            *filename,
                                             cgvfig_list     *cgvstruct,
                                             void            *lofig,
                                             void            *cnsf,
                                             int              inmem);
char                *getcgvfileext          (cgvfig_list     *cgvstruct);

void                 cgv_HiLight            (cgvfig_list     *cgvf,
                                             cgv_interaction *itr,
                                             int              complete,
                                             markobject       mark);
cgvfig_list         *cgv_Extract            (cgvfig_list     *cgvf,
                                             cgv_interaction *itr,
                                             int              complete);
void                 cgv_extract            (cgvfig_list     *Figure,
                                             cgvbox_list     *box,
                                             int              mode,
                                             int              depth);
void                 complete_extract_list  (cgvfig_list     *cgvf,
                                             cgv_interaction *itr,
                                             int              complete,
                                             markobject       mark);
void                 cgv_extract_net        (cgvfig_list     *Figure,
                                             cgvcon_list     *c,
                                             int              mode,
                                             int              depth);
cgvfig_list         *finish_extract         (cgvfig_list     *parent);


// CGV INTERACTION OBJECTS / FUNCTIONS
cgv_interaction     *cgv_NetObject          (cgv_interaction *head,
                                             char            *name);
cgv_interaction     *cgv_GateObject         (cgv_interaction *head,
                                             char            *name);
cgv_interaction     *cgv_ConnectorObject    (cgv_interaction *head,
                                             char            *name);
void                 cgv_FreeObjects        (cgv_interaction *head);

void                 cgv_SetDefaultLibrary  (library         *l);
symbol_list         *cgv_getlofigcellsymbol (library         *ptlib,
                                             char            *symname);
symbol_list         *cgv_getselfcellsymbol  (library         *ptlib,
                                             char            *symname);
void                 parsecorresp           (char            *name);
char                *getcorrespgate         (char            *name);
char                *getcorrespgatepin      (char            *name,
                                             char            *con);

#endif
