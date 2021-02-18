/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_fit.h                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"

#include MUT_H
#include MCC_H
#include STM_H
#include TTV_H

extern long MCC_TASUD ;
extern long MCC_TASDU ;
extern long MCC_TASDD ;
extern long MCC_TASUU ;
extern long MCC_TASSUD ;
extern long MCC_TASSDU ;
extern long MCC_TASSDD ;
extern long MCC_TASSUU ;

extern int mcc_getdelay                  __P((char *,char *, char *,int,char ,char,FILE *)) ;
extern void mcc_runtas                   __P((char *,int ,int)) ;
extern void mcc_fit                      __P((elp_lotrs_param *,elp_lotrs_param *,int cmponly)) ;
