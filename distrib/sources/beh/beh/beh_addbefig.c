
/* ###--------------------------------------------------------------### */
/* file		: beh_addbefig.c					*/
/* date		: Mar 10 1997						*/
/* version	: v110							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_addbefig						*/
/* description	: create an empty BEFIG structure at the top of the list*/
/* called func.	: namealloc, mbkalloc					*/
/* ###--------------------------------------------------------------### */

struct befig *beh_addbefig (lastbefig, name)

struct befig *lastbefig;	/* pointer on the last befig structure	*/
char         *name     ;	/* figure's name			*/

{
  struct befig  *ptfig   ;
  unsigned char  err_flg = 0;

  ptfig = (struct befig *) mbkalloc (sizeof(struct befig));

  switch (BEH_TIMEUNIT)
  {
    case BEH_TU__FS:
         BEH_CNV_FS = 1.0    ;
         BEH_CNV_PS = 1.0E3  ;
         BEH_CNV_NS = 1.0E6  ;
         BEH_CNV_US = 1.0E9  ;
         BEH_CNV_MS = 1.0E12 ;
         break;
    case BEH_TU__PS:
         BEH_CNV_FS = 1.0E-3 ;
         BEH_CNV_PS = 1.0    ;
         BEH_CNV_NS = 1.0E3  ;
         BEH_CNV_US = 1.0E6  ;
         BEH_CNV_MS = 1.0E9  ;
         break;
    case BEH_TU__NS:
         BEH_CNV_FS = 1.0E-6 ;
         BEH_CNV_PS = 1.0E-3 ;
         BEH_CNV_NS = 1.0    ;
         BEH_CNV_US = 1.0E3  ;
         BEH_CNV_MS = 1.0E6  ;
         break;
    case BEH_TU__US:
         BEH_CNV_FS = 1.0E-9 ;
         BEH_CNV_PS = 1.0E-6 ;
         BEH_CNV_NS = 1.0E-3 ;
         BEH_CNV_US = 1.0    ;
         BEH_CNV_MS = 1.0E3  ;
         break;
    case BEH_TU__MS:
         BEH_CNV_FS = 1.0E-12;
         BEH_CNV_PS = 1.0E-9 ;
         BEH_CNV_NS = 1.0E-6 ;
         BEH_CNV_US = 1.0E-3 ;
         BEH_CNV_MS = 1.0    ;
         break;
    default:
         err_flg = beh_error (200, NULL);
  }

  ptfig->NAME      = namealloc (name);
  ptfig->BEREG     = NULL            ;
  ptfig->BEVECTREG = NULL            ;
  ptfig->BEMSG     = NULL            ;
  ptfig->BERIN     = NULL            ;
  ptfig->BEOUT     = NULL            ;
  ptfig->BEVECTOUT = NULL            ;
  ptfig->BEBUS     = NULL            ;
  ptfig->BEVECTBUS = NULL            ;
  ptfig->BEAUX     = NULL            ;
  ptfig->BEVECTAUX = NULL            ;
  ptfig->BEDLY     = NULL            ;
  ptfig->BEBUX     = NULL            ;
  ptfig->BEVECTBUX = NULL            ;
  ptfig->BEPOR     = NULL            ;
  ptfig->BEVECTPOR = NULL            ;
  ptfig->CIRCUI    = NULL            ;
  ptfig->USER      = NULL            ;
  ptfig->BEGEN     = NULL            ;
  ptfig->ERRFLG    = err_flg         ;
  ptfig->TYPE      = 0               ;
  ptfig->FLAG      = 0               ;
  ptfig->TIME_UNIT = BEH_TIMEUNIT    ;
  ptfig->NEXT      = lastbefig       ;

  return (ptfig);
}
