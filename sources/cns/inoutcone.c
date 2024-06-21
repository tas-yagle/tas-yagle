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
 
/* 10/26/95 Cone Netlist Structure functions: inoutcone.c                     */

#include"cnsall.h"

/*============================================================================*
 | function checkincone();                                                    |
 | return a number, sum of the links and branches of a specified cone (that   |
 | may eventually be deprived of one branch and, or of one link) being driven |
 | by a certain cone or connector                                             |
 *============================================================================*/
int 
checkincone(ptcone, ptbranch, ptlink, data)
    cone_list      *ptcone;
    branch_list    *ptbranch;
    link_list      *ptlink;
    void           *data;

{
    branch_list    *ptbranch1;
    link_list      *ptlink1;
    ptype_list     *ptptype1;
    int             counter = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns checkincone\n");

    if (ptcone == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " checkincone() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    for (ptbranch1 = ptcone->BREXT; ptbranch1 != NULL; ptbranch1 = ptbranch1->NEXT) {
        if (ptbranch1 != ptbranch) {
            for (ptlink1 = ptbranch1->LINK; ptlink1 != NULL; ptlink1 = ptlink1->NEXT) {
                if (ptlink1 != ptlink) {
                    if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
                        if ((ptptype1 = getptype(ptlink1->ULINK.LOTRS->USER, (long) CNS_DRIVINGCONE)) != NULL) {
                            if (ptptype1->DATA == data) {
                                counter++;
                            }
                        }
                    }
                    else if ((ptlink1->TYPE & CNS_EXT) == CNS_EXT) {
                        if (ptlink1->ULINK.LOCON == (locon_list *) data) {
                            counter++;
                        }
                    }
                }
            }
        }
    }
    for (ptbranch1 = ptcone->BRVDD; ptbranch1 != NULL; ptbranch1 = ptbranch1->NEXT) {
        if (ptbranch1 != ptbranch) {
            for (ptlink1 = ptbranch1->LINK; ptlink1 != NULL; ptlink1 = ptlink1->NEXT) {
                if (ptlink1 != ptlink) {
                    if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
                        if ((ptptype1 = getptype(ptlink1->ULINK.LOTRS->USER, (long) CNS_DRIVINGCONE)) != NULL) {
                            if (ptptype1->DATA == data) {
                                counter++;
                            }
                        }
                    }
                    else if ((ptlink1->TYPE & CNS_EXT) == CNS_EXT) {
                        if (ptlink1->ULINK.LOCON == (locon_list *) data) {
                            counter++;
                        }
                    }
                }
            }
        }
    }
    for (ptbranch1 = ptcone->BRGND; ptbranch1 != NULL; ptbranch1 = ptbranch1->NEXT) {
        if (ptbranch1 != ptbranch) {
            for (ptlink1 = ptbranch1->LINK; ptlink1 != NULL; ptlink1 = ptlink1->NEXT) {
                if (ptlink1 != ptlink) {
                    if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
                        if ((ptptype1 = getptype(ptlink1->ULINK.LOTRS->USER, (long) CNS_DRIVINGCONE)) != NULL) {
                            if (ptptype1->DATA == data) {
                                counter++;
                            }
                        }
                    }
                    else if ((ptlink1->TYPE & CNS_EXT) == CNS_EXT) {
                        if (ptlink1->ULINK.LOCON == (locon_list *) data) {
                            counter++;
                        }
                    }
                }
            }
        }
    }
    for (ptbranch1 = ptcone->BRVSS; ptbranch1 != NULL; ptbranch1 = ptbranch1->NEXT) {
        if (ptbranch1 != ptbranch) {
            for (ptlink1 = ptbranch1->LINK; ptlink1 != NULL; ptlink1 = ptlink1->NEXT) {
                if (ptlink1 != ptlink) {
                    if ((ptlink1->TYPE & CNS_ACTIVE) == CNS_ACTIVE) {
                        if ((ptptype1 = getptype(ptlink1->ULINK.LOTRS->USER, (long) CNS_DRIVINGCONE)) != NULL) {
                            if (ptptype1->DATA == data) {
                                counter++;
                            }
                        }
                    }
                    else if ((ptlink1->TYPE & CNS_EXT) == CNS_EXT) {
                        if (ptlink1->ULINK.LOCON == (locon_list *) data) {
                            counter++;
                        }
                    }
                }
            }
        }
    }
    return (counter);
}

/*============================================================================*
 | function addincone();                                                      |
 | add an element to a cone's INCONE field after having checked its unicity   |
 *============================================================================*/
void 
addincone(ptcone, type, data)
    cone_list      *ptcone;
    long            type;
    void           *data;

