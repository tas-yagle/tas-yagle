
/* ###--------------------------------------------------------------### */
/* file		: beh_addbeder.c					*/
/* date		: Mar  17 1995						*/
/* version	: v109							*/
/* authors	: Laurent VUILLEMIN					*/
/* content	: low-level function					*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_addbeder						*/
/* description	: create a BEDER structure at the top of the list	*/
/* called func.	: mbkalloc						*/
/* ###--------------------------------------------------------------### */

beder_list *beh_addbeder (last_beder, bdd_expr, index)

beder_list *last_beder;
pNode bdd_expr  ;
short         index     ;

  {
  struct beder *ptder;

  ptder        = (struct beder *) mbkalloc (sizeof(struct beder));

  ptder->BDD   = bdd_expr  ;
  ptder->INDEX = index     ;
  ptder->NEXT  = last_beder;

  return (ptder);
  }
