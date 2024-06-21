
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbepor.c					*/
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
/* function	: beh_rmvbepor						*/
/* description	: delete a BEPOR structure and return the pointer of	*/
/*		  the next object.					*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

struct bepor *beh_rmvbepor (listbepor, beporname)

struct bepor *listbepor;	/* list of bepor containing the object	*/
char         *beporname;	/* name of the BEPOR to be deleted	*/

  {
  struct bepor  headpor;
  struct bepor *ptlastpor;
  struct bepor *ptbepor;

  if (listbepor != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headpor.NEXT = listbepor;
    headpor.NAME = NULL     ;
    ptbepor      = &headpor;
    while ((ptbepor != NULL) && (ptbepor->NAME != beporname))
      {
      ptlastpor = ptbepor;
      ptbepor   = ptbepor->NEXT;
      }

    if (ptbepor != NULL)
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
