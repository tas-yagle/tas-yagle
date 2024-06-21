/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_alloc.h                                                 */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* declaration de variable */
extern chain_list *HEAD_DELAY ;
extern chain_list *HEAD_FRONT ;
extern chain_list *HEAD_CARACCON ;
extern chain_list *HEAD_CARACLINK ;

/* declaration de fonction */
extern void tas_initcnsfigalloc __P(( cnsfig_list* ));
extern void tas_freecnsfigalloc __P(( cnsfig_list* ));
extern void tas_freecnsfig      __P(( cnsfig_list* ));
extern void tas_freelofig       __P(( lofig_list*  ));
