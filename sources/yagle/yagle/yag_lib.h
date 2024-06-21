/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag300.h                                                    */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#ifndef YAG
#define YAG

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include CNS_H

/* disassembly mode masks */

#define YAG_GENBEFIG   ((long) 0x00000001 )
#define YAG_GENLOFIG   ((long) 0x00000002 )

/* info constants */

#define YAG_INFODLATCH       ((long) 0x00000001 )
#define YAG_INFONOTDLATCH    ((long) 0x00000002 )
#define YAG_INFOPRECHARGE    ((long) 0x00000004 )
#define YAG_INFONOTPRECHARGE ((long) 0x00000008 )
#define YAG_INFORSMARK       ((long) 0x00000010 )
#define YAG_INFORSLEGAL      ((long) 0x00000020 )
#define YAG_INFORSILLEGAL    ((long) 0x00000040 )
#define YAG_INFOMODELLOOP    ((long) 0x00000100 )
#define YAG_INFONOTMODELLOOP ((long) 0x00000200 )

/* transistor overtyping */

#define CAPACITE       ((char) 0x10 )
#define BLEEDER        ((char) 0x20 )
#define RESIST         ((char) 0x40 )
#define DIODE          ((char) 0x80 )
#define USED           ((char) 0x01 )
#define ORIENTED       ((char) 0x02 )

/* branch overtyping */

#define YAG_PULLUP     ((long) 0x20000000 )
#define YAG_MARKFALSE  ((long) 0x00200000 )  
#define YAG_TREATED    ((long) 0x00100000 )
#define YAG_FALSECONF  ((long) 0x40000000 )  
#define YAG_DUALBRANCH ((long) 0x01000000 )  
#define YAG_TEMPBRANCH ((long) 0x02000000 )  
#define YAG_IGNORE     ((long) 0x04000000 )  
#define YAG_ASYNC      ((long) 0x08000000 )  
#define YAG_SYNC       ((long) 0x00400000 )  
#define YAG_FALSE_UP   ((long) 0x00800000 )  
#define YAG_FALSE_DN   ((long) 0x80000000 )  
#define YAG_LATCH_NF   ((long) 0x10000000 )  

/* cone overtyping */

#define YAG_DELETED    ((long) 0x80000000 )
#define YAG_PARTIAL    ((long) 0x00800000 )
#define YAG_FORCED     ((long) 0x20000000 )
#define YAG_MARK       ((long) 0x10000000 )
#define YAG_TEMPCONE   ((long) 0x08000000 )
#define YAG_CELLOUT    ((long) 0x04000000 )
#define YAG_HASDUAL    ((long) 0x02000000 )
#define YAG_CONESEEN   ((long) 0x01000000 )
#define YAG_CONSTRAINT ((long) 0x00040000 )
#define YAG_STOP       ((long) 0x00400000 )
#define YAG_GLUECONE   ((long) 0x00200000 )  
#define YAG_VISITED    ((long) 0x00100000 )  
#define YAG_LOOPCONF   ((long) 0x00080000 )  
#define YAG_FALSECONF  ((long) 0x40000000 )  
#define YAG_FORCEPRIM  ((long) 0x00020000 )
#define YAG_AUTOLATCH  ((long) 0x00010000 )

/* cone tectype overtyping */

#define YAG_SENSITIVE   ((long) 0x01000000 )
#define YAG_SPLITTIMING ((long) 0x02000000 )
#define YAG_NOTLATCH    ((long) 0x04000000 )
#define YAG_LEVELHOLD   ((long) 0x08000000 )
#define YAG_RESCONF     ((long) 0x10000000 )
#define YAG_BADCONE     ((long) 0x20000000 )

/* edge overtyping */

#define YAG_MARK            ((long) 0x10000000 )  
#define YAG_PRECHCOM        ((long) 0x20000000 )  
#define YAG_FALSECONF       ((long) 0x40000000 )  
#define YAG_NOT_FUNCTIONAL  ((long) 0x80000000 )  
#define YAG_EDGE_MASK       ((long) 0x00ffffff )  

