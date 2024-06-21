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
 
/* 10/26/95 Cone Netlist Structure functions: branch.c                        */

#include"cnsall.h"

static void    *addbranchpage();
static branch_list *getfreebranch();

/* define number of branches per page                                         */
#define BRANCHESPERPAGE 200

static chain_list *BRANCHPAGES; /* chain list of branch pages          */
static int      PAGEBRANCHINDEX = 0;    /* branch index in current page        */
static branch_list *FREEBRANCHLIST;     /* list of free branches in current page */
static branch_list *FREEDBRANCHLIST;    /* list of freed branches              */

/*============================================================================*
 | function initbranchmem();                                                  |
 | initialize branch memory structure                                         |
 *============================================================================*/
void *
initbranchmem()
{
    branch_list    *ptbranch;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns initbranchmem\n");

    ptbranch = (branch_list *) mbkalloc(BRANCHESPERPAGE * (sizeof(branch_list)));
    BRANCHPAGES = addchain((chain_list *) NULL, (void *) ptbranch);
    FREEDBRANCHLIST = NULL;
    PAGEBRANCHINDEX = 1;
    FREEBRANCHLIST = ptbranch;

    return NULL;
}

/*============================================================================*
 | function addbranchpage();                                                  |
 | add a new branch page                                                      |
 *============================================================================*/
static void *
addbranchpage()
{
    branch_list    *ptbranch;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addbranchpage\n");

    ptbranch = (branch_list *) mbkalloc(BRANCHESPERPAGE * (sizeof(branch_list)));
    BRANCHPAGES = addchain(BRANCHPAGES, (void *) ptbranch);
    PAGEBRANCHINDEX = 1;
    FREEBRANCHLIST = ptbranch;

    return NULL;
}

/*============================================================================*
 | function getfreebranch();                                                  |
 | get a new branch                                                           |
 *============================================================================*/
static branch_list *
getfreebranch()
{
    branch_list    *ptbranch;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getfreebranch in\n");

    if (FREEDBRANCHLIST != NULL) {
        ptbranch = FREEDBRANCHLIST;
        FREEDBRANCHLIST = FREEDBRANCHLIST->NEXT;
        ptbranch->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreebranch out\n");
        return (ptbranch);
    }
    else if (PAGEBRANCHINDEX < BRANCHESPERPAGE) {
        PAGEBRANCHINDEX++;
        FREEBRANCHLIST++;
        FREEBRANCHLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreebranch out\n");
        return (FREEBRANCHLIST);
    }
    else {
        addbranchpage();
        FREEBRANCHLIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreebranch out\n");
        return (FREEBRANCHLIST);
    }
}

/*============================================================================*
 | function freebranch();                                                     |
 | free a branch                                                              |
 *============================================================================*/
void *
freebranch(ptbranch)
    branch_list    *ptbranch;

{
    link_list      *ptlklist;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freebranch in\n");

    if (ptbranch == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freebranch() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns freebranch out\n");
        EXIT(-1);
    }

    if ((ptlklist = ptbranch->LINK) != NULL)
        freelklist(ptlklist);
    ptbranch->NEXT = FREEDBRANCHLIST;
    FREEDBRANCHLIST = ptbranch;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freebranch out\n");

    return NULL;
}

/*============================================================================*
 | function freebrlist();                                                     |
 | free a branch list                                                         |
 *============================================================================*/
void *
freebrlist(ptbrlist)
    branch_list    *ptbrlist;

{
    branch_list    *ptbranch;
    link_list      *ptlklist;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freebrlist in\n");

    if (ptbrlist == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freebrlist() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns freebrlist out\n");
        EXIT(-1);
    }

    for (ptbranch = ptbrlist; ptbranch->NEXT != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptlklist = ptbranch->LINK) != NULL)
            freelklist(ptlklist);
    }
    if ((ptlklist = ptbranch->LINK) != NULL)
        freelklist(ptlklist);
    ptbranch->NEXT = FREEDBRANCHLIST;
    FREEDBRANCHLIST = ptbrlist;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freebrlist out\n");

    return NULL;
}

