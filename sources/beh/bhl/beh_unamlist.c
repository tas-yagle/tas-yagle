
/* ###--------------------------------------------------------------### */
/* file		: beh_unamlist.c					*/
/* date		: Mar 10 1997						*/
/* version	: v110							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* description	: expression level function				*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include "bhl_lib.h"

/* ###--------------------------------------------------------------### */
/* function	: beh_unamlist						*/
/* description	: find the list of terminals' name for an expression.	*/
/*		  The value returned by the function is the list of	*/
/*		  terminals. Each terminal is represented by its name	*/
/*		  in the list. A given terminal may only appear once	*/
/*		  in the list. Constante values are not added		*/
/*		  to the list.						*/
/* called func.	: beh_namelist, delchain				*/
/* ###--------------------------------------------------------------### */

struct chain *beh_unamlist(chain_list *pt_exp)

  {
  struct chain *res_chain;
  struct chain *tmp_chain;
  struct chain *del_chain;
  struct chain *prv_chain;

	/* ###------------------------------------------------------### */
	/*   find the list of terminals					*/
	/* ###------------------------------------------------------### */

  res_chain = beh_namelist (pt_exp);

	/* ###------------------------------------------------------### */
	/*   delete multiple refernces to the same terminal		*/
	/* ###------------------------------------------------------### */

  tmp_chain = res_chain;

  while (tmp_chain != NULL)
    {
    prv_chain = tmp_chain      ;
    del_chain = tmp_chain->NEXT;

    while (del_chain != NULL)
      {
      if (del_chain->DATA == tmp_chain->DATA)
        {
        del_chain       = delchain (del_chain, del_chain);
        prv_chain->NEXT = del_chain;
        }
      else
        {
        prv_chain = del_chain      ;
        del_chain = del_chain->NEXT;
        }
      }

    tmp_chain = tmp_chain->NEXT;
    }

  return (res_chain);
  }
