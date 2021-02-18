
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_delbequad.c					*/
/* date		: Sep  3 1993						*/
/* version	: v106							*/
/* authors	: Laurent VUILLEMIN 					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_delbequad						*/
/* description	: delete a BEQUAD structure. A warning is printed out	*/
/*		  if the object to be deleted is not empty when the	*/
/*		  mode is N						*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_delbequad (ptbequad, mode)

struct bequad *ptbequad;	/* pointer of the BEQUAD to be deleted	*/
char           mode;		/* recursive delete or not (Y or N)	*/

  {

  if (ptbequad != NULL)
    {
	/* ###------------------------------------------------------### */
	/*    If the object has been found check the mode and, if asked	*/
	/* delete pointed objects recursively.				*/
	/* ###------------------------------------------------------### */

    if (mode == 'N')
      {
      if ((ptbequad->BDD     != NULL) || (ptbequad->GEX      != NULL) ||
	  (ptbequad->VARLIST != NULL) || (ptbequad->DERIVATE != NULL))
        beh_warning(308,NULL,NULL);
        //(stderr, "BEH_warning : bequad not empty\n");
      }
    else
      {
      beh_frebeder (ptbequad->DERIVATE);
      freechain    (ptbequad->VARLIST );
      freeptype(ptbequad->USER);
      }

    mbkfree (ptbequad);
    }
  }
