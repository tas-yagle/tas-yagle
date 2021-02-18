
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbinod.c					*/
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
/* function	: beh_addbinode						*/
/* description	: create a BINODE structure at the top the list		*/
/* called func.	: mbkalloc						*/
/* ###--------------------------------------------------------------### */

struct binode *beh_addbinode (lastbinode, condition, value)

struct binode *lastbinode;	/* pointer on the last binode structure	*/
struct node   *condition;	/* guard expression (BDD)		*/
struct node   *value;		/* value expression (BDD)		*/

  {
  struct binode *ptbinode;

  ptbinode          = (struct binode *) mbkalloc (sizeof(struct binode));
  ptbinode->TIME    = 0;
  ptbinode->TIMEVAR    = NULL;
  ptbinode->CNDNODE = condition;
  ptbinode->VALNODE = value;
  ptbinode->NEXT    = lastbinode;

  return (ptbinode);
  }
