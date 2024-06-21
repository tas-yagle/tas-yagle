
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbereg.c					*/
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
/* function	: beh_rmvbereg						*/
/* description	: delete a BEREG structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, beh_frebiabl, beh_frebinode			*/
/* ###--------------------------------------------------------------### */

bereg_list *beh_rmvbereg (listbereg, beregname, mode)

bereg_list *listbereg;	/* list of bereg containing the object	*/
char         *beregname;	/* name of the BEREG to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct bereg  headreg;
  struct bereg *ptlastreg;
  struct bereg *ptbereg;

  if (listbereg != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headreg.NEXT   = listbereg;
    headreg.NAME   = NULL     ;
    headreg.BIABL  = NULL     ;
    headreg.BINODE = NULL     ;
    ptbereg        = &headreg;
    while ((ptbereg != NULL) && (ptbereg->NAME != beregname))
      {
      ptlastreg = ptbereg;
      ptbereg   = ptbereg->NEXT;
      }

    if (ptbereg != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if ((ptbereg->BIABL != NULL) || (ptbereg->BINODE != NULL))
          beh_warning(309,beregname,NULL);
          //(stderr,"BEH_warning : bereg `%s` not empty\n", beregname);
        }
      else
        {
        beh_frebiabl  (ptbereg->BIABL);
        beh_frebinode (ptbereg->BINODE);
        }

      ptlastreg->NEXT = ptbereg->NEXT;
      mbkfree (ptbereg);
      }

    listbereg = headreg.NEXT;
    }

  return(listbereg);
  }
