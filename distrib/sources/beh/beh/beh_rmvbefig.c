
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_rmvbefig.c					*/
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
/* function	: beh_rmvbefig						*/
/* description	: delete a BEFIG structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N.	*/
/* called func.	: mbkfree     , beh_frebereg, beh_frebemsg,		*/
/*		  beh_freberin, beh_frebeout, beh_frebebus,		*/
/*		  beh_frebeaux, beh_frebebux, beh_frebepor,		*/
/*		  beh_frebegen, getptype				*/
/* ###--------------------------------------------------------------### */

befig_list *beh_rmvbefig (listbefig, befigname, mode)

befig_list *listbefig;	/* list of befig containing the object	*/
char         *befigname;	/* name of the BEFIG to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct befig  headfig;
  struct befig *ptlastfig;
  struct befig *ptbefig;
  ptype_list *ptptype;

  if (listbefig != NULL)
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headfig.NEXT  = listbefig;
    headfig.NAME  = NULL     ;
    headfig.BEREG = NULL     ;
    headfig.BEMSG = NULL     ;
    headfig.BERIN = NULL     ;
    headfig.BEAUX = NULL     ;
    headfig.BEBUX = NULL     ;
    headfig.BEDLY = NULL     ;
    headfig.BEPOR = NULL     ;
    headfig.USER  = NULL     ;
    ptbefig       = &headfig;
    while ((ptbefig != NULL) && (ptbefig->NAME != befigname))
      {
      ptlastfig = ptbefig;
      ptbefig   = ptbefig->NEXT;
      }

    if (ptbefig != NULL)
      {

	/* ###------------------------------------------------------### */
	/*    If the object doesn't exist return the list without	*/
	/* modification.						*/
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

      if (mode == 'N')
        {
        if ((ptbefig->BEREG != NULL) || (ptbefig->BEMSG != NULL) ||
            (ptbefig->BERIN != NULL) || (ptbefig->BEOUT != NULL) ||
            (ptbefig->BEBUS != NULL) || (ptbefig->BEAUX != NULL) ||
            (ptbefig->BEBUX != NULL) || (ptbefig->BEDLY != NULL) ||
            (ptbefig->BEPOR != NULL) || (ptbefig->USER  != NULL))

          beh_warning(304,befigname,NULL);
          // (stderr,"BEH_warning : befig `%s` not empty\n", befigname);
        }
      else
        {
        beh_frebereg (ptbefig->BEREG);
        beh_frebemsg (ptbefig->BEMSG);
        beh_freberin (ptbefig->BERIN);
        beh_frebeout (ptbefig->BEOUT);
        beh_frebebus (ptbefig->BEBUS);
        beh_frebeaux (ptbefig->BEAUX);
        beh_frebeaux (ptbefig->BEDLY);
        beh_frebebux (ptbefig->BEBUX);
        beh_frebepor (ptbefig->BEPOR);
        if ((ptptype = getptype (ptbefig->USER,BEH_GENERIC)) != NULL)
          beh_frebegen (ptptype->DATA);
        }

      ptlastfig->NEXT = ptbefig->NEXT;
      mbkfree (ptbefig);
      }

    listbefig = headfig.NEXT;
    }

  return(listbefig);
  }
