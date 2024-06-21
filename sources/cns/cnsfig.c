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
 
/* 10/14/97 Cone Netlist Structure functions: cnsfig.c                        */

#include"cnsall.h"
#include BEG_H

static void    *addcnsfigpage();
static cnsfig_list *getfreecnsfig();

/* define number of cnsfigs per page                                          */
#define CNSFIGSPERPAGE 50

static chain_list *CNSFIGPAGES; /* chain list of cnsfig pages          */
static int      PAGECNSFIGINDEX = 0;    /* cnsfig index in current page        */
static cnsfig_list *FREECNSFIGLIST;     /* list of free cnsfigs in current page */
static cnsfig_list *FREEDCNSFIGLIST;    /* list of freed cnsfigs in current page */

/*============================================================================*
 | function initcnsfigmem();                                                  |
 | initialize cnsfig memory structure                                         |
 *============================================================================*/
void *
initcnsfigmem()
{
    cnsfig_list    *ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns initcnsfigmem\n");

    ptcnsfig = (cnsfig_list *) mbkalloc(CNSFIGSPERPAGE * (sizeof(cnsfig_list)));
    CNSFIGPAGES = addchain((chain_list *) NULL, (void *) ptcnsfig);
    FREEDCNSFIGLIST = NULL;
    PAGECNSFIGINDEX = 1;
    FREECNSFIGLIST = ptcnsfig;

    return NULL;
}

/*============================================================================*
 | function addcnsfigpage();                                                  |
 | add a new cnsfig page                                                      |
 *============================================================================*/
static void *
addcnsfigpage()
{
    cnsfig_list    *ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnsfigpage\n");

    ptcnsfig = (cnsfig_list *) mbkalloc(CNSFIGSPERPAGE * (sizeof(cnsfig_list)));
    CNSFIGPAGES = addchain(CNSFIGPAGES, (void *) ptcnsfig);
    PAGECNSFIGINDEX = 1;
    FREECNSFIGLIST = ptcnsfig;

    return NULL;
}

/*============================================================================*
 | function getfreecnsfig();                                                  |
 | get a new cnsfig                                                           |
 *============================================================================*/
static cnsfig_list *
getfreecnsfig()
{
    cnsfig_list    *ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getfreecnsfig in\n");

    if (FREEDCNSFIGLIST != NULL) {
        ptcnsfig = FREEDCNSFIGLIST;
        FREEDCNSFIGLIST = FREEDCNSFIGLIST->NEXT;
        ptcnsfig->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecnsfig out\n");
        return (ptcnsfig);
    }
    else if (PAGECNSFIGINDEX < CNSFIGSPERPAGE) {
        PAGECNSFIGINDEX++;
        FREECNSFIGLIST++;
        FREECNSFIGLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecnsfig out\n");
        return (FREECNSFIGLIST);
    }
    else {
        addcnsfigpage();
        FREECNSFIGLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecnsfig out\n");
        return (FREECNSFIGLIST);
    }
}

/*============================================================================*
 | function freecnsfig();                                                     |
 | free a cnsfig                                                              |
 *============================================================================*/
void *
freecnsfig(ptcnsfig)
    cnsfig_list    *ptcnsfig;

{
    cone_list      *ptcelist;
    cell_list      *ptcllist;
    lofig_list     *ptlofig;
    short           result;
    befig_list     *ptbefig;
    ptype_list *pt;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecnsfig in\n");

    if (ptcnsfig == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freecnsfig() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns freecnsfig out\n");
        EXIT(-1);
    }

    if ((pt=getptype(ptcnsfig->USER, CNS_CONE_HASHTABLE))!=NULL)
       delht((ht *)pt->DATA);

    if ((ptcelist = ptcnsfig->CONE) != NULL)
        freecelist(ptcelist);
    if ((ptcllist = ptcnsfig->CELL) != NULL)
        freecllist(ptcllist);
    if ((ptlofig = ptcnsfig->LOFIG) != NULL)
        result = dellofig(ptcnsfig->NAME);
    if ((ptbefig = ptcnsfig->BEFIG) != NULL)
        beg_delBefig(ptbefig);
//        ptbefig = beh_delbefig(ptbefig, ptbefig, 'Y');
    freeptype(ptcnsfig->USER);
    ptcnsfig->NEXT = FREEDCNSFIGLIST;
    FREEDCNSFIGLIST = ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecnsfig out\n");
    return NULL;
}

