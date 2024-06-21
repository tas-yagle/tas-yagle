
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbefig.c					*/
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
/* function	: beh_delbefig						*/
/* description	: delete a BEFIG structure and return the pointer of	*/
/*		  the next object. A warning is printed out if the	*/
/*		  object to be deleted is not empty when the mode is N.	*/
/* called func.	: mbkfree     , beh_frebereg, beh_frebemsg,		*/
/*		  beh_freberin, beh_frebeout, beh_frebebus,		*/
/*		  beh_frebeaux, beh_frebebux, beh_frebepor,		*/
/*		  beh_frebegen, getptype				*/
/* ###--------------------------------------------------------------### */

befig_list *beh_delbefig (listbefig, ptbefig, mode)

befig_list *listbefig;	/* list of befig containing the object	*/
befig_list *ptbefig;		/* pointer of the BEFIG to be deleted	*/
char          mode;		/* recursive delete or not (Y or N)	*/

  {
  struct befig  headfig;
  struct befig *ptlastfig;
  ptype_list *ptptype;

  if ((listbefig != NULL) && (ptbefig != NULL))
    {

	/* ###------------------------------------------------------### */
	/*    Search the object to be deleted				*/
	/* ###------------------------------------------------------### */

    headfig.NEXT = listbefig;
    ptlastfig    = &headfig;
    while ((ptlastfig != NULL) && (ptlastfig->NEXT != ptbefig))
      ptlastfig = ptlastfig->NEXT;

    if (ptlastfig != NULL)
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
            (ptbefig->BEPOR != NULL) || (ptbefig->USER  != NULL) ||
            (ptbefig->BEVECTREG)     || (ptbefig->BEVECTOUT)     ||
            (ptbefig->BEVECTBUS)     || (ptbefig->BEVECTAUX)     ||
            (ptbefig->BEVECTBUS)     || (ptbefig->BEVECTPOR) )

          beh_warning(304,ptbefig->NAME,NULL);
        // (stderr,"BEH_warning : befig `%s` not empty\n", ptbefig->NAME);
        }
      else
        {
          beh_frebereg     (listbefig->BEREG);
          beh_frebevectreg (listbefig->BEVECTREG);
          beh_frebemsg     (listbefig->BEMSG);
          beh_freberin     (listbefig->BERIN);
          beh_frebeout     (listbefig->BEOUT);
          beh_frebevectout (listbefig->BEVECTOUT);
          beh_frebebus     (listbefig->BEBUS);
          beh_frebevectbus (listbefig->BEVECTBUS);
          beh_frebeaux     (listbefig->BEAUX);
          beh_frebevectaux (listbefig->BEVECTAUX);
          beh_frebeaux     (listbefig->BEDLY);
          beh_frebebux     (listbefig->BEBUX);
          beh_frebevectbux (listbefig->BEVECTBUX);
          beh_frebepor     (listbefig->BEPOR);
          beh_frebevectpor (listbefig->BEVECTPOR);
        if ((ptptype = getptype (ptbefig->USER,BEH_GENERIC)) != NULL)
          beh_frebegen (ptptype->DATA);
        }

      ptlastfig->NEXT = ptbefig->NEXT;
      if (ptbefig->CIRCUI) destroyCct(ptbefig->CIRCUI);
      mbkfree (ptbefig);
      }

    listbefig = headfig.NEXT;
    }

  return(listbefig);
  }
