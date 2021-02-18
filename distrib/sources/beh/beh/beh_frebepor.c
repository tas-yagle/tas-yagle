
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebepor.c					*/
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
/* function	: beh_frebepor						*/
/* description	: delete a list of BEAUX structures and all objects	*/
/*		  pointed by any os BEAUXs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebepor (listbepor)

struct bepor *listbepor;	/* list of bepor to be deleted		*/

  {
  struct bepor *ptbepor;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, delete the object itself	*/
	/* ###------------------------------------------------------### */

  while (listbepor != NULL)
    {
    ptbepor   = listbepor;
    listbepor = listbepor->NEXT;
    mbkfree (ptbepor);
    }

  }
