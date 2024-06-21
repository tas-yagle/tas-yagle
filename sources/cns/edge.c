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
 
/* 10/26/95 Cone Netlist Structure functions: edge.c                          */

#include"cnsall.h"

static void    *addedgepage();
static edge_list *getfreeedge();

/* define number of edges per page                                            */
#define EDGESPERPAGE 300

static chain_list *EDGEPAGES;   /* chain list of edge pages                  */
static int      PAGEEDGEINDEX = 0;      /* edge index in current page                */
static edge_list *FREEEDGELIST; /* list of free edges in current page        */
static edge_list *FREEDEDGELIST;        /* list of freed edges in current page       */

/*============================================================================*
 | function initedgemem();                                                    |
 | initialize edge memory structure                                           |
 *============================================================================*/
void *
initedgemem()
{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns initedgemem\n");

    ptedge = (edge_list *) mbkalloc(EDGESPERPAGE * (sizeof(edge_list)));
    EDGEPAGES = addchain((chain_list *) NULL, (void *) ptedge);
    FREEDEDGELIST = NULL;
    PAGEEDGEINDEX = 1;
    FREEEDGELIST = ptedge;

    return NULL;
}

/*============================================================================*
 | function addedgepage();                                                    |
 | add a new edge page                                                        |
 *============================================================================*/
static void *
addedgepage()
{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addedgepage\n");

    ptedge = (edge_list *) mbkalloc(EDGESPERPAGE * (sizeof(edge_list)));
    EDGEPAGES = addchain(EDGEPAGES, (void *) ptedge);
    PAGEEDGEINDEX = 1;
    FREEEDGELIST = ptedge;
    
    return NULL;
}

/*============================================================================*
 | function getfreeedge();                                                    |
 | get a new edge                                                             |
 *============================================================================*/
static edge_list *
getfreeedge()
{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getfreeedge in\n");

    if (FREEDEDGELIST != NULL) {
        ptedge = FREEDEDGELIST;
        FREEDEDGELIST = FREEDEDGELIST->NEXT;
        ptedge->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreeedge out\n");
        return (ptedge);
    }
    else if (PAGEEDGEINDEX < EDGESPERPAGE) {
        PAGEEDGEINDEX++;
        FREEEDGELIST++;
        FREEEDGELIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreeedge out\n");
        return (FREEEDGELIST);
    }
    else {
        addedgepage();
        FREEEDGELIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreeedge out\n");
        return (FREEEDGELIST);
    }
}

/*============================================================================*
 | function freeedge();                                                       |
 | free an edge                                                               |
 *============================================================================*/
void *
freeedge(ptedge)
    edge_list      *ptedge;

{

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freeedge\n");

    if (ptedge == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freeedge() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    ptedge->NEXT = FREEDEDGELIST;
    FREEDEDGELIST = ptedge;

    return NULL;
}

/*============================================================================*
 | function freeeelist();                                                     |
 | free an edge list                                                          |
 *============================================================================*/
void *
freeeelist(pteelist)
    edge_list      *pteelist;

{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freeeelist\n");

    if (pteelist == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freeeelist() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    for (ptedge = pteelist; ptedge->NEXT != NULL; ptedge = ptedge->NEXT);
    ptedge->NEXT = FREEDEDGELIST;
    FREEDEDGELIST = pteelist;

    return NULL;
}

/*============================================================================*
 | function addedge();                                                        |
 | add an element to an edge_list                                             |
 *============================================================================*/
edge_list *
addedge(ptedgehead, type, data)
    edge_list      *ptedgehead;
    long            type;
    void           *data;

{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addedge\n");

    ptedge = getfreeedge();
    ptedge->NEXT = ptedgehead;
    ptedge->TYPE = type;
    ptedge->UEDGE.PTR = data;
    ptedge->USER = NULL;
    return (ptedge);
}

/*============================================================================*
 | function deledge();                                                        |
 | delete an element from an edge list and free that element from memory      |
 *============================================================================*/
edge_list *
deledge(ptedgehead, ptedge2del)
    edge_list      *ptedgehead;
    edge_list      *ptedge2del;

{
    edge_list      *ptedge;
    edge_list      *ptedge2sav;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns deledge in\n");

    if ((ptedgehead == NULL) || (ptedge2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " deledge() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns deledge out\n");
        EXIT(-1);
    }

    if (ptedge2del == ptedgehead) {
        ptedge = ptedgehead->NEXT;
        ptedgehead->NEXT = NULL;
        freeedge(ptedgehead);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns deledge out\n");
        return (ptedge);
    }
    else {
        for (ptedge = ptedgehead; ptedge; ptedge = ptedge->NEXT) {
            if (ptedge == ptedge2del) {
                break;
            }
            ptedge2sav = ptedge;
        }
        if (ptedge != NULL) {
            ptedge2sav->NEXT = ptedge->NEXT;
            ptedge2del->NEXT = NULL;
            freeedge(ptedge2del);
        }
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns deledge out\n");
        return (ptedgehead);
    }
}

/*============================================================================*
 | function appendedge();                                                     |
 | chain two edge lists. WARNING: no consistency check                        |
 *============================================================================*/
edge_list *
appendedge(ptedge1, ptedge2)
    edge_list      *ptedge1;
    edge_list      *ptedge2;

{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendedge\n");

    if (ptedge1 == NULL) {
        return (ptedge2);
    }
    else {
        for (ptedge = ptedge1; ptedge->NEXT != NULL; ptedge = ptedge->NEXT);
        ptedge->NEXT = ptedge2;
        return (ptedge1);
    }
}

/*============================================================================*
 | function viewedge();                                                       |
 | display the content of a given edge structure                              |
 *============================================================================*/
void 
viewedge(ptedge, depth)
    edge_list      *ptedge;
    int             depth;

{
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewedge in\n");

    if (ptedge == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewedge() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    printf("    edge->NEXT      = 0x%lx\n", (unsigned long)ptedge->NEXT);
    printf("    edge->TYPE      = %ld\n", ptedge->TYPE);
    printf("    edge->UEDGE.PTR = 0x%lx\n", (unsigned long)ptedge->UEDGE.PTR);
    printf("    edge->USER      = 0x%lx\n", (unsigned long)ptedge->USER);
    if ((depth >= 1) && (ptedge->USER != NULL)) {
        for (ptptype = ptedge->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewedge out\n");
}
