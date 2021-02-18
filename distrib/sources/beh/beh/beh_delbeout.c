
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbeout.c					*/
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
/* function	: beh_delbeout						*/
/* description	: delete a BEOUT structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, freeExpr					*/
/* ###--------------------------------------------------------------### */

beout_list *beh_delbeout (listbeout, ptbeout, mode)

beout_list *listbeout;	/* list of beout containing the object	*/
beout_list *ptbeout;		/* pointer of the BEOUT to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct beout  headout;
  struct beout *ptlastout;

  if ((listbeout != NULL) && (ptbeout != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headout.NEXT = listbeout;
    ptlastout    = &headout;
    while ((ptlastout != NULL) && (ptlastout->NEXT != ptbeout))
      ptlastout = ptlastout->NEXT;

    if (ptlastout != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if (ptbeout->ABL != NULL)
          beh_warning(307,ptbeout->NAME,NULL);
        //(stderr,"BEH_warning : beout `%s` not empty\n", ptbeout->NAME);
        }
      else
        freeExpr (ptbeout->ABL);

      ptlastout->NEXT = ptbeout->NEXT;
      mbkfree (ptbeout);
      }

    listbeout = headout.NEXT;
    }

  return(listbeout);
  }
