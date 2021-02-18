
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbebux.c					*/
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
/* function	: beh_delbebux						*/
/* description	: delete a BEBUX structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, beh_frebiabl, beh_frebinode			*/
/* ###--------------------------------------------------------------### */

bebux_list *beh_delbebux (listbebux, ptbebux, mode)

bebux_list *listbebux;	/* list of bebux containing the object	*/
bebux_list *ptbebux;		/* pointer of the BEBUX to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct bebux  headbux;
  struct bebux *ptlastbux;

  if ((listbebux != NULL) && (ptbebux != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headbux.NEXT = listbebux;
    ptlastbux    = &headbux;
    while ((ptlastbux != NULL) && (ptlastbux->NEXT != ptbebux))
      ptlastbux = ptlastbux->NEXT;

    if (ptlastbux != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if ((ptbebux->BIABL != NULL) || (ptbebux->BINODE != NULL))
          beh_warning(302,ptbebux->NAME,NULL);
        //(stderr,"BEH_warning : bebux `%s` not empty\n", ptbebux->NAME);
        }
      else
        {
        beh_frebiabl  (ptbebux->BIABL);
        beh_frebinode (ptbebux->BINODE);
        }

      ptlastbux->NEXT = ptbebux->NEXT;
      mbkfree (ptbebux);
      }

    listbebux = headbux.NEXT;
    }

  return(listbebux);
  }
