
/* ###--------------------------------------------------------------### */
/*                            */
/* file      : beh_frebefig.c               */
/* date      : Sep  3 1993                  */
/* version   : v106                     */
/* authors   : Pirouz BAZARGAN SABET               */
/* content   : low-level function               */
/*                           */
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function   : beh_frebefig                  */
/* description   : delete a list of BEFIG structures and all objects   */
/*        pointed by any os BEFIGs in the list         */
/* called func.   : mbkfree                  */
/* ###--------------------------------------------------------------### */

void beh_frebefig (listbefig)

struct befig *listbefig;   /* list of befig to be deleted      */

  {
  struct befig *ptbefig;
  ptype_list *ptptype;

   /* ###------------------------------------------------------### */
   /*    for each object of the list, first delete pointed objects   */
   /* then, delete the object itself            */
   /* ###------------------------------------------------------### */

  while (listbefig != NULL)
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
    if ((ptptype = getptype (listbefig->USER,BEH_GENERIC)) != NULL)
      beh_frebegen (ptptype->DATA);

    ptbefig   = listbefig;
    listbefig = listbefig->NEXT;
    if (ptbefig->CIRCUI) destroyCct(ptbefig->CIRCUI);
    mbkfree (ptbefig);
    }

  }
