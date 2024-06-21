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
 
/* 10/26/95 Cone Netlist Structure functions: cnslocon.c                      */

#include"cnsall.h"

/*============================================================================*
 | function addcnslocon();                                                    |
 | add an element to a locon_list                                             |
 *============================================================================*/
void 
addcnslocon(ptcnsfig, index, name, direction)
    cnsfig_list    *ptcnsfig;
    long            index;
    char           *name;
    char            direction;

{
    ptype_list     *ptptype;
    locon_list     *ptlocon;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnslocon\n");

    if (ptcnsfig == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    if (index == 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: index 0 may not be employed !\n");
        EXIT(-1);
    }

    if (name == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: NULL name !\n");
        EXIT(-1);
    }

    if (CNS_DEBUG_MODE >= CNS_DEBUG) {
        for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            if (strcmp(ptlocon->NAME, name) == 0) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslocon() impossible: connector named %s already exists !\n", name);
                EXIT(-1);
            }
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) == NULL)
                continue;
            if (((long ) ptptype->DATA) == index) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslocon() impossible: connector indexed %ld already exists !\n", index);
                EXIT(-1);
            }
        }
    }

    if ((direction != CNS_I) && (direction != CNS_O) && (direction != CNS_B) && (direction != CNS_Z) && (direction != CNS_T) && (direction != CNS_X) && (direction != CNS_VDDC) && (direction != CNS_GNDC) && (direction != CNS_VSSC)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: type for connector named %s unknown !\n", name);
        EXIT(-1);
    }

    ptlocon = (locon_list *) mbkalloc(sizeof(locon_list));
    ptlocon->NEXT = ptcnsfig->LOCON;
    ptlocon->NAME = namealloc(name);
    ptlocon->SIG = (losig_list *) NULL;
    ptlocon->ROOT = (void *) ptcnsfig;
    ptlocon->SIG = (losig_list *) NULL;
    ptlocon->DIRECTION = direction;
    ptlocon->TYPE = 'E';
    ptlocon->PNODE = (num_list *) NULL;
    ptlocon->USER = (ptype_list *) NULL;
    ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) index);

    ptcnsfig->LOCON = ptlocon;
}

/*============================================================================*
 | function addcnslocontolist();                                              |
 | add an element to a locon_list                                             |
 *============================================================================*/
locon_list * 
addcnslocontolist(ptheadlocon, index, name, direction)
    locon_list     *ptheadlocon;
    long            index;
    char           *name;
    char            direction;
{
    ptype_list     *ptptype;
    locon_list     *ptlocon;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnslocon\n");

    if (index == 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: index 0 may not be employed !\n");
        EXIT(-1);
    }

    if (name == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: NULL name !\n");
        EXIT(-1);
    }

    if (CNS_DEBUG_MODE >= CNS_DEBUG) {
        for (ptlocon = ptheadlocon; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            if (strcmp(ptlocon->NAME, name) == 0) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslocon() impossible: connector named %s already exists !\n", name);
                EXIT(-1);
            }
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) == NULL)
                continue;
            if (((long ) ptptype->DATA) == index) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslocon() impossible: connector indexed %ld already exists !\n", index);
                EXIT(-1);
            }
        }
    }

    if ((direction != CNS_I) && (direction != CNS_O) && (direction != CNS_B) && (direction != CNS_Z) && (direction != CNS_T) && (direction != CNS_X) && (direction != CNS_VDDC) && (direction != CNS_GNDC) && (direction != CNS_VSSC)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: type for connector named %s unknown !\n", name);
        EXIT(-1);
    }

    ptlocon = (locon_list *) mbkalloc(sizeof(locon_list));
    ptlocon->NEXT = ptheadlocon;
    ptlocon->NAME = namealloc(name);
    ptlocon->SIG = (losig_list *) NULL;
    ptlocon->ROOT = NULL;
    ptlocon->SIG = (losig_list *) NULL;
    ptlocon->DIRECTION = direction;
    ptlocon->TYPE = 'E';
    ptlocon->PNODE = (num_list *) NULL;
    ptlocon->USER = (ptype_list *) NULL;
    ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) index);

    return ptlocon;
}

/*============================================================================*
 | function addintcnslocon();                                                 |
 | add an element to a locon_list                                             |
 *============================================================================*/
void 
addcnsintlocon(ptcnsfig, index, name, direction)
    cnsfig_list    *ptcnsfig;
    long            index;
    char           *name;
    char            direction;

