/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_drivers.h                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

biabl_list *yagExtractBusDrivers(pCircuit busCct, cone_list *buscone, chain_list *writesupport, pNode conflict, pNode write);
biabl_list *yagExtractLatchDrivers(pCircuit latchCct, cone_list *latchcone, chain_list *writesupport, pNode conflict, pNode hz, pNode write, chain_list *looplist);
