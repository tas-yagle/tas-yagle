
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addberin.c					*/
/* date		: Sep 20 1994						*/
/* version	: v107							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_addberin						*/
/* description	: create a BERIN structure at the top the list		*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct berin *beh_addberin (lastberin, name)

struct berin *lastberin;	/* pointer on the last berin structure	*/
char         *name;		/* signal's name			*/

  {
  struct berin *ptrin;

  name         = namealloc (name);

  ptrin        = (struct berin *) mbkalloc (sizeof(struct berin));
  ptrin->NAME  = name;
  ptrin->NEXT  = lastberin;

  ptrin->REG_REF = NULL;
  ptrin->REG_VEC = NULL;
  ptrin->MSG_REF = NULL;
  ptrin->AUX_REF = NULL;
  ptrin->AUX_VEC = NULL;
  ptrin->BUX_REF = NULL;
  ptrin->BUX_VEC = NULL;
  ptrin->BUS_REF = NULL;
  ptrin->BUS_VEC = NULL;
  ptrin->OUT_REF = NULL;
  ptrin->OUT_VEC = NULL;
  ptrin->DLY_REF = NULL;

  ptrin->LEFT = -1;
  ptrin->RIGHT = -1;

  return (ptrin);
  }
