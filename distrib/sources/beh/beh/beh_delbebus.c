
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbebus.c					*/
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
/* function	: beh_delbebus						*/
/* description	: delete a BEBUS structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, beh_frebiabl, beh_frebinode			*/
/* ###--------------------------------------------------------------### */

bebus_list *beh_delbebus (listbebus, ptbebus, mode)

bebus_list *listbebus;	/* list of bebus containing the object	*/
bebus_list *ptbebus;		/* pointer of the BEBUS to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct bebus  headbus;
  struct bebus *ptlastbus;

  if ((listbebus != NULL) && (ptbebus != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headbus.NEXT = listbebus;
    ptlastbus    = &headbus;
    while ((ptlastbus != NULL) && (ptlastbus->NEXT != ptbebus))
      ptlastbus = ptlastbus->NEXT;

    if (ptlastbus != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if ((ptbebus->BIABL != NULL) || (ptbebus->BINODE != NULL))
          beh_warning(301,ptbebus->NAME,NULL);
        //(stderr,"BEH_warning : bebus `%s` not empty\n", ptbebus->NAME);
        }
      else
        {
        beh_frebiabl  (ptbebus->BIABL);
        beh_frebinode (ptbebus->BINODE);
        }

      ptlastbus->NEXT = ptbebus->NEXT;
      mbkfree (ptbebus);
      }

    listbebus = headbus.NEXT;
    }

  return(listbebus);
  }
