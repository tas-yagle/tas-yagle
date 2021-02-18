/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_latch.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* latch feedback types */

#define FB_UNKNOWN       (long)0
#define FB_INV           ((long) 0x00000001)
#define FB_NAND          ((long) 0x00000002)
#define FB_NOR           ((long) 0x00000003)
#define FB_TRISTATE      ((long) 0x00000004)
#define FB_PASSINV       ((long) 0x00000005)
#define FB_SWITCHINV     ((long) 0x00000006)
#define FB_PASSNAND      ((long) 0x00000007)
#define FB_SWITCHNAND    ((long) 0x00000008)
#define FB_PASSNOR       ((long) 0x00000009)
#define FB_SWITCHNOR     ((long) 0x0000000a)

/* latch forward cone types */

#define FW_UNKNOWN       (long)0
#define FW_INV           ((long) 0x00000100)
#define FW_NAND          ((long) 0x00000200)
#define FW_NOR           ((long) 0x00000300)

/* Miscellaneous latch types */

#define DIFF             ((long) 0x00100000)
#define DLATCH           ((long) 0x00200000)

/* bistable types */

#define BISTABLE_NAND    ((long) 0x00010000)
#define BISTABLE_NOR     ((long) 0x00020000)

/* pattern matching return types */

#define PM_UNKNOWN       0
#define PM_INV           1
#define PM_NAND          2
#define PM_NOR           3
#define PM_TRISTATE      4
#define PM_PASSINV       5
#define PM_SWITCHINV     6
#define PM_PASSNAND      7
#define PM_SWITCHNAND    8
#define PM_PASSNOR       9
#define PM_SWITCHNOR     10

void            yagMarkLevelHold (cone_list *ptbleedcone, cone_list *ptcone, chain_list *transchain);
int             yagCountConnections(chain_list *loconchain, int only_source_drain);
chain_list     *yagCheckLoop (cone_list *ptcone, int fIgnoreFalse);
void            yagExtractBleeder (inffig_list *ifl, cone_list *ptcone);
void            yagDetectBleeder (cone_list *ptcone);
void            yagRmvThruBleed (cone_list *ptcone);
void            yagMarkLoopConf (cone_list *ptcone);
void            yagMatchLatch (cone_list *ptcone);
void            yagMatchSimpleLatch (cone_list *ptcone);
void            yagPairMemsym (inffig_list *ifl, cone_list *ptcone);
int             yagMatchNAND (cone_list *ptcone, cone_list *ptincone);
int             yagMatchNOR (cone_list *ptcone, cone_list *ptincone);
void            yagSelectRSLatch (cone_list *ptcone0, cone_list *ptcone1);
int             yagMatchNOT (cone_list *ptcone, cone_list *ptincone);
float           yagInverterStrength (cone_list *ptcone0, cone_list *ptcone1);
int             yagMatchInversion (cone_list *ptcone, cone_list *ptincone, float *ptstrength, chain_list **pttranslist);
int             yagMatchSwitchedInversion (cone_list *ptcone, cone_list *ptincone, chain_list **pttranslist);
int             yagIsInverse(cone_list *ptcone, cone_list *ptincone);
void            yagRmvThruLatch (cone_list *ptcone);

