/****************************************************************************/
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_hierbeh.h                                               */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 01/08/1999     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

void yagSortModeledInstances(lofig_list *ptlofig, chain_list **ptmodeled, chain_list **ptunmodeled);
lofig_list *yagBuildHierarchy(cnsfig_list *ptcnsfig, lofig_list *ptlofig, lofig_list **ptptrootlofig, chain_list *instances);
void yagGuessRootConnectorDirections(lofig_list *ptrootlofig);
