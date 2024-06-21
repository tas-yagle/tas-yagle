/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_cells.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagMatchFlipFlopCells __P((cone_list *headcone, int fBuildBefig));
void            yagAutoFlipFlop(cone_list *headcone, int fBuildBefig);

befig_list     *yagMakeLatchBehaviour __P((cone_list *ptcone, int fUseSigNames));
befig_list     *yagBuildBefigSimpleLatch __P((char *figname, chain_list *datasiglist, chain_list *conditionlist, cone_list *latchcone, char *reg, char *Q, int fUseSigNames));
