
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbiabl.c					*/
/* date		: Nov  6 1995						*/
/* version	: v109							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_addbiabl						*/
/* description	: create a BIABL structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct biabl *beh_addbiabl (lastbiabl, label, condition, value)

char         *label;		/* block's label			*/
struct biabl *lastbiabl;	/* pointer on the last biabl structure	*/
struct chain *condition;	/* guard expression (ABL)		*/
struct chain *value;		/* value expression (ABL)		*/

  {
  struct biabl  *ptbiabl;

  ptbiabl         = (struct biabl *) mbkalloc (sizeof(struct biabl));
  label           = namealloc (label);

  ptbiabl->LABEL  = label;
  ptbiabl->CNDABL = condition;
  ptbiabl->VALABL = value;
  ptbiabl->LOOP	  = NULL;
  ptbiabl->TIME   = 0;
  ptbiabl->TIMEVAR  = NULL;
  ptbiabl->FLAG   = 0;
  ptbiabl->USER   = NULL;
  ptbiabl->TIMER   = 0;
  ptbiabl->TIMEF   = 0;
  ptbiabl->NEXT   = lastbiabl;

  return (ptbiabl);
  }
