
/* ###--------------------------------------------------------------### */
/* file		: beh_delbeder.c					*/
/* date		: Oct 25 1995						*/
/* version	: v109							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_delbeder						*/
/* description	: delete a BEDER structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

beder_list *beh_delbeder (listbeder, ptbeder, mode)

beder_list *listbeder;	/* list of beder containing the object	*/
beder_list *ptbeder  ;	/* pointer of the beder to be deleted	*/
char          mode     ;	/* recursive delete or not (Y or N)	*/

  {
  struct beder  headder  ;
  struct beder *ptlastder;

  if ((listbeder != NULL) && (ptbeder != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headder.NEXT = listbeder;
    ptlastder    = &headder;

    while ((ptlastder != NULL) && (ptlastder->NEXT != ptbeder))
      ptlastder = ptlastder->NEXT;

    if (ptlastder != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if ((ptbeder->BDD != NULL) )
          beh_warning(303,NULL,NULL);
         //(stderr,"BEH_warning : beder not empty\n");
        }

      ptlastder->NEXT = ptbeder->NEXT;
      mbkfree (ptbeder);
      }

    listbeder = headder.NEXT;
    }

  return(listbeder);
  }
