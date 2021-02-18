
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebeaux.c					*/
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
/* function	: beh_frebeaux						*/
/* description	: delete a list of BEAUX structures and all objects	*/
/*		  pointed by any os BEAUXs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebeaux (listbeaux)

struct beaux *listbeaux;	/* list of beaux to be deleted		*/

  {
  struct beaux *ptbeaux;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbeaux != NULL)
    {
    freeExpr (listbeaux->ABL);

    ptbeaux   = listbeaux;
    listbeaux = listbeaux->NEXT;
    mbkfree (ptbeaux);
    }

  }
