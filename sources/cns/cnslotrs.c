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
 
/* 10/26/95 Cone Netlist Structure functions: cnslotrs.c                      */

#include"cnsall.h"

/*============================================================================*
 | function addcnslotrs();                                                    |
 | add an element to a lotrs_list                                             |
 *============================================================================*/
void 
addcnslotrs(ptcnsfig, index, drvcn, x, y, w, l, ps, pd, xs, xd, type, ptgrid,
	                  ptsource, ptdrain, ptbulk, name)
    cnsfig_list    *ptcnsfig;
    long            index;
    cone_list      *drvcn;
    long            x, y;
    long            w, l, ps, pd, xs, xd;
    char            type;
    losig_list     *ptgrid, *ptsource, *ptdrain, *ptbulk;
    char           *name;

{
    lotrs_list     *ptlotrs;
    locon_list     *ptlocon;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcnslotrs\n");

    if (ptcnsfig == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslotrs() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    if (index == 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslotrs() impossible: index 0 may not be employed !\n");
        EXIT(-1);
    }

    if (CNS_DEBUG_MODE >= CNS_DEBUG) {
        for (ptlotrs = ptcnsfig->LOTRS; ptlotrs != NULL; ptlotrs = ptlotrs->NEXT) {
            if ((ptptype = getptype(ptlotrs->USER, (long) CNS_INDEX)) == NULL)
                continue;
            if (((long ) ptptype->DATA) == index) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcnslotrs() impossible: transistor indexed %ld already exists !\n", index);
                EXIT(-1);
            }
        }
    }

    if (!MLO_IS_TRANSN(type) && !MLO_IS_TRANSP(type)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcnslotrs() impossible: type for transistor indexed %ld unknown !\n", index);
        EXIT(-1);
    }

    ptlotrs = (lotrs_list *) mbkalloc(sizeof(lotrs_list));
    ptlotrs->NEXT = ptcnsfig->LOTRS;
    ptlotrs->DRAIN = (locon_list *) NULL;
    ptlotrs->GRID = (locon_list *) NULL;
    ptlotrs->SOURCE = (locon_list *) NULL;
    ptlotrs->BULK = (locon_list *) NULL;
    ptlotrs->X = x;
    ptlotrs->Y = y;
    ptlotrs->WIDTH = w;
    ptlotrs->LENGTH = l;
    ptlotrs->PS = ps;
    ptlotrs->PD = pd;
    ptlotrs->XS = xs;
    ptlotrs->XD = xd;
    ptlotrs->TYPE = type;
    ptlotrs->MODINDEX = EMPTYHT ;
    ptlotrs->TRNAME = namealloc(name);
    ptlotrs->USER = (ptype_list *) NULL;
    ptlotrs->USER = addptype(ptlotrs->USER, (long) CNS_INDEX, (void *) index);
	
    if (ptgrid != NULL || ptdrain != NULL || ptsource != NULL) {

    	ptlocon = (locon_list *)mbkalloc(sizeof(locon_list));
        ptlocon->NAME   = namealloc("grid");
        ptlocon->SIG    = ptgrid;
        ptlocon->ROOT   = (void *)ptlotrs;
        ptlocon->TYPE   = 'T';
        ptlocon->DIRECTION = 'I';
        ptlocon->USER   = NULL;
        ptlocon->PNODE  = NULL;
        ptlotrs->GRID   = ptlocon;

    	ptlocon = (locon_list *)mbkalloc(sizeof(locon_list));
        ptlocon->NAME   = namealloc("drain");
        ptlocon->SIG    = ptdrain;
        ptlocon->ROOT   = (void *)ptlotrs;
        ptlocon->TYPE   = 'T';
        ptlocon->DIRECTION = 'I';
        ptlocon->USER   = NULL;
        ptlocon->PNODE  = NULL;
        ptlotrs->DRAIN   = ptlocon;

    	ptlocon = (locon_list *)mbkalloc(sizeof(locon_list));
        ptlocon->NAME   = namealloc("source");
        ptlocon->SIG    = ptsource;
        ptlocon->ROOT   = (void *)ptlotrs;
        ptlocon->TYPE   = 'T';
        ptlocon->DIRECTION = 'I';
        ptlocon->USER   = NULL;
        ptlocon->PNODE  = NULL;
        ptlotrs->SOURCE   = ptlocon;

    	ptlocon = (locon_list *)mbkalloc(sizeof(locon_list));
        ptlocon->NAME   = namealloc("bulk");
        ptlocon->SIG    = ptbulk;
        ptlocon->ROOT   = (void *)ptlotrs;
        ptlocon->TYPE   = 'T';
        ptlocon->DIRECTION = 'I';
        ptlocon->USER   = NULL;
        ptlocon->PNODE  = NULL;
        ptlotrs->BULK   = ptlocon;
	}
	else {
        ptlotrs->GRID   = NULL;
	    ptlotrs->DRAIN   = NULL;
	    ptlotrs->SOURCE   = NULL;
        ptlotrs->BULK   = NULL;
    }
	
    if (drvcn != NULL) {
        ptlotrs->USER = addptype(ptlotrs->USER, (long) CNS_DRIVINGCONE, (void *) drvcn);
        if ((ptptype = getptype(drvcn->USER, (long) CNS_LOTRSGRID)) != NULL) {
            ptptype->DATA = (void *) addchain((chain_list *) ptptype->DATA, (void *) ptlotrs);
        }
        else {
            drvcn->USER = addptype(drvcn->USER, (long) CNS_LOTRSGRID, (void *) addchain((chain_list *) NULL, (void *) ptlotrs));
        }
    }

    ptcnsfig->LOTRS = ptlotrs;
}