/*============================================================================*
 | function addbranch();                                                      |
 | add an element to a branch list                                            |
 *============================================================================*/
branch_list *
addbranch(ptbranchhead, type, ptlink)
    branch_list    *ptbranchhead;
    long            type;
    link_list      *ptlink;

{
    branch_list    *ptbranch;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addbranch in\n");

    ptbranch = getfreebranch();
    ptbranch->NEXT = ptbranchhead;
    ptbranch->TYPE = type;
    ptbranch->LINK = ptlink;
    ptbranch->USER = NULL;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addbranch out\n");
    return (ptbranch);
}

/*============================================================================*
 | function addconebranch();                                                  |
 | add a branch to a cone and update its INCONE field and the OUTCONE fields  |
 | of the respective driving cones. WARNING: the cone's information on        |
 | parallel branches is not updated.                                          |
 *============================================================================*/
void 
addconebranch(ptcone, ptconebr, type, ptlink)
    cone_list      *ptcone;
    branch_list    *ptconebr;
    long            type;
    link_list      *ptlink;

{
    branch_list    *ptbranch;
    link_list      *ptlink1;
    lotrs_list     *ptlotrs1;
    ptype_list     *ptptype1;
    cone_list      *ptcone1;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addconebranch in\n");

    ptbranch = getfreebranch();
    ptbranch->NEXT = ptconebr;
    ptbranch->TYPE = type;
    ptbranch->LINK = ptlink;
    ptbranch->USER = NULL;
    ptconebr = ptbranch;

    for (ptlink1 = ptlink; ptlink1 != NULL; ptlink1 = ptlink1->NEXT) {
        if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
            ptlotrs1 = ptlink1->ULINK.LOTRS;
            if ((ptptype1 = getptype(ptlotrs1->USER, (long) CNS_DRIVINGCONE)) != NULL) {
                ptcone1 = (cone_list *) ptptype1->DATA;
                addincone(ptcone, (long) CNS_CONE, (void *) ptcone1);
                addoutcone(ptcone1, (long) CNS_CONE, (void *) ptcone);
            }
            else {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addconebranch() impossible: no cone drives transistor !\n");
                if (CNS_TRACE_MODE >= CNS_TRACE)
                    (void) printf("TRA_cns addconebranch out\n");
                EXIT(-1);
            }
        }
        else if ((ptlink1->TYPE & CNS_EXT) == CNS_EXT) {
            addincone(ptcone, (long) CNS_EXT, (void *) ptlink1->ULINK.LOCON);
        }
        else if ((ptlink1->TYPE & CNS_PASSIVE) != CNS_PASSIVE) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " addconebranch() impossible: unknown link type !\n");
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns addconebranch out\n");
            EXIT(-1);
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addconebranch out\n");
}

/*============================================================================*
 | function delbranch();                                                      |
 | delete an element from a branch list and free that element from memory     |
 *============================================================================*/
branch_list *
delbranch(ptbranchhead, ptbranch2del)
    branch_list    *ptbranchhead;
    branch_list    *ptbranch2del;

{
    short           flag = 0;
    branch_list    *ptbranch;
    chain_list     *ptchain;
    branch_list    *ptbranch2sav;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delbranch in\n");

    if ((ptbranchhead == NULL) || (ptbranch2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " delbranch() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delbranch out\n");
        EXIT(-1);
    }

    if ((ptbranch2del->TYPE & CNS_PARALLEL) == CNS_PARALLEL) {
        if ((ptbranch2del->TYPE & CNS_PARALLEL_INS) != CNS_PARALLEL_INS)
            flag = 1;
    }

    if (ptbranch2del == ptbranchhead) {
        ptbranch = ptbranchhead->NEXT;
        ptbranchhead->NEXT = NULL;
        freebranch(ptbranchhead);
        if (flag)
            ptchain = parabrs(ptbranch);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delbranch out\n");
        return (ptbranch);
    }
    else {
        for (ptbranch = ptbranchhead; ptbranch; ptbranch = ptbranch->NEXT) {
            if (ptbranch == ptbranch2del) {
                break;
            }
            ptbranch2sav = ptbranch;
        }
        if (ptbranch != NULL) {
            ptbranch2sav->NEXT = ptbranch->NEXT;
            ptbranch2del->NEXT = NULL;
            freebranch(ptbranch2del);
        }
        if (flag)
            ptchain = parabrs(ptbranchhead);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delbranch out\n");
        return (ptbranchhead);
    }
}