/*============================================================================*
 | function freecglist();                                                     |
 | free a cnsfig list                                                         |
 *============================================================================*/
void *
freecglist(ptcglist)
    cnsfig_list    *ptcglist;

{
    cnsfig_list    *ptcnsfig;
    cone_list      *ptcelist;
    cell_list      *ptcllist;
    lofig_list     *ptlofig;
    short           result;
    befig_list     *ptbefig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecglist in\n");

    if (ptcglist == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freecglist() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns freecglist out\n");
        EXIT(-1);
    }

    for (ptcnsfig = ptcglist; ptcnsfig->NEXT != NULL; ptcnsfig = ptcnsfig->NEXT) {
        if ((ptcelist = ptcnsfig->CONE) != NULL)
            freecelist(ptcelist);
        if ((ptcllist = ptcnsfig->CELL) != NULL)
            freecllist(ptcllist);
        if ((ptlofig = ptcnsfig->LOFIG) != NULL)
            result = dellofig(ptcnsfig->NAME);
        if ((ptbefig = ptcnsfig->BEFIG) != NULL)
            ptbefig = beh_delbefig(ptbefig, ptbefig, 'Y');
    }
    if ((ptcelist = ptcnsfig->CONE) != NULL)
        freecelist(ptcelist);
    if ((ptcllist = ptcnsfig->CELL) != NULL)
        freecllist(ptcllist);
    if ((ptlofig = ptcnsfig->LOFIG) != NULL)
        result = dellofig(ptcnsfig->NAME);
    if ((ptbefig = ptcnsfig->BEFIG) != NULL)
        ptbefig = beh_delbefig(ptbefig, ptbefig, 'Y');
    ptcnsfig->NEXT = FREECNSFIGLIST;
    FREEDCNSFIGLIST = ptcglist;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecglist out\n");

    return NULL;
}

/*============================================================================*
 | function addcnsfig();                                                      |
 | add an element to a cnsfig list                                            |
 *============================================================================*/
cnsfig_list *
addcnsfig(ptcnsfighead, name, ptlocon, ptintcon, ptlotrs, ptloins, ptcone, ptcell, ptlofig, ptbefig)
    cnsfig_list    *ptcnsfighead;
    char           *name;
    locon_list     *ptlocon;
    locon_list     *ptintcon;
    lotrs_list     *ptlotrs;
    loins_list     *ptloins;
    cone_list      *ptcone;
    cell_list      *ptcell;
    lofig_list     *ptlofig;
    befig_list     *ptbefig;

{
    cnsfig_list    *ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnsfig in\n");

    for (ptcnsfig = ptcnsfighead; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
        if (strcmp(ptcnsfig->NAME, name) == 0) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " addcnsfig() impossible: figure %s already exists\n", ptcnsfig->NAME);
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns addcnsfig out\n");
            EXIT(-1);
        }
    }

    ptcnsfig = getfreecnsfig();
    ptcnsfig->NEXT = ptcnsfighead;
    ptcnsfig->NAME = namealloc(name);
    ptcnsfig->LOCON = ptlocon;
    ptcnsfig->INTCON = ptintcon;
    ptcnsfig->LOTRS = ptlotrs;
    ptcnsfig->LOINS = ptloins;
    ptcnsfig->CONE = ptcone;
    ptcnsfig->CELL = ptcell;
    ptcnsfig->LOFIG = ptlofig;
    ptcnsfig->BEFIG = ptbefig;
    ptcnsfig->USER = NULL;
    CNS_HEADCNSFIG = ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnsfig out\n");
    return (ptcnsfig);
}

