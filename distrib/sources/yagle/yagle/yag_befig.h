/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_befig.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagBuildBefig __P((cnsfig_list *ptcnsfig, char *name));
void            yagBuildCoreBefig __P((lofig_list *ptlofig, cnsfig_list *ptcnsfig, chain_list *instances));
void            yagAddBefigBehaviour __P((befig_list *ptbefig, befig_list *ptcircuitbefig, int merge));
biabl_list     *yagMergeBiabl __P((char *name, biabl_list *ptbiabl_orig, biabl_list *ptbiabl_new, int is_mem));
void            yagAddConeBehaviour __P((cone_list *ptcone, befig_list *ptbefig, cone_list *ptlinkcone, mbk_match_rules *keepbus_mr));
void            yagOrientInterface __P((cnsfig_list *ptcnsfig));
void            yagReorientInterface __P((cnsfig_list *ptcnsfig, befig_list *ptcellbefig));
void            yagSimplifyExpr __P((befig_list *ptbefig));
void            yagSimplifyProcesses __P((befig_list *ptbefig));
chain_list     *yagMorganReduce __P((chain_list *expr));
befig_list     *yagSuppressBeaux __P((befig_list *ptbefig));
chain_list     *yagReduceAbl __P((chain_list *expr));
chain_list     *yagReduceRegCndAbl __P((chain_list *expr));
pCircuit        yagBuildCircuitFromExpression(chain_list *expr);
