
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbepor.c					*/
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
/* function	: beh_addbepor						*/
/* description	: create a BEPOR structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

bepor_list *beh_addbepor (lastbepor, name, dir, type)

bepor_list *lastbepor;	/* pointer on the last bepor structure	*/
char         *name;		/* signal's name			*/
char          dir;     		/* signal's mode (I, O, B, Z, or T)	*/
char          type;     	/* signal'type mark (M or W)		*/

  {
  struct bepor *ptbepor;

  name               = namealloc (name);

  ptbepor            = (struct bepor *) mbkalloc (sizeof(struct bepor));
  ptbepor->NAME      = name;
  ptbepor->DIRECTION = dir;
  ptbepor->TYPE      = type;
  ptbepor->NEXT      = lastbepor;

  return (ptbepor);
  }
