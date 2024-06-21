/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* dynamic table management functions */

typedef struct table {
    struct table  *NEXT;
    int     SIZE;
    int     ALLOC;
    void    **DATA;
} table;

table          *newtable (void);
void            addtableitem (table *pttable, void *item);
void            deltable (table *pttable);
void            yag_freetable (void);

/* treatment of _s convention for Bull */

long            yagGetVal_s (losig_list *ptsig);
void            yagAddVal_s (inffig_list *ifl, lofig_list *ptfig);
long            yagTestVal_s (char *name, char c, inffig_list *ifl, chain_list *alldirout);

/* Bdd encapsulation */

pNode           yagAblToBddCct (pCircuit pC, chain_list *expr, jmp_buf abandon_env);
pNode           yagApplyBdd (short oper, chain_list *pt, jmp_buf abandon_env);
pNode           yagApplyBinBdd (short oper, pNode pBdd1, pNode pBddGc, jmp_buf abandon_env);
pNode           yagNotBdd (pNode pBdd, jmp_buf abandon_env);
pNode           yagConstraintBdd (pNode pBdd1, pNode pBddGc, jmp_buf abandon_env);
pNode           yagSimplestBdd (pNode pBdd1, pNode pBdd2);
int             yagBddSupportContained (pNode pBdd, pNode pBddGc);
chain_list     *yagCheckConstraint (chain_list *refsupport, chain_list *constraintsupport, chain_list *constraint, ht **FASTSEARCH);

/* inf signal & transistor markings */

void            yagInfMarkTrans(inffig_list *ifl, lofig_list *ptlofig, chain_list *ptfulllist);
void            yagInfMarkSig(inffig_list *ifl, lofig_list *ptlofig, chain_list *ptfulllist);

void            yagEndAddCheckDirective(void);
int             yagAddCheckDirective(inffig_list *ifl, char *dataname, char *clockname, int clockstate);
void            yagAddFilterDirective(inffig_list *ifl, char *outname, int outstate);

/* signal handlers */

void            yagTrapKill (int sig);
void            yagTrapCore (int sig);

void yagDriveConeSignalAliases(cnsfig_list *cf, ht *renamed, ht *morealiases);
void yagFreeAliasHT(ht *morealiases);
void yagMarkUnusedTrans(lofig_list *ptlofig);


