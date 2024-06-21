
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_getgenva.c					*/
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
/* function	: beh_getgenval						*/
/* description	: Search an find a BEGEN structure (known from its name)*/
/*		  then return its value field. If the BEGEN is not found*/
/*		  a NULL pointer is returned				*/
/* called func.	:							*/
/* ###--------------------------------------------------------------### */

void *beh_getgenval (listbegen, begenname)

struct begen *listbegen;	/* head of BEGEN list			*/
char         *begenname;	/* name of the structure 		*/

  {
  struct begen *ptbegen;
  void         *value = NULL;

	/* ###------------------------------------------------------### */
	/*    searching the object					*/
	/* ###------------------------------------------------------### */

  ptbegen = listbegen;
  while ((ptbegen != NULL) && (ptbegen->NAME != begenname))
    ptbegen = ptbegen->NEXT;

	/* ###------------------------------------------------------### */
	/*    if found return its value field				*/
	/* ###------------------------------------------------------### */

  if (ptbegen != NULL)
    value = ptbegen->VALUE;

  return (value);
  }
