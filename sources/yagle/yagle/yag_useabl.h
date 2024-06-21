/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_useabl.h                                                */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

chain_list     *yagExpandExpr __P((gnode_list *ptnode, chain_list *rootexpr, int negate));
chain_list     *yagExpandTerm __P((gnode_list *ptnode, chain_list *headarg));
biabl_list     *yagMakeBiabl __P((abl_pair *constexpr));
biabl_list     *yagMakeLatchExpr __P((cone_list *ptcone));
biabl_list     *yagMakeTristateExpr __P((cone_list *ptcone));
chain_list     *yagMakeDualExpr __P((cone_list *ptcone));
int             yagOneExpr __P((chain_list *expr));
int             yagZeroExpr __P((chain_list *expr));

