/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : beh_warning.c                                               */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include AVT_H

/****************************************************************************/
/*{{{                    beh_warning()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
unsigned int beh_warning(int code, char *str1, char *str2)
{
  char buf[16];

  sprintf(buf,"%d",code);

  switch (code)
  {
    case 312:
    case 313:
         avt_errmsg(BEH_ERRMSG,buf,AVT_WARNING,str1,str2);
         break;
    case 300:
    case 301:
    case 302:
    case 304:
    case 305:
    case 306:
    case 307:
    case 309:
    case 314:
    case 315:
    case 316:
    case 317:
    case 318:
    case 319:
         avt_errmsg(BEH_ERRMSG,buf,AVT_WARNING,str1);
         break;
    case 303:
    case 308:
    case 310:
    case 311:
    case 320:
         avt_errmsg(BEH_ERRMSG,buf,AVT_WARNING);
         break;

    default:
         avt_errmsg(BEH_ERRMSG,"0",AVT_WARNING);
         break;
  }

  return (1);
}


/*}}}************************************************************************/
