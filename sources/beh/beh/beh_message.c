
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_message.c						*/
/* date		: Oct 30 1995						*/
/* version	: v109							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>

/* ###--------------------------------------------------------------### */
/* function	: beh_message						*/
/* description	: print a message on the standard output		*/
/* called func.	: none							*/
/* ###--------------------------------------------------------------### */

void beh_message (code, str1)

int   code;
char *str1;

  {
  (void) fprintf (stdout, "BEH : ");

  switch (code)
    {
    case 3:
      (void) fprintf (stdout, "Compiling `%s` (Behaviour) ...\n", str1);
      break;
    case 13:
      (void) fprintf (stdout, "Saving '%s' in a vhdl file (vbe)\n", str1);
      break;
    case 14:
      (void) fprintf (stdout, "Builting '%s' ...\n", str1);
      break;
    default:
      (void) fprintf (stdout, "%s\n", str1);
    }
  }
