
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbeaux.c					*/
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
/* function	: beh_addbeaux						*/
/* description	: create a BEAUX structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct beaux *beh_addbeaux (struct beaux *lastbeaux, char         *name, struct chain *abl_expr, struct node  *bdd_expr, long flags)

{
  struct beaux *ptaux;

  name        = namealloc (name);

  ptaux       = (struct beaux *) mbkalloc (sizeof(struct beaux));
  ptaux->NAME = name;
  ptaux->TIME = 0;
  ptaux->TIMER = ptaux->TIMEF = 0;
  ptaux->TIMEVAR = NULL;
  ptaux->ABL  = abl_expr;
  ptaux->NODE = bdd_expr;
  ptaux->NEXT = lastbeaux;
  ptaux->FLAGS= flags;

  return (ptaux);
}
