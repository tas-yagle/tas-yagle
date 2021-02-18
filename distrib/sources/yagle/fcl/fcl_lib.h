/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.10                                                  */
/*    Fichier : fcl110.h                                                    */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#ifndef FCL
#define FCL

#include CNS_H

/* Transferred signal types */

#define FCL_LATCH            ((long) 0x80000000 )
#define FCL_MEMSYM           ((long) 0x40000000 )
#define FCL_RS               ((long) 0x20000000 )
#define FCL_FLIPFLOP         ((long) 0x10000000 )
#define FCL_MASTER           ((long) 0x08000000 )
#define FCL_SLAVE            ((long) 0x04000000 )
#define FCL_STOP             ((long) 0x02000000 )
#define FCL_BYPASS           ((long) 0x01000000 )
#define FCL_BLOCKER          ((long) 0x00000001 )
#define FCL_NEVER            ((long) 0x00000002 )
#define FCL_CELLOUT          ((long) 0x00000004 )
#define FCL_NOFALSEBRANCH    ((long) 0x00000008 )
#define FCL_VDD              ((long) 0x00000010 )
#define FCL_VSS              ((long) 0x00000020 )
#define FCL_MATCHNAME        ((long) 0x00000040 )
#define FCL_SENSITIVE        ((long) 0x00000080 )
#define FCL_NOTLATCH         ((long) 0x00000100 )

/* Transferred transistor types */

#define FCL_BLEEDER          ((long) 0x80000000 )
#define FCL_FEEDBACK         ((long) 0x40000000 )
#define FCL_COMMAND          ((long) 0x20000000 )
#define FCL_NOT_FUNCTIONAL   ((long) 0x10000000 )
#define FCL_BLOCKER          ((long) 0x00000001 )
#define FCL_NEVER            ((long) 0x00000002 )
#define FCL_NOSHARE          ((long) 0x00000080 )
#define FCL_MATCHSIZE        ((long) 0x00000040 )
#define FCL_SHARE_TRANS      ((long) 0x00000020 )
#define FCL_SHORT            ((long) 0x00000010 )
#define FCL_USED             ((long) 0x00000100 )

/* Transferred instance types */
#define FCL_SHARE_INS        ((long) 0x00000020 )

/* for the signal USER field */

#define FCL_TRANSFER_PTYPE     ((long) 0x40000001 )
#define FCL_FORCECONE_PTYPE    ((long) 0x40000002 )
#define FCL_CELL_PTYPE         ((long) 0x40000003 )
#define FCL_LOCON_PTYPE        ((long) 0x40000004 )
#define FCL_LABEL_PTYPE        ((long) 0x40000005 )
#define FCL_MATCH_PTYPE        ((long) 0x40000006 )
#define FCL_CORRESP_PTYPE      ((long) 0x40000007 )
#define FCL_COUP_LIST_PTYPE    ((long) 0x40000017 )

/* for the transistor USER field */

#define FCL_TRANSFER_PTYPE     ((long) 0x40000001 )
#define FCL_LOINS_PTYPE        ((long) 0x40000007 )
#define FCL_MARK_PTYPE         ((long) 0x40000008 )
#define FCL_MATCHSIZE_PTYPE    ((long) 0x40000009 )
#define FCL_LABEL_PTYPE        ((long) 0x40000005 )
#define FCL_MATCH_PTYPE        ((long) 0x40000006 )
#define FCL_CORRESP_PTYPE      ((long) 0x40000007 )

/* for the loins USER field */

#define FCL_TRANSFER_PTYPE     ((long) 0x40000001 )
#define FCL_MARK_PTYPE         ((long) 0x40000008 )
#define FCL_TRANSLIST_PTYPE    ((long) 0x4000000a )
#define FCL_INSLIST_PTYPE      ((long) 0x40000016 )

/* for the lofig USER field */

#define FCL_LOCON_PTYPE        ((long) 0x40000004 )
#define FCL_CMPUP_PTYPE        ((long) 0x4000000c )
#define FCL_CMPDN_PTYPE        ((long) 0x4000000d )
#define FCL_MUXUP_PTYPE        ((long) 0x4000000e )
#define FCL_MUXDN_PTYPE        ((long) 0x4000000f )
#define FCL_NETOUTPUT_PTYPE    ((long) 0x40000010 )
#define FCL_MODEL_PTYPE        ((long) 0x40000011 )
#define FCL_NUMTRANS_PTYPE     ((long) 0x40000012 )
#define FCL_MARK_PTYPE         ((long) 0x40000008 )

/* for the locon USER field */
#define FCL_REAL_CORRESP_PTYPE ((long) 0x40000013 )
#define FCL_WEIGHT_PTYPE       ((long) 0x40000014 )
#define FCL_COUPLING_PTYPE     ((long) 0x40000015 )

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Type mark constants */

#define NUMNETTYPES 16
#define NUMTRANSTYPES 10

#define FCL_TRANSTYPE 2
#define FCL_NETTYPE   4
#define FCL_NETOUTPUT 5
#define FCL_CMPUP     8
#define FCL_CMPDN     9
#define FCL_MUXUP     10
#define FCL_MUXDN     11

/* data Structures */

typedef struct fclcorresp {
    struct fclcorresp *NEXT;
    char            TYPE;
    void           *ORIG;
    void           *CORRESP;
} fclcorresp_list;

/* Arrays for type name to index conversion */

extern char *nettype_names[NUMNETTYPES];
extern long  nettype_values[NUMNETTYPES];
extern char *transtype_names[NUMTRANSTYPES];
extern long  transtype_values[NUMTRANSTYPES];

/* Global variables */

extern FILE    *FCL_OUTPUT;

extern short    FCL_FILE;
extern short    FCL_CUT;

extern int      FCL_SIZE_TOLERANCE;

extern short    FCL_TRACE_LEVEL;

extern short    FCL_DISPLAY_MATRIX;

extern char    *FCL_LIB_NAME;
extern char    *FCL_LIB_PATH;

extern chain_list *FCL_ANY_NMOS;
extern chain_list *FCL_ANY_PMOS;

extern num_list   *FCL_ANY_NMOS_IDX;
extern num_list   *FCL_ANY_PMOS_IDX;

extern cell_list  *FCL_CELL_LIST;
extern chain_list *FCL_INSTANCE_LIST;
extern chain_list *FCL_INSTANCE_BEFIGS;
extern locon_list *FCL_LOCON_LIST;

extern int      FCL_BUILD_CELLS;
extern int      FCL_BUILD_INSTANCES;
extern int      FCL_BUILD_CORRESP;
extern int      FCL_NEED_ZERO;
extern int      FCL_NEED_ONE;

extern ht      *FCL_REAL_CORRESP_HT;

extern fclcorresp_list *FCL_REAL_CORRESP_LIST;

cell_list       *fclFindCells();
chain_list      *fclFindInstances();
fclcorresp_list *fclFindCorrespondance();
void             fclMarkInstances();
void             fclFind();
befig_list      *fclGetBefig();
void             fclenv();
void             fclMarkTrans();
void             fclUnmarkTrans();
void             fclCleanShareMarks();
void             fclCleanTransferMarks();
void             fclCleanCouplingMarks();
void             fclCleanTransList();
void             fclDeleteInstance();
void             fclFreeCorrespList();
void             fclExtendSpice();
void             fclRmvSpiceExtensions();
void fclUpdateLofigchain(chain_list *inslist, char *figurename);

#define isFclMarked(trans) (getptype(trans->USER, FCL_MARK_PTYPE) != NULL ? TRUE : FALSE)

#endif


