
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebinod.c					*/
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
/* function	: beh_frebinode						*/
/* description	: delete a list of BINODE structures and all objects	*/
/*		  pointed by any os BINODEs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebinode (listbinode)

struct binode *listbinode;	/* list of binode to be deleted		*/

  {
  struct binode *ptbinode;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbinode != NULL)
    {
    ptbinode   = listbinode;
    listbinode = listbinode->NEXT;
    mbkfree (ptbinode);
    }

  }