/*============================================================================*
 | function delconebranch();                                                  |
 | delete a branch from a cone and update its INCONE field and the OUTCONE    |
 | fields of the respective driving cones                                     |
 *============================================================================*/
void 
delconebranch(ptcone, ptconebr, ptbranch2del)
    cone_list      *ptcone;
    branch_list    *ptconebr;
    branch_list    *ptbranch2del;

{
    short           flag = 0;
    branch_list    *ptbranch;
    branch_list    *value = NULL;
    branch_list    *ptbranch2sav;
    link_list      *ptlink1;
    lotrs_list     *ptlotrs1;
    ptype_list     *ptptype1;
    cone_list      *ptcone1;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delconebranch in\n");

    if ((ptconebr == NULL) || (ptbranch2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " delconebranch() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delconebranch out\n");
        EXIT(-1);
    }

    if ((ptbranch2del->TYPE & CNS_PARALLEL) == CNS_PARALLEL) {
        if ((ptbranch2del->TYPE & CNS_PARALLEL_INS) != CNS_PARALLEL_INS)
            flag = 1;
    }

    if (ptbranch2del == ptconebr) {
        ptbranch = ptconebr->NEXT;
        ptconebr->NEXT = NULL;
        value = ptconebr;
        ptconebr = ptbranch;
    }
    else {
        for (ptbranch = ptconebr; ptbranch; ptbranch = ptbranch->NEXT) {
            if (ptbranch == ptbranch2del) {
                break;
            }
            ptbranch2sav = ptbranch;
        }
        if (ptbranch != NULL) {
            ptbranch2sav->NEXT = ptbranch->NEXT;
            ptbranch2del->NEXT = NULL;
            value = ptbranch2del;
        }
    }

    for (ptlink1 = ptbranch2del->LINK; ptlink1 != NULL; ptlink1 = ptlink1->NEXT) {
        if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
            ptlotrs1 = ptlink1->ULINK.LOTRS;
            if ((ptptype1 = getptype(ptlotrs1->USER, (long) CNS_DRIVINGCONE)) != NULL) {
                ptcone1 = (cone_list *) ptptype1->DATA;
                if (checkincone(ptcone, (branch_list *) NULL, (link_list *) NULL, (void *) ptcone1) == 0) {
                    delincone(ptcone, 1, (void *) ptcone1);
                    deloutcone(ptcone1, 1, (void *) ptcone);
                }
            }
            else {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addconebranch() impossible: no cone drives transistor !\n");
                if (CNS_TRACE_MODE >= CNS_TRACE)
                    (void) printf("TRA_cns addconebranch out\n");
                EXIT(-1);
            }
        }
        else if ((ptlink1->TYPE & CNS_EXT) == CNS_EXT) {
            if (checkincone(ptcone, (branch_list *) NULL, (link_list *) NULL, (void *) ptlink1->ULINK.LOCON) == 0) {
                delincone(ptcone, 1, (void *) ptlink1->ULINK.LOCON);
            }
        }
        else if ((ptlink1->TYPE & CNS_PASSIVE) != CNS_PASSIVE) {
            (void) fflush(stdout);
            (void) fprintf(stderr, "*** cns error ***");
            (void) fprintf(stderr, " delconebranch() impossible: unknown link type !\n");
            if (CNS_TRACE_MODE >= CNS_TRACE)
                (void) printf("TRA_cns delconebranch out\n");
            EXIT(-1);
        }
    }
    freebranch(value);

    if (flag)
        coneparabrs(ptcone);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delconebranch out\n");
}

