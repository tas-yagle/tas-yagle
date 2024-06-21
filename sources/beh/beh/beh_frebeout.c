
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebeout.c					*/
/* date		: Sep  3 1993						*/
/* version	: v106							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_frebeout						*/
/* description	: delete a list of BEOUT structures and all objects	*/
/*		  pointed by any os BEOUTs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebeout (listbeout)

struct beout *listbeout;	/* list of beout to be deleted		*/

  {
  struct beout *ptbeout;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbeout != NULL)
    {
    freeExpr (listbeout->ABL);

    ptbeout   = listbeout;
    listbeout = listbeout->NEXT;
    mbkfree (ptbeout);
    }

  }
