
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_frebebux.c					*/
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
/* function	: beh_frebebux						*/
/* description	: delete a list of BEBUX structures and all objects	*/
/*		  pointed by any os BEBUXs in the list			*/
/* called func.	: mbkfree						*/
/* ###--------------------------------------------------------------### */

void beh_frebebux (listbebux)

struct bebux *listbebux;	/* list of bebux to be deleted		*/

  {
  struct bebux *ptbebux;

	/* ###------------------------------------------------------### */
	/*    for each object of the list, first delete pointed objects	*/
	/* then, delete the object itself				*/
	/* ###------------------------------------------------------### */

  while (listbebux != NULL)
    {
    beh_frebiabl  (listbebux->BIABL);
    beh_frebinode (listbebux->BINODE);

    ptbebux   = listbebux;
    listbebux = listbebux->NEXT;
    mbkfree (ptbebux);
    }

  }
