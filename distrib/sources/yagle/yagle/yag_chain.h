/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_chain.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagChainTrans(lotrs_list *pttrans);
void            yagChainDual(cone_list *ptcone);
void            yagChainBranch(cone_list *ptcone, branch_list *ptbranch);
void            yagChainCone(cone_list *ptcone);
void            yagBuildOutputs(cone_list *ptcone);
ht             *yagGetTransList(inffig_list *ifl, cone_list *ptcone, losig_list *ptsig);
edge_list      *yagGetConeInputs(inffig_list *ifl, cone_list *ptcone);
