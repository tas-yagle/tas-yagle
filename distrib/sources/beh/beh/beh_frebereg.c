
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebereg.c					*/
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
/* function	: beh_frebereg						*/
/* description	: delete a list of BEAUX structures and all objects	*/
/*		  pointed by any os BEAUXs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebereg (listbereg)

struct bereg *listbereg;	/* list of bereg to be deleted		*/

  {
  struct bereg *ptbereg;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbereg != NULL)
    {
    beh_frebiabl  (listbereg->BIABL);
    beh_frebinode (listbereg->BINODE);

    ptbereg   = listbereg;
    listbereg = listbereg->NEXT;
    mbkfree (ptbereg);
    }

  }
