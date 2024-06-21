
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_toolbug.c						*/
/* date		: Sep  3 1993						*/
/* version	: v106							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include AVT_H

/* ###--------------------------------------------------------------### */
/* function	: beh_toolbug						*/
/* description	: print an error message on the standard error output	*/
/* called func.	: none							*/
/* ###--------------------------------------------------------------### */

void beh_toolbug (code, str1, str2, nbr1)

int   code;
char *str1;
char *str2;
int   nbr1;

{
  char     buf[16];

  sprintf(buf,"%d",code+100);
  switch (code)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 10:
    case 16 :
    case 100 :
    case 103 :
         avt_errmsg(BEH_ERRMSG,buf,AVT_FATAL,str1);
         break;
    case 19:
    case 20:
    case 101 :
         avt_errmsg(BEH_ERRMSG,buf,AVT_FATAL,str1,str2);
         break;
    case 102 :
    case 15 :
         avt_errmsg(BEH_ERRMSG,buf,AVT_FATAL,str1,nbr1);
         break;
    case 199:
    default:
         avt_errmsg(BEH_ERRMSG,"199",AVT_FATAL,str1);
  }
}
