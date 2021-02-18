/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebiabl.c					*/
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
/* function	: beh_frebiabl						*/
/* description	: delete a list of BIABL structures and all objects	*/
/*		  pointed by any os BIABLs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebiabl (listbiabl)

struct biabl *listbiabl;	/* list of biabl to be deleted		*/

{
  struct biabl *ptbiabl;

  /* ###------------------------------------------------------### */
  /*    for each object of the list, first delete pointed objects	*/
  /* then, delete the object itself				*/
  /* ###------------------------------------------------------### */

  while (listbiabl != NULL)
  {
    freeExpr (listbiabl->VALABL);
    freeExpr (listbiabl->CNDABL);
    freeptype(listbiabl->USER);

    ptbiabl   = listbiabl;
    listbiabl = listbiabl->NEXT;
    mbkfree (ptbiabl);
  }

}
