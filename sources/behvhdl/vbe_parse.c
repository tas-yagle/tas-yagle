
/* ###--------------------------------------------------------------### */
/* file		: vbe_parse.c						*/
/* date		: Oct 30 1995						*/
/* version	: v201						*/
/* author	: L.A TABUSSE & H.N. VUONG & P. BAZARGAN-SABET		*/
/* content	: vbeloadbefig						*/
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

extern void vbe_bcomprestart(FILE *inputfile);

/* ###--------------------------------------------------------------### */
/* function	: vbeloadbefig						*/
/* description	: analyze a behavioural VHDL description and produce a	*/
/*		  set of data structure. The function returns a pointer	*/
/*		  on a BEFIG. Errors are repported in the returned	*/
/*		  structure.						*/
/* ###--------------------------------------------------------------### */

struct befig *
vbeloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode)
{
  unsigned int          check_mode ;
  char                 *suffix     ;
  char                 *str        ;
  static unsigned int   call_nbr   = 0   ;

  ptbefig = NULL; /* unused parameter */
  suffix = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = namealloc("vbe");

  if ((vbe_bcompin = mbkfopen (figname, suffix, READ_TEXT)) != NULL)
    {
    sprintf (BVL_CURFIL, "%s.%s", figname, suffix);
    }

  if (vbe_bcompin == NULL)
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

  if ((trace_mode & BVL_KEEPAUX) != 0) BVL_AUXMOD = 1;
  else BVL_AUXMOD = 0;

  BEH_LINNUM = 1;

  if (call_nbr != 0) vbe_bcomprestart (vbe_bcompin);
  call_nbr++;

  vbe_bcompparse ();
  fclose (vbe_bcompin);
  vbe_bcompin = NULL;

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

  BVL_HEDFIG->ERRFLG = beh_chkbefig (BVL_HEDFIG, check_mode);

  // fprintf( stdout, "BVL (vbe) : parse completed\n");
  return (BVL_HEDFIG);
}
