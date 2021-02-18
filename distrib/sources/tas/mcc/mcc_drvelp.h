/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_drvelp.h                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"

extern void mcc_addmodele                 __P((int,int,elp_lotrs_param*,elp_lotrs_param*,
                                               elpmodel_list**,elpmodel_list**)) ;
extern void mcc_drvelp                    __P((int, int,elp_lotrs_param*,elp_lotrs_param*,int)) ;
