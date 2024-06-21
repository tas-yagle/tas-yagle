#ifndef RCNH
#define RCNH

#define RCN_LOFIGCACHE      20021106
#define RCN_LOFIGCACHE_FREE 20021107
/* node type for lonode ptype */
#define RCN_LOCON             ((long) 19981201)
#define RCN_REPORTED          ((long) 19981301)

/* flags for lonode */

#define RCN_FLAG_ONE     ((unsigned char) 0x00000001) /* Terminal node        */
#define RCN_FLAG_TWO     ((unsigned char) 0x00000002) /* two wires connected  */
#define RCN_FLAG_CROSS   ((unsigned char) 0x00000004) /* many wires connected */
#define RCN_FLAG_LOCON   ((unsigned char) 0x00000008) /* the node is a locon  */
#define RCN_FLAG_PASS    ((unsigned char) 0x00000010) /* yet treated          */
#define RCN_FLAG_PASS2   ((unsigned char) 0x00000020) /* yet treated          */

#define RCN_FLAG_TRUE    ((unsigned char) 0x0000000F) /* true node            */

/* flags for lowire */

#define RCN_FLAG_PASS    ((unsigned char) 0x00000010) /* yet treated          */
#define RCN_FLAG_DEL     ((unsigned char) 0x00000020) /* not a valid wire     */
#define RCN_FLAG_IGNORE  ((unsigned char) 0x00000040) /* wire to ignore       */

/* State of a losig */
#define MBK_RC_A ((int) 1)
#define MBK_RC_B ((int) 2)
#define MBK_RC_C ((int) 3)
#define MBK_RC_D ((int) 4)
#define MBK_RC_E ((int) 5)

/* macro to use with flags */

#define RCN_SETFLAG(m,b)        (m=(m)|(b))
#define RCN_CLEARFLAG(m,b)      (m=(m)&(~b))
#define RCN_GETFLAG(m,b)        ((m)&(b))

/* Macro to get the node table */
#define RCN_LONODE              15543
#define RCN_SIZETAB		15544
#define RCN_CACHELINE           15545

/* Ptype in lofig for order of physical order of locon on figure */
#define PH_INTERF 19980318
#define PH_REAL_INTERF 20060119

/* Ptype in locon for name of physical connector (spice parser) */
#define PNODENAME 19981103

/* Ptype in lonode for retreive the component connexe */
#define CONNEXE 19990808

/* Ptype in loctc to identify characterization capas */
#define CHARAC_CAPA_PTYPE 20021218

/* Ptype in lowire to retreive list of parallel lowire */
#define RCNPARA 20040311

/* Flags for field RCNCACHE of losig */
#define RCNCACHEFLAG_WF         0x01  /* Write forbidden                      */
#define RCNCACHEFLAG_LOAD       0x02  /* Signal loaded                        */
#define RCNCACHEFLAG_DISABLE    0x04  /* Cache disable                        */
#define RCNCACHEFLAG_NETCAPAOK  0x08  /* Wire & CTC capa included in RCCAPA   */
#define RCNCACHEFLAG_NOPARA     0x10  /* Net doesn't contain parallel wire    */

/*******************************************************************************
* interconnect network  structures                                             *
*******************************************************************************/

typedef struct lorcnet                        /* logical rc network           */
{
float              RCCAPA;                    /* total capacitance            */
struct lowire     *PWIRE;                     /* wire list                    */
long               NBNODE;                    /* number of lonode             */
chain_list        *PCTC;                      /* cross talk capacitance       */
ptype_list        *USER;                      /* Application specific         */
}
lorcnet_list;

typedef struct lowire                         /* logical wire                 */
{
struct lowire           *NEXT;          /* next wire                          */
long                     NODE1;         /* interconnect network node1         */
long                     NODE2;         /* interconnect network node2         */
unsigned char            FLAG;          /* flags                              */
float                    RESI;          /* wire resistance                    */
float                    CAPA;          /* wire capacitance                   */
ptype_list              *USER;          /* Application specific               */
}
lowire_list;

typedef struct lonode                   /* logical node                       */
{
struct chain            *WIRELIST;      /* wire list                          */
struct chain            *CTCLIST;       /* cross talk capacitance list        */
long                     FLAG;          /* Flags                              */
long                     INDEX;         /* node index                         */
ptype_list              *USER;          /* wire list                          */
}
lonode_list;

typedef struct loctc                           /* cross talk capacitance      */
{
struct losig  *SIG1;                           /* First losig                 */
struct losig  *SIG2;                           /* Second losig                */
long           NODE1;                          /* first lonode                */
long           NODE2;                          /* first lonode                */
chain_list    *PREV1;
chain_list    *PREV2;
float          CAPA;                           /* cross talk capacitance      */
ptype_list    *USER;                           /* application specific        */
} loctc_list;

#define RCN_SIZEOFLOWIRE (sizeof(lowire_list))
#define RCN_SIZEOFLOCTC  (sizeof(loctc_list)+2*sizeof(chain_list))

#define rcn_ctcothersig(ctc,sig) ((ctc)->SIG1==(sig)?(ctc)->SIG2:(ctc)->SIG1)
#define rcn_ctcothernode(ctc,sig) ((ctc)->SIG1==(sig)?(ctc)->NODE2:(ctc)->NODE1)
#define rcn_ctcnode(ctc,sig) ((ctc)->SIG1==(sig)?(ctc)->NODE1:(ctc)->NODE2)
#define rcn_ctcnode_set(ctc,sig,val) if ((ctc)->SIG1==(sig)) (ctc)->NODE1=val; else (ctc)->NODE2=val;

/* Valeur de retour de rcn_treetrip() */

#define RCNTREETRIPOK     1
#define RCNTREETRIPUSERKO 2
#define RCNTREETRIPLOOP   3

#define CAPA_IS_DIODE 666007

/* Valeur de retour de rcn_tripconnexe() */

#define RCNTRIPCONNEXE_OK     1
#define RCNTRIPCONNEXE_USERKO 2
