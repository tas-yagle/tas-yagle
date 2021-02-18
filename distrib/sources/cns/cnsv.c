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
 
/* 10/26/95 Cone Netlist Structure variables: cnsv.c                          */

#include"cnsall.h"

/* CNS environment variables */

char           *CNS_VERSION = NULL;
//int             CNS_DRIVE_NORMAL = 0;
//int             CNS_DRIVE_VERBOSE = 0;
//int             CNS_DRIVE_FULL = 0;
int             CNS_TRACE_MODE = 0;
int             CNS_DEBUG_MODE = 0;
int             CNS_VIEW_LEVEL = 0;
char           *CNS_TECHNO = NULL;
int             CNS_POWERNUM = 0;
int             CNS_MAXLINKNUM = 0;


/* CNS structure variables */

cnsfig_list    *CNS_HEADCNSFIG = NULL;


/* CNS connector names */

char           *CNS_VDDNAME = NULL;
char           *CNS_GNDNAME = NULL;
char           *CNS_VSSNAME = NULL;
char           *CNS_GRIDNAME = NULL;
char           *CNS_SOURCENAME = NULL;
char           *CNS_DRAINNAME = NULL;
char           *CNS_BULKNAME = NULL;


/* CNS structure debug variables */

int             CNS_DEBUG_LEVEL = 0;
int             CNS_LC_VIEW = 0;
int             CNS_LT_VIEW = 0;
int             CNS_CF_VIEW = 0;
int             CNS_CE_VIEW = 0;
int             CNS_IC_VIEW = 0;
int             CNS_OC_VIEW = 0;
int             CNS_BH_VIEW = 0;
int             CNS_LK_VIEW = 0;
