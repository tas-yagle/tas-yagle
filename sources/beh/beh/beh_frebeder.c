
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebeder.c					*/
/* date		: Mar  22 1995						*/
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
/* function	: beh_frebeder						*/
/* description	: delete a list of BEDER structures and all objects	*/
/*		  pointed by any os BEDERs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebeder (listbeder)

struct beder *listbeder;	/* list of beder to be deleted		*/

  {
  struct beder *ptbeder;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbeder != NULL)
    {
    ptbeder   = listbeder;
    listbeder = listbeder->NEXT;
    mbkfree (ptbeder);
    }

  }
