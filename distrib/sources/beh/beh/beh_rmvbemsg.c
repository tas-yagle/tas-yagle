
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbemsg.c					*/
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
/* function	: beh_rmvbemsg						*/
/* description	: delete a BEMSG structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N	*/
/* called func.	: mbkfree, freeExpr					*/
/* ###--------------------------------------------------------------### */

bemsg_list *beh_rmvbemsg (listbemsg, bemsglabl, mode)

bemsg_list *listbemsg;	/* list of bemsg containing the object	*/
char         *bemsglabl;	/* label of the BEMSG to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct bemsg  headmsg;
  struct bemsg *ptlastmsg;
  struct bemsg *ptbemsg;

  if (listbemsg != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headmsg.NEXT  = listbemsg;
    headmsg.LABEL = NULL     ;
    headmsg.ABL   = NULL     ;
    ptbemsg       = &headmsg;
    while ((ptbemsg != NULL) && (ptbemsg->LABEL != bemsglabl))
      {
      ptlastmsg = ptbemsg;
      ptbemsg   = ptbemsg->NEXT;
      }

    if (ptbemsg != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if (ptbemsg->ABL != NULL)
          beh_warning(306,bemsglabl,NULL);
          // (stderr,"BEH_warning : bemsg `%s` not empty\n", bemsglabl);
        }
      else
        freeExpr (ptbemsg->ABL);

      ptlastmsg->NEXT = ptbemsg->NEXT;
      mbkfree (ptbemsg);
      }

    listbemsg = headmsg.NEXT;
    }

  return(listbemsg);
  }