/*============================================================================*
 | function appendbranch();                                                   |
 | chain two branch lists. WARNING: no consistency check                      |
 *============================================================================*/
branch_list *
appendbranch(ptbranch1, ptbranch2)
    branch_list    *ptbranch1;
    branch_list    *ptbranch2;

{
    branch_list    *ptbranch = NULL;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendbranch\n");

    if (ptbranch1 == NULL) {
        return (ptbranch2);
    }
    else {
        for (ptbranch = ptbranch1; ptbranch->NEXT != NULL; ptbranch = ptbranch->NEXT) {
        }
        ptbranch->NEXT = ptbranch2;
        return (ptbranch1);
    }
}

/*============================================================================*
 | function appendconebranch();                                               |
 | chain a branch list to that of a cone and update its INCONE field          |
 *============================================================================*/
void 
appendconebranch(ptcone, ptconebr, ptbranch2)
    cone_list      *ptcone;
    branch_list    *ptconebr;
    branch_list    *ptbranch2;

{
    branch_list    *ptbranch;
    link_list      *ptlink;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendconebranch in\n");

    if (ptconebr == NULL) {
        ptconebr = ptbranch2;
    }
    else {
        for (ptbranch = ptconebr; ptbranch->NEXT != NULL; ptbranch = ptbranch->NEXT);
        ptbranch->NEXT = ptbranch2;
    }

    for (ptbranch = ptbranch2; ptbranch->NEXT != NULL; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
                addincone(ptcone, (long) CNS_CONE, (void *) ptlink->ULINK.LOTRS->GRID);
            }
            else if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) {
                addincone(ptcone, (long) CNS_EXT, (void *) ptlink->ULINK.LOCON);
            }
            else if ((ptlink->TYPE & CNS_PASSIVE) != CNS_PASSIVE) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " appendbranch() impossible: unknown link type !\n");
                if (CNS_TRACE_MODE >= CNS_TRACE)
                    (void) printf("TRA_cns appendconebranch out\n");
                EXIT(-1);
            }
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendconebranch out\n");
}

/*============================================================================*
 | function viewbranch();                                                     |
 | display the content of a given branch structure                            |
 *============================================================================*/
