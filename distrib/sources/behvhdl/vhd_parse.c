
/* ###--------------------------------------------------------------### */
/*									*/
/* file		: vhd_parse.c						*/
/* date		: Oct 30 1995						*/
/* version	: v11							*/
/* author	: L.A TABUSSE & H.N. VUONG & P. BAZARGAN-SABET		*/
/* content	: vhdlloadbefig						*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include AVT_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BVL_H
#include "bvl_utype.h"
#include "bvl_utdef.h"
#include "bvl_parse.h"


extern void vhd_bcomprestart (FILE *inputfile);



char *vhdfindbefig(char *name)
{
  char                 *suffix     ;
  suffix = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = namealloc("vhd");

  return filepath(name, suffix);
}

/* ###--------------------------------------------------------------### */
/* function	: vhdloadbefig						*/
/* description	: analyze a behavioural VHDL description and produce a	*/
/*		  set of data structure. The function returns a pointer	*/
/*		  on a BEFIG. Errors are repported in the returned	*/
/*		  structure.						*/
/* ###--------------------------------------------------------------### */

struct befig *
vhdloadbefig (befig_list *pt_befig, char *figname, unsigned int trace_mode)
{
  unsigned int          check_mode ;
  char                 *suffix     ;
  char                 *str        ;
  static unsigned int   call_nbr   = 0   ;

  // added to prevent warning
  if (pt_befig) call_nbr =0 ;

  suffix = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = namealloc("vhd");

  if ((vhd_bcompin = mbkfopen (figname, suffix, READ_TEXT)) != NULL)
    {
    sprintf (BVL_CURFIL, "%s.%s", figname, suffix);
    }

  if (vhd_bcompin == NULL)
    {
    beh_error (100, figname);
    EXIT (1);
    }

	/* ###------------------------------------------------------### */
	/*    call the compiler on the current file			*/
	/*      - print a message if the trace mode is actif		*/
	/*      - add internal signals to the primary input list if	*/
	/*        "keep auxiliary" mode is actif			*/
	/* ###------------------------------------------------------### */

  if ((trace_mode & BVL_TRACE) != 0)
    beh_message (3, BVL_CURFIL);

  if ((trace_mode & BVL_KEEPAUX) != 0)
    BVL_AUXMOD = 1;
  else
    BVL_AUXMOD = 0;

  BEH_LINNUM = 1;

  if ((trace_mode & BVL_CHECKEMPTY) != 0)
    BVL_CHECK = 0;
  else
    BVL_CHECK = 1;

  if (call_nbr != 0)
    vhd_bcomprestart (vhd_bcompin);
  call_nbr++;

  vhd_bcompparse ();
  fclose (vhd_bcompin);
  vhd_bcompin = NULL;

	/* ###------------------------------------------------------### */
	/*    check the consistency of the compiled description		*/
	/* ###------------------------------------------------------### */

  if ((trace_mode & BVL_CHECKEMPTY) != 0)
    {
    check_mode       = BEH_CHK_EMPTY;
    BVL_HEDFIG->FLAG = BEH_ARC_C    ;
    }
  else
    {
    check_mode       = BEH_CHK_DRIVERS;
    BVL_HEDFIG->FLAG = BEH_ARC_VHDL   ;
    }

  if (BVL_HEDFIG->ERRFLG == 0)
    BVL_HEDFIG->ERRFLG = beh_chkbefig (BVL_HEDFIG, check_mode);

  // fprintf( stdout, "BVL (vhd) : parse completed\n");
  return (BVL_HEDFIG);
}
