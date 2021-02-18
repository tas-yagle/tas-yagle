/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_test.h                                                  */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagViewBefig (befig_list *fig);
void            yagDisplayGraph (graph *ptgraph);
void            yagDisplayUsedGraph (graph *ptgraph, int level);
void            yagDisplayUsedGraphLog (int lib, int loglevel, graph *ptgraph, int level);
int             yagTestGraphCycle (graph *ptgraph);
void            yagCountDualTrans (cnsfig_list *ptcnsfig);
int             yagCountCones (cone_list *headcone);
chain_list     *yagCircuitLoops (cnsfig_list *ptcnsfig);
void            yagSaveCircuitLoops (chain_list *list_loop);
void            yagStatLofig (lofig_list *lofig);
void            yagDisplayBddExprLog (int lib, int loglevel, char *message, pCircuit theCct, pNode bdd);

