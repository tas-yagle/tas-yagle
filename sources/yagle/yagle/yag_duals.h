/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_duals.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

cone_list       *yagMakeConeList __P((inffig_list *ifl, lofig_list *pt_mbkfig, cnsfig_list *pt_cnsfig));
chain_list      *yagTryDual __P((losig_list *ptsig, branch_group *ptbranches, int *ptpartial));
