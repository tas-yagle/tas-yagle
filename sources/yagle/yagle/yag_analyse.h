/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_analyse.h                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

jmp_buf        *yagAbandonAnalyse(void);
int             yagCheckAddTransLink(branch_list *ptbranch, lotrs_list *pttrans);
int             yagRemoveFalseBranches(cone_list *ptcone);
pNode           yagCalcBranchConductance(branch_list *ptbranch);
int             yagDetectDualGlitcherBranches(cone_list *ptcone);
int             yagCheckAddExtLink(cone_list *ptcone, locon_list *ptcon, cone_list *ptrootcone);
graph          *yagMakeConeGraph(cone_list *ptcone);
void            yagDeleteConeGraph(graph *ptgraph);
chain_list     *yagGuessMutex(cnsfig_list *ptcnsfig);
void            yagAnalyseCone(cone_list *ptcone);
int             yagCheckLocalDuality(cone_list *ptcone);
void            yagDetectNonConflictualFeedback(cone_list *ptcone);
pNode           yagApplyConstraints(pNode expr, jmp_buf *ptabandon_env);
void            yagDetectPrecharge(cone_list *ptcone, int fIgnoreFalse);
void            yagDetectRedundant(cone_list *ptcone);
int             yagDetectDualGlitcherBranches(cone_list *ptcone);
