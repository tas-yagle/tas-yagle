
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbepor.c					*/
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
/* function	: beh_delbepor						*/
/* description	: delete a BEPOR structure and return the pointer of	*/
/*		  the next object.					*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

struct bepor *beh_delbepor (listbepor, ptbepor)

struct bepor *listbepor;	/* list of bepor containing the object	*/
struct bepor *ptbepor;		/* pointer of the BEPOR to be deleted	*/

  {
  struct bepor  headpor;
  struct bepor *ptlastpor;

  if ((listbepor != NULL) && (ptbepor != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headpor.NEXT = listbepor;
    ptlastpor    = &headpor;
    while ((ptlastpor != NULL) && (ptlastpor->NEXT != ptbepor))
      ptlastpor = ptlastpor->NEXT;

    if (ptlastpor != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/* ###------------------------------------------------------### */

      ptlastpor->NEXT = ptbepor->NEXT;
      mbkfree (ptbepor);
      }

    listbepor = headpor.NEXT;
    }

  return(listbepor);
  }
