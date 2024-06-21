/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_MBK_utils.h                                             */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 07/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/


/*******************************************************************************
* function addloins()                                                          *
*******************************************************************************/
extern loins_list *gen_addloins(lofig_list *ptfig, char *insname, lofig_list *ptnewfig, chain_list *sigchain, chain_list *sigtreechain);

/*******************************************************************************
* function Dump_loinscon()                                                      *
*******************************************************************************/
extern void Dump_loinscon(locon_list  *ptcon);

/*******************************************************************************
* function Dump_lofigcon()                                                      *
*******************************************************************************/
extern void Dump_lofigcon(locon_list  *ptcon);

/*******************************************************************************
* function Dump_losig()                                                         *
*******************************************************************************/
extern void Dump_losig(losig_list  *ptsig);

/*******************************************************************************
* function Dump_loins()                                                         *
*******************************************************************************/
extern void Dump_loins(loins_list  *ptins);

/*******************************************************************************
* function Dump_lotrs()                                                         *
*******************************************************************************/
extern void Dump_lotrs(lotrs_list  *pttrs);


