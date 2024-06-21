
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbebux.c					*/
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
/* function	: beh_addbebux						*/
/* description	: create a BEBUX structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

bebux_list *beh_addbebux (lastbebux, name, biabl, binode, type, flags)

bebux_list  *lastbebux;	/* pointer on the last bebux structure	*/
char          *name;		/* signal's name			*/
biabl_list  *biabl;		/* signal's expression (ABL)		*/
binode_list *binode;		/* signal's expression (BDD)		*/
char           type;		/* signal's type mark (M or W)		*/
char flags;
  {
  struct bebux *ptbux;

  name          = namealloc (name);

  ptbux         = (struct bebux *) mbkalloc (sizeof(struct bebux));
  ptbux->NAME   = name;
  ptbux->BIABL  = biabl;
  ptbux->BINODE = binode;
  ptbux->TYPE   = type;
  ptbux->FLAGS   = flags;
  ptbux->NEXT   = lastbebux;

  return (ptbux);
  }
