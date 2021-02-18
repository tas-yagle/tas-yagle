
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbiabl.c					*/
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
/* function	: beh_delbiabl						*/
/* description	: delete a BIABL structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, freeExpr					*/
/* ###--------------------------------------------------------------### */

biabl_list *beh_delbiabl (listbiabl, ptbiabl, mode)

biabl_list *listbiabl;	/* list of biabl containing the object	*/
biabl_list *ptbiabl;		/* pointer of the BIABL to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct biabl  headabl;
  struct biabl *ptlastabl;

  if ((listbiabl != NULL) && (ptbiabl != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headabl.NEXT = listbiabl;
    ptlastabl    = &headabl;
    while ((ptlastabl != NULL) && (ptlastabl->NEXT != ptbiabl))
      ptlastabl = ptlastabl->NEXT;

    if (ptlastabl != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if ((ptbiabl->VALABL != NULL) || (ptbiabl->CNDABL != NULL))
          beh_warning(310,NULL,NULL);
          //(stderr,"BEH_warning : biabl not empty\n");
        }
      else
        {
        freeExpr (ptbiabl->VALABL);
        freeExpr (ptbiabl->CNDABL);
        freeptype(ptbiabl->USER);
        }

      ptlastabl->NEXT = ptbiabl->NEXT;
      mbkfree (ptbiabl);
      }

    listbiabl = headabl.NEXT;
    }

  return(listbiabl);
  }
