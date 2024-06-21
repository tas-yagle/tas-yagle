/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_matrix.h                                                */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define BUFFER  100 

typedef struct matrice {
     struct matrice *NEXT;
     struct matrice *PREV;
     void           **tab;
 } matrice_list;     

int           fclFindCorrespondingLosig(matrice_list *ptmatrice, int niveau, int connecteur, fcl_label prevcon, int precniveau, int loop, int niveau_donedrainsource);
int           fclFindCorrespondingLotrs(matrice_list *ptmatrice, int niveau, int connecteur, int loop);
int           fclFindCorrespondingLoins(matrice_list *ptmatrice, int niveau, int connecteur, int loop);

chain_list   *fils_sig();

void          tab_ajoute();
matrice_list *tab_del();
matrice_list *tab_init();
void          tab_free();
matrice_list *tab_copy();
chain_list   *fils_sig(); 
void          printmatrice(); 
void          displaymatrice(); 

extern int           niveau_last;

