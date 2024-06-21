/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_cellbeh.h                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

befig_list     *yagBuildBefigMSSC __P((char *name, losig_list *D_sig, cone_list *D_cone, char *D, char *CK, char *S, char *R, char *NR, char *reg, char *Q, char *QN, int fall_edge, chain_list *conelist));
befig_list     *yagBuildBefigMSDIFF __P((char *name, char *D, char *W, char *reg, char *Q, char *QN, chain_list *conelist));
befig_list     *yagBuildBefigMSSX __P((char *name, char *D, char *CK, char *reg, char *Q, int fall_edge, chain_list *conelist));
befig_list     *yagBuildBefigFFT2 __P((char *name, losig_list *D_sig, cone_list *D_cone, char *D, char *CPN, char *LD, char *RE, char *reg, char *Q, char *QN, chain_list *conelist));
befig_list     *yagBuildBefigFD2R __P((char *name, losig_list *D_sig, cone_list *D_cone, char *D, char *CPN, char *RE, char *reg, char *Q, char *QN, chain_list *conelist));
befig_list     *yagBuildBefigFD2S __P((char *name, losig_list *D_sig, cone_list *D_cone, char *D, char *CPN, char *RE, char *reg, char *Q, char *QN, chain_list *conelist));
