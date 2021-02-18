
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_freberin.c					*/
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
/* function	: beh_freberin						*/
/* description	: delete a list of BEAUX structures and all objects	*/
/*		  pointed by any os BEAUXs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_freberin (listberin)

struct berin *listberin;	/* list of berin to be deleted		*/

  {
  struct berin *ptberin;

	/* ###------------------------------------------------------### */
	/*    for each object of the list delete the object itself	*/
	/* ###------------------------------------------------------### */

  while (listberin != NULL)
    {
    ptberin   = listberin;
  
    freechain(ptberin->OUT_REF);
    freechain(ptberin->OUT_VEC);
    
    freechain(ptberin->BUS_REF);
    freechain(ptberin->BUS_VEC);
    
    freechain(ptberin->AUX_REF);
    freechain(ptberin->AUX_VEC);
    
    freechain(ptberin->REG_REF);
    freechain(ptberin->REG_VEC);
    
    freechain(ptberin->BUX_REF);
    freechain(ptberin->BUX_VEC);
    
    freechain(ptberin->MSG_REF);
    
    freechain(ptberin->DLY_REF);

    listberin = listberin->NEXT;
    mbkfree (ptberin);
    }

  }
