
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbinod.c					*/
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
/* function	: beh_delbinode						*/
/* description	: delete a BINODE structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

binode_list *beh_delbinode (listbinode, ptbinode, mode)

binode_list *listbinode;	/* list of binode containing the object	*/
binode_list *ptbinode;	/* pointer of the BINODE to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct binode  headnode;
  struct binode *ptlastnode;

  if ((listbinode != NULL) && (ptbinode != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headnode.NEXT = listbinode;
    ptlastnode    = &headnode;
    while ((ptlastnode != NULL) && (ptlastnode->NEXT != ptbinode))
      ptlastnode = ptlastnode->NEXT;

    if (ptlastnode != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
      {
        if ((ptbinode->VALNODE != NULL) || (ptbinode->CNDNODE != NULL))
          beh_warning(311,NULL,NULL);
        //(stderr,"BEH_warning : binode not empty\n");
      }

      ptlastnode->NEXT = ptbinode->NEXT;
      mbkfree (ptbinode);
      }

    listbinode = headnode.NEXT;
    }

  return(listbinode);
  }
