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
 
/* 11/02/97 Cone Netlist Structure functions: cnsstats.c                      */

#include       "cnsall.h"

/*============================================================================*
 | function cnsstats();                                                       |
 | write cnsfig statistics onto input file                                    |
 *============================================================================*/
void 
cnsstats(ptcnsfig, ptfile)
    cnsfig_list    *ptcnsfig;
    FILE           *ptfile;

{
    cone_list      *ptcone;
    long            countvdd = 0;
    long            countgnd = 0;
    long            countvss = 0;
    long            countext = 0;
    long            countconflict = 0;
    long            counttri = 0;
    long            countmemsym = 0;
    long            countlatch = 0;
    long            countrs = 0;
    long            countflipflop = 0;
    long            countcones = 0;
    long            countcmos = 0;
    long            countdualcmos = 0;
    long            countvdddegraded = 0;
    long            countgnddegraded = 0;
    long            countvssdegraded = 0;
    long            countnor = 0;
    long            countor = 0;
    long            countprnor = 0;
    long            countsbe = 0;
    long            countsbd = 0;
    long            counttristate_o = 0;
    long            countsquirt_o = 0;
    long            countlatch_o = 0;
    long            countprbus = 0;
    long            countzero = 0;
    long            countone = 0;
    cell_list      *ptcell;
    long            countcells = 0;
    long            countsuperbuffer = 0;
    long            counttristate_e = 0;
    long            countsquirtbuffer = 0;
    long            countlatch_e = 0;
    long            countread = 0;
    long            countmsff = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns cnsstats\n");

    if ((ptcnsfig == NULL) || (ptfile == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " cnsstats() impossible: NULL pointer !\n");
        EXIT(-1);
    }

    fprintf(ptfile, "\n");

    fprintf(ptfile, "\n technology....................................... %s\n", CNS_TECHNO);
    fprintf(ptfile, "\n power number..................................... %ld\n", (long)CNS_POWERNUM);
    fprintf(ptfile, "\n maximum series link number....................... %ld\n", (long)CNS_MAXLINKNUM);
    fprintf(ptfile, "\n vdd power connector name......................... %s\n", CNS_VDDNAME);
    fprintf(ptfile, "\n gnd power connector name......................... %s\n", CNS_GNDNAME);
    fprintf(ptfile, "\n vss power connector name......................... %s\n", CNS_VSSNAME);
    fprintf(ptfile, "\n transistor grid connector name................... %s\n", CNS_GRIDNAME);
    fprintf(ptfile, "\n transistor source connector name................. %s\n", CNS_SOURCENAME);
    fprintf(ptfile, "\n transistor drain connector name.................. %s\n", CNS_DRAINNAME);

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {

        /* ------------------------------------------------------------------------
           count vdd cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_VDD) == CNS_VDD) {
            countvdd++;
        }
        /* ------------------------------------------------------------------------
           count gnd cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_GND) == CNS_GND) {
            countgnd++;
        }
        /* ------------------------------------------------------------------------
           count vss cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_VSS) == CNS_VSS) {
            countvss++;
        }
        /* ------------------------------------------------------------------------
           count ext cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
            countext++;
        }
        /* ------------------------------------------------------------------------
           count conflicting cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
            countconflict++;
        }
        /* ------------------------------------------------------------------------
           count tristate cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_TRI) == CNS_TRI) {
            counttri++;
        }
        /* ------------------------------------------------------------------------
           count memsym cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
            countmemsym++;
        }
        /* ------------------------------------------------------------------------
           count latch cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
            countlatch++;
        }
        /* ------------------------------------------------------------------------
           count rs cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_RS) == CNS_RS) {
            countrs++;
        }
        /* ------------------------------------------------------------------------
           count flip-flop cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP) {
            countflipflop++;
        }
        /* ------------------------------------------------------------------------
           count cmos cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_CMOS) == CNS_CMOS) {
            countcmos++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count dual cmos cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) {
            countdualcmos++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count vdd degraded cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED) {
            countvdddegraded++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count gnd degraded cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_GND_DEGRADED) == CNS_GND_DEGRADED) {
            countgnddegraded++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count vss degraded cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED) {
            countvssdegraded++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count nor cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_NOR) == CNS_NOR) {
            countnor++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count or cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_OR) == CNS_OR) {
            countor++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count precharged nor cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_PR_NOR) == CNS_PR_NOR) {
            countprnor++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count sbe cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_SBE) == CNS_SBE) {
            countsbe++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count sbd cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_SBD) == CNS_SBD) {
            countsbd++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count tristate_o cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_TRISTATE_O) == CNS_TRISTATE_O) {
            counttristate_o++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count squirt_o cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_SQUIRT_O) == CNS_SQUIRT_O) {
            countsquirt_o++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count latch_o cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_LATCH_O) == CNS_LATCH_O) {
            countlatch_o++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count precharged bus cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_PR_BUS) == CNS_PR_BUS) {
            countprbus++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count zero cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
            countzero++;
            countcones++;
        }
        /* ------------------------------------------------------------------------
           count one cones
           ------------------------------------------------------------------------ */
        if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
            countone++;
            countcones++;
        }
    }

    for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {

        /* ------------------------------------------------------------------------
           count super buffer cells
           ------------------------------------------------------------------------ */
        if ((ptcell->TYPE & CNS_SUPER_BUFFER) == CNS_SUPER_BUFFER) {
            countsuperbuffer++;
            countcells++;
        }
        /* ------------------------------------------------------------------------
           count tristate cells
           ------------------------------------------------------------------------ */
        if ((ptcell->TYPE & CNS_TRISTATE_E) == CNS_TRISTATE_E) {
            counttristate_e++;
            countcells++;
        }
        /* ------------------------------------------------------------------------
           count squirt buffer cells
           ------------------------------------------------------------------------ */
        if ((ptcell->TYPE & CNS_SQUIRT_BUFFER) == CNS_SQUIRT_BUFFER) {
            countsquirtbuffer++;
            countcells++;
        }
        /* ------------------------------------------------------------------------
           count latch cells
           ------------------------------------------------------------------------ */
        if ((ptcell->TYPE & CNS_LATCH_E) == CNS_LATCH_E) {
            countlatch_e++;
            countcells++;
        }
        /* ------------------------------------------------------------------------
           count read cells
           ------------------------------------------------------------------------ */
        if ((ptcell->TYPE & CNS_READ) == CNS_READ) {
            countread++;
            countcells++;
        }
        /* ------------------------------------------------------------------------
           count master slave flip flop cells
           ------------------------------------------------------------------------ */
        if ((ptcell->TYPE & CNS_MS_FF) == CNS_MS_FF) {
            countmsff++;
            countcells++;
        }
    }

    fprintf(ptfile, "\n");

    fprintf(ptfile, "\n vdd cones........................................ %ld\n", countvdd);
    fprintf(ptfile, "\n gnd cones........................................ %ld\n", countgnd);
    fprintf(ptfile, "\n vss cones........................................ %ld\n", countvss);
    fprintf(ptfile, "\n conflicting cones................................ %ld\n", countconflict);
    fprintf(ptfile, "\n tristate cones................................... %ld\n", counttri);
    fprintf(ptfile, "\n memsym cones..................................... %ld\n", countmemsym);
    fprintf(ptfile, "\n latch cones...................................... %ld\n", countlatch);
    fprintf(ptfile, "\n rs cones......................................... %ld\n", countrs);
    fprintf(ptfile, "\n flip-flop cones.................................. %ld\n", countflipflop);

    fprintf(ptfile, "\n");

    fprintf(ptfile, "\n cmos cones....................................... %ld\n", countcmos);
    fprintf(ptfile, "\n dual cmos cones.................................. %ld\n", countdualcmos);
    fprintf(ptfile, "\n vdd degraded cones............................... %ld\n", countvdddegraded);
    fprintf(ptfile, "\n gnd degraded cones............................... %ld\n", countgnddegraded);
    fprintf(ptfile, "\n vss degraded cones............................... %ld\n", countvssdegraded);
    fprintf(ptfile, "\n nor cones........................................ %ld\n", countnor);
    fprintf(ptfile, "\n or cones......................................... %ld\n", countor);
    fprintf(ptfile, "\n precharged nor cones............................. %ld\n", countprnor);
    fprintf(ptfile, "\n sbe cones........................................ %ld\n", countsbe);
    fprintf(ptfile, "\n sbd cones........................................ %ld\n", countsbd);
    fprintf(ptfile, "\n tristate_o cones................................. %ld\n", counttristate_o);
    fprintf(ptfile, "\n squirt_o cones................................... %ld\n", countsquirt_o);
    fprintf(ptfile, "\n latch_o cones.................................... %ld\n", countlatch_o);
    fprintf(ptfile, "\n precharged bus cones............................. %ld\n", countprbus);
    fprintf(ptfile, "\n zero cones....................................... %ld\n", countzero);
    fprintf(ptfile, "\n one cones........................................ %ld\n", countone);
    fprintf(ptfile, "\n total number of cones --------------------------> %ld\n", countcones);

    fprintf(ptfile, "\n");

    fprintf(ptfile, "\n super buffer cells............................... %ld\n", countsuperbuffer);
    fprintf(ptfile, "\n tristate cells................................... %ld\n", counttristate_e);
    fprintf(ptfile, "\n squirt buffer cells.............................. %ld\n", countsquirtbuffer);
    fprintf(ptfile, "\n latch cells...................................... %ld\n", countlatch_e);
    fprintf(ptfile, "\n read cells....................................... %ld\n", countread);
    fprintf(ptfile, "\n master slave flip flop cells..................... %ld\n", countmsff);
    fprintf(ptfile, "\n total number of cells --------------------------> %ld\n", countcells);

}
