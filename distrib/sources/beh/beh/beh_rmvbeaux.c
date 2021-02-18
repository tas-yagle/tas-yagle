
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbeaux.c					*/
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
/* function	: beh_rmvbeaux						*/
/* description	: delete a BEAUX structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, freeExpr					*/
/* ###--------------------------------------------------------------### */

beaux_list *beh_rmvbeaux (listbeaux, beauxname, mode)

beaux_list *listbeaux;	/* list of beaux containing the object	*/
char         *beauxname;	/* name of the BEAUX to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct beaux  headaux;
  struct beaux *ptlastaux;
  struct beaux *ptbeaux;

  if (listbeaux != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headaux.NEXT = listbeaux;
    headaux.NAME = NULL     ;
    headaux.ABL  = NULL     ;
    ptbeaux      = &headaux;
    while ((ptbeaux != NULL) && (ptbeaux->NAME != beauxname))
      {
      ptlastaux = ptbeaux;
      ptbeaux   = ptbeaux->NEXT;
      }

    if (ptbeaux != NULL)
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
          beh_warning(300,beauxname,NULL);
          //(stderr,"BEH_warning : beaux `%s` not empty\n", beauxname);
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
