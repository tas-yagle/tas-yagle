/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_error.c                                                 */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "cbh_error.h"
#include AVT_H

int cbh_error(msg,code)
char    *msg;
int      code;
{
  //fprintf(stderr,"[CBH_WAR] %s",msg);
  switch (code)
  {
    case 0  : 
         avt_errmsg(CBH_ERRMSG,"001",AVT_WARNING,msg); break;
         // fprintf(stderr,": possible cause library not charged\n"); break;
    default :
         avt_errmsg(CBH_ERRMSG,"003",AVT_WARNING,msg);
         // fprintf(stderr,": unknown cause\n");
  }
  return 1;
}
