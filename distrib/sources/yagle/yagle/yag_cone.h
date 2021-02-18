/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_cone.h                                                  */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

long            yagScanCones (inffig_list *ifl, cone_list *headcone);
int             yagExpandCone (inffig_list *ifl, cone_list *ptcone);
cone_list      *yagAddGlueCone (losig_list *ptsig, cone_list *ptcone);
cone_list      *yagMakeGlueCone (cone_list *headgluecone, losig_list *ptsig, cone_list *ptcone);
jmp_buf        *yagAbandonGrow (void);


