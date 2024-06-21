
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbeout.c					*/
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
/* function	: beh_rmvbeout						*/
/* description	: delete a BEOUT structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, freeExpr					*/
/* ###--------------------------------------------------------------### */

beout_list *beh_rmvbeout (listbeout, beoutname, mode)

beout_list *listbeout;	/* list of beout containing the object	*/
char         *beoutname;	/* name of the BEOUT to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct beout  headout;
  struct beout *ptlastout;
  struct beout *ptbeout;

  if (listbeout != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headout.NEXT = listbeout;
    headout.NAME = NULL     ;
    headout.ABL  = NULL     ;
    ptbeout      = &headout;
    while ((ptbeout != NULL) && (ptbeout->NAME != beoutname))
      {
      ptlastout = ptbeout;
      ptbeout   = ptbeout->NEXT;
      }

    if (ptbeout != NULL)
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
          beh_warning(307,beoutname,NULL);
          //(stderr,"BEH_warning : beout `%s` not empty\n", beoutname);
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
