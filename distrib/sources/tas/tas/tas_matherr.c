/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Verison 5                                               */
/*    Fichier : tas_matherr.c                                               */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* recuperation des erreurs mathematiques au lieu du systeme                */
/****************************************************************************/

#include "tas.h"

/****************************************************************************/
/*                          fonction matherr()                              */
/* pour debugger les calculs mathematiques.                                 */
/****************************************************************************/
int matherr(x)
register struct exception *x;

{
tas_error(17,(char *)x,TAS_ERROR) ;
return(0);
}
