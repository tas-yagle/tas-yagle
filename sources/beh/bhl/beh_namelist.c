
/* ###--------------------------------------------------------------### */
/* file		: beh_namelist.c					*/
/* date		: Mar 10 1997						*/
/* version	: v110							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* description	: expression level function				*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include "bhl_lib.h"

/* ###--------------------------------------------------------------### */
/* function	: beh_namelist						*/
/* description	: find the list of terminals' name for an expression.	*/
/*		  The value returned by the function is the list of	*/
/*		  terminals. Each terminal is represented by its name	*/
/*		  in the list. A given terminal may appear several	*/
/*		  times in the list. Constante values are not added	*/
/*		  to the list.						*/
/* called func.	: beh_namelist, namealloc, addchain, append		*/
/* ###--------------------------------------------------------------### */

chain_list *beh_namelist (chain_list *pt_exp)

  {
  struct chain *res_chain    = NULL;
  struct chain *tmp_chain    = NULL;
  struct chain *pt_opr       ;
  char         *name         ;

  static char  *str_z        = NULL;
  static char  *str_o        = NULL;
  static char  *str_d        = NULL;
  static char  *str_h        = NULL;
  static char  *str_u        = NULL;
  static char   buffer [128] ;

	/* ###------------------------------------------------------### */
	/*   initialize 3 variable with constantes (first call only)	*/
	/* ###------------------------------------------------------### */

  if (str_z == NULL)
    {
    str_z = namealloc ("'0'");
    str_o = namealloc ("'1'");
    str_d = namealloc ("'d'");
    str_h = namealloc ("'z'");
    str_u = namealloc ("'u'");
    }

  if (pt_exp != NULL)
    {
	/* ###------------------------------------------------------### */
	/*   check that the expression is not empty			*/
	/* ###------------------------------------------------------### */

    if (pt_exp->NEXT != NULL)
      {
	/* ###------------------------------------------------------### */
	/*   If the expression is not a terminal :			*/
	/*								*/
	/*     - if the operator is STABLE, add the varaiable's name	*/
	/*       to the list. Add also the "delayed" variable.		*/
	/*     - otherwies, call recursively the function on each	*/
	/*       operand, and append the returned list to the previous	*/
	/*       variable list.						*/
	/* ###------------------------------------------------------### */

      if (((int) ((long)((struct chain *) pt_exp->DATA)->DATA)) == STABLE)
        {
        name      = ((struct chain *) pt_exp->NEXT->DATA)->DATA;
        res_chain = addchain (NULL, name);
        sprintf   (buffer, "%s'delayed", name);
        name      = namealloc (buffer);
        res_chain = addchain (res_chain, name);
        }
      else
        {
        pt_opr = pt_exp->NEXT;
        while (pt_opr != NULL)
          {
          tmp_chain = beh_namelist (pt_opr->DATA);
          res_chain = append (res_chain, tmp_chain);
          pt_opr    = pt_opr->NEXT;
          }
        }
      }
    else
      {
	/* ###------------------------------------------------------### */
	/*   if the expression is a terminal but not a constante, add	*/
	/* the variable's name to the list of variables (do not check	*/
	/* if the variable has already been added to the list).		*/
	/* ###------------------------------------------------------### */

      if ((pt_exp->DATA != str_z) && (pt_exp->DATA != str_o) &&
          (pt_exp->DATA != str_d) && (pt_exp->DATA != str_h) && (pt_exp->DATA != str_u)  )
        {
        res_chain = addchain (NULL, pt_exp->DATA);
        }
      }
    }

  return (res_chain);
  }
