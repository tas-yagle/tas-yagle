
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvberin.c					*/
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
/* function	: beh_rmvberin						*/
/* description	: delete a BERIN structure and return the pointer of	*/
/*		  the next object.					*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

struct berin *beh_rmvberin (listberin, berinname)

struct berin *listberin;	/* list of berin containing the object	*/
char         *berinname;	/* name of the BERIN to be deleted	*/

  {
  struct berin  headrin;
  struct berin *ptlastrin;
  struct berin *ptberin;

  if (listberin != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headrin.NEXT    = listberin;
    headrin.NAME    = NULL;
    headrin.OUT_REF = NULL;
    headrin.MSG_REF = NULL;
    headrin.AUX_REF = NULL;
    headrin.BUX_REF = NULL;
    headrin.BUS_REF = NULL;
    headrin.REG_REF = NULL;
    headrin.DLY_REF = NULL;
    ptberin      = &headrin;
    while ((ptberin != NULL) && (ptberin->NAME != berinname))
      {
      ptlastrin = ptberin;
      ptberin   = ptberin->NEXT;
      }

    if (ptberin != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/* ###------------------------------------------------------### */

      freechain (ptberin->OUT_REF);
      freechain (ptberin->MSG_REF);
      freechain (ptberin->AUX_REF);
      freechain (ptberin->BUX_REF);
      freechain (ptberin->BUS_REF);
      freechain (ptberin->REG_REF);
      freechain (ptberin->DLY_REF);

      ptlastrin->NEXT = ptberin->NEXT;
      mbkfree (ptberin);
      }

    listberin = headrin.NEXT;
    }

  return(listberin);
  }