/*============================================================================*
 | function getcnslotrs();                                                    |
 | returns a transistor of a given index                                      |
 *============================================================================*/
lotrs_list *
getcnslotrs(ptcnsfig, index)
    cnsfig_list    *ptcnsfig;
    long            index;

{
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getcnslotrs\n");

    if (ptcnsfig == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " getcnslotrs() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    if (index == 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns warning ***");
        (void) fprintf(stderr, " getcnslotrs() impossible: NULL pointer !\n");
    }

    if (index != 0) {
        for (ptlotrs = ptcnsfig->LOTRS; ptlotrs != NULL; ptlotrs = ptlotrs->NEXT) {
            if ((ptptype = getptype(ptlotrs->USER, (long) CNS_INDEX)) != NULL) {
                if (((long ) ptptype->DATA) == index)
                    return (ptlotrs);
            }
        }
    }

    return ((lotrs_list *) NULL);
}

/*============================================================================*
 | function viewcnslotrs();                                                   |
 | display the content of a given transistor structure                        |
 *============================================================================*/
void 
viewcnslotrs(ptlotrs, depth)
    lotrs_list     *ptlotrs;
    int             depth;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnslotrs in\n");

    if (ptlotrs == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewcnslotrs() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewcnslotrs out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_LT_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%strans->NEXT         = 0x%lx\n", margin, (unsigned long)ptlotrs->NEXT);
    printf("%strans->TYPE         = 0x%lx\n", margin, (unsigned long)ptlotrs->TYPE);
    printf("%strans->DRAIN        = 0x%lx\n", margin, (unsigned long)ptlotrs->DRAIN);
    printf("%strans->GRID         = 0x%lx\n", margin, (unsigned long)ptlotrs->GRID);
    printf("%strans->SOURCE       = 0x%lx\n", margin, (unsigned long)ptlotrs->SOURCE);
    printf("%strans->X,Y          = %ld,%ld\n", margin, ptlotrs->X, ptlotrs->Y);
    printf("%strans->WIDTH,LENGTH = %ld,%ld\n", margin, ptlotrs->WIDTH, ptlotrs->LENGTH);
    printf("%strans->PS,PD        = %ld,%ld\n", margin, ptlotrs->PS, ptlotrs->PD);
    printf("%strans->XS,XD        = %ld,%ld\n", margin, ptlotrs->XS, ptlotrs->XD);
    printf("%strans->USER         = 0x%lx\n", margin, (unsigned long)ptlotrs->USER);
    if ((depth >= 1) && (ptlotrs->USER != NULL)) {
        for (ptptype = ptlotrs->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnslotrs out\n");
}

/*============================================================================*
 | function cone2lotrs();                                                     |
 | add a cone to a transistor's CNS_CONE USER field if it isn't already in    |
 | the list (unicity check)                                                   |
 *============================================================================*/
void 
cone2lotrs(ptcone, ptlotrs)
    cone_list      *ptcone;
    lotrs_list     *ptlotrs;

{
    ptype_list     *ptptype;
    chain_list     *ptchain;
    short           found = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns cone2lotrs\n");

    if ((ptcone != NULL) && (ptlotrs != NULL)) {
        if ((ptptype = getptype(ptlotrs->USER, (long) CNS_CONE)) != NULL) {
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
            ptlotrs->USER = addptype(ptlotrs->USER, CNS_CONE, (void *) addchain((chain_list *) NULL, (void *) ptcone));
        }
    }
}
