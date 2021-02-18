/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void fclExit();
fcl_label CountChain();
fcl_label getlabeltrs();
fcl_label getlabellocon();
fcl_label getlabelsig();
fcl_label getlabelsigtrs();
void fclCorruptLotrs();
void fclCorruptLosig();
lotrs_list *whichtrs();
void fclPrintTime();
void fclChrono();