/*============================================================================*
 | function delcnsfig();                                                      |
 | delete an element from a cnsfig list and free that element from memory     |
 *============================================================================*/
cnsfig_list *
delcnsfig(ptcnsfighead, ptcnsfig2del)
    cnsfig_list    *ptcnsfighead;
    cnsfig_list    *ptcnsfig2del;

{
    cnsfig_list    *ptcnsfig;
    cnsfig_list    *ptcnsfig2sav;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delcnsfig in\n");

    if ((ptcnsfighead == NULL) || (ptcnsfig2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " delcnsfig() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcnsfig out\n");
        EXIT(-1);
    }

    if (ptcnsfig2del == ptcnsfighead) {
        ptcnsfig = ptcnsfighead->NEXT;
        ptcnsfighead->NEXT = NULL;
        freecnsfig(ptcnsfighead);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcnsfig out\n");
        return (ptcnsfig);
    }
    else {
        for (ptcnsfig = ptcnsfighead; ptcnsfig; ptcnsfig = ptcnsfig->NEXT) {
            if (ptcnsfig == ptcnsfig2del) {
                break;
            }
            ptcnsfig2sav = ptcnsfig;
        }
        if (ptcnsfig != NULL) {
            ptcnsfig2sav->NEXT = ptcnsfig->NEXT;
            ptcnsfig2del->NEXT = NULL;
            freecnsfig(ptcnsfig2del);
        }
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcnsfig out\n");
        return (ptcnsfighead);
    }
}

/*============================================================================*
 | function appendcnsfig();                                                   |
 | chain two cnsfig lists. WARNING: no consistency check                      |
 *============================================================================*/
cnsfig_list *
appendcnsfig(ptcnsfig1, ptcnsfig2)
    cnsfig_list    *ptcnsfig1;
    cnsfig_list    *ptcnsfig2;

{
    cnsfig_list    *ptcnsfig;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendcnsfig\n");

    if (ptcnsfig1 == NULL) {
        return (ptcnsfig2);
    }
    else {
        for (ptcnsfig = ptcnsfig1; ptcnsfig->NEXT != NULL; ptcnsfig = ptcnsfig->NEXT);
        ptcnsfig->NEXT = ptcnsfig2;
        return (ptcnsfig1);
    }
}

/*============================================================================*
 | function addconelotrsfig();                                                |
 | complete the CNS_LOTRS USER field of all cones in the figure               |
 *============================================================================*/
void 
addconelotrsfig(ptcnsfig)
    cnsfig_list    *ptcnsfig;

{
    cone_list      *ptcone;
    
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        addconelotrs(ptcone);
    }
}

/*============================================================================*
 | function cnsfigchain();                                                    |
 | complete the CNS_CONE USER field of all transistors of the figure          |
 | and the CNS_LOTRS USER field of all cones                                  |
 *============================================================================*/
void 
cnsfigchain(ptcnsfig)
    cnsfig_list    *ptcnsfig;

{
    cone_list      *ptcone;
    chain_list     *ptchain;
    lotrs_list     *ptlotrs;
    ptype_list     *ptuser;

    addconelotrsfig(ptcnsfig);
    
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptuser = getptype(ptcone->USER, CNS_LOTRS);
        if (ptuser != NULL) {
            for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlotrs = (lotrs_list *)ptchain->DATA;
                cone2lotrs(ptcone, ptlotrs);
            }
        }
    }
}

/*============================================================================*
 | function freelotrsconefig();                                               |
 | free the CNS_CONE USER field of all transistors of the figure              |
 *============================================================================*/
void 
freelotrsconefig(ptcnsfig)
    cnsfig_list    *ptcnsfig;

