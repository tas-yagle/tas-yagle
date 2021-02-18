/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_main.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* declaration de variable */
extern lofig_list *TAS_LOFIG;
extern cnsfig_list *TAS_CNSFIG;
/* declaration de fonction */
extern ttvfig_list* tas_main __P(( void )) ;
extern ttvfig_list* tas_core __P(( int keeplofig )) ;
void tas_displaymemoryusage __P(( cnsfig_list *cnsfig ));