/* cell types */

#define YAG_CELL_MS_SC         ((long) 0x00000001 )
#define YAG_CELL_MSS_SC        ((long) 0x00000002 )
#define YAG_CELL_MSR_SC        ((long) 0x00000003 )
#define YAG_CELL_MSNR_SC       ((long) 0x00000023 )
#define YAG_CELL_MS_SC_RT      ((long) 0x00000011 )
#define YAG_CELL_MSS_SC_RT     ((long) 0x00000012 )
#define YAG_CELL_MSR_SC_RT     ((long) 0x00000013 )
#define YAG_CELL_MSNR_SC_RT    ((long) 0x00000033 )
#define YAG_CELL_MSDIFF        ((long) 0x00000004 )
#define YAG_CELL_MS_SX         ((long) 0x00000101 )
#define YAG_CELL_MS_SX_RT      ((long) 0x00000111 )
#define YAG_CELL_FFT2          ((long) 0x00000005 )
#define YAG_CELL_FD2R          ((long) 0x00000006 )
#define YAG_CELL_FD2S          ((long) 0x00000007 )
#define YAG_CELL_LD1           ((long) 0x00000008 )
#define YAG_CELL_LD1R          ((long) 0x00000009 )
#define YAG_CELL_BINAND        ((long) 0x0000000a )
#define YAG_CELL_BINOR         ((long) 0x0000000b )
#define YAG_CELL_LATCH         ((long) 0x0000000c )
#define YAG_CELLMASK           ((long) 0x0000ffff )

/* for the cnsfig USER field */

#define YAG_TEMPCONE_PTYPE     ((long) 0x10000001 )
#define YAG_GLUECONE_PTYPE     ((long) 0x10000002 )
#define YAG_CONE_NETLIST_PTYPE ((long) 0x10000004 )

/* for the cone USER field */

#define YAG_MEMORY_PTYPE       ((long) 0x10000002 )
#define YAG_INPUTS_PTYPE       ((long) 0x10000005 )
#define YAG_WEIGHT_PTYPE       ((long) 0x10000006 )
#define YAG_MODEL_PTYPE        ((long) 0x10000007 )
#define YAG_OLDNAME_PTYPE      ((long) 0x10000008 )
#define YAG_OLDINDEX_PTYPE     ((long) 0x10000009 )
#define YAG_LATCHINFO_PTYPE    ((long) 0x1000000b )
#define YAG_BISTABLE_PTYPE     ((long) 0x1000000c )
#define YAG_INCONE_PTYPE       ((long) 0x10000012 )
#define YAG_BACK_PTYPE         ((long) 0x10000013 )
#define YAG_DUALEXPR_PTYPE     ((long) 0x10000014 )
#define YAG_DUALINPUTS_PTYPE   ((long) 0x10000015 )
#define YAG_LATCHBEFIG_PTYPE   ((long) 0x10000016 )
#define YAG_BUSBEFIG_PTYPE     ((long) 0x10000017 )
#define YAG_INFO_PTYPE         ((long) 0x10000018 )
#define YAG_STOP_PTYPE         ((long) 0x10000019 )
#define YAG_SAVEINPUTS_PTYPE   ((long) 0x10000020 )
#define YAG_MARK_PTYPE         ((long) 0x10000111 )

/* for the edge USER field */

#define YAG_WEIGHT_PTYPE       ((long) 0x10000006 )
#define YAG_MODEL_PTYPE        ((long) 0x10000007 )

/* for the branch USER field */

#define YAG_WEIGHT_PTYPE       ((long) 0x10000006 )
#define YAG_SIGLIST_PTYPE      ((long) 0x1000000d )
#define YAG_LOCONLIST_PTYPE    ((long) 0x1000000e )
#define YAG_BRANCHEXPR_PTYPE   ((long) 0x1000000f )
#define YAG_BRANCHBDD_PTYPE    ((long) 0x10000010 )

