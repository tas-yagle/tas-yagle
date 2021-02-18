
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbegen.c					*/
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
/* function	: beh_delbegen						*/
/* description	: delete a BEGEN structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

begen_list *beh_delbegen (listbegen, ptbegen, mode)

begen_list *listbegen;	/* list of begen containing the object	*/
begen_list *ptbegen;		/* pointer of the BEGEN to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct begen  headgen;
  struct begen *ptlastgen;

  if ((listbegen != NULL) && (ptbegen != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headgen.NEXT = listbegen;
    ptlastgen    = &headgen;
    while ((ptlastgen != NULL) && (ptlastgen->NEXT != ptbegen))
      ptlastgen = ptlastgen->NEXT;

    if (ptlastgen != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if (ptbegen->VALUE != NULL)
          beh_warning(305,ptbegen->NAME,NULL);
          //(stderr,"BEH_warning : begen `%s` not empty\n", ptbegen->NAME);
        }
      else
        mbkfree (ptbegen->VALUE);

      ptlastgen->NEXT = ptbegen->NEXT;
      mbkfree (ptbegen);
      }

    listbegen = headgen.NEXT;
    }

  return(listbegen);
  }
