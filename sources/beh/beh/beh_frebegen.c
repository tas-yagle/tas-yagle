
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebegen.c					*/
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
/* function	: beh_frebegen						*/
/* description	: delete a list of BEGEN structures and all objects	*/
/*		  pointed by any os BEGENs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebegen (listbegen)

struct begen *listbegen;	/* list of begen to be deleted		*/

  {
  struct begen *ptbegen;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbegen != NULL)
    {
    mbkfree (listbegen->VALUE);

    ptbegen   = listbegen;
    listbegen = listbegen->NEXT;
    mbkfree (ptbegen);
    }

  }
