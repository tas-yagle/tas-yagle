
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbeder.c					*/
/* date		: Mar 22 1995						*/
/* version	: v100							*/
/* authors	: Laurent VUILLEMIN					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_rmvbeder						*/
/* description	: delete a BERIN structure and return the pointer of	*/
/*		  the next object.					*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

beder_list *beh_rmvbeder (listbeder, bederindex)

beder_list *listbeder;	/* list of beder containing the object	*/
short         bederindex;	/* index of the BEDER to be deleted	*/

  {
  struct beder  headder;
  struct beder *ptlastder;
  struct beder *ptbeder;

  if (listbeder != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headder.NEXT    = listbeder;
    headder.INDEX=0;
    ptbeder      = &headder;
    while ((ptbeder != NULL) && (ptbeder->INDEX	 != bederindex))
      {
      ptlastder = ptbeder;
      ptbeder   = ptbeder->NEXT;
      }

    if (ptbeder != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/* ###------------------------------------------------------### */


      ptlastder->NEXT = ptbeder->NEXT;
      mbkfree (ptbeder);
      }

    listbeder = headder.NEXT;
    }

  return(listbeder);
  }
