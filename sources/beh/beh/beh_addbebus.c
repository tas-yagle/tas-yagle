
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbebus.c					*/
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
/* function	: beh_addbebus						*/
/* description	: create a BEBUS structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

bebus_list *beh_addbebus (lastbebus, name, biabl, binode, type, flags)

bebus_list  *lastbebus;	/* pointer on the last bebus structure	*/
char          *name;		/* port's name				*/
biabl_list  *biabl;		/* port's drivers (ABL)			*/
binode_list *binode;		/* port's drivers (BDD)			*/
char           type;		/* port type mark (M or W)		*/
char flags;

  {
  struct bebus *ptbus;

  name          = namealloc (name);

  ptbus         = (struct bebus *) mbkalloc (sizeof(struct bebus));
  ptbus->NAME   = name;
  ptbus->BIABL  = biabl;
  ptbus->BINODE = binode;
  ptbus->TYPE   = type;
  ptbus->FLAGS   = flags;
  ptbus->NEXT   = lastbebus;

  return (ptbus);
  }
