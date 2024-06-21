 
/* 10/24/95 Cone Netlist Structure: cns.h                                     */

/* struct lotrs: same as MBK
   typedef struct lotrs {
   struct lotrs   *NEXT;
   struct locon   *DRAIN;
   struct locon   *GRID;
   struct locon   *SOURCE;
   struct locon   *BULK;
   char           *TRNAME;
   long            X,Y;
   unsigned short  WIDTH, LENGTH;
   unsigned short  PS, PD;
   unsigned short  XS, XD;
   char            TYPE;
   ptype_list   *USER;
   } lotrs_list; */

/* struct locon: same as MBK
   typedef struct locon {
   struct locon   *NEXT;
   char           *NAME;
   struct losig   *SIG;
   void           *ROOT;
   char            DIRECTION;
   char            TYPE;
   struct num     *PNODE;
   ptype_list   *USER;
   } locon_list; */

/* for cone expr */
typedef struct {
    chain_list  *UP;
    chain_list  *DN;
} abl_pair;

/* ulink union */
typedef union ulink {
    struct lotrs   *LOTRS;      /* if the ulink is a transistor    */
    struct locon   *LOCON;      /* if the ulink is a connector     */
    void           *PTR;        /* if the ulink is of unknown type */
} uulink;

/* uedge union */
typedef union uedge {
    struct cone    *CONE;       /* if the uedge is a cone          */
    struct locon   *LOCON;      /* if the uedge is a connector     */
    void           *PTR;        /* if the uedge is of unknown type */
} uuedge;

/* link structure */
typedef struct link {
    struct link    *NEXT;       /* next branch link                           */
    long            TYPE;       /* link type                                  */
    union ulink     ULINK;      /* the ulink depends on the type of the link, */
    /* it may be a connector or a transistor      */
    struct losig   *SIG;        /* link signal                              */
    ptype_list   *USER;       /* user defined, probably not empty           */
} link_list;

/* branch structure */
typedef struct branch {
    struct branch  *NEXT;       /* next cone branch                 */
    long            TYPE;       /* branch type                      */
    struct link    *LINK;       /* branch link list                 */
    ptype_list   *USER;       /* user defined, probably not empty */
} branch_list;

/* input and output lists of cones */
typedef struct edge {
    struct edge    *NEXT;       /* next input or output                       */
    long            TYPE;       /* type of input or output                    */
    union uedge     UEDGE;      /* the uedge depends on the type of the edge, */
    /* it may be a cone or a connector            */
    ptype_list   *USER;       /* user defined, probably not empty           */
} edge_list;

/* cone structure */
typedef struct cone {
    struct cone    *NEXT;       /* next cone                               */
    long            INDEX;      /* cone index, unique within a figure      */
    char           *NAME;       /* cone name                               */
    long            TYPE;       /* cone type                               */
    long            TECTYPE;    /* cone technologically oriented type      */
#ifdef USEOLDTEMP
    long            XM, Xm,     /* cone Max and min X coordinates          */
                    YM, Ym;     /* cone Max and min Y coordinates          */
#endif
    struct edge    *INCONE,     /* cone input list                         */
                   *OUTCONE;    /* cone output list                        */
    struct branch  *BREXT,      /* list of branches leading to a connector */
                   *BRVDD,      /* list of branches leading to VDD         */
                   *BRGND,      /* list of branches leading to GND         */
                   *BRVSS;      /* list of branches leading to VSS         */
    chain_list     *CELLS;      /* list of cells the cone belongs to       */
    ptype_list   *USER;       /* user defined, probably not empty        */
} cone_list;

/* cell structure */
typedef struct cell {
    struct cell    *NEXT;       /* next cell                         */
    long            TYPE;       /* cell type, check #defines         */
    chain_list     *CONES;      /* list of cones making the cell     */
    struct befig   *BEFIG;      /* the cell's behavioral description */
    ptype_list   *USER;       /* user defined, probably not empty  */
} cell_list;

/* cone netlist structure figure */
typedef struct cnsfig {
    struct cnsfig  *NEXT;       /* next cns figure                      */
    char           *NAME;       /* figure name                          */
    struct locon   *LOCON;      /* external connector list              */
    struct locon   *INTCON;     /* internal connector list              */
    struct lotrs   *LOTRS;      /* transistor list                      */
    struct loins   *LOINS;      /* instance list                        */
    struct cone    *CONE;       /* cone list                            */
    struct cell    *CELL;       /* cell list                            */
    struct lofig   *LOFIG;      /* the cns figure's MBK lofig figure    */
    struct befig   *BEFIG;      /* the cns figure's behavioral figure   */
    ptype_list   *USER;       /* user defined, probably not empty     */
} cnsfig_list;

/* Global context */
typedef struct {
    cnsfig_list *CNS_HEAD;
} cnsContext;

/* power supply structure */
typedef struct alim {
    struct alim  *NEXT;
    float         VDDMIN;
    float         VDDMAX;
    float         VSSMIN;
    float         VSSMAX;
} alim_list;

