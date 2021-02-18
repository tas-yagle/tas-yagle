
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbegen.c					*/
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
/* function	: beh_addbegen						*/
/* description	: create a BEGEN structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct begen *beh_addbegen (lastbegen, name, type, value)

struct begen *lastbegen;	/* pointer on the last begen structure	*/
char         *name;		/* generic's name			*/
char         *type;		/* generic's type mark			*/
void         *value;		/* generic's value			*/

  {

  struct begen *ptgen;

  name         = namealloc (name);
  type         = namealloc (type);
  
  ptgen        = (struct begen *) mbkalloc (sizeof(struct begen));
  ptgen->NAME  = name;
  ptgen->TYPE  = type;
  ptgen->VALUE = value;
  ptgen->NEXT  = lastbegen;

  return (ptgen);
  }
