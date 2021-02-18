
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbeout.c					*/
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
/* function	: beh_addbeout						*/
/* description	: create a BEOUT structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct beout *beh_addbeout (struct beout *lastbeout, char *name, struct chain *abl_expr, struct node  *bdd_expr,long flags)

  {
  struct beout *ptout;

  name         = namealloc (name);

  ptout        = (struct beout *) mbkalloc (sizeof(struct beout));
  ptout->NAME  = name;
  ptout->ABL   = abl_expr;
  ptout->TIME  = 0;
  ptout->TIMER  = 0;
  ptout->TIMEF  = 0;
  ptout->TIMEVAR  = NULL;
  ptout->NODE  = bdd_expr;
  ptout->FLAGS = flags;
  ptout->NEXT  = lastbeout;

  return (ptout);
  }