/* for the cell USER field */

#define YAG_SIGLIST_PTYPE      ((long) 0x1000000d )

/* for the connector USER field */

#define YAG_CONSTRAINT_PTYPE   ((long) 0x1000000f )
#define YAG_OLDNAME_PTYPE      ((long) 0x10000008 )
#define YAG_INOUT_PTYPE        ((long) 0x1000000b )
#define YAG_LOCON_PTYPE        ((long) 0x1000000c )
#define YAG_INPUT_PTYPE        ((long) 0x1000000d )

/* for the signal USER field */

#define YAG_CONE_PTYPE         ((long) 0x10000010 )
#define YAG_VAL_S_PTYPE        ((long) 0x10000011 )
#define YAG_TOPSIG_PTYPE       ((long) 0x10000012 )
#define YAG_MARKSIG_PTYPE      ((long) 0x10000013 )
#define YAG_CHECK_PTYPE        ((long) 0x10000014 )
#define YAG_DONE_PTYPE         ((long) 0x10000015 )

/* for the instance USER field */

#define YAG_MARKINS_PTYPE      ((long) 0x10000013 )

/* for the transistor USER field */

#define YAG_GRIDCON_PTYPE      ((long) 0x10000014 )
#define YAG_BIDIR_PTYPE        ((long) 0x10000015 )
#define YAG_TRANS_SIGN         ((long) 0x10000016 )
#define YAG_WIDTH_PTYPE        ((long) 0x10000017 )

/* for bequad USER field */

#define YAG_CONE_PTYPE         ((long) 0x10000010 )
#define YAG_CONELIST_PTYPE     ((long) 0x10000011 )
#define YAG_LOCK_PTYPE         ((long) 0x10000012 )

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define YAG_YES 1
#define YAG_NO 0

#define VDD_BRANCH 0
#define VSS_BRANCH 1
#define EXT_BRANCH 2
#define GND_BRANCH 3

#define A_LOT_OF_BRANCHES 200

#define YAGBUFSIZE 1024

#define YAG_NO_TIMING 0
#define YAG_MAX_TIMING 1
#define YAG_MIN_TIMING 2
#define YAG_MED_TIMING 3

/* RS detection mode masks */
#define YAG_RS_MODEL         ((short) 0x08 )
#define YAG_RS_LATCH         ((short) 0x10 )
#define YAG_RS_TOLERANT      ((short) 0x20 )
#define YAG_RS_LEGAL         ((short) 0x40 )
#define YAG_RS_ILLEGAL       ((short) 0x80 )

/* speed up flags */
#define YAG_HAS_INF_INPUTS 1

/* branch group */

typedef struct {
    branch_list     *BREXT,     /* list of external branches */
                    *BRVDD,     /* list of branches to VDD */
                    *BRVSS,     /* list of branches to VSS */
                    *BRGND;     /* list of branches to GND */
} branch_group;

/* functional dependancy graph structure */

typedef union {
    cone_list      *CONE;
    locon_list     *LOCON;
    void           *PTR;
} yag_gnodedata;

typedef struct gnode {
    struct gnode   *NEXT;
    long            TYPE;
    yag_gnodedata   OBJECT;
    short           VISITED;
    short           DEPTH;
    short           TREEBIT;
    short           NUMSONS;
    chain_list     *SONS;
    chain_list     *FATHERS;
} gnode_list;

typedef struct graph {
    struct graph   *NEXT;
    gnode_list     *HEADNODE;
    ptype_list     *ROOTNODES;
    chain_list     *PRIMVARS;
    ptype_list     *CONSTRAINTS;
    ptype_list     *EXTRA_CONSTRAINTS;
    ht             *HASHTAB;
    ht             *NAMEHASHTAB;
    short           WIDTH;
    int             COMPLETE;
} graph;

/* Global variables  to be positionned for yagDisassemble */

