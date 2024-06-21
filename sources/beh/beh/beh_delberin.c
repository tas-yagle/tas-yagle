
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delberin.c					*/
/* date		: Sep 20 1994						*/
/* version	: v107							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_delberin						*/
/* description	: delete a BERIN structure and return the pointer of	*/
/*		  the next object.					*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

struct berin *beh_delberin (listberin, ptberin)

struct berin *listberin;	/* list of berin containing the object	*/
struct berin *ptberin;		/* pointer of the BERIN to be deleted	*/

{
  struct berin  headrin;
  struct berin *ptlastrin;

  if ((listberin != NULL) && (ptberin != NULL))
  {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headrin.NEXT = listberin;
    ptlastrin    = &headrin;
    while ((ptlastrin != NULL) && (ptlastrin->NEXT != ptberin))
      ptlastrin = ptlastrin->NEXT;
    
    if (ptlastrin != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/* ###------------------------------------------------------### */

      freechain(ptberin->OUT_REF);
      freechain(ptberin->OUT_VEC);
      freechain(ptberin->BUS_REF);
      freechain(ptberin->BUS_VEC);
      freechain(ptberin->AUX_REF);
      freechain(ptberin->AUX_VEC);
      freechain(ptberin->REG_REF);
      freechain(ptberin->REG_VEC);
      freechain(ptberin->BUX_REF);
      freechain(ptberin->BUX_VEC);
      freechain(ptberin->MSG_REF);
      freechain(ptberin->DLY_REF);

      ptlastrin->NEXT = ptberin->NEXT;
      mbkfree (ptberin);
    }

    listberin = headrin.NEXT;
  }

  return(listberin);
}