void 
viewbranch(ptbranch, depth)
    branch_list    *ptbranch;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    link_list      *ptlink;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewbranch in\n");

    if (ptbranch == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewbranch() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewbranch out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_BH_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%sbranch->NEXT = 0x%lx\n", margin, (unsigned long)ptbranch->NEXT);
    printf("%sbranch->TYPE = 0x%lx\n", margin, (unsigned long)ptbranch->TYPE);
    printf("%sbranch->LINK = 0x%lx\n", margin, (unsigned long)ptbranch->LINK);
    if ((depth >= 1) && (ptbranch->LINK != NULL)) {
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            viewlink(ptlink, depth - 1);
        }
    }
    printf("%sbranch->USER = 0x%lx\n", margin, (unsigned long)ptbranch->USER);
    if ((depth >= 1) && (ptbranch->USER != NULL)) {
        for (ptptype = ptbranch->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewbranch out\n");
}

/*============================================================================*
 | function parabrs();                                                        |
 | identify and type parallel branches within input list and return a chain   |
 | list that contains chain lists of parallel branches                        |
 *============================================================================*/
chain_list *
parabrs(ptbranch)
    branch_list    *ptbranch;

{
    branch_list    *ptbranch1;
    branch_list    *ptbranch2;
    chain_list     *ptchain1;
    short           found;
    chain_list     *ptchain = NULL;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns parabrs in\n");

    for (ptbranch1 = ptbranch; ptbranch1 != NULL; ptbranch1 = ptbranch1->NEXT) {
        ptbranch1->TYPE &= ~CNS_PARALLEL_INS;
    }
    for (ptbranch1 = ptbranch; ptbranch1 != NULL; ptbranch1 = ptbranch1->NEXT) {
        if ((ptbranch1->TYPE & CNS_PARALLEL) == CNS_PARALLEL)
            continue;
        found = 0;
        if ((ptbranch2 = ptbranch1->NEXT) != NULL) {
            ptchain1 = addchain((chain_list *) NULL, (void *) ptbranch1);
            for (ptbranch2 = ptbranch1->NEXT; ptbranch2 != NULL; ptbranch2 = ptbranch2->NEXT) {
                if (testparabrs(ptbranch1, ptbranch2)) {
                    found = 1;
                    ptbranch2->TYPE |= CNS_PARALLEL_INS;
                    ptchain1 = addchain(ptchain1, (void *) ptbranch2);
                }
            }
            if (ptchain1->NEXT != NULL) {
                ptchain = addchain(ptchain, (void *) ptchain1);
            }
            else
                freechain(ptchain1);
            if (found) {
                ptbranch1->TYPE |= CNS_PARALLEL;
            }
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns parabrs out\n");

    return (ptchain);
}

/*============================================================================*
 | function testparabrs();                                                    |
 | test to see if the two input branches are parallel and return 1 if it is   |
 | the case. WARNING: this function may be eroneous if the two branches are   |
 | from two different cones                                                   |
 *============================================================================*/
short 
testparabrs(ptbranch1, ptbranch2)
    branch_list    *ptbranch1;
    branch_list    *ptbranch2;

{
    link_list      *ptlink1;
    link_list      *ptlink2;
    short           result = 0;
    lotrs_list     *ptlotrs1;
    lotrs_list     *ptlotrs2;
    ptype_list     *ptptype1;
    ptype_list     *ptptype2;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns testparabrs\n");

    ptlink1 = ptbranch1->LINK;
    ptlink2 = ptbranch2->LINK;

    while ((ptlink1 != NULL) && (ptlink2 != NULL)) {
        if (((ptlink1->TYPE & CNS_EXT) == CNS_EXT) && (ptlink1->TYPE == ptlink2->TYPE)) {
            if (ptlink1->ULINK.LOCON != ptlink2->ULINK.LOCON)
                return (0);
            else
                result = 1;
        }
        else if (ptlink1->TYPE == ptlink2->TYPE) {
            if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
                ptlotrs1 = ptlink1->ULINK.LOTRS;
                ptlotrs2 = ptlink2->ULINK.LOTRS;
                if (((ptptype1 = getptype(ptlotrs1->USER, (long) CNS_DRIVINGCONE)) != NULL) && ((ptptype2 = getptype(ptlotrs2->USER, (long) CNS_DRIVINGCONE)) != NULL)) {
                    if (ptptype1->DATA != ptptype2->DATA)
                        return (0);
                    else
                        result = 1;
                }
                else
                    return (0);
            }
            else
                result = 1;
        }
        else
            return (0);
        ptlink1 = ptlink1->NEXT;
        ptlink2 = ptlink2->NEXT;
    }

    if ((ptlink1 != NULL) || (ptlink2 != NULL))
        return (0);

    return (result);
}

/*============================================================================*
 | function brlotrsnum();                                                     |
 | returns the maximum number of series transistors found in the input        |
 | branch list                                                                |
 *============================================================================*/
short 
brlotrsnum(ptbrlist)
    branch_list    *ptbrlist;

{
    branch_list    *ptbranch;
    short           number;
    short           result = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns brlotrsnum in\n");

    for (ptbranch = ptbrlist; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS)
            continue;
        if ((number = lotrsnum(ptbranch->LINK)) > result)
            result = number;
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns brlotrsnum out\n");

    return (result);
}