typedef struct yag_context {
    struct yag_context *NEXT;
    short            YAG_DEPTH;                 /* functional analysis depth       */
    short            YAG_MAX_LINKS;             /* max length of one branch        */
    int              YAG_BDDCEILING;            /* max nodes in BDD system         */
    short            YAG_USE_FCF;               /* !=0 if FCF analysis used        */
    short            YAG_ORIENT;                /* transistor orientation          */
    short            YAG_PROP_HZ;               /* FCF through HZ nodes            */
    short            YAG_MAKE_CELLS;            /* cell detection                  */
    short            YAG_GENIUS;                /* GENIUS hierarchical recognition */
    short            YAG_ONLY_GENIUS;           /* only use GENIUS                 */
    short            YAG_CELL_SHARE;            /* cones in multiple cells         */
    short            YAG_DETECT_LATCHES;        /* latch detection                 */
    short            YAG_DETECT_PRECHARGE;      /* precharge detection              */
    short            YAG_BLEEDER_STRICTNESS;    /* level 0-2 of bleeder strictness */
    short            YAG_LOOP_ANALYSIS;         /* functional analysis of loops    */
    short            YAG_AUTO_RS;               /* automatic RS detection          */
    short            YAG_BUS_ANALYSIS;          /* full bus analysis               */
    short            YAG_BUS_DEPTH;             /* depth for bus analysis          */
    short            YAG_AUTOLOOP_CEILING;      /* max inputs for auto latch       */
    short            YAG_AUTOLOOP_DEPTH;        /* analysis depth for auto latch   */
    short            YAG_FCL_DETECT;            /* cell detection using FCL        */
    short            YAG_ONLY_FCL;              /* only use FCL                    */
    short            YAG_ONE_SUPPLY;            /* single vdd and vss in vbe       */
    short            YAG_NO_SUPPLY;             /* no vdd or vss in vbe            */
    short            YAG_NOTSTRICT;             /* undriven gate not error         */
    short            YAG_REMOVE_PARA;           /* remove parallel transistors     */
    short            YAG_RELAX_ALGO;            /* give-up algo during making gates*/
    short            YAG_RELAX_LINKS;           /* max links during give-up algo   */
    short            YAG_MINIMISE_CONES;        /* remove parallel branches        */
    short            YAG_MINIMISE_INV;          /* remove simple inversions        */
    short            YAG_GEN_SIGNATURE;         /* generate signature for cones    */
    short            YAG_ASSUME_PRECEDE;        /* assume no conflicts for bux/bus */
    short            YAG_BLEEDER_PRECHARGE;     /* assume bleeder maintains one    */
    short            YAG_TRISTATE_MEMORY;       /* internal tristates memorise     */
    short            YAG_MARK_TRISTATE_MEMORY;  /* internal tristates marked latch */
    short            YAG_INTERFACE_VECTORS;     /* vectorise interface             */
    short            YAG_SIMPLIFY_EXPR;         /* simplify elementary expressions */
    short            YAG_SIMPLIFY_PROCESSES;    /* simplify processes              */
    short            YAG_COMPACT_BEHAVIOUR;     /* compact the data flow           */
    short            YAG_ELP;                   /* update capas from elp file      */
    short            YAG_BLOCK_BIDIR;           /* block and report bidir trans.   */
    double           YAG_THRESHOLD;             /* latch resolution threshold      */
    short            YAG_TAS_TIMING;            /* calculate delays with Tas       */
    double           YAG_SPLITTIMING_RATIO;     /* factor for up/down separation   */
    double           YAG_SENSITIVE_RATIO;       /* factor for sensitive timing     */
    short            YAG_SENSITIVE_MAX;         /* max driver for sensitive timing */
    short            YAG_HELP_S;                /* use _s orientation info         */
    short            YAG_STAT_MODE;             /* save a .stat file               */
    char            *YAG_DEBUG_CONE;            /* name of cone to debug           */
    short            YAG_SILENT_MODE;           /* no messages displayed on stdout */
    short            YAG_LOOP_MODE;             /* generate a .loop file           */
    short            YAG_KEEP_REDUNDANT;        /* keep redundant branches         */
    short            YAG_KEEP_GLITCHERS;        /* keep glitched branches          */
    short            YAG_STRICT_CKLATCH;        /* unmark all non CKLATCH commands */
    short            YAG_USESTMSOLVER;          /* STM conflict resolution         */
    short            YAG_UNMARK_BISTABLES;      /* Bistables not latches           */
    short            YAG_LATCH_REQUIRE_CLOCK;   /* Use INF clockdefs latche recog. */
    short            YAG_AUTO_FLIPFLOP;         /* automatic flip-flop recog.      */
    short            YAG_AUTO_ASYNC;            /* automatic set-reset recog.      */
    short            YAG_AUTO_MEMSYM;           /* automatic memsym recog.         */
    short            YAG_DETECT_REDUNDANT;      /* detect redundant branches       */
    short            YAG_DETECT_LEVELHOLD;      /* detect level-hold loops         */
    short            YAG_SIMPLE_LATCH;          /* simple latch detection          */
    short            YAG_USE_CONNECTOR_DIRECTION; /* connector direction orients transistors */
    short            YAG_CLOCK_GATE;            /* detect clock gating             */
    short            YAG_DETECT_SIMPLE_MEMSYM;  /* simple memsym                   */
    short            YAG_MEMSYM_HEURISTIC;      /* memsym branch suppression       */
    short            YAG_STUCK_LATCH;           /* stuck latch allowed             */
    short            YAG_DELAYED_RS;            /* non-overlapping clockgen        */

/* Names passed on command line */

    char            *YAG_FILENAME;       /* filename & default figure name */
    char            *YAG_FIGNAME;        /* cone netlist figure name */
    char            *YAG_OUTNAME;        /* behavioural figure name */

/* Additional global variables  to be positionned for yagle_main */

    short YAG_FILE;                      /* generate a .cns file with simplified lofig */
    short YAG_CNSLO;                     /* generate a .cns file with full lofig       */
    short YAG_CONE_NETLIST;              /* generate a hierarchical cone netlist       */
    short YAG_BEFIG;                     /* generate a behavioural model               */
    short YAG_BLACKBOX_SUPPRESS;         /* suppress instances in blackbox list        */
    short YAG_BLACKBOX_IGNORE;           /* ignore instances in blackbox list          */
    short YAG_NORC;                      /* delete RC interconnects                    */
    short YAG_HIERARCHICAL_MODE;         /* hierarchical disassembly                   */

/* Don't need to be positioned */

    char            *YAGLE_TOOLNAME;
    char             YAGLE_LANG;

    long             YAGLE_NB_ERR;
    chain_list      *YAGLE_INSTANCE_BEFIGS;
    befig_list      *YAG_BEFIG_LIST;

/* Statistic and error files */ 

    FILE            *YAGLE_ERR_FILE;
    FILE            *YAGLE_STAT_FILE;
    FILE            *YAGLE_LOOP_FILE;

/* Internal variables */
    lofig_list      *YAG_CURLOFIG;              /* logical figure list head           */
    cnsfig_list     *YAG_CURCNSFIG;             /* disassembled figure list head      */
    chain_list      *YAG_CONSTRAINT_LIST;       /* List of user specified constraints */
    int              YAG_MAXNODES;              /* max number of BDD nodes            */ 
    int              YAG_LASTNODECOUNT;         /* previous BDD node count            */
    int              YAG_REQUIRE_COMPLETE_GRAPH;/* Functional analysis modes          */
    int              YAG_BIABL_INDEX;           /* index for guarded expression label */
    pCircuit         YAG_CURCIRCUIT;            /* circuit of cone under analysis     */
    graph           *YAG_CONE_GRAPH;            /* dependency graph of current cone   */
    graph           *YAG_SUPPORT_GRAPH;         /* support graph of current cone      */
    int              YAG_ITERATIONS;            /* cone building iteration count      */
    int              YAG_FLAGS;                 /* speed up flags / added zinaps      */
} yag_context_list;

