/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.10                                                */
/*    Fichier : yag_split.h                                                 */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire LIP6 equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 01/08/1999     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

lofig_list     *yagCutLofig (inffig_list *ifl, lofig_list *ptlofig, chain_list *instances, lofig_list **ptpttopfig, int copytrans);
lofig_list     *yagBuildGeniusBehHierarchy(lofig_list *pttoplofig, lofig_list *ptcorelofig, chain_list *instances, char *name);

