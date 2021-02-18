/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_curv.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"

extern void mcc_printplot                __P((int, int, float, float, mcc_modellist*, mcc_modellist*)) ;          
extern void mcc_technoParameters         __P((elp_lotrs_param*,elp_lotrs_param*)) ;          
extern void mcc_visu                     __P(()) ;          
extern void mcc_drvmtfile                __P((FILE *)) ;
extern void mcc_gencurv                  __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
char* mcc_getmccname( mcc_modellist *model );