{
    ptype_list     *ptptype;
    locon_list     *ptlocon;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnslocon\n");

    if (ptcnsfig == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    if (index == 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: index 0 may not be employed !\n");
        EXIT(-1);
    }

    if (name == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: NULL name !\n");
        EXIT(-1);
    }

    if (CNS_DEBUG_MODE >= CNS_DEBUG) {
        for (ptlocon = ptcnsfig->INTCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            if (strcmp(ptlocon->NAME, name) == 0) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslocon() impossible: connector named %s already exists !\n", name);
                EXIT(-1);
            }
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) == NULL)
                continue;
            if (((long ) ptptype->DATA) == index) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslocon() impossible: connector indexed %ld already exists !\n", index);
                EXIT(-1);
            }
        }
    }

    if ((direction != CNS_I) && (direction != CNS_O) && (direction != CNS_B) && (direction != CNS_Z) && (direction != CNS_T) && (direction != CNS_X) && (direction != CNS_VDDC) && (direction != CNS_GNDC) && (direction != CNS_VSSC)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslocon() impossible: type for connector named %s unknown !\n", name);
        EXIT(-1);
    }

    ptlocon = (locon_list *) mbkalloc(sizeof(locon_list));
    ptlocon->NEXT = ptcnsfig->INTCON;
    ptlocon->NAME = namealloc(name);
    ptlocon->SIG = (losig_list *) NULL;
    ptlocon->ROOT = (void *) ptcnsfig;
    ptlocon->SIG = (losig_list *) NULL;
    ptlocon->DIRECTION = direction;
    ptlocon->TYPE = 'C';
    ptlocon->PNODE = (num_list *) NULL;
    ptlocon->USER = (ptype_list *) NULL;
    ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) index);

    ptcnsfig->INTCON = ptlocon;
}

/*============================================================================*
 | function getcnslocon();                                                    |
 | returns a connector of a given index                                       |
 *============================================================================*/
locon_list *
getcnslocon(ptcnsfig, index, name)
    cnsfig_list    *ptcnsfig;
    long            index;
    char           *name;

{
    locon_list     *ptlocon;
    ptype_list     *ptptype;
    short           foundindex = 0;
    short           foundname = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getcnslocon\n");

    if ((ptcnsfig == NULL) || ((index == 0) && (name == NULL))) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns warning ***");
        (void) fprintf(stderr, " getcnslocon() impossible: NULL pointer(s) !\n");
    }

    if ((index != 0) && (name != NULL)) {
        for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
                if (((long ) ptptype->DATA) == index)
                    foundindex = 1;
            }
            if (strcmp(ptlocon->NAME, name) == 0)
                foundname = 1;
            if (foundindex && foundname)
                return (ptlocon);
            else if (foundindex) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns warning ***");
                (void) fprintf(stderr, " getcnslocon() conflict: found index but name does not match !\n");
                return (ptlocon);
            }
            else if (foundname) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns warning ***");
                (void) fprintf(stderr, " getcnslocon() conflict: found name but index does not match !\n");
                return (ptlocon);
            }
        }
    }
    else if (index != 0) {
        for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
                if (((long ) ptptype->DATA) == index)
                    return (ptlocon);
            }
        }
    }
    else if (name != NULL) {
        for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            if (strcmp(ptlocon->NAME, name) == 0)
                return (ptlocon);
        }
    }

    return ((locon_list *) NULL);
}

/*============================================================================*
 | function viewcnslocon();                                                   |
 | display the content of a given connector structure                         |
 *============================================================================*/
void 
viewcnslocon(ptlocon, depth)
    locon_list     *ptlocon;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnslocon in\n");

    if (ptlocon == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewcnslocon() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewcnslocon out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_LC_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%slocon->NEXT      = 0x%lx\n", margin, (unsigned long)ptlocon->NEXT);
    printf("%slocon->TYPE      = %c\n", margin, ptlocon->TYPE);
    printf("%slocon->DIRECTION = %c\n", margin, ptlocon->DIRECTION);
    printf("%slocon->NAME      = %s\n", margin, ptlocon->NAME);
    printf("%slocon->SIG       = 0x%lx\n", margin, (unsigned long)ptlocon->SIG);
    printf("%slocon->ROOT      = 0x%lx\n", margin, (unsigned long)ptlocon->ROOT);
    printf("%slocon->USER      = 0x%lx\n", margin, (unsigned long)ptlocon->USER);
    if ((depth >= 1) && (ptlocon->USER != NULL)) {
        for (ptptype = ptlocon->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnslocon out\n");
}

/*============================================================================*
 | function cone2locon();                                                     |
 | add a cone to a connector's CNS_CONE USER field if it isn't already in     |
 | the list (unicity check)                                                   |
 *============================================================================*/
void 
cone2locon(ptcone, ptlocon)
    cone_list      *ptcone;
    locon_list     *ptlocon;

{
    ptype_list     *ptptype;
    chain_list     *ptchain;
    short           found = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns cone2locon\n");

    if ((ptcone != NULL) && (ptlocon != NULL)) {
        if ((ptptype = getptype(ptlocon->USER, (long) CNS_CONE)) != NULL) {
            for (ptchain = (chain_list *) ptptype->DATA; ptchain != NULL; ptchain = ptchain->NEXT) {
                if ((cone_list *) ptchain->DATA == ptcone) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ptptype->DATA = addchain((chain_list *) ptptype->DATA, (void *) ptcone);
            }
        }
        else {
            ptlocon->USER = addptype(ptlocon->USER, CNS_CONE, (void *) addchain((chain_list *) NULL, (void *) ptcone));
        }
    }
}
