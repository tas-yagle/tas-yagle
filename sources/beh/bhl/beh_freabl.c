
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_freabl.c						*/
/* date		: Sep  9 1993						*/
/* version	: v106							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* description	: high level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include "bhl_lib.h"

/* ###--------------------------------------------------------------### */
/* function	: beh_freabl						*/
/* description	: free all ABLs in BEFIG structure			*/
/* called func.	: freeExpr						*/
/* ###--------------------------------------------------------------### */

void beh_freabl (befig_list *ptr_befig)
  {
  struct beaux *ptr_beaux;
  struct bemsg *ptr_bemsg;
  struct beout *ptr_beout;
  struct bebus *ptr_bebus;
  struct biabl *ptr_biabl;
  struct bebux *ptr_bebux;
  struct bereg *ptr_bereg;

	/* ###------------------------------------------------------### */
	/*    check that the unit exists				*/
	/* ###------------------------------------------------------### */

  if (ptr_befig != NULL)
    {
	/* ###------------------------------------------------------### */
	/*    release expression in simple internal signals' list	*/
	/* ###------------------------------------------------------### */

    ptr_beaux = ptr_befig->BEAUX;
    while (ptr_beaux != NULL)
      {
      if (ptr_beaux->ABL != NULL)
        {
        freeExpr (ptr_beaux->ABL);
        ptr_beaux->ABL = NULL;
        }
      ptr_beaux = ptr_beaux->NEXT;
      }

	/* ###------------------------------------------------------### */
	/*    release expression in delayed signals' list		*/
	/* ###------------------------------------------------------### */

    ptr_beaux = ptr_befig->BEDLY;
    while (ptr_beaux != NULL)
      {
      if (ptr_beaux->ABL != NULL)
        {
        freeExpr (ptr_beaux->ABL);
        ptr_beaux->ABL = NULL;
        }
      ptr_beaux = ptr_beaux->NEXT;
      }

	/* ###------------------------------------------------------### */
	/*    release expression in assertions' list			*/
	/* ###------------------------------------------------------### */

    ptr_bemsg = ptr_befig->BEMSG;
    while (ptr_bemsg != NULL)
      {
      if (ptr_bemsg->ABL != NULL)
        {
        freeExpr (ptr_bemsg->ABL);
        ptr_bemsg->ABL = NULL;
        }
      ptr_bemsg = ptr_bemsg->NEXT;
      }

	/* ###------------------------------------------------------### */
	/*    release expression in simple output ports' list		*/
	/* ###------------------------------------------------------### */

    ptr_beout = ptr_befig->BEOUT;
    while (ptr_beout != NULL)
      {
      if (ptr_beout->ABL != NULL) 
        {
        freeExpr (ptr_beout->ABL);
        ptr_beout->ABL = NULL;
        }
      ptr_beout = ptr_beout->NEXT;
      }

	/* ###------------------------------------------------------### */
	/*    release expression in bussed output ports' list		*/
	/* ###------------------------------------------------------### */

    ptr_bebus = ptr_befig->BEBUS;
    while (ptr_bebus != NULL)
      {
      ptr_biabl = ptr_bebus->BIABL;
      while (ptr_biabl != NULL)
        {
        if (ptr_biabl->CNDABL != NULL)
          {
          freeExpr(ptr_biabl->CNDABL);
          ptr_biabl->CNDABL = NULL;
          }
        if (ptr_biabl->VALABL != NULL)
          {
          freeExpr(ptr_biabl->VALABL);
          ptr_biabl->VALABL = NULL;
          }
        ptr_biabl = ptr_biabl->NEXT;
        }
      ptr_bebus = ptr_bebus->NEXT;
      }

	/* ###------------------------------------------------------### */
	/*    release expression in bussed internal signals' list	*/
	/* ###------------------------------------------------------### */

    ptr_bebux = ptr_befig->BEBUX;
    while (ptr_bebux != NULL)
      {
      ptr_biabl  = ptr_bebux->BIABL;
      while (ptr_biabl != NULL)
        {
        if (ptr_biabl->CNDABL != NULL)
          {
          freeExpr (ptr_biabl->CNDABL);
          ptr_biabl->CNDABL = NULL;
          }
        if (ptr_biabl->VALABL != NULL)
          {
          freeExpr   (ptr_biabl->VALABL);
          ptr_biabl->VALABL = NULL;
          }

        ptr_biabl  = ptr_biabl->NEXT;
        }
      ptr_bebux = ptr_bebux->NEXT;
      }

	/* ###------------------------------------------------------### */
	/*    release expression in internal registers' list		*/
	/* ###------------------------------------------------------### */

    ptr_bereg = ptr_befig->BEREG;
    while (ptr_bereg != NULL)
      {
      ptr_biabl = ptr_bereg->BIABL;
      while (ptr_biabl != NULL)
        {
        if ( ptr_biabl->CNDABL != NULL )
          {
          freeExpr(ptr_biabl->CNDABL);
          ptr_biabl->CNDABL = NULL;
          }
        if ( ptr_biabl->VALABL != NULL )
          {
          freeExpr(ptr_biabl->VALABL);
          ptr_biabl->VALABL = NULL;
          }
        ptr_biabl = ptr_biabl->NEXT;
        }
      ptr_bereg = ptr_bereg->NEXT;
      }
    }
  }
