/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_resolve.h                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

pNode yagResolveConflicts __P((pCircuit directCct, pCircuit primaryCct, pNode *conflictBdd, cone_list *ptcone));
pNode yagUnexpandBddCct __P((pCircuit sourceCct, pCircuit destCct, pNode ptbdd));
pNode yagSearchBddList __P((chain_list *ptlist, short index));

