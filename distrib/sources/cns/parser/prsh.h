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
 
/* 10/26/95 parser functions: prsh.h                                          */

branch_list    *idx2br();
int             yywrap();

cone_list   *getcone             (cnsfig_list    *ptcnsfig,
                                  long            index,
                                  char           *name);
cnsfig_list *addcnsfig           (cnsfig_list    *ptcnsfighead,
                                  char           *name,
                                  locon_list     *ptlocon,
                                  locon_list     *ptintcon,
                                  lotrs_list     *ptlotrs,
                                  loins_list     *ptloins,
                                  cone_list      *ptcone,
                                  cell_list      *ptcell,
                                  lofig_list     *ptlofig,
                                  befig_list     *ptbefig);
void         addcnslocon         (cnsfig_list    *ptcnsfig,
                                  long            index,
                                  char           *name,
                                  char            direction);
void         addcnslotrs         (cnsfig_list    *ptcnsfig,
                                  long            index,
                                  cone_list      *drvcn,
                                  long            x,
                                  long            y,
                                  long            w,
                                  long            l,
                                  long            ps,
                                  long            pd,
                                  long            xs,
                                  long            xd,
                                  char            type,
                                  losig_list     *ptgrid,
                                  losig_list     *ptsource,
                                  losig_list     *ptdrain,
                                  losig_list     *ptbulk,
                                  char           *name);
cone_list   *addcone             (cone_list      *ptconehead,
                                  long            index,
                                  char           *name,
                                  long            type,
                                  long            tectype,
                                  long            xmax,
                                  long            xmin,
                                  long            ymax,
                                  long            ymin,
                                  edge_list      *ptincone,
                                  edge_list      *ptoutcone,
                                  branch_list    *ptbrext,
                                  branch_list    *ptbrvdd,
                                  branch_list    *ptbrgnd,
                                  branch_list    *ptbrvss,
                                  chain_list     *ptchain);
cell_list   *addcell             (cell_list      *ptcellhead,
                                  long            type,
                                  chain_list     *ptchain,
                                  befig_list     *ptbefig);
edge_list   *addedge             (edge_list      *ptedgehead,
                                  long            type,
                                  void           *data);
branch_list *addbranch           (branch_list    *ptbranchhead,
                                  long            type,
                                  link_list      *ptlink);
locon_list  *getcnslocon         (cnsfig_list    *ptcnsfig,
                                  long            index,
                                  char           *name);
link_list   *addlink             (link_list      *ptlinkhead,
                                  long            type,
                                  void           *data,
                                  losig_list     *ptsig);