extern yag_context_list *YAG_CONTEXT;

/* Function declarations */

/* Primary interface functions */

cnsfig_list    *yagle_main(void);
cnsfig_list    *yagDisassemble(char *figname, lofig_list *ptmbkfig, long mode);
void            yagenv(void (*initfunc)());
void            yagrestore(void);

/* general CNS utility functions */

void            yagFindSupplies __P((inffig_list *ifl, lofig_list *ptlofig, int silent));
void            yagFindInternalSupplies __P((inffig_list *ifl, lofig_list *ptlofig, int silent));
void            yagTestTransistors __P((lofig_list *ptmbkfig, int silent));
void            yagExit __P((int code));
void            yagPrintTime __P((struct rusage *start, struct rusage *end, time_t rstart, time_t rend));
void            yagChrono __P((struct rusage *t, time_t *rt));
int             yagDepthFirstProcess __P((cone_list *headcone, int  (*processCone)()));
void            yagDestroyBranch __P((cone_list *ptcone, branch_list *ptdel));
branch_list    *yagAppendBranch __P((branch_list *pt1, branch_list *pt2));
void            yagDelBranchList __P((branch_list *ptlist));
branch_list    *yagCopyBranch __P((branch_list *pthead, branch_list *ptbranch));
int             yagCountBranches __P((cone_list *ptcone));
link_list      *yagCopyLinkList __P((link_list *ptheadlink));
int             yagCountLinks __P((link_list *ptheadlink));
int             yagCountActiveLinks __P((link_list *ptheadlink));
link_list      *yagAddTransLink __P((link_list *ptheadlink, lotrs_list *pttrans, losig_list *ptsig));
void            yagFreeEdgeList __P((edge_list *ptlist));
int             yagCountEdges __P((edge_list *ptlist));
int             yagCountActiveEdges __P((edge_list *ptlist));
int             yagCountConeEdges __P((edge_list *ptlist));
cone_list *     yagGetNextConeEdge (edge_list *ptlist);
edge_list      *yagGetEdge __P((edge_list *ptedgelist, void *ptinput));
edge_list      *yagGetEdge_QUICK __P((edge_list *ptedgelist, void *ptinput, ht **FAST));
chain_list     *yagGetChain __P((chain_list *pthead, void *ptdata));
chain_list     *yagGetChain_QUICK __P((chain_list *pthead, void *ptdata, ht **FAST));
void           *yagGetChainInd __P((chain_list *ptchain, int index));
chain_list     *yagRmvChain __P((chain_list *pthead, void *ptdata));
int             yagCountChains __P((chain_list *headchain));
chain_list     *yagCopyChainList __P((chain_list *headchain));
chain_list     *yagUnionChainList __P((chain_list *ptchain1, chain_list *ptchain2));
ptype_list     *yagGetPtype __P((ptype_list *pthead, void *ptdata));

/* signal name processing functions */

int             yagTestJoker __P((char *testname, char *oldname));
char           *yagVectorizeName __P((char *name));
char           *yagGetName __P((inffig_list *ifl, losig_list *pt_sig));
char           *yagTreatName __P((char *str));
char           *yagDownName __P((char *name));

/* BDD utility functions */

void            yagControlBdd __P((int mode));

/* lofig utility functions */

locon_list     *yagGetExtLocon __P((losig_list *ptsig));
chain_list     *yagGetExtLoconList __P((losig_list *ptsig));
char            yagMergeDirection __P((char dir1, char dir2));

/* branch resistance calculation */

float           yagCalcBranchResistance __P((branch_list *ptbranch));
float           yagCalcParallelResistance __P((chain_list *ptbranchlist));
float           yagCalcMaxDownRes __P((cone_list *ptcone));
float           yagCalcMaxUpRes __P((cone_list *ptcone));

#endif

