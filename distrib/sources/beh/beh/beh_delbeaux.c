
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbeaux.c					*/
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
/* function	: beh_delbeaux						*/
/* description	: delete a BEAUX structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, freeExpr					*/
/* ###--------------------------------------------------------------### */

beaux_list *beh_delbeaux (listbeaux, ptbeaux, mode)

beaux_list *listbeaux;	/* list of beaux containing the object	*/
beaux_list *ptbeaux;		/* pointer of the BEAUX to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct beaux  headaux;
  struct beaux *ptlastaux;

  if ((listbeaux != NULL) && (ptbeaux != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headaux.NEXT = listbeaux;
    ptlastaux    = &headaux;
    while ((ptlastaux != NULL) && (ptlastaux->NEXT != ptbeaux))
      ptlastaux = ptlastaux->NEXT;

    if (ptlastaux != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if (ptbeaux->ABL != NULL)
          beh_warning(300,ptbeaux->NAME,NULL);
          // (stderr,"BEH_warning : beaux `%s` not empty\n", ptbeaux->NAME);
        }
      else
        freeExpr (ptbeaux->ABL);

      ptlastaux->NEXT = ptbeaux->NEXT;
      mbkfree (ptbeaux);
      }

    listbeaux = headaux.NEXT;
    }

  return(listbeaux);
  }
