
/* ###--------------------------------------------------------------### */
/* file		: beh_chkbefig.c					*/
/* date		: Oct 30 1995						*/
/* version	: v109							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* description	: high level function					*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include "bhl_lib.h"

/* ###--------------------------------------------------------------### */
/* function	: beh_chkbefig						*/
/* description	: check the consistency of a behavioural description	*/
/* called func.	: none							*/
/* ###--------------------------------------------------------------### */

int beh_chkbefig (befig_list *pt_befig, unsigned int  mode)

  {
  struct beout *pt_beout ;
  struct beaux *pt_beaux ;
  struct bebus *pt_bebus ;
  struct bebux *pt_bebux ;
  struct bereg *pt_bereg ;
  int           err_flg  = 0;

	/* ###------------------------------------------------------### */
	/*   check that the description is not empty			*/
	/* ###------------------------------------------------------### */

  if (pt_befig == NULL)
    err_flg = 1;

  else
    { 
	/* ###------------------------------------------------------### */
	/*   check that each output has at least one driver. Outputs	*/
	/* are simple output ports, bussed output ports, simple		*/
	/* internal signals, delayed internal signals, bussed internal	*/
	/* signals, and internal registers.				*/
	/* ###------------------------------------------------------### */

    if ((mode & BEH_CHK_DRIVERS) != 0)
      {
	/* ###------------------------------------------------------### */
	/*   simple output ports					*/
	/* ###------------------------------------------------------### */

      pt_beout = pt_befig->BEOUT;
      while (pt_beout != NULL)
        {
        if (pt_beout->ABL == NULL)
          err_flg += beh_error (40, pt_beout->NAME);
        pt_beout = pt_beout->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   simple internal signals					*/
	/* ###------------------------------------------------------### */

      pt_beaux = pt_befig->BEAUX;
      while (pt_beaux != NULL)
        {
        if (pt_beaux->ABL == NULL)
          err_flg += beh_error (40, pt_beaux->NAME);
        pt_beaux = pt_beaux->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   delayed internal signals					*/
	/* ###------------------------------------------------------### */

      pt_beaux = pt_befig->BEDLY;
      while (pt_beaux != NULL)
        {
        if (pt_beaux->ABL == NULL)
          err_flg += beh_error (40, pt_beaux->NAME);
        pt_beaux = pt_beaux->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   bussed output ports					*/
	/* ###------------------------------------------------------### */

      pt_bebus = pt_befig->BEBUS;
      while (pt_bebus != NULL)
        {
        if (pt_bebus->BIABL == NULL)
          err_flg += beh_error (40, pt_bebus->NAME);
        pt_bebus = pt_bebus->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   bussed internal signals					*/
	/* ###------------------------------------------------------### */

      pt_bebux = pt_befig->BEBUX;
      while (pt_bebux != NULL)
        {
        if (pt_bebux->BIABL == NULL)
          err_flg += beh_error (40, pt_bebux->NAME);
        pt_bebux = pt_bebux->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   internal registers						*/
	/* ###------------------------------------------------------### */

      pt_bereg = pt_befig->BEREG;
      while (pt_bereg != NULL)
        {
        if (pt_bereg->BIABL == NULL)
          err_flg += beh_error (40, pt_bereg->NAME);
        pt_bereg = pt_bereg->NEXT;
        }

      }

	/* ###------------------------------------------------------### */
	/*   check that the descriptiona has an empty architecture.	*/
	/* that means, no internal signal is declared and output ports	*/
	/* have no dreivers.						*/
	/* ###------------------------------------------------------### */

    if ((mode & BEH_CHK_EMPTY) != 0)
      {
	/* ###------------------------------------------------------### */
	/*   check that simple output ports have no drivers		*/
	/* ###------------------------------------------------------### */

      pt_beout = pt_befig->BEOUT;
      while (pt_beout != NULL)
        {
        if (pt_beout->ABL != NULL)
          break;
        pt_beout = pt_beout->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   check that bussed output ports have no drivers		*/
	/* ###------------------------------------------------------### */

      pt_bebus = pt_befig->BEBUS;
      while (pt_bebus != NULL)
        {
        if (pt_bebus->BIABL != NULL)
          break;
        pt_bebus = pt_bebus->NEXT;
        }

	/* ###------------------------------------------------------### */
	/*   check that the architecture is empty			*/
	/* ###------------------------------------------------------### */

      if ((pt_befig->BEAUX != NULL) || (pt_befig->BEBUX != NULL) ||
          (pt_befig->BEDLY != NULL) || (pt_befig->BEREG != NULL) ||
          (pt_befig->BEMSG != NULL) || (pt_beout        != NULL) ||
          (pt_bebus        != NULL) )
        err_flg += beh_error (41, pt_befig->NAME);

      }

    pt_befig->ERRFLG = err_flg;
    }

  return (err_flg);
  }
