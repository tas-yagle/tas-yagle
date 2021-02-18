/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - DŽpartement ASIM
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
 
/* 10/26/95 Cone Netlist Structure functions: link.c                          */

#include"cnsall.h"

static void    *addlinkpage();
static link_list *getfreelink();

/* define number of links per page                                            */
#define LINKSPERPAGE 300

static chain_list *LINKPAGES;   /* chain list of link pages                  */
static int      PAGELINKINDEX = 0;      /* link index in current page                */
static link_list *FREELINKLIST; /* list of free links in current page        */
static link_list *FREEDLINKLIST;        /* list of freed links in current page       */

/*============================================================================*
 | function initlinkmem();                                                    |
 | initialize link memory structure                                           |
 *============================================================================*/
void *
initlinkmem()
{
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns initlinkmem\n");

    ptlink = (link_list *) mbkalloc(LINKSPERPAGE * (sizeof(link_list)));

    LINKPAGES = addchain((chain_list *) NULL, (void *) ptlink);
    FREEDLINKLIST = NULL;
    PAGELINKINDEX = 1;
    FREELINKLIST = ptlink;

    return NULL;
}

/*============================================================================*
 | function addlinkpage();                                                    |
 | add a new link page                                                        |
 *============================================================================*/
static void *
addlinkpage()
{
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addlinkpage\n");

    ptlink = (link_list *) mbkalloc(LINKSPERPAGE * (sizeof(link_list)));

    LINKPAGES = addchain(LINKPAGES, (void *) ptlink);
    PAGELINKINDEX = 1;
    FREELINKLIST = ptlink;

    return NULL;
}

/*============================================================================*
 | function getfreelink();                                                    |
 | get a new link                                                             |
 *============================================================================*/
static link_list *
getfreelink()
{
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getfreelink in\n");

    if (FREEDLINKLIST != NULL) {
        ptlink = FREEDLINKLIST;
        FREEDLINKLIST = FREEDLINKLIST->NEXT;
        ptlink->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreelink out\n");
        return (ptlink);
    }
    else if (PAGELINKINDEX < LINKSPERPAGE) {
        PAGELINKINDEX++;
        FREELINKLIST++;
        FREELINKLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreelink out\n");
        return (FREELINKLIST);
    }
    else {
        addlinkpage();
        FREELINKLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreelink out\n");
        return (FREELINKLIST);
    }
}

/*============================================================================*
 | function freelink();                                                       |
 | free a link                                                                |
 *============================================================================*/
void *
freelink(ptlink)
    link_list      *ptlink;

{

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freelink\n");

    if (ptlink == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freelink() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    ptlink->NEXT = FREEDLINKLIST;
    FREEDLINKLIST = ptlink;

    return NULL;
}

/*============================================================================*
 | function freelklist();                                                     |
 | free a link list                                                           |
 *============================================================================*/
void *
freelklist(ptlklist)
    link_list      *ptlklist;

{
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freelklist\n");

    if (ptlklist == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freelklist() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    for (ptlink = ptlklist; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
    ptlink->NEXT = FREEDLINKLIST;
    FREEDLINKLIST = ptlklist;

    return NULL;
}

/*============================================================================*
 | function addlink();                                                        |
 | add an element to a link list                                              |
 *============================================================================*/
link_list *
addlink(ptlinkhead, type, data, ptsig)
    link_list      *ptlinkhead;
    long            type;
    void           *data;
    losig_list     *ptsig;

{
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addlink in\n");

    ptlink = getfreelink();
    ptlink->NEXT = ptlinkhead;
    ptlink->TYPE = type;
    ptlink->ULINK.PTR = data;
    ptlink->SIG = ptsig;
    ptlink->USER = NULL;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addlink out\n");
    return (ptlink);
}

/*============================================================================*
 | function dellink();                                                        |
 | delete an element from a link list and free that element from memory       |
 *============================================================================*/
link_list *
dellink(ptlinkhead, ptlink2del)
    link_list      *ptlinkhead;
    link_list      *ptlink2del;

{
    link_list      *ptlink;
    link_list      *ptlink2sav;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns dellink in\n");

    if ((ptlinkhead == NULL) || (ptlink2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " dellink() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns dellink out\n");
        EXIT(-1);
    }

    if (ptlink2del == ptlinkhead) {
        ptlink = ptlinkhead->NEXT;
        ptlinkhead->NEXT = NULL;
        freelink(ptlinkhead);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns dellink out\n");
        return (ptlink);
    }
    else {
        for (ptlink = ptlinkhead; ptlink; ptlink = ptlink->NEXT) {
            if (ptlink == ptlink2del) {
                break;
            }
            ptlink2sav = ptlink;
        }
        if (ptlink != NULL) {
            ptlink2sav->NEXT = ptlink->NEXT;
            ptlink2del->NEXT = NULL;
            freelink(ptlink2del);
        }
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns dellink out\n");
        return (ptlinkhead);
    }
}

/*============================================================================*
 | function appendlink();                                                     |
 | chain two link lists. WARNING: no consistency check                        |
 *============================================================================*/
link_list *
appendlink(ptlink1, ptlink2)
    link_list      *ptlink1;
    link_list      *ptlink2;

{
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendlink\n");

    if (ptlink1 == NULL) {
        return (ptlink2);
    }
    else {
        for (ptlink = ptlink1; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
        ptlink->NEXT = ptlink2;
        return (ptlink1);
    }
}

/*============================================================================*
 | function viewlink();                                                       |
 | display the content of a given link structure                              |
 *============================================================================*/
void 
viewlink(ptlink, depth)
    link_list      *ptlink;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewlink in\n");

    if (ptlink == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewlink() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewlink out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_LK_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%slink->NEXT      = 0x%lx\n", margin, (unsigned long)ptlink->NEXT);
    printf("%slink->TYPE      = 0x%lx\n", margin, (unsigned long)ptlink->TYPE);
    printf("%slink->ULINK.PTR = 0x%lx\n", margin, (unsigned long)ptlink->ULINK.PTR);
    if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
        if ((depth >= 1) && (ptlink->ULINK.LOTRS != NULL)) {
            viewcnslotrs(ptlink->ULINK.LOTRS, depth - 1);
        }
    }
    else {
        if ((depth >= 1) && (ptlink->ULINK.LOCON != NULL)) {
            viewcnslocon(ptlink->ULINK.LOCON, depth - 1);
        }
    }
    printf("%slink->USER  = 0x%lx\n", margin, (unsigned long)ptlink->USER);
    if ((depth >= 1) && (ptlink->USER != NULL)) {
        for (ptptype = ptlink->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewlink out\n");
}

/*============================================================================*
 | function lotrsnum();                                                       |
 | returns the number of series transistors in the input link list            |
 *============================================================================*/
short 
lotrsnum(ptlklist)
    link_list      *ptlklist;

{
    link_list      *ptlink;
    short           count = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns lotrsnum\n");

    for (ptlink = ptlklist; ptlink != NULL; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & CNS_EXT) != CNS_EXT)
            count++;
    }

    return (count);
}
