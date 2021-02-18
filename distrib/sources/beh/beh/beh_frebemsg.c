
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebemsg.c					*/
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
/* function	: beh_frebemsg						*/
/* description	: delete a list of BEMSG structures and all objects	*/
/*		  pointed by any os BEMSGs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebemsg (listbemsg)

struct bemsg *listbemsg;	/* list of bemsg to be deleted		*/

  {
  struct bemsg *ptbemsg;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbemsg != NULL)
    {
    freeExpr (listbemsg->ABL);

    ptbemsg   = listbemsg;
    listbemsg = listbemsg->NEXT;
    mbkfree (ptbemsg);
    }

  }
