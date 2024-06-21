
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_error.c						*/
/* date		: Oct 30 1995						*/
/* version	: v109							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include AVT_H

/* ###--------------------------------------------------------------### */
/* function	: beh_error						*/
/* description	: print an errorr message				*/
/* called func.	: none							*/
/* ###--------------------------------------------------------------### */

unsigned int beh_error (code, str1)

int   code;
char *str1;

{
  char buf[16];

  sprintf(buf,"%03d",code);

  switch (code)
  {
    case 1:
    case 40:
    case 41:
    case 68:
    case 69:
    case 100:
         avt_errmsg(BEH_ERRMSG,buf,AVT_ERROR,str1);
         break;
    case 2:
    case 4:
    case 5:
    case 6:
    case 70:
    case 107:
         avt_errmsg(BEH_ERRMSG,buf,AVT_ERROR);
         break;

    default:
         avt_errmsg(BEH_ERRMSG,"0",AVT_ERROR);
    case 3: // do nothing
         break;
  }

  return (1);
}
