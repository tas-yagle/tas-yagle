/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Dpartement ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
/* 11/21/96 Cone Netlist Structure functions: cnsfigprs.c                     */

#include"cnsall.h"

extern FILE    *cnsfigyyin;
int             cnsfiglineno;
extern int cnsfigyyrestart();
extern int cnsfigyydebug;

/*============================================================================*
 | function getloadedcnsfig();                                                |
 | searches in the list of CNS figures in memory if one of them has the name  |
 | given as argument. If it is the case this funcion sends its pointer back,  |
 | if not return NULL pointer.                                                |
 *============================================================================*/
cnsfig_list *
getloadedcnsfig(char *name)
{
    cnsfig_list    *ptcnsfig;

    for (ptcnsfig = CNS_HEADCNSFIG; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
        if (strcmp(ptcnsfig->NAME, name) == 0) break;
    }
    return (ptcnsfig);
}

/*============================================================================*
 | function getcnsfig();                                                      |
 | searches in the list of CNS figures in memory if one of them has the name  |
 | given as argument. If it is the case this funcion sends its pointer back,  |
 | if not it tries to load it from disk by calling loadcnsfig().              |
 *============================================================================*/
cnsfig_list *
getcnsfig(char *name, lofig_list *ptlofig)
{
    cnsfig_list    *ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE) (void) printf("TRA_cns getcnsfig in\n");
    for (ptcnsfig = CNS_HEADCNSFIG; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
        if (strcmp(ptcnsfig->NAME, name) == 0) break;
    }
    if (ptcnsfig == NULL) ptcnsfig = loadcnsfig(name, ptlofig );
    if (CNS_TRACE_MODE >= CNS_TRACE) (void) printf("TRA_cns getcnsfig out\n");

    if (ptcnsfig->LOFIG != NULL) cnslofigchain(ptcnsfig->LOFIG, ptcnsfig);
    return (ptcnsfig);
}

/*============================================================================*
 | function loadcnsfig();                                                     |
 | looks for the file "name.cns" first in CATA_LIB and then in WORK_LIB       |
 | before parsing the file.                                                   |
 *============================================================================*/
cnsfig_list *
loadcnsfig(char *filename, lofig_list *ptlofig)

{
    cnsfig_list    *ptcnsfig;
    
    //static int first_time=1;
    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns loadcnsfig in\n");

/* check to see if the figure already exists */
    for (ptcnsfig = CNS_HEADCNSFIG; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
        if (strcmp(ptcnsfig->NAME, filename) == 0)
            break;
    }

/* if it doesn't exists open file */
    if (ptcnsfig == NULL) {

        /* open file */
        if ((cnsfigyyin = mbkfopen(filename, "cns", READ_TEXT)) == NULL) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " loadcnsfig() impossible: can't open file %s.cns\n", filename);
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns loadcnsfig out\n");
            EXIT(-1);
        }

        /* call CNS parser (cnsfig.yac) */
        cnsfiglineno = 1;
        cnsfigyydebug = 0;
        setcnslofig(ptlofig);
	
	// zinaps le 31/12/2002
/*	if (first_time==1) first_time=0;
        else cnsfigyyrestart(); 
*/
	if (cnsfigyyparse() != 0) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " loadcnsfig() impossible: error parsing file %s.cns\n", filename);
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns loadcnsfig out\n");
            EXIT(-1);
        }

        /* close file */
        if (fclose(cnsfigyyin) != 0) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " loadcnsfig() impossible: can't close file %s.cns\n", filename);
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns loadcnsfig out\n");
            EXIT(-1);
        }

        /* check for unicity between file name and CNS figure name */
        ptcnsfig = CNS_HEADCNSFIG;
        if (strcmp(ptcnsfig->NAME, filename) != 0) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " loadcnsfig(): prefix name of file %s.cns should be\n", filename);
            (void) fprintf(stderr, " identical to CNS figure name %s\n", ptcnsfig->NAME);
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns loadcnsfig out\n");
            EXIT(-1);
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns loadcnsfig out\n");

    mbk_comcheck( 0, cns_signcns(ptcnsfig), 0 );
    return (ptcnsfig);
}