{
    lotrs_list     *ptlotrs;
    ptype_list     *ptuser;
    
    for (ptlotrs = ptcnsfig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        ptuser = getptype(ptlotrs->USER, CNS_CONE);
        if (ptuser != NULL) {
            if (ptuser->DATA != NULL) freechain((chain_list *)ptuser->DATA);
            ptlotrs->USER = delptype(ptlotrs->USER, CNS_CONE);
        }
    }    
}

/*============================================================================*
 | function freeconelotrsfig();                                               |
 | free the CNS_LOTRS USER field of all cones of the figure                   |
 *============================================================================*/
void 
freeconelotrsfig(ptcnsfig)
    cnsfig_list    *ptcnsfig;

{
    cone_list      *ptcone;
    
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        freeconelotrs(ptcone);
    }
}

/*============================================================================*
 | function viewcnsfig();                                                     |
 | display the content of a given cnsfig structure                            |
 *============================================================================*/
void 
viewcnsfig(ptcnsfig, depth)
    cnsfig_list    *ptcnsfig;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    locon_list     *ptlocon;
    lotrs_list     *ptlotrs;
    cone_list      *ptcone;
    cell_list      *ptcell;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnsfig in\n");

    if (depth < 0) {
        depth = CNS_CF_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%scnsfig->NEXT  = 0x%lx\n", margin, (unsigned long)ptcnsfig->NEXT);
    printf("%scnsfig->NAME  = %s\n", margin, ptcnsfig->NAME);
    printf("%scnsfig->LOCON = 0x%lx\n", margin, (unsigned long)ptcnsfig->LOCON);
    if ((depth >= 1) && (ptcnsfig->LOCON != NULL)) {
        for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            viewcnslocon(ptlocon, depth - 1);
        }
    }
    printf("%scnsfig->LOTRS = 0x%lx\n", margin, (unsigned long)ptcnsfig->LOTRS);
    if ((depth >= 1) && (ptcnsfig->LOTRS != NULL)) {
        for (ptlotrs = ptcnsfig->LOTRS; ptlotrs != NULL; ptlotrs = ptlotrs->NEXT) {
            viewcnslotrs(ptlotrs, depth - 1);
        }
    }
    printf("%scnsfig->CONE  = 0x%lx\n", margin, (unsigned long)ptcnsfig->CONE);
    if ((depth >= 1) && (ptcnsfig->CONE != NULL)) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            viewcone(ptcone, depth - 1);
        }
    }
    printf("%scnsfig->CELL  = 0x%lx\n", margin, (unsigned long)ptcnsfig->CELL);
    if ((depth >= 1) && (ptcnsfig->CELL != NULL)) {
        printf("%s", margin);
        for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {
            viewcell(ptcell, depth - 1);
        }
    }
    printf("%scnsfig->LOFIG = 0x%lx\n", margin, (unsigned long)ptcnsfig->LOFIG);
    printf("%scnsfig->BEFIG = 0x%lx\n", margin, (unsigned long)ptcnsfig->BEFIG);
    printf("%scnsfig->USER  = 0x%lx\n", margin, (unsigned long)ptcnsfig->USER);
    if ((depth >= 1) && (ptcnsfig->USER != NULL)) {
        for (ptptype = ptcnsfig->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnsfig out\n");
}

/*============================================================================*
 | function cnslofigchain();                                                  |
 | create correct lofigchain including internal connectors                    |
 *============================================================================*/
void
cnslofigchain(ptlofig, ptcnsfig)
    lofig_list  *ptlofig;
    cnsfig_list *ptcnsfig;
{
    locon_list  *ptcon;
    ptype_list  *ptuser;

    lofigchain(ptlofig);
    for (ptcon = ptcnsfig->INTCON; ptcon; ptcon = ptcon->NEXT) {
        ptuser = getptype(ptcon->SIG->USER, LOFIGCHAIN);
        if (ptuser != NULL) {
            ptuser->DATA = addchain(ptuser->DATA, ptcon);
        }
    }
}

