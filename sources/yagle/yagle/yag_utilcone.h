/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_utilcone.h                                              */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void            yagResetCone (cone_list *cone);
void            yagCleanCone (cone_list *cone);
cone_list      *yagMakeCone (cone_list *headcone, losig_list *ptsig, branch_group *ptbranches, long type, long tectype);
cone_list      *yagMakeSupplyCone (cone_list *headcone, losig_list *ptsig);
cone_list      *yagMakeInCone (cone_list *headcone, losig_list *ptsig, long type, long tectype);
cone_list      *yagMakeOutCone (cone_list *headcone, losig_list *ptsig, branch_group *ptbranches, long type, long tectype);
cone_list      *yagMakeInoutCone (cone_list *headcone, losig_list *ptsig, branch_group *ptbranches, long type, long tectype);
int             yagCheckBranches (cone_list *ptcone);
int             yagCheckExtOut (cone_list *ptcone);
void            yagCheckExtIn (cone_list *ptcone);
int             yagIsOutput (cone_list *ptcone);
void            yagRmvFalseBrext (cone_list *ptcone);
float           yagGetConeCapa (cone_list *ptcone, lofig_list *ptlofig);
int             yagCheckUniqueConnection(cone_list *ptincone, cone_list *ptcone);
