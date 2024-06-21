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
 
/* 10/14/97 Cone Netlist Structure functions: cell.c                          */

#include"cnsall.h"

static void    *addcellpage();
static cell_list *getfreecell();

/* define number of cells per page                                            */
#define CELLSPERPAGE 50

static chain_list *CELLPAGES;   /* chain list of cell pages                  */
static int      PAGECELLINDEX = 0;      /* cell index in current page                */
static cell_list *FREECELLLIST; /* list of free cells in current page        */
static cell_list *FREEDCELLLIST;        /* list of freed cells in current page       */

/*============================================================================*
 | function initcellmem();                                                    |
 | initialize cell memory structure                                           |
 *============================================================================*/
void *
initcellmem()
{
    cell_list      *ptcell;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns initcellmem\n");

    ptcell = (cell_list *) mbkalloc(CELLSPERPAGE * (sizeof(cell_list)));
    CELLPAGES = addchain((chain_list *) NULL, (void *) ptcell);
    FREEDCELLLIST = NULL;
    PAGECELLINDEX = 1;
    FREECELLLIST = ptcell;

    return NULL;
}

/*============================================================================*
 | function addcellpage();                                                    |
 | add a new cell page                                                        |
 *============================================================================*/
static void *
addcellpage()
{
    cell_list      *ptcell;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcellpage\n");

    ptcell = (cell_list *) mbkalloc(CELLSPERPAGE * (sizeof(cell_list)));
    CELLPAGES = addchain(CELLPAGES, (void *) ptcell);
    PAGECELLINDEX = 1;
    FREECELLLIST = ptcell;

    return NULL;
}

/*============================================================================*
 | function getfreecell();                                                    |
 | get a new cell                                                             |
 *============================================================================*/
static cell_list *
getfreecell()
{
    cell_list      *ptcell;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getfreecell in\n");

    if (FREEDCELLLIST != NULL) {
        ptcell = FREEDCELLLIST;
        FREEDCELLLIST = FREEDCELLLIST->NEXT;
        ptcell->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecell out\n");
        return (ptcell);
    }
    else if (PAGECELLINDEX < CELLSPERPAGE) {
        PAGECELLINDEX++;
        FREECELLLIST++;
        FREECELLLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecell out\n");
        return (FREECELLLIST);
    }
    else {
        addcellpage();
        FREECELLLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecell out\n");
        return (FREECELLLIST);
    }
}

/*============================================================================*
 | function freecell();                                                       |
 | free a cell                                                                |
 *============================================================================*/
void *
freecell(ptcell)
    cell_list      *ptcell;

{
    chain_list     *ptchain;
    befig_list     *ptbefig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecell\n");

    if (ptcell == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freecell() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    if ((ptchain = ptcell->CONES) != NULL)
        freechain(ptchain);
    if ((ptbefig = ptcell->BEFIG) != NULL)
        ptbefig = beh_delbefig(ptbefig, ptbefig, 'Y');
    ptcell->NEXT = FREEDCELLLIST;
    FREEDCELLLIST = ptcell;

    return NULL;
}

/*============================================================================*
 | function freecllist();                                                     |
 | free a cell list                                                           |
 *============================================================================*/
void *
freecllist(ptcllist)
    cell_list      *ptcllist;

{
    cell_list      *ptcell;
    chain_list     *ptchain;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecllist\n");

    if (ptcllist == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freecllist() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    for (ptcell = ptcllist; ptcell->NEXT != NULL; ptcell = ptcell->NEXT) {
        if ((ptchain = ptcell->CONES) != NULL)
            freechain(ptchain);
    }
    if ((ptchain = ptcell->CONES) != NULL)
        freechain(ptchain);
    ptcell->NEXT = FREEDCELLLIST;
    FREEDCELLLIST = ptcllist;

    return NULL;
}

/*============================================================================*
 | function addcell();                                                        |
 | add an element to a cell list                                              |
 *============================================================================*/
cell_list *
addcell(ptcellhead, type, ptchain, ptbefig)
    cell_list      *ptcellhead;
    long            type;
    chain_list     *ptchain;
    befig_list     *ptbefig;

{
    cell_list      *ptcell;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcell in\n");

    ptcell = getfreecell();
    ptcell->NEXT = ptcellhead;
    ptcell->TYPE = type;
    ptcell->CONES = ptchain;
    ptcell->BEFIG = ptbefig;
    ptcell->USER = NULL;
    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcell out\n");
    return (ptcell);
}

/*============================================================================*
 | function delcell();                                                        |
 | delete an element from a cell list and free that element from memory       |
 *============================================================================*/
cell_list *
delcell(ptcellhead, ptcell2del)
    cell_list      *ptcellhead;
    cell_list      *ptcell2del;

{
    cell_list      *ptcell;
    cell_list      *ptcell2sav;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delcell in\n");

    if ((ptcellhead == NULL) || (ptcell2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " delcell() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcell out\n");
        EXIT(-1);
    }

    if (ptcell2del == ptcellhead) {
        ptcell = ptcellhead->NEXT;
        ptcellhead->NEXT = NULL;
        freecell(ptcellhead);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcell out\n");
        return (ptcell);
    }
    else {
        for (ptcell = ptcellhead; ptcell; ptcell = ptcell->NEXT) {
            if (ptcell == ptcell2del) {
                break;
            }
            ptcell2sav = ptcell;
        }
        if (ptcell != NULL) {
            ptcell2sav->NEXT = ptcell->NEXT;
            ptcell2del->NEXT = NULL;
            freecell(ptcell2del);
        }
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcell out\n");
        return (ptcellhead);
    }
}

/*============================================================================*
 | function appendcell();                                                     |
 | chain two cell lists. WARNING: no consistency check                        |
 *============================================================================*/
cell_list *
appendcell(ptcell1, ptcell2)
    cell_list      *ptcell1;
    cell_list      *ptcell2;

{
    cell_list      *ptcell;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendcell\n");

    if (ptcell1 == NULL) {
        return (ptcell2);
    }
    else {
        for (ptcell = ptcell1; ptcell->NEXT != NULL; ptcell = ptcell->NEXT);
        ptcell->NEXT = ptcell2;
        return (ptcell1);
    }
}

/*============================================================================*
 | function viewcell();                                                       |
 | display the content of a given cell structure                              |
 *============================================================================*/
void 
viewcell(ptcell, depth)
    cell_list      *ptcell;
    int             depth;

{
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcell in\n");

    if (ptcell == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewcell() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewcell out\n");
        EXIT(-1);
    }

    printf("    cell->NEXT  = 0x%lx\n", (unsigned long)ptcell->NEXT);
    printf("    cell->TYPE  = %ld\n", ptcell->TYPE);
    printf("    cell->CONES = 0x%lx\n", (unsigned long)ptcell->CONES);
    printf("    cell->BEFIG = 0x%lx\n", (unsigned long)ptcell->BEFIG);
    printf("    cell->USER  = 0x%lx\n", (unsigned long)ptcell->USER);
    if ((depth >= 1) && (ptcell->USER != NULL)) {
        for (ptptype = ptcell->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcell out\n");
}
