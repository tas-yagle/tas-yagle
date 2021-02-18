
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbereg.c					*/
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
/* function	: beh_addbereg						*/
/* description	: create a BEREG structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

bereg_list *beh_addbereg (bereg_list  *lastbereg, char *name, biabl_list *biabl, binode_list *binode, char flags)
  {
  bereg_list *ptreg;

  name          = namealloc (name);

  ptreg         = (struct bereg *) mbkalloc (sizeof(struct bereg));
  ptreg->NAME   = name;
  ptreg->BIABL  = biabl;
  ptreg->BINODE = binode;
  ptreg->FLAGS   = flags;
  ptreg->NEXT   = lastbereg;

  return (ptreg);
  }
