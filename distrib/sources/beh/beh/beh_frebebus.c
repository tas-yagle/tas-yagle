
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebebus.c					*/
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
/* function	: beh_frebebus						*/
/* description	: delete a list of BEBUS structures and all objects	*/
/*		  pointed by any os BEBUSs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebebus (listbebus)

struct bebus *listbebus;	/* list of bebus to be deleted		*/

  {
  struct bebus *ptbebus;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbebus != NULL)
    {
    beh_frebiabl  (listbebus->BIABL);
    beh_frebinode (listbebus->BINODE);

    ptbebus   = listbebus;
    listbebus = listbebus->NEXT;
    mbkfree (ptbebus);
    }

  }
