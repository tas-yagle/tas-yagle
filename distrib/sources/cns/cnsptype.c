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
 
/* 10/26/95 Cone Netlist Structure functions: cnsptype.c                      */

#include"cnsall.h"

/*============================================================================*
 | function viewcnsptype();                                                   |
 | display the content of a given ptype structure                             |
 *============================================================================*/
void 
viewcnsptype(ptptype)
    ptype_list     *ptptype;

{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcnsptype\n");

    if (ptptype == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewcnsptype() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewcnsptype out\n");
        EXIT(-1);
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%sptype->NEXT  = 0x%lx\n", margin, (unsigned long)ptptype->NEXT);
    printf("%sptype->TYPE  = 0x%lx\n", margin, (unsigned long)ptptype->TYPE);
    printf("%sptype->DATA  = 0x%lx\n", margin, (unsigned long)ptptype->DATA);
    printf("\n");
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);
}