{
    edge_list      *ptedge;
    int             found = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addincone in\n");

    if ((ptcone == NULL) || (type == 0) || (data == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addincone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns addincone out\n");
        EXIT(-1);
    }

    for (ptedge = ptcone->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if (ptedge->UEDGE.PTR == data) {
            found = 1;
            break;
        }
    }
    if (!found) {
        ptcone->INCONE = addedge(ptcone->INCONE, type, data);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addincone out\n");
}

/*============================================================================*
 | function delincone();                                                      |
 | delete an element of a cone's INCONE field                                 |
 *============================================================================*/
void 
delincone(ptcone, type, data)
    cone_list      *ptcone;
    long            type;
    void           *data;

{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delincone in\n");

    if ((ptcone == NULL) || (type == 0) || (data == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " delincone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delincone out\n");
        EXIT(-1);
    }

    for (ptedge = ptcone->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if (ptedge->UEDGE.PTR == data) {
            ptcone->INCONE = deledge(ptcone->INCONE, ptedge);
            break;
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delincone out\n");
}

/*============================================================================*
 | function appendincone();                                                   |
 | chain two INCONE edge lists while verifying unicity                        |
 *============================================================================*/
void 
appendincone(ptcone1, ptcone2)
    cone_list      *ptcone1;
    cone_list      *ptcone2;

{
    edge_list      *ptedge2;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendincone in\n");

    if ((ptcone1 == NULL) || (ptcone2 == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " appendincone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns appendincone out\n");
        EXIT(-1);
    }

    for (ptedge2 = ptcone2->INCONE; ptedge2 != NULL; ptedge2 = ptedge2->NEXT) {
        addincone(ptcone1, ptedge2->TYPE, ptedge2->UEDGE.PTR);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendincone out\n");
}

/*============================================================================*
 | function viewincone();                                                     |
 | display the content of any given incone structure                          |
 *============================================================================*/
void 
viewincone(ptincone, depth)
    edge_list      *ptincone;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewincone in\n");

    if (ptincone == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewincone() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewincone out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_IC_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%sincone->NEXT      = 0x%lx\n", margin, (unsigned long)ptincone->NEXT);
    printf("%sincone->TYPE      = 0x%lx\n", margin, (unsigned long)ptincone->TYPE);
    printf("%sincone->UEDGE.PTR = 0x%lx\n", margin, (unsigned long)ptincone->UEDGE.PTR);
    if ((ptincone->TYPE & CNS_EXT) != CNS_EXT) {
        if ((depth >= 1) && (ptincone->UEDGE.CONE != NULL)) {
            viewcone(ptincone->UEDGE.CONE, depth - 1);
        }
    }
    else {
        if ((depth >= 1) && (ptincone->UEDGE.LOCON != NULL)) {
            viewcnslocon(ptincone->UEDGE.LOCON, depth - 1);
        }
    }
    printf("%sincone->USER  = 0x%lx\n", margin, (unsigned long)ptincone->USER);
    if ((depth >= 1) && (ptincone->USER != NULL)) {
        for (ptptype = ptincone->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewincone out\n");
}

/*============================================================================*
 | function addoutcone();                                                     |
 | add an element to a cone's OUTCONE field after having checked its unicity  |
 *============================================================================*/
void 
addoutcone(ptcone, type, data)
    cone_list      *ptcone;
    long            type;
    void           *data;

{
    edge_list      *ptedge;
    int             found = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addoutcone in\n");

    if ((ptcone == NULL) || (type == 0) || (data == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addoutcone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns addoutcone out\n");
        EXIT(-1);
    }

    for (ptedge = ptcone->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if (ptedge->UEDGE.PTR == data) {
            found = 1;
            break;
        }
    }
    if (!found) {
        ptcone->OUTCONE = addedge(ptcone->OUTCONE, type, data);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addoutcone out\n");
}

/*============================================================================*
 | function deloutcone();                                                     |
 | delete an element of a cone's OUTCONE field                                |
 *============================================================================*/
void 
deloutcone(ptcone, type, data)
    cone_list      *ptcone;
    long            type;
    void           *data;

{
    edge_list      *ptedge;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns deloutcone in\n");

    if ((ptcone == NULL) || (type == 0) || (data == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " deloutcone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns deloutcone out\n");
        EXIT(-1);
    }

    for (ptedge = ptcone->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if (ptedge->UEDGE.PTR == data) {
            ptcone->OUTCONE = deledge(ptcone->OUTCONE, ptedge);
            break;
        }
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns deloutcone out\n");
}

/*============================================================================*
 | function appendoutcone();                                                  |
 | chain two OUTCONE edge lists while verifying unicity                       |
 *============================================================================*/
void 
appendoutcone(ptcone1, ptcone2)
    cone_list      *ptcone1;
    cone_list      *ptcone2;

{
    edge_list      *ptedge2;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendoutcone in\n");

    if ((ptcone1 == NULL) || (ptcone2 == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " appendoutcone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns appendoutcone out\n");
        EXIT(-1);
    }

    for (ptedge2 = ptcone2->OUTCONE; ptedge2 != NULL; ptedge2 = ptedge2->NEXT) {
        addoutcone(ptcone1, ptedge2->TYPE, ptedge2->UEDGE.PTR);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendoutcone out\n");
}

/*============================================================================*
 | function viewoutcone();                                                    |
 | display the content of any given outcone structure                         |
 *============================================================================*/
void 
viewoutcone(ptoutcone, depth)
    edge_list      *ptoutcone;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewoutcone in\n");

    if (ptoutcone == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewoutcone() impossible: NULL pointer(s) !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewoutcone out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_OC_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%soutcone->NEXT      = 0x%lx\n", margin, (unsigned long)ptoutcone->NEXT);
    printf("%soutcone->TYPE      = 0x%lx\n", margin, (unsigned long)ptoutcone->TYPE);
    printf("%soutcone->UEDGE.PTR = 0x%lx\n", margin, (unsigned long)ptoutcone->UEDGE.PTR);
    if ((ptoutcone->TYPE & CNS_EXT) != CNS_EXT) {
        if ((depth >= 1) && (ptoutcone->UEDGE.CONE != NULL)) {
            viewcone(ptoutcone->UEDGE.CONE, depth - 1);
        }
    }
    else {
        if ((depth >= 1) && (ptoutcone->UEDGE.LOCON != NULL)) {
            viewcnslocon(ptoutcone->UEDGE.LOCON, depth - 1);
        }
    }
    printf("%soutcone->USER  = 0x%lx\n", margin, (unsigned long)ptoutcone->USER);
    if ((depth >= 1) && (ptoutcone->USER != NULL)) {
        for (ptptype = ptoutcone->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewoutcone out\n");
}
