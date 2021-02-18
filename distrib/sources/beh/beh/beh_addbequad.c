
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbequad.c					*/
/* date		: Mar  17 1995						*/
/* version	: v000							*/
/* authors	: Laurent VUILLEMIN					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_addbequad						*/
/* description	: create a BEQUAD structure at the top of the list	*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct bequad *beh_addbequad ( bdd_expr, gex_expr, var_list, der_list)

struct node *bdd_expr;
struct begex *gex_expr;
struct chain *var_list;
struct beder *der_list;

  {
  struct bequad *ptquad;


  ptquad           = (struct bequad *) mbkalloc (sizeof(struct bequad));
  ptquad->BDD      = bdd_expr;
  ptquad->GEX      = gex_expr;
  ptquad->VARLIST  = var_list;
  ptquad->DERIVATE = der_list;
  ptquad->USER     = NULL;
  return (ptquad);
  }
