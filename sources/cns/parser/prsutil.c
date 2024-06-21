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
 
/* 05/30/95 Cone Netlist Structure functions: prsutil.c                       */

#include       "prsall.h"

/*============================================================================*
 | function idx2br();                                                         |
 *============================================================================*/
branch_list *
idx2br(ptbrstruc, index)
    sbrstruc       *ptbrstruc;
    long            index;

{
    branch_list    *ptbranch = NULL;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns idx2br\n");

    for (ptbranch = ptbrstruc->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptptype = getptype(ptbranch->USER, CNS_INDEX)) != NULL) {
            if (((long ) ptptype->DATA) == index)
                return (ptbranch);
        }
    }
    for (ptbranch = ptbrstruc->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptptype = getptype(ptbranch->USER, CNS_INDEX)) != NULL) {
            if (((long ) ptptype->DATA) == index)
                return (ptbranch);
        }
    }
    for (ptbranch = ptbrstruc->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptptype = getptype(ptbranch->USER, CNS_INDEX)) != NULL) {
            if (((long ) ptptype->DATA) == index)
                return (ptbranch);
        }
    }
    for (ptbranch = ptbrstruc->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptptype = getptype(ptbranch->USER, CNS_INDEX)) != NULL) {
            if (((long ) ptptype->DATA) == index)
                return (ptbranch);
        }
    }

    return (ptbranch);
}
