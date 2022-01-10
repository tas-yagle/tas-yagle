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
 
/* 10/06/97 Cone Netlist Structure functions: cnsenv.c                        */

#include"cnsall.h"

/*============================================================================*
 | function cnsenv(); set memory management algorithms and                    |
 | set user preferences for the Cone Netlist Structure environement variables |
 *============================================================================*/
static int donecnsenv=0;
void 
cnsenv()
{
    char           *str;


    if (!donecnsenv) {
/* ---------------------------------------------------------------------------
   set CNS's version information
   --------------------------------------------------------------------------- */
      CNS_VERSION = namealloc("2.10");

/* ---------------------------------------------------------------------------
   CNS edge, link, branch, cone, cell and cnsfig memory management procedures
   --------------------------------------------------------------------------- */
      initedgemem();
      initlinkmem();
      initbranchmem();
      initconemem();
      initcellmem();
      initcnsfigmem();
    }
    
    str = (char *) getenv("CNS_TRACE_MODE");
    if (str != NULL) {
        CNS_TRACE_MODE = atoi(str);
    }
    else {
        CNS_TRACE_MODE = CNS_SILENT;
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns cnsenv\n");

    str = (char *) getenv("CNS_DEBUG_MODE");
    if (str != NULL) {
        CNS_DEBUG_MODE = atoi(str);
    }
    else {
        CNS_DEBUG_MODE = CNS_SILENT;
    }

    str = (char *) getenv("CNS_TECHNO");
    if (str != NULL) {
        CNS_TECHNO = namealloc(str);
    }
    else {
        CNS_TECHNO = namealloc("npmosfet");
    }

    str = (char *) getenv("CNS_MAXLINKNUM");
    if (str != NULL) {
        CNS_MAXLINKNUM = atoi(str);
    }
    else {
        if (strcmp(CNS_TECHNO, "npmosfet") == 0)
            CNS_MAXLINKNUM = 6;
        else
            CNS_MAXLINKNUM = 2;
    }

    str = (char *) getenv("CNS_POWERNUM");
    if (str != NULL) {
        CNS_POWERNUM = atoi(str);
    }
    else {
        CNS_POWERNUM = 2;
    }

    str = (char *) getenv("CNS_VDDNAME");
    if (str != NULL) {
        CNS_VDDNAME = namealloc(str);
    }
    else {
        CNS_VDDNAME = namealloc(VDD) ;
    }

    str = (char *) getenv("CNS_GNDNAME");
    if (str != NULL) {
        CNS_GNDNAME = namealloc(str);
    }
    else {
        CNS_GNDNAME = namealloc("gnd");
    }

    str = (char *) getenv("CNS_VSSNAME");
    if (str != NULL) {
        CNS_VSSNAME = namealloc(str);
    }
    else {
        CNS_VSSNAME = namealloc(VSS) ;

    }

    str = (char *) getenv("CNS_GRIDNAME");
    if (str != NULL) {
        CNS_GRIDNAME = namealloc(str);
    }
    else {
        CNS_GRIDNAME = namealloc("grid");
    }

    str = (char *) getenv("CNS_SOURCENAME");
    if (str != NULL) {
        CNS_SOURCENAME = namealloc(str);
    }
    else {
        CNS_SOURCENAME = namealloc("source");
    }

    str = (char *) getenv("CNS_DRAINNAME");
    if (str != NULL) {
        CNS_DRAINNAME = namealloc(str);
    }
    else {
        CNS_DRAINNAME = namealloc("drain");
    }

    str = (char *) getenv("CNS_BULKNAME");
    if (str != NULL) {
        CNS_BULKNAME = namealloc(str);
    }
    else {
        CNS_BULKNAME = namealloc("bulk");
    }

    CNS_VIEW_LEVEL = 0;

    str = getenv("CNS_LC_VIEW");
    if (str != NULL)
        CNS_LC_VIEW = atoi(str);
    else
        CNS_LC_VIEW = -1;

    str = getenv("CNS_LT_VIEW");
    if (str != NULL)
        CNS_LT_VIEW = atoi(str);
    else
        CNS_LT_VIEW = -1;

    str = getenv("CNS_CF_VIEW");
    if (str != NULL)
        CNS_CF_VIEW = atoi(str);
    else
        CNS_CF_VIEW = -1;

    str = getenv("CNS_CE_VIEW");
    if (str != NULL)
        CNS_CE_VIEW = atoi(str);
    else
        CNS_CE_VIEW = -1;

    str = getenv("CNS_IC_VIEW");
    if (str != NULL)
        CNS_IC_VIEW = atoi(str);
    else
        CNS_IC_VIEW = -1;

    str = getenv("CNS_OC_VIEW");
    if (str != NULL)
        CNS_OC_VIEW = atoi(str);
    else
        CNS_OC_VIEW = -1;

    str = getenv("CNS_BH_VIEW");
    if (str != NULL)
        CNS_BH_VIEW = atoi(str);
    else
        CNS_BH_VIEW = -1;

    str = getenv("CNS_LK_VIEW");
    if (str != NULL)
        CNS_LK_VIEW = atoi(str);
    else
        CNS_LK_VIEW = -1;
    donecnsenv=1;
}


void cnsSwitchContext(cnsContext *ctx)
{
  cnsfig_list *tmp;
  tmp=CNS_HEADCNSFIG;
  CNS_HEADCNSFIG=ctx->CNS_HEAD;
  ctx->CNS_HEAD=tmp;
}

cnsContext *cnsCreateContext()
{
  cnsContext *ctx;
  ctx=(cnsContext *)mbkalloc(sizeof(cnsContext));
  ctx->CNS_HEAD=NULL;
  return ctx;
}

void cnsFreeContext(mbkContext *ctx)
{
  mbkfree(ctx);
}
