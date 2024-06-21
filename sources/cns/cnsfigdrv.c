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
 
/* 10/23/97 Cone Netlist Structure functions: savecnsfig.c                    */

#include"cnsall.h"

/*============================================================================*
 | function savecnsfig();                                                     |
 | Cone Netlist Structure driver                                              |
 *============================================================================*/
void 
savecnsfig(ptcnsfig, ptlofig)
    cnsfig_list    *ptcnsfig;
    lofig_list     *ptlofig;
{
    if (V_BOOL_TAB[__CNS_DRIVE_VERBOSE].VALUE) savecnvfig(ptcnsfig);
    if (V_BOOL_TAB[__CNS_DRIVE_NORMAL].VALUE) savecnslofig(ptcnsfig, ptlofig, 1);
}

/*============================================================================*
 | function savecnslofig();                                                   |
 | Cone Netlist Structure with embedded lofig driver                          |
 *============================================================================*/
void 
savecnslofig(ptcnsfig, ptlofig, complete)
    cnsfig_list    *ptcnsfig;
    lofig_list     *ptlofig;
    int             complete;
{
    FILE           *ptfile;
    time_t          counter;
    locon_list     *ptlocon;
    short           error = 0;
    lotrs_list     *ptlotrs;
    loins_list     *ptloins;
    losig_list     *ptlosig;
    lorcnet_list   *ptrcnet;
    lowire_list    *ptwire;
    loctc_list     *ptctc;
    ptype_list     *ptptype;
    cone_list      *ptcone;
    char            lotrs_type[8];
    char            losig_type[8];
    cell_list      *ptcell;
    char            cell_type[16];
    long            counter1;
    chain_list     *ptchain;
    num_list       *ptnum;
    cone_list      *ptcone0;
    char            cone_type[8];
    char            cone_tectype[16];
    edge_list      *ptedge;
    cone_list      *ptcone1;
    branch_list    *ptbranch;
    chain_list     *ptchain0;
    ptype_list     *ptptype0;
    chain_list     *ptchain1;
    ptype_list     *ptptype1;
    char           *ptname;
    optparam_list  *ptopt;
    int             first;
    int cachemode;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns savecnsfig in\n");

    if (ptcnsfig == NULL || ptlofig == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " savecnsfig() impossible for %s.cns : both logical and CNS figures are required\n", ptcnsfig->NAME);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns savecnsfig out\n");
        EXIT(-1);
    }

/* ---------------------------------------------------------------------------
   open file
   --------------------------------------------------------------------------- */

    if ((ptfile = mbkfopen(ptcnsfig->NAME, "cns", WRITE_TEXT)) == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " savecnsfig() impossible: can't open file %s.cns\n", ptcnsfig->NAME);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns savecnsfig out\n");
        EXIT(-1);
    }

/* ---------------------------------------------------------------------------
   header
   --------------------------------------------------------------------------- */
    time(&counter);
    fprintf(ptfile, "(CNS V%s %s %s %s %s %s", CNS_VERSION, CNS_TECHNO, IN_LO, ptcnsfig->NAME, getenv("USER"), ctime(&counter));

/* ---------------------------------------------------------------------------
   capacity scale
   --------------------------------------------------------------------------- */
    fprintf(ptfile, "\n(SCALE\n(X %ld))\n", SCALE_X);

/* ---------------------------------------------------------------------------
   signal indexing
   --------------------------------------------------------------------------- */
    if (ptlofig != NULL) {
        counter = 0;
        for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
            counter++;
            if ((ptptype = getptype(ptlosig->USER, (long) CNS_INDEX)) != NULL) {
                ptptype->DATA = (void *) counter;
            }
            else {
                ptlosig->USER = addptype(ptlosig->USER, (long) CNS_INDEX, (void *) counter);
            }
        }
    }
/* ---------------------------------------------------------------------------
   Cone Netlist Structure external connector list
   --------------------------------------------------------------------------- */
    counter = 0;
    fprintf(ptfile, "\n(CONNECTORS");
    for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
        counter++;
        if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) counter);
        }
        if (strchr(ptlocon->NAME, ' ') != NULL) {
            fprintf(ptfile, "\n(%ld \"%s\"", counter, ptlocon->NAME);
        }
        else fprintf(ptfile, "\n(%ld %s", counter, ptlocon->NAME);
        if (ptlocon->DIRECTION == CNS_I)
            fprintf(ptfile, " I");
        else if (ptlocon->DIRECTION == CNS_O)
            fprintf(ptfile, " O");
        else if (ptlocon->DIRECTION == CNS_B)
            fprintf(ptfile, " B");
        else if (ptlocon->DIRECTION == CNS_Z)
            fprintf(ptfile, " Z");
        else if (ptlocon->DIRECTION == CNS_T)
            fprintf(ptfile, " T");
        else if (ptlocon->DIRECTION == CNS_X)
            fprintf(ptfile, " X");
        else if (ptlocon->DIRECTION == CNS_VDDC)
            fprintf(ptfile, " Vdd");
        else if (ptlocon->DIRECTION == CNS_GNDC)
            fprintf(ptfile, " Gnd");
        else if (ptlocon->DIRECTION == CNS_VSSC)
            fprintf(ptfile, " Vss");
        else {
            fprintf(ptfile, " ?");
            error = 1;
        }
        if (ptlocon->TYPE == 'I')
            fprintf(ptfile, " I");
        else if (ptlocon->TYPE == 'E')
            fprintf(ptfile, " E");
        else if (ptlocon->TYPE == 'X')
            fprintf(ptfile, " X");
        else {
            fprintf(ptfile, " ?");
            error = 1;
        }
        if (ptlofig != NULL && (ptptype = getptype(ptlocon->SIG->USER, (long) CNS_INDEX)) != NULL) {
            fprintf(ptfile, " %ld", (long) ptptype->DATA);
            if (ptlocon->PNODE != NULL && complete) {
                fprintf(ptfile, " (#");
                for (ptnum = ptlocon->PNODE; ptnum; ptnum = ptnum->NEXT) {
                    fprintf(ptfile, " %ld", ptnum->DATA);
                }
                fprintf(ptfile, ")");
            }
        }
        ptptype = getptype(ptlocon->USER, CNS_TYPELOCON);
        if (ptptype != NULL) {
            if (((long)ptptype->DATA & CNS_ONE) == CNS_ONE) fprintf(ptfile, " - 1");
            else if (((long)ptptype->DATA & CNS_ZERO) == CNS_ZERO) fprintf(ptfile, " - 0");
        }
        fprintf(ptfile, ")");
    }
    fprintf(ptfile, ")\n");

/* ---------------------------------------------------------------------------
   Cone Netlist Structure internal connector list
   --------------------------------------------------------------------------- */
    if (ptcnsfig->INTCON != NULL) {
        fprintf(ptfile, "\n(INTERNAL");
        for (ptlocon = ptcnsfig->INTCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            counter++;
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
                ptptype->DATA = (void *) counter;
            }
            else {
                ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) counter);
            }
            if (strchr(ptlocon->NAME, ' ') != NULL) {
                fprintf(ptfile, "\n(%ld \"%s\"", counter, ptlocon->NAME);
            }
            else fprintf(ptfile, "\n(%ld %s", counter, ptlocon->NAME);
            if (ptlocon->DIRECTION == CNS_I)
                fprintf(ptfile, " I");
            else if (ptlocon->DIRECTION == CNS_O)
                fprintf(ptfile, " O");
            else if (ptlocon->DIRECTION == CNS_B)
                fprintf(ptfile, " B");
            else if (ptlocon->DIRECTION == CNS_Z)
                fprintf(ptfile, " Z");
            else if (ptlocon->DIRECTION == CNS_T)
                fprintf(ptfile, " T");
            else if (ptlocon->DIRECTION == CNS_X)
                fprintf(ptfile, " X");
            else if (ptlocon->DIRECTION == CNS_VDDC)
                fprintf(ptfile, " Vdd");
            else if (ptlocon->DIRECTION == CNS_GNDC)
                fprintf(ptfile, " Gnd");
            else if (ptlocon->DIRECTION == CNS_VSSC)
                fprintf(ptfile, " Vss");
            else {
                fprintf(ptfile, " ?");
                error = 1;
            }
            if (ptlofig != NULL && (ptptype = getptype(ptlocon->SIG->USER, (long) CNS_INDEX)) != NULL) {
                fprintf(ptfile, " %ld", (long) ptptype->DATA);
                if (ptlocon->PNODE != NULL) {
                    fprintf(ptfile, " (#");
                    for (ptnum = ptlocon->PNODE; ptnum; ptnum = ptnum->NEXT) {
                        fprintf(ptfile, " %ld", ptnum->DATA);
                    }
                    fprintf(ptfile, ")");
                }
            }
            fprintf(ptfile, ")");
        }
        fprintf(ptfile, ")\n");
    }

/* ---------------------------------------------------------------------------
   Lofig Structure signal list
   --------------------------------------------------------------------------- */
    if (ptlofig!= NULL) {
        cachemode=rcn_hascache(ptlofig);
        fprintf(ptfile, "\n(SIGNALS");
        for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
            ptptype = getptype(ptlosig->USER, CNS_INDEX);
            counter = (long) ptptype->DATA;
            if (complete) fprintf(ptfile, "\n");
            fprintf(ptfile, "\n(%ld (", counter);
            if ((ptchain = ptlosig->NAMECHAIN) != NULL) {
                ptname = (char *)ptchain->DATA;
                if (strchr(ptname, ' ') != NULL) {
                    fprintf(ptfile, "\"%s\"", ptname);
                }
                else fprintf(ptfile, "%s", ptname);
                while ((ptchain = ptchain->NEXT) != NULL) {
                    ptname = (char *)ptchain->DATA;
                    if (strchr(ptname, ' ') != NULL) {
                        fprintf(ptfile, " \"%s\"", ptname);
                    }
                    else fprintf(ptfile, " %s", ptname);
                }
            }
            else
                fprintf(ptfile, "cns_sig%ld", counter);
            strcpy(losig_type, "");
            switch (ptlosig->TYPE) {
            case CNS_SIGINT:
                strcat(losig_type, "I");
                break;
            case CNS_SIGEXT:
                strcat(losig_type, "E");
                break;
            case CNS_SIGVDD:
                strcat(losig_type, "D");
                break;
            case CNS_SIGVSS:
                strcat(losig_type, "S");
                break;
            case CNS_SIGTRANSP:
                strcat(losig_type, "T");
                break;
            default:
                strcat(losig_type, "?");
            }
            fprintf(ptfile, ") %s", losig_type);
            if ((ptptype = getptype(ptlosig->USER, LOSIGALIM)) != NULL) {
                fprintf(ptfile, " (%ld)", (long)ptptype->DATA);
            }
            if ((ptrcnet = ptlosig->PRCN) != NULL && complete) {
              if (!(cachemode && (mbk_LosigIsVDD(ptlosig) || mbk_LosigIsVSS(ptlosig))))
                {
                  rcn_lock_signal(ptlofig, ptlosig);
                  fprintf(ptfile, " %3.2f", rcn_getcapa( ptlofig, ptlosig ) * 1000);
                  if (ptrcnet->PWIRE != NULL) {
                      fprintf(ptfile, "\n(WIRES");
                      for (ptwire = ptrcnet->PWIRE; ptwire; ptwire = ptwire->NEXT) {
                          fprintf(ptfile, "\n(X");
                          fprintf(ptfile, " %ld %ld %g %g)", ptwire->NODE1, ptwire->NODE2, ptwire->RESI, ptwire->CAPA * 1000);
                      }
                      fprintf(ptfile, ")");
                  }
                  rcn_unlock_signal(ptlofig, ptlosig);
                }
            }
            else if (ptrcnet != NULL) {
                fprintf(ptfile, " %3.2f", rcn_getcapa( ptlofig, ptlosig ) * 1000);
            }
            else
                fprintf(ptfile, " 0");
            fprintf(ptfile, ")");
        }
        fprintf(ptfile, ")\n");
    }

/* ---------------------------------------------------------------------------
   Crosstalk Capacitances list              
   --------------------------------------------------------------------------- */
    if (ptlofig != NULL && complete) {
        fprintf(ptfile, "\n(CROSSTALKS");
        for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
            if (ptlosig->PRCN == NULL) continue;
            if (cachemode && (mbk_LosigIsVDD(ptlosig) || mbk_LosigIsVSS(ptlosig))) continue;
            rcn_lock_signal(ptlofig, ptlosig);
            for (ptchain = ptlosig->PRCN->PCTC; ptchain; ptchain = ptchain->NEXT) {
                ptctc = (loctc_list *)ptchain->DATA;
                if (rcn_capacitancetooutput(cachemode, ptlosig, ptctc)) {
                    fprintf(ptfile, "\n(K %g", ptctc->CAPA * 1000);
                    ptptype = getptype(ptctc->SIG1->USER, CNS_INDEX);
                    counter = (long) ptptype->DATA;
                    fprintf(ptfile, " %ld", counter);
                    fprintf(ptfile, " %ld", ptctc->NODE1);
                    ptptype = getptype(ptctc->SIG2->USER, CNS_INDEX);
                    counter = (long) ptptype->DATA;
                    fprintf(ptfile, " %ld", counter);
                    fprintf(ptfile, " %ld)", ptctc->NODE2);
                }
            }
            rcn_unlock_signal(ptlofig, ptlosig);
        }
        fprintf(ptfile, ")\n");
    }

/* ---------------------------------------------------------------------------
   Cone Netlist Structure transistor list
   --------------------------------------------------------------------------- */
    counter = 0;
    fprintf(ptfile, "\n(TRANSISTORS");
    for (ptlotrs = ptcnsfig->LOTRS; ptlotrs != NULL; ptlotrs = ptlotrs->NEXT) {
        counter++;
        if ((ptptype = getptype(ptlotrs->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptlotrs->USER = addptype(ptlotrs->USER, (long) CNS_INDEX, (void *) counter);
        }
        fprintf(ptfile, "\n(%ld", counter);
        if (ptlotrs->TRNAME != NULL) {
            if (isdigit((int)*(ptlotrs->TRNAME))) fprintf(ptfile, " cns_%s", ptlotrs->TRNAME);
            else fprintf(ptfile, " %s", ptlotrs->TRNAME);
        }
        else {
            fprintf(ptfile, " noname");
        }
        if ((ptptype = getptype(ptlotrs->USER, (long) CNS_DRIVINGCONE)) != NULL) {
            fprintf(ptfile, " %ld", ((cone_list *) ptptype->DATA)->INDEX);
        }
        else if ((ptptype = getptype(ptlotrs->USER, (long) CNS_LINKTYPE)) != NULL) {
            if (((long) ptptype->DATA & CNS_ACTIVE) == CNS_ACTIVE) {
                if ((ptcone = (cone_list *) ptlotrs->GRID) != NULL) {
                    fprintf(ptfile, " %ld", ptcone->INDEX);
                }
                else {
                    fprintf(ptfile, " ?");
                    error = 1;
                }
            }
            else if (((long) ptptype->DATA & CNS_PULL) == CNS_PULL)
                fprintf(ptfile, " puller");
            else if (((long) ptptype->DATA & (CNS_DIODE_UP & CNS_DIODE_DOWN)) == (CNS_DIODE_UP & CNS_DIODE_DOWN))
                fprintf(ptfile, " diode");
            else if (((long) ptptype->DATA & CNS_RESIST) == CNS_RESIST)
                fprintf(ptfile, " resistance");
            else if (((long) ptptype->DATA & CNS_CAPA) == CNS_CAPA)
                fprintf(ptfile, " capacity");
            else {
                fprintf(ptfile, " ?");
                error = 1;
            }
        }
        /*else if ((ptcone = (cone_list *) ptlotrs->GRID) != NULL) {
            fprintf(ptfile, " %ld", ptcone->INDEX);
        }*/
        else {
            fprintf(ptfile, " 0");
        }
        if (ptlotrs->X != LONG_MIN) fprintf(ptfile, " %ld", ptlotrs->X);
        else fprintf(ptfile, " 0");
        if (ptlotrs->X != LONG_MIN) fprintf(ptfile, " %ld", ptlotrs->Y);
        else fprintf(ptfile, " 0");
        fprintf(ptfile, " %ld %ld", ptlotrs->WIDTH, ptlotrs->LENGTH);
        fprintf(ptfile, " %ld %ld", ptlotrs->PS, ptlotrs->PD);
        fprintf(ptfile, " %ld %ld", ptlotrs->XS, ptlotrs->XD);
        if (ptlofig != NULL) {
            ptlosig = ptlotrs->DRAIN->SIG;
            ptptype = getptype(ptlosig->USER, CNS_INDEX);
            fprintf(ptfile, " %ld", (long) ptptype->DATA);
            if (ptlotrs->DRAIN->PNODE != NULL && complete) {
                fprintf(ptfile, " (#");
                for (ptnum = ptlotrs->DRAIN->PNODE; ptnum; ptnum = ptnum->NEXT) {
                    fprintf(ptfile, " %ld", (long) ptnum->DATA);
                }
                fprintf(ptfile, ")");
            }
            ptlosig = ptlotrs->GRID->SIG;
            ptptype = getptype(ptlosig->USER, CNS_INDEX);
            fprintf(ptfile, " %ld", (long) ptptype->DATA);
            if (ptlotrs->GRID->PNODE != NULL && complete) {
                fprintf(ptfile, " (#");
                for (ptnum = ptlotrs->GRID->PNODE; ptnum; ptnum = ptnum->NEXT) {
                    fprintf(ptfile, " %ld", (long) ptnum->DATA);
                }
                fprintf(ptfile, ")");
            }
            ptlosig = ptlotrs->SOURCE->SIG;
            ptptype = getptype(ptlosig->USER, CNS_INDEX);
            fprintf(ptfile, " %ld", (long) ptptype->DATA);
            if (ptlotrs->SOURCE->PNODE != NULL && complete) {
                fprintf(ptfile, " (#");
                for (ptnum = ptlotrs->SOURCE->PNODE; ptnum; ptnum = ptnum->NEXT) {
                    fprintf(ptfile, " %ld", (long) ptnum->DATA);
                }
                fprintf(ptfile, ")");
            }
	    if (ptlotrs->BULK==NULL) ptlosig=NULL;
	    else ptlosig = ptlotrs->BULK->SIG;
            if (ptlosig != NULL) {
	            ptptype = getptype(ptlosig->USER, CNS_INDEX);
	            fprintf(ptfile, " %ld", (long) ptptype->DATA);
	            if (ptlotrs->BULK->PNODE != NULL && complete) {
	                fprintf(ptfile, " (#");
	                for (ptnum = ptlotrs->BULK->PNODE; ptnum; ptnum = ptnum->NEXT) {
	                    fprintf(ptfile, " %ld", (long) ptnum->DATA);
	                }
	                fprintf(ptfile, ")");
	            }
            }
            else fprintf(ptfile, " 0");
        }
        strcpy(lotrs_type, "");
        if (strcmp(CNS_TECHNO, "edmesfet") == 0) {
            if (MLO_IS_TRANSN(ptlotrs->TYPE))
                strcat(lotrs_type, "E");
            else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                strcat(lotrs_type, "D");
        }
        else if (strcmp(CNS_TECHNO, "npmosfet") == 0) {
            if (MLO_IS_TRANSN(ptlotrs->TYPE))
                strcat(lotrs_type, "N");
            else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                strcat(lotrs_type, "P");
        }
        if (strlen(lotrs_type) == 0) {
            strcat(lotrs_type, "?");
            error = 1;
        }
        if (ptlotrs->MODINDEX == EMPTYHT) fprintf(ptfile, " %s", lotrs_type);
        else fprintf(ptfile, " %s %s", lotrs_type, getlotrsmodel(ptlotrs));
        fprintf(ptfile, " (");
        if ((ptptype = getptype(ptlotrs->USER, OPT_PARAMS)) != NULL) {
            first = 1;
            for (ptopt = (optparam_list*)ptptype->DATA; ptopt; ptopt = ptopt->NEXT) {
                if (isknowntrsparam(ptopt->UNAME.STANDARD)) {
                    if (**(ptopt->UNAME.STANDARD) != '$') {
                        if (first) {
                            first = 0;
                        }
                        else fprintf(ptfile, " ");
                        fprintf(ptfile, "%s=%g", *(ptopt->UNAME.STANDARD), ptopt->UDATA.VALUE);
                    }
                }
            }            
        }
        fprintf(ptfile, ")");
        fprintf(ptfile, " (");
        if ((ptptype = getptype(ptlotrs->USER, PARAM_CONTEXT)) != NULL) {
           char *subcktname;
           eqt_param *from=(eqt_param *)ptptype->DATA;      
           int i;
           subcktname=mbk_get_subckt_name(ptlotrs);
           fprintf(ptfile, " %s", subcktname!=NULL?subcktname:"");
           for (i = 0; i < from->NUMBER; i++)
             fprintf(ptfile, " %s=%g", from->EBI[i].NAME, from->EBI[i].VAL);
        }
        fprintf(ptfile, ")");
        fprintf(ptfile, ")");
    }
    fprintf(ptfile, ")\n");

/* ---------------------------------------------------------------------------
   Cone Netlist Structure instance list
   --------------------------------------------------------------------------- */
    counter = 0;
    fprintf(ptfile, "\n(INSTANCES");
    for (ptloins = ptcnsfig->LOINS; ptloins != NULL; ptloins = ptloins->NEXT) {
        counter++;
        if ((ptptype = getptype(ptloins->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptloins->USER = addptype(ptloins->USER, (long) CNS_INDEX, (void *) counter);
        }
        fprintf(ptfile, "\n\n(%ld", counter);
        if (strchr(ptloins->INSNAME, ' ') != NULL) {
            fprintf(ptfile, " \"%s\" %s", ptloins->INSNAME, ptloins->FIGNAME);
        }
        else fprintf(ptfile, " %s %s", ptloins->INSNAME, ptloins->FIGNAME);
        counter1 = 0;
        fprintf(ptfile, "\n(CONNECTORS");
        ptloins->LOCON = (locon_list *)reverse((chain_list *)ptloins->LOCON);
        for (ptlocon = ptloins->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            counter1++;
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
                ptptype->DATA = (void *) counter1;
            }
            else {
                ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) counter1);
            }
            if (strchr(ptlocon->NAME, ' ') != NULL) {
                fprintf(ptfile, "\n(%ld \"%s\"", counter1, ptlocon->NAME);
            }
            else fprintf(ptfile, "\n(%ld %s", counter1, ptlocon->NAME);
            if (ptlocon->DIRECTION == CNS_I)
                fprintf(ptfile, " I");
            else if (ptlocon->DIRECTION == CNS_O)
                fprintf(ptfile, " O");
            else if (ptlocon->DIRECTION == CNS_B)
                fprintf(ptfile, " B");
            else if (ptlocon->DIRECTION == CNS_Z)
                fprintf(ptfile, " Z");
            else if (ptlocon->DIRECTION == CNS_T)
                fprintf(ptfile, " T");
            else if (ptlocon->DIRECTION == CNS_X)
                fprintf(ptfile, " X");
            else if (ptlocon->DIRECTION == CNS_VDDC)
                fprintf(ptfile, " Vdd");
            else if (ptlocon->DIRECTION == CNS_GNDC)
                fprintf(ptfile, " Gnd");
            else if (ptlocon->DIRECTION == CNS_VSSC)
                fprintf(ptfile, " Vss");
            else {
                fprintf(ptfile, " ?");
                error = 1;
            }
            if (ptlofig != NULL && (ptptype = getptype(ptlocon->SIG->USER, (long) CNS_INDEX)) != NULL) {
                fprintf(ptfile, " %ld", (long) ptptype->DATA);
                if (ptlocon->PNODE != NULL && complete) {
                    fprintf(ptfile, " (#");
                    for (ptnum = ptlocon->PNODE; ptnum; ptnum = ptnum->NEXT) {
                        fprintf(ptfile, " %ld", ptnum->DATA);
                    }
                    fprintf(ptfile, ")");
                }
            }
            fprintf(ptfile, ")");
        }
        ptloins->LOCON = (locon_list *)reverse((chain_list *)ptloins->LOCON);
        fprintf(ptfile, "))");
    }
    fprintf(ptfile, ")\n");

/* ---------------------------------------------------------------------------
   Cone Netlist Structure cell list
   --------------------------------------------------------------------------- */
    if (ptcnsfig->CELL != NULL) {
        fprintf(ptfile, "\n(CELLS");
        counter = 0;
        for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {
            if (ptcell->CONES == NULL) continue;
            counter++;
            if ((ptptype = getptype(ptcell->USER, (long) CNS_INDEX)) != NULL) {
                ptptype->DATA = (void *) counter;
            }
            else {
                ptcell->USER = addptype(ptcell->USER, (long) CNS_INDEX, (void *) counter);
            }
            fprintf(ptfile, "\n\n(%ld", counter);
            strcpy(cell_type, "");
            if ((ptcell->TYPE & CNS_UNKNOWN) == CNS_UNKNOWN) {
                if (ptcell->TYPE != 0) {
                    fprintf(ptfile, " %ld", (ptcell->TYPE & ~CNS_UNKNOWN));
                }
                else {
                    strcat(cell_type, "?");
                    error = 1;
                }
            }
            else {
                if ((ptcell->TYPE & CNS_SUPER_BUFFER) == CNS_SUPER_BUFFER)
                    strcat(cell_type, "Super_Buffer");
                else if ((ptcell->TYPE & CNS_TRISTATE_E) == CNS_TRISTATE_E)
                    strcat(cell_type, "Tristate");
                else if ((ptcell->TYPE & CNS_SQUIRT_BUFFER) == CNS_SQUIRT_BUFFER)
                    strcat(cell_type, "Squirt_Buffer");
                else if ((ptcell->TYPE & CNS_LATCH_E) == CNS_LATCH_E)
                    strcat(cell_type, "Latch");
                else if ((ptcell->TYPE & CNS_READ) == CNS_READ)
                    strcat(cell_type, "Read");
                else if ((ptcell->TYPE & CNS_MS_FF) == CNS_MS_FF)
                    strcat(cell_type, "Ms_Ff");
                if (strlen(cell_type) == 0) {
                    strcat(cell_type, "?");
                    error = 1;
                }
                fprintf(ptfile, " %s", cell_type);
            }
            if ((ptchain = ptcell->CONES) != NULL) {
                fprintf(ptfile, "\n(%ld", ((cone_list *) ptchain->DATA)->INDEX);
                counter1 = 1;
                for (ptchain = ptchain->NEXT; ptchain != NULL; ptchain = ptchain->NEXT) {
                    if (counter1++ == 9) {
                        fprintf(ptfile, "\n");
                        counter1 = 1;
                    }
                    if (ptchain->DATA != NULL) {
                        fprintf(ptfile, " %ld", ((cone_list *) ptchain->DATA)->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
            }
            else {
                fprintf(ptfile, "\n(?");
                error = 1;
            }
            fprintf(ptfile, "))");
        }
        fprintf(ptfile, ")\n");
    }

/* ---------------------------------------------------------------------------
   Cone Netlist Structure cone list
   --------------------------------------------------------------------------- */
    fprintf(ptfile, "\n(CONES");
    for (ptcone0 = ptcnsfig->CONE; ptcone0 != NULL; ptcone0 = ptcone0->NEXT) {
        if (strchr(ptcone0->NAME, ' ') != NULL) {
            fprintf(ptfile, "\n\n(%ld \"%s\"", ptcone0->INDEX, ptcone0->NAME);
        }
        else fprintf(ptfile, "\n\n(%ld %s", ptcone0->INDEX, ptcone0->NAME);
        strcpy(cone_type, "");
        if ((ptcone0->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
            strcat(cone_type, "M");
        if ((ptcone0->TYPE & CNS_LATCH) == CNS_LATCH)
            strcat(cone_type, "L");
        if ((ptcone0->TYPE & CNS_RS) == CNS_RS)
            strcat(cone_type, "S");
        if ((ptcone0->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP)
            strcat(cone_type, "F");
        if ((ptcone0->TYPE & CNS_MASTER) == CNS_MASTER)
            strcat(cone_type, "m");
        if ((ptcone0->TYPE & CNS_SLAVE) == CNS_SLAVE)
            strcat(cone_type, "e");
        if ((ptcone0->TYPE & CNS_PRECHARGE) == CNS_PRECHARGE)
            strcat(cone_type, "P");
        if ((ptcone0->TYPE & CNS_TRI) == CNS_TRI)
            strcat(cone_type, "T");
        if ((ptcone0->TYPE & CNS_CONFLICT) == CNS_CONFLICT)
            strcat(cone_type, "C");
        if ((ptcone0->TYPE & CNS_EXT) == CNS_EXT)
            strcat(cone_type, "Ext");
        if ((ptcone0->TYPE & CNS_VDD) == CNS_VDD)
            strcat(cone_type, "Vdd");
        if ((ptcone0->TYPE & CNS_GND) == CNS_GND)
            strcat(cone_type, "Gnd");
        if ((ptcone0->TYPE & CNS_VSS) == CNS_VSS)
            strcat(cone_type, "Vss");
/* if (strlen(cone_type) == 0) {
   strcat(cone_type, "?");
   error = 1;
   } */
        fprintf(ptfile, " %s -", cone_type);

        strcpy(cone_tectype, "");
        if ((ptcone0->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS)
            strcat(cone_tectype, "dC");
        else if ((ptcone0->TECTYPE & CNS_CMOS) == CNS_CMOS)
            strcat(cone_tectype, "Cm");
        if ((ptcone0->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED)
            strcat(cone_tectype, "VddD");
        if ((ptcone0->TECTYPE & CNS_GND_DEGRADED) == CNS_GND_DEGRADED)
            strcat(cone_tectype, "GndD");
        if ((ptcone0->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED)
            strcat(cone_tectype, "VssD");
        if ((ptcone0->TECTYPE & CNS_NOR) == CNS_NOR)
            strcat(cone_tectype, "Nor");
        if ((ptcone0->TECTYPE & CNS_OR) == CNS_OR)
            strcat(cone_tectype, "Or");
        if ((ptcone0->TECTYPE & CNS_PR_NOR) == CNS_PR_NOR)
            strcat(cone_tectype, "Pr_Nor");
        if ((ptcone0->TECTYPE & CNS_NAND) == CNS_NAND)
            strcat(cone_tectype, "Nand");
        if ((ptcone0->TECTYPE & CNS_SBE) == CNS_SBE)
            strcat(cone_tectype, "Sbe");
        if ((ptcone0->TECTYPE & CNS_SBD) == CNS_SBD)
            strcat(cone_tectype, "Sbd");
        if ((ptcone0->TECTYPE & CNS_TRISTATE_O) == CNS_TRISTATE_O)
            strcat(cone_tectype, "Tristate_o");
        if ((ptcone0->TECTYPE & CNS_SQUIRT_O) == CNS_SQUIRT_O)
            strcat(cone_tectype, "Squirt_o");
        if ((ptcone0->TECTYPE & CNS_LATCH_O) == CNS_LATCH_O)
            strcat(cone_tectype, "Latch_o");
        if ((ptcone0->TECTYPE & CNS_PR_BUS) == CNS_PR_BUS)
            strcat(cone_tectype, "Pr_Bus");
        if ((ptcone0->TECTYPE & CNS_ZERO) == CNS_ZERO)
            strcat(cone_tectype, "Zero");
        if ((ptcone0->TECTYPE & CNS_ONE) == CNS_ONE)
            strcat(cone_tectype, "One");
        if (strlen(cone_tectype) == 0) {
            strcat(cone_tectype, "X");
        }
        fprintf(ptfile, " %s", cone_tectype);

#ifdef USEOLDTEMP
        fprintf(ptfile, " %ld %ld %ld %ld", ptcone0->XM, ptcone0->Xm, ptcone0->YM, ptcone0->Ym);
#else
        fprintf(ptfile, " %d %d %d %d", 0, 0, 0, 0);
#endif
        /* ------------------------------------------------------------------------
           cone inputs
           ------------------------------------------------------------------------ */
        fprintf(ptfile, "\n(INPUT");
        if (ptcone0->INCONE != NULL) {
            for (ptedge = ptcone0->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
                    fprintf(ptfile, "\n(Cone");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                            fprintf(ptfile, " bleeder");
                        }
                        if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                            fprintf(ptfile, " command");
                        }
                        if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                            fprintf(ptfile, " async");
                        }
                        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                            fprintf(ptfile, " loop");
                        }
                        if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                            fprintf(ptfile, " feedback");
                        }
                        if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                            fprintf(ptfile, " memsym");
                        }
                        if ((ptedge->TYPE & CNS_HZCOM) == CNS_HZCOM) {
                            fprintf(ptfile, " hz_command");
                        }
                        fprintf(ptfile, " %ld)", ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_VDD) == CNS_VDD) {
                    fprintf(ptfile, "\n(Vdd");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        fprintf(ptfile, " %ld)", ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_GND) == CNS_GND) {
                    fprintf(ptfile, "\n(Gnd");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        fprintf(ptfile, " %ld)", ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_VSS) == CNS_VSS) {
                    fprintf(ptfile, "\n(Vss");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        fprintf(ptfile, " %ld)", ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
                    fprintf(ptfile, "\n(Ext");
                    ptlocon = ptedge->UEDGE.LOCON;
                    if ((ptlocon != NULL) && ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL)) {
                        fprintf(ptfile, " %ld)", (long) ptptype->DATA);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else {
                    fprintf(ptfile, "\n(? ?)");
                    error = 1;
                }
            }
        }
        fprintf(ptfile, ")");

        /* ------------------------------------------------------------------------
           cone outputs
           ------------------------------------------------------------------------ */
        fprintf(ptfile, "\n(OUTPUT");
        if (ptcone0->OUTCONE != NULL) {
            for (ptedge = ptcone0->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
                    fprintf(ptfile, "\n(Cone");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                            fprintf(ptfile, " bleeder");
                        }
                        if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                            fprintf(ptfile, " command");
                        }
                        if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                            fprintf(ptfile, " async");
                        }
                        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                            fprintf(ptfile, " loop");
                        }
                        if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                            fprintf(ptfile, " feedback");
                        }
                        if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                            fprintf(ptfile, " memsym");
                        }
                        fprintf(ptfile, " %ld)", ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
                    fprintf(ptfile, "\n(Ext");
                    ptlocon = ptedge->UEDGE.LOCON;
                    if ((ptlocon != NULL) && ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL)) {
                        fprintf(ptfile, " %ld)", (long) ptptype->DATA);
                    }
                    else {
                        fprintf(ptfile, " ?)");
                        error = 1;
                    }
                }
                else {
                    fprintf(ptfile, "\n(? ?)");
                    error = 1;
                }
            }
        }
        fprintf(ptfile, ")");

        /* ------------------------------------------------------------------------
           cone branches
           ------------------------------------------------------------------------ */
        fprintf(ptfile, "\n(BRANCHES");
        counter = 0;
        for (ptbranch = ptcone0->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbr(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        for (ptbranch = ptcone0->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbr(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        for (ptbranch = ptcone0->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbr(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        for (ptbranch = ptcone0->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbr(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        fprintf(ptfile, ")");


        /* ------------------------------------------------------------------------
           cells containg the cone
           ------------------------------------------------------------------------ */
        if ((ptchain = ptcone0->CELLS) != NULL) {
            fprintf(ptfile, "\n(CELLS");
            if ((ptptype = getptype(((cell_list *) ptchain->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                fprintf(ptfile, "\n(%ld", (long) ptptype->DATA);
            }
            else {
                fprintf(ptfile, "\n(?");
            }
            counter1 = 1;
            for (ptchain = ptchain->NEXT; ptchain != NULL; ptchain = ptchain->NEXT) {
                if (counter1++ == 9) {
                    fprintf(ptfile, "\n");
                    counter1 = 1;
                }
                if (ptchain->DATA != NULL) {
                    if ((ptptype = getptype(((cell_list *) ptchain->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                        fprintf(ptfile, " %ld", (long) ptptype->DATA);
                    }
                    else {
                        fprintf(ptfile, " ?");
                    }
                }
                else {
                    fprintf(ptfile, " ?");
                    error = 1;
                }
            }
            fprintf(ptfile, "))");
        }

        /* ------------------------------------------------------------------------
           cone user fields
           ------------------------------------------------------------------------ */
        if (((getptype(ptcone0->USER, (long) CNS_PARALLEL)) != NULL)
            || ((getptype(ptcone0->USER, (long) CNS_BLEEDER)) != NULL)
            || ((getptype(ptcone0->USER, (long) CNS_SIGNATURE)) != NULL)
            || ((getptype(ptcone0->USER, (long) CNS_SWITCH)) != NULL)
            || getptype(ptcone0->USER, (long) CNS_SIGNAL) != NULL) {
            fprintf(ptfile, "\n(USER");

            /* ---------------------------------------------------------------------
               cone parallel branches
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_PARALLEL)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n(PARALLEL");
                    for (ptchain0 = ptchain0; ptchain0 != NULL; ptchain0 = ptchain0->NEXT) {
                        if (ptchain0->DATA != NULL) {
                            ptchain1 = (chain_list *) ptchain0->DATA;
                            if ((ptptype1 = getptype(((branch_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                fprintf(ptfile, "\n(%ld", (long) ptptype1->DATA);
                            }
                            else {
                                fprintf(ptfile, "\n(?");
                            }
                            counter1 = 1;
                            for (ptchain1 = ptchain1->NEXT; ptchain1 != NULL; ptchain1 = ptchain1->NEXT) {
                                if (counter1++ == 9) {
                                    fprintf(ptfile, "\n");
                                    counter1 = 1;
                                }
                                if ((ptptype1 = getptype(((branch_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                    fprintf(ptfile, " %ld", (long) ptptype1->DATA);
                                }
                                else {
                                    fprintf(ptfile, " ?");
                                }
                            }
                            fprintf(ptfile, ")");
                        }
                    }
                    fprintf(ptfile, ")");
                }
            }

            /* ---------------------------------------------------------------------
               cone bleeders
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_BLEEDER)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n(BLEEDER");
                    if ((ptptype0 = getptype(((branch_list *) ptchain0->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                        fprintf(ptfile, "\n(%ld", (long) ptptype0->DATA);
                    }
                    else {
                        fprintf(ptfile, "\n(?");
                    }
                    counter1 = 1;
                    for (ptchain1 = ptchain0->NEXT; ptchain1 != NULL; ptchain1 = ptchain1->NEXT) {
                        if (counter1++ == 9) {
                            fprintf(ptfile, "\n");
                            counter1 = 1;
                        }
                        if ((ptptype1 = getptype(((branch_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                            fprintf(ptfile, " %ld", (long) ptptype1->DATA);
                        }
                        else {
                            fprintf(ptfile, " ?");
                        }
                    }
                    fprintf(ptfile, "))");
                }
            }

            /* ---------------------------------------------------------------------
               cone signature
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_SIGNATURE)) != NULL) {
                if (ptptype->DATA != NULL) {
                    fprintf(ptfile, "\n(SIGNATURE");
                    fprintf(ptfile, "\n(%s", ((char *) ptptype->DATA));
                    fprintf(ptfile, "))");
                }
            }
            /* ---------------------------------------------------------------------
               cone signal
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_SIGNAL)) != NULL && ptlofig != NULL) {
                if (ptptype->DATA != NULL) {
                    ptlosig = (losig_list *)ptptype->DATA;
                    ptptype1 = getptype(ptlosig->USER, CNS_INDEX);
                    fprintf(ptfile, "\n(SIGIDX");
                    fprintf(ptfile, "\n(%ld", (long)ptptype1->DATA);
                    fprintf(ptfile, "))");
                }
            }
            /* ---------------------------------------------------------------------
               cone switches
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_SWITCH)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n(SWITCH\n");
                    counter1 = 1;
                    for (ptchain0 = ptchain0; ptchain0 != NULL; ptchain0 = ptchain0->NEXT) {
                        if (counter1++ == 5) {
                            fprintf(ptfile, "\n");
                            counter1 = 1;
                        }
                        if ((ptchain1 = (chain_list *) ptchain0->DATA) != NULL) {
                            if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                fprintf(ptfile, " (%ld", (long) ptptype1->DATA);
                            }
                            else {
                                fprintf(ptfile, " (?");
                            }
                            if ((ptchain1 = ptchain1->NEXT) != NULL) {
                                if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                    fprintf(ptfile, " %ld)", (long) ptptype1->DATA);
                                }
                                else {
                                    fprintf(ptfile, " ?)");
                                }
                            }
                            else {
                                fprintf(ptfile, " ?)");
                            }
                        }
                        else {
                            fprintf(ptfile, " (? ?)");
                        }
                    }
                    fprintf(ptfile, ")");
                }
            }
            /* ---------------------------------------------------------------------
               cone parallel transistors
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_PARATRANS)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n(PARATRANS");
                    for (ptchain0 = ptchain0; ptchain0 != NULL; ptchain0 = ptchain0->NEXT) {
                        if (ptchain0->DATA != NULL) {
                            ptchain1 = (chain_list *)getptype(((lotrs_list *)ptchain0->DATA)->USER, MBK_TRANS_PARALLEL)->DATA;
                            if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                fprintf(ptfile, "\n(%ld", (long) ptptype1->DATA);
                            }
                            else {
                                fprintf(ptfile, "\n(?");
                            }
                            counter1 = 1;
                            for (ptchain1 = ptchain1->NEXT; ptchain1 != NULL; ptchain1 = ptchain1->NEXT) {
                                if (counter1++ == 9) {
                                    fprintf(ptfile, "\n");
                                    counter1 = 1;
                                }
                                if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                    fprintf(ptfile, " %ld", (long) ptptype1->DATA);
                                }
                                else {
                                    fprintf(ptfile, " ?");
                                }
                            }
                            fprintf(ptfile, ")");
                        }
                    }
                    fprintf(ptfile, ")");
                }
            }

            /* ---------------------------------------------------------------------
               end of cone user fields
               --------------------------------------------------------------------- */
            fprintf(ptfile, ")");
        }

        /* ------------------------------------------------------------------------
           end of cone
           ------------------------------------------------------------------------ */
        fprintf(ptfile, ")");
    }

/* ---------------------------------------------------------------------------
   end of cones
   --------------------------------------------------------------------------- */
    fprintf(ptfile, ")");

/* ---------------------------------------------------------------------------
   end of cnsfig
   --------------------------------------------------------------------------- */
    fprintf(ptfile, ")\n");

    for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {
       if (ptcell->CONES == NULL) continue;
       ptcell->USER = testanddelptype(ptcell->USER, (long) CNS_INDEX);
    }
/* ---------------------------------------------------------------------------
   close file
   --------------------------------------------------------------------------- */
    if (error == 1) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns warning ***");
        (void) fprintf(stderr, " savecnsfig(): unknown fields found (marked by '?')\n");
    }

    if (fclose(ptfile) != 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " savecnsfig() impossible: can't close file %s.cns\n", ptcnsfig->NAME);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns savecnsfig out\n");
        EXIT(-1);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns savecnsfig out\n");
}

/*============================================================================*
 | function treatbr();                                                        |
 *============================================================================*/
short 
treatbr(ptfile, ptbranch, counter)
    FILE           *ptfile;
    branch_list    *ptbranch;
    long            counter;

{
    char            branch_type[10];
    char            lotrs_type[8];
    short           error = 0;
    link_list      *ptlink = NULL;
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptptype = NULL;
    locon_list     *ptlocon = NULL;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns treatbr\n");

    if (ptbranch->LINK != NULL) {
        strcpy(branch_type, "");
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            strcat(branch_type, "Nf");
        if ((ptbranch->TYPE & CNS_NOT_UP) == CNS_NOT_UP)
            strcat(branch_type, "Nu");
        if ((ptbranch->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN)
            strcat(branch_type, "Nd");
        if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER)
            strcat(branch_type, "B");
        if ((ptbranch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED)
            strcat(branch_type, "Dd");
        if ((ptbranch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED)
            strcat(branch_type, "Ds");
        if ((ptbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)
            strcat(branch_type, "F");
        if ((ptbranch->TYPE & CNS_NOTCONFLICTUAL) == CNS_NOTCONFLICTUAL)
            strcat(branch_type, "Nx");
        if ((ptbranch->TYPE & CNS_RESBRANCH) == CNS_RESBRANCH)
            strcat(branch_type, "R");
        if ((ptbranch->TYPE & CNS_IGNORE) == CNS_IGNORE)
            strcat(branch_type, "I");
        if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS)
            strcat(branch_type, "p");
        else if ((ptbranch->TYPE & CNS_PARALLEL) == CNS_PARALLEL)
            strcat(branch_type, "P");
        if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD)
            strcat(branch_type, "Vdd");
        if ((ptbranch->TYPE & CNS_GND) == CNS_GND)
            strcat(branch_type, "Gnd");
        if ((ptbranch->TYPE & CNS_VSS) == CNS_VSS)
            strcat(branch_type, "Vss");
        if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT)
            strcat(branch_type, "Ext");
        if (strlen(branch_type) == 0) {
            strcat(branch_type, "?");
            error = 1;
        }
        if ((ptptype = getptype(ptbranch->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptbranch->USER = addptype(ptbranch->USER, (long) CNS_INDEX, (void *) counter);
        }
        fprintf(ptfile, "\n(%ld %s", counter, branch_type);
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                strcpy(lotrs_type, "");
                if ((ptlink->TYPE & CNS_SW) == CNS_SW)
                    strcat(lotrs_type, "S");
                if ((ptlink->TYPE & CNS_PULL) == CNS_PULL)
                    strcat(lotrs_type, "P");
                if ((ptlink->TYPE & CNS_DRIV_PULL) == CNS_DRIV_PULL)
                    strcat(lotrs_type, "p");
                if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP)
                    strcat(lotrs_type, "DU");
                if ((ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN)
                    strcat(lotrs_type, "DD");
                if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT)
                    strcat(lotrs_type, "X");
                if ((ptlink->TYPE & CNS_RESIST) == CNS_RESIST)
                    strcat(lotrs_type, "R");
                if ((ptlink->TYPE & CNS_CAPA) == CNS_CAPA)
                    strcat(lotrs_type, "C");
                if ((ptlink->TYPE & CNS_DIPOLE) == CNS_DIPOLE)
                    strcat(lotrs_type, "d");
                if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH)
                    strcat(lotrs_type, "_s");
                if ((ptlink->TYPE & CNS_COMMAND) == CNS_COMMAND)
                    strcat(lotrs_type, "_c");
                if ((ptlink->TYPE & CNS_ASYNCLINK) == CNS_ASYNCLINK)
                    strcat(lotrs_type, "_a");
                if (strlen(lotrs_type) == 0) {
                    strcat(lotrs_type, "?");
                    error = 1;
                }
                fprintf(ptfile, "\n(%s", lotrs_type);
                ptlotrs = ptlink->ULINK.LOTRS;
                if (ptlotrs != NULL) {
                    if (strcmp(CNS_TECHNO, "edmesfet") == 0) {
                        if (MLO_IS_TRANSN(ptlotrs->TYPE))
                            fprintf(ptfile, " E");
                        else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                            fprintf(ptfile, " D");
                        else {
                            fprintf(ptfile, " ?");
                            error = 1;
                        }
                    }
                    else if (strcmp(CNS_TECHNO, "npmosfet") == 0) {
                        if (MLO_IS_TRANSN(ptlotrs->TYPE))
                            fprintf(ptfile, " N");
                        else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                            fprintf(ptfile, " P");
                        else {
                            fprintf(ptfile, " ?");
                            error = 1;
                        }
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                    if ((ptptype = getptype(ptlotrs->USER, (long) CNS_INDEX)) != NULL) {
                        fprintf(ptfile, " %ld", (long) ptptype->DATA);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
            }
            else {
                if ((ptlink->TYPE & CNS_IN) == CNS_IN) {
                    fprintf(ptfile, "\n(In");
                }
                else if ((ptlink->TYPE & CNS_INOUT) == CNS_INOUT) {
                    fprintf(ptfile, "\n(InOut");
                }
                else {
                    fprintf(ptfile, "\n(?");
                    error = 1;
                }
                ptlocon = ptlink->ULINK.LOCON;
                if ((ptlocon != NULL) && ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL)) {
                    fprintf(ptfile, " %ld", (long) ptptype->DATA);
                }
                else {
                    fprintf(ptfile, " ?");
                    error = 1;
                }
            }
            ptptype = getptype(ptlink->SIG->USER, CNS_INDEX);
            fprintf(ptfile, " %ld", (long) ptptype->DATA);
            fprintf(ptfile, ")");
        }
        fprintf(ptfile, ")");
    }

    return (error);
}

/*============================================================================*
 | function savecnvfig();                                                     |
 | Verbose Cone Netlist driver                                                |
 *============================================================================*/
void
savecnvfig(ptcnsfig)
    cnsfig_list    *ptcnsfig;
{
    FILE           *ptfile;
    time_t          counter;
    locon_list     *ptlocon;
    short           error = 0;
    lotrs_list     *ptlotrs;
    ptype_list     *ptptype;
    cone_list      *ptcone;
    char            lotrs_type[8];
    cell_list      *ptcell;
    char            cell_type[16];
    long            counter1;
    chain_list     *ptchain;
    cone_list      *ptcone0;
    char            cone_type[256];
    char            cone_tectype[256];
    edge_list      *ptedge;
    cone_list      *ptcone1;
    branch_list    *ptbranch;
    chain_list     *ptchain0;
    ptype_list     *ptptype0;
    chain_list     *ptchain1;
    ptype_list     *ptptype1;
    char            buffer[2048];

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns savecnsfig in\n");

/* ---------------------------------------------------------------------------
   open file
   --------------------------------------------------------------------------- */

    if ((ptfile = mbkfopen(ptcnsfig->NAME, "cnv", WRITE_TEXT)) == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " savecnvfig() impossible: can't open file %s.cnv\n", ptcnsfig->NAME);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns savecnvfig out\n");
        EXIT(-1);
    }

/* ---------------------------------------------------------------------------
   header
   --------------------------------------------------------------------------- */


    sprintf(buffer, "CNS V%s %s\n"
                    "Figure: %s\n"
                    "Netlist format: %s\n",
            CNS_VERSION, CNS_TECHNO, ptcnsfig->NAME, IN_LO);
    
    avt_printExecInfo(ptfile, "*", buffer, "");

/* ---------------------------------------------------------------------------
   capacity scale
   --------------------------------------------------------------------------- */
    fprintf(ptfile, "\nCapacitance Scale: %ld\n", SCALE_X);

/* ---------------------------------------------------------------------------
   Cone Netlist Structure external connector list
   --------------------------------------------------------------------------- */
    counter = 0;
    fprintf(ptfile, "\nEXTERNAL CONNECTORS\n");
    for (ptlocon = ptcnsfig->LOCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
        counter++;
        if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) counter);
        }    
        if (strchr(ptlocon->NAME, ' ') != NULL) {
            fprintf(ptfile, "\n  %ld) %s[%d]", counter, vectorradical(ptlocon->NAME), vectorindex(ptlocon->NAME));
        }
        else fprintf(ptfile, "\n  %ld) %s", counter, ptlocon->NAME);
        if (ptlocon->DIRECTION == CNS_I)
            fprintf(ptfile, " (Input)");
        else if (ptlocon->DIRECTION == CNS_O)
            fprintf(ptfile, " (Output)");
        else if (ptlocon->DIRECTION == CNS_B)
            fprintf(ptfile, " (Bidirectional)");
        else if (ptlocon->DIRECTION == CNS_Z)
            fprintf(ptfile, " (Tristate output)");
        else if (ptlocon->DIRECTION == CNS_T)
            fprintf(ptfile, " (Bidirectional tristate)");
        else if (ptlocon->DIRECTION == CNS_X)
            fprintf(ptfile, " (Unknown)");
        else if (ptlocon->DIRECTION == CNS_VDDC)
            fprintf(ptfile, " (Vdd)");
        else if (ptlocon->DIRECTION == CNS_GNDC)
            fprintf(ptfile, " (Gnd)");
        else if (ptlocon->DIRECTION == CNS_VSSC)
            fprintf(ptfile, " (Vss)");
        else {
            fprintf(ptfile, " (?)");
            error = 1;
        }
        ptptype = getptype(ptlocon->USER, CNS_TYPELOCON);
        if (ptptype != NULL) {
            if (((long)ptptype->DATA & CNS_ONE) == CNS_ONE) fprintf(ptfile, " Stuck One");
            else if (((long)ptptype->DATA & CNS_ZERO) == CNS_ZERO) fprintf(ptfile, " Stuck Zero");
        }
    }
    fprintf(ptfile, "\n");

/* ---------------------------------------------------------------------------
   Cone Netlist Structure internal connector list
   --------------------------------------------------------------------------- */
    if (ptcnsfig->INTCON != NULL) {
        fprintf(ptfile, "\nCONNECTORS TO INSTANCES\n");
        for (ptlocon = ptcnsfig->INTCON; ptlocon != NULL; ptlocon = ptlocon->NEXT) {
            counter++;
            if ((ptptype = getptype(ptlocon->USER, (long) CNS_INDEX)) != NULL) {
                ptptype->DATA = (void *) counter;
            }
            else {
                ptlocon->USER = addptype(ptlocon->USER, (long) CNS_INDEX, (void *) counter);
            }
            if (strchr(ptlocon->NAME, ' ') != NULL) {
                fprintf(ptfile, "\n  %ld) %s[%d]", counter, vectorradical(ptlocon->NAME), vectorindex(ptlocon->NAME));
            }
            else fprintf(ptfile, "\n  %ld) %s", counter, ptlocon->NAME);
            if (ptlocon->DIRECTION == CNS_I)
                fprintf(ptfile, " (Input)");
            else if (ptlocon->DIRECTION == CNS_O)
                fprintf(ptfile, " (Output)");
            else if (ptlocon->DIRECTION == CNS_B)
                fprintf(ptfile, " (Bidirectional)");
            else if (ptlocon->DIRECTION == CNS_Z)
                fprintf(ptfile, " (Tristate output)");
            else if (ptlocon->DIRECTION == CNS_T)
                fprintf(ptfile, " (Bidirectional tristate)");
            else if (ptlocon->DIRECTION == CNS_X)
                fprintf(ptfile, " (Unknown)");
            else if (ptlocon->DIRECTION == CNS_VDDC)
                fprintf(ptfile, " (Vdd)");
            else if (ptlocon->DIRECTION == CNS_GNDC)
                fprintf(ptfile, " (Gnd)");
            else if (ptlocon->DIRECTION == CNS_VSSC)
                fprintf(ptfile, " (Vss)");
            else {
                fprintf(ptfile, " (?)");
                error = 1;
            }
        }
        fprintf(ptfile, "\n");
    }

/* ---------------------------------------------------------------------------
   Cone Netlist Structure transistor list
   --------------------------------------------------------------------------- */
    counter = 0;
    fprintf(ptfile, "\nTRANSISTORS\n");
    for (ptlotrs = ptcnsfig->LOTRS; ptlotrs != NULL; ptlotrs = ptlotrs->NEXT) {
        counter++;
        if ((ptptype = getptype(ptlotrs->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptlotrs->USER = addptype(ptlotrs->USER, (long) CNS_INDEX, (void *) counter);
        }
        fprintf(ptfile, "\n  %ld)", counter);
        if (ptlotrs->TRNAME != NULL) {
            if (isdigit((int)*(ptlotrs->TRNAME))) fprintf(ptfile, " cns_%s", ptlotrs->TRNAME);
            else fprintf(ptfile, " %s", ptlotrs->TRNAME);
        }
        else {
            fprintf(ptfile, " noname");
        }
        strcpy(lotrs_type, "");
        if (strcmp(CNS_TECHNO, "edmesfet") == 0) {
            if (MLO_IS_TRANSN(ptlotrs->TYPE))
                strcat(lotrs_type, "TE");
            else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                strcat(lotrs_type, "TD");
        }
        else if (strcmp(CNS_TECHNO, "npmosfet") == 0) {
            if (MLO_IS_TRANSN(ptlotrs->TYPE))
                strcat(lotrs_type, "TN");
            else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                strcat(lotrs_type, "TP");
        }
        if (strlen(lotrs_type) == 0) {
            strcat(lotrs_type, "?");
            error = 1;
        }
        fprintf(ptfile, ", Type %s", lotrs_type);
        if (ptlotrs->MODINDEX != EMPTYHT) fprintf(ptfile, " (%s)", getlotrsmodel(ptlotrs));
        if ((ptptype = getptype(ptlotrs->USER, (long) CNS_DRIVINGCONE)) != NULL) {
            fprintf(ptfile, " driven by '%s',", ((cone_list *)ptptype->DATA)->NAME);
        }
        else if ((ptptype = getptype(ptlotrs->USER, (long) CNS_LINKTYPE)) != NULL) {
            if (((long) ptptype->DATA & CNS_ACTIVE) == CNS_ACTIVE) {
                if ((ptcone = (cone_list *) ptlotrs->GRID) != NULL) {
                    fprintf(ptfile, " driven by %ld,", ptcone->INDEX);
                }
                else {
                    fprintf(ptfile, " driven by ?,");
                    error = 1;
                }
            }
            else if (((long) ptptype->DATA & CNS_PULL) == CNS_PULL)
                fprintf(ptfile, " puller,");
            else if (((long) ptptype->DATA & (CNS_DIODE_UP & CNS_DIODE_DOWN)) == (CNS_DIODE_UP & CNS_DIODE_DOWN))
                fprintf(ptfile, " diode,");
            else if (((long) ptptype->DATA & CNS_RESIST) == CNS_RESIST)
                fprintf(ptfile, " resistance,");
            else if (((long) ptptype->DATA & CNS_CAPA) == CNS_CAPA)
                fprintf(ptfile, " capacity,");
            else {
                fprintf(ptfile, " ?");
                error = 1;
            }
        }
        else {
            fprintf(ptfile, " 0");
        }
        if (ptlotrs->X != LONG_MIN && ptlotrs->Y != LONG_MIN) {
            fprintf(ptfile, " Position = (%ld,%ld),", ptlotrs->X, ptlotrs->Y);
        }
        fprintf(ptfile, " Width=%ld, Length=%ld,", ptlotrs->WIDTH, ptlotrs->LENGTH);
        fprintf(ptfile, " PS=%ld, PD=%ld", ptlotrs->PS, ptlotrs->PD);
        fprintf(ptfile, " XS=%ld, XD=%ld", ptlotrs->XS, ptlotrs->XD);
    }
    fprintf(ptfile, "\n");

/* ---------------------------------------------------------------------------
   Cone Netlist Structure cell list
   --------------------------------------------------------------------------- */
    if (ptcnsfig->CELL != NULL) {
        fprintf(ptfile, "\nCELLS");
        counter = 0;
        for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {
            if (ptcell->CONES == NULL) continue;
            counter++;
            if ((ptptype = getptype(ptcell->USER, (long) CNS_INDEX)) != NULL) {
                ptptype->DATA = (void *) counter;
            }
            else {
                ptcell->USER = addptype(ptcell->USER, (long) CNS_INDEX, (void *) counter);
            }
            fprintf(ptfile, "\n\n  %ld)", counter);
            if ((ptcell->TYPE & CNS_UNKNOWN) == CNS_UNKNOWN) {
                if (ptcell->TYPE != 0) {
                    fprintf(ptfile, " Model %ld", (ptcell->TYPE & ~CNS_UNKNOWN));
                }
                else {
                    fprintf(ptfile, " Model ?");
                    error = 1;
                }
            }
            else {
                strcpy(cell_type, "");
                if ((ptcell->TYPE & CNS_SUPER_BUFFER) == CNS_SUPER_BUFFER)
                    strcat(cell_type, "Super_Buffer");
                else if ((ptcell->TYPE & CNS_TRISTATE_E) == CNS_TRISTATE_E)
                    strcat(cell_type, "Tristate");
                else if ((ptcell->TYPE & CNS_SQUIRT_BUFFER) == CNS_SQUIRT_BUFFER)
                    strcat(cell_type, "Squirt_Buffer");
                else if ((ptcell->TYPE & CNS_LATCH_E) == CNS_LATCH_E)
                    strcat(cell_type, "Latch");
                else if ((ptcell->TYPE & CNS_READ) == CNS_READ)
                    strcat(cell_type, "Read");
                else if ((ptcell->TYPE & CNS_MS_FF) == CNS_MS_FF)
                    strcat(cell_type, "Ms_Ff");
                if (strlen(cell_type) == 0) {
                    strcat(cell_type, "?");
                    error = 1;
                }
                fprintf(ptfile, " %s", cell_type);
            }
            for (ptchain = ptcell->CONES; ptchain != NULL; ptchain = ptchain->NEXT) {
                if (ptchain->DATA != NULL) {
                    fprintf(ptfile, "\n    %s", ((cone_list *) ptchain->DATA)->NAME);
                }
                else {
                    fprintf(ptfile, " ?");
                    error = 1;
                }
            }
        }
        fprintf(ptfile, "\n");
    }

/* ---------------------------------------------------------------------------
   Cone Netlist Structure cone list
   --------------------------------------------------------------------------- */
    fprintf(ptfile, "\nCONES");
    for (ptcone0 = ptcnsfig->CONE; ptcone0 != NULL; ptcone0 = ptcone0->NEXT) {
        if (strchr(ptcone0->NAME, ' ') != NULL) {
            fprintf(ptfile, "\n\n%ld) %s[%d]", ptcone0->INDEX, vectorradical(ptcone0->NAME), vectorindex(ptcone0->NAME));
        }
        else fprintf(ptfile, "\n\n%ld) %s", ptcone0->INDEX, ptcone0->NAME);
        strcpy(cone_type, "");
        if ((ptcone0->TYPE & CNS_MEMSYM) == CNS_MEMSYM)
            strcat(cone_type, "MemSym,");
        if ((ptcone0->TYPE & CNS_LATCH) == CNS_LATCH)
            strcat(cone_type, "Latch,");
        if ((ptcone0->TYPE & CNS_RS) == CNS_RS)
            strcat(cone_type, "RS,");
        if ((ptcone0->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP)
            strcat(cone_type, "Flip-Flop,");
        if ((ptcone0->TYPE & CNS_MASTER) == CNS_MASTER)
            strcat(cone_type, "Master,");
        if ((ptcone0->TYPE & CNS_SLAVE) == CNS_SLAVE)
            strcat(cone_type, "Slave,");
        if ((ptcone0->TYPE & CNS_PRECHARGE) == CNS_PRECHARGE)
            strcat(cone_type, "Precharge,");
        if ((ptcone0->TYPE & CNS_TRI) == CNS_TRI)
            strcat(cone_type, "Tristate,");
        if ((ptcone0->TYPE & CNS_CONFLICT) == CNS_CONFLICT)
            strcat(cone_type, "Conflict,");
        if ((ptcone0->TYPE & CNS_EXT) == CNS_EXT)
            strcat(cone_type, "External,");
        if ((ptcone0->TYPE & CNS_VDD) == CNS_VDD)
            strcat(cone_type, "Vdd,");
        if ((ptcone0->TYPE & CNS_GND) == CNS_GND)
            strcat(cone_type, "Gnd,");
        if ((ptcone0->TYPE & CNS_VSS) == CNS_VSS)
            strcat(cone_type, "Vss,");
        if (strlen(cone_type) > 0) cone_type[strlen(cone_type)-1] = '\0';
        fprintf(ptfile, "\n    Type: %s", cone_type);

        strcpy(cone_tectype, "");
        if ((ptcone0->TECTYPE & CNS_ZERO) == CNS_ZERO)
            strcat(cone_tectype, "Stuck Zero,");
        if ((ptcone0->TECTYPE & CNS_ONE) == CNS_ONE)
            strcat(cone_tectype, "Stuck One,");
        if ((ptcone0->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS)
            strcat(cone_tectype, "CMOS dual,");
        else if ((ptcone0->TECTYPE & CNS_CMOS) == CNS_CMOS)
            strcat(cone_tectype, "CMOS,");
        if ((ptcone0->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED)
            strcat(cone_tectype, "Degraded Vdd,");
        if ((ptcone0->TECTYPE & CNS_GND_DEGRADED) == CNS_GND_DEGRADED)
            strcat(cone_tectype, "Degraded Gnd,");
        if ((ptcone0->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED)
            strcat(cone_tectype, "Degraded Vss,");

        if (strlen(cone_tectype) > 0) cone_tectype[strlen(cone_tectype)-1] = '\0';
        fprintf(ptfile, "\n    TecTytpe: %s", cone_tectype);

        /* ------------------------------------------------------------------------
           cone inputs
           ------------------------------------------------------------------------ */
        fprintf(ptfile, "\n  INPUTS");
        if (ptcone0->INCONE != NULL) {
            for (ptedge = ptcone0->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
                    fprintf(ptfile, "\n    Cone");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                            fprintf(ptfile, " bleeder");
                        }
                        if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                            fprintf(ptfile, " command");
                        }
                        if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                            fprintf(ptfile, " async");
                        }
                        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                            fprintf(ptfile, " loop");
                        }
                        if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                            fprintf(ptfile, " feedback");
                        }
                        if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                            fprintf(ptfile, " memsym");
                        }
                        if ((ptedge->TYPE & CNS_HZCOM) == CNS_HZCOM) {
                            fprintf(ptfile, " hz_command");
                        }
                        fprintf(ptfile, " '%s' (Index=%ld)", ptcone1->NAME, ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_VDD) == CNS_VDD) {
                    fprintf(ptfile, "\n    Vdd");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        fprintf(ptfile, " '%s' (Index=%ld)", ptcone1->NAME, ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_GND) == CNS_GND) {
                    fprintf(ptfile, "\n    Gnd");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        fprintf(ptfile, " '%s' (Index=%ld)", ptcone1->NAME, ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_VSS) == CNS_VSS) {
                    fprintf(ptfile, "\n    Vss");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        fprintf(ptfile, " '%s' (Index=%ld)", ptcone1->NAME, ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
                    fprintf(ptfile, "\n    Ext");
                    ptlocon = ptedge->UEDGE.LOCON;
                    if (ptlocon != NULL) {
                        fprintf(ptfile, " '%s'", ptlocon->NAME);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else {
                    fprintf(ptfile, "\n    ? ?");
                    error = 1;
                }
            }
        }

        /* ------------------------------------------------------------------------
           cone outputs
           ------------------------------------------------------------------------ */
        fprintf(ptfile, "\n  OUTPUTS");
        if (ptcone0->OUTCONE != NULL) {
            for (ptedge = ptcone0->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
                    fprintf(ptfile, "\n    Cone");
                    ptcone1 = ptedge->UEDGE.CONE;
                    if (ptcone1 != NULL) {
                        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                            fprintf(ptfile, " bleeder");
                        }
                        if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                            fprintf(ptfile, " command");
                        }
                        if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                            fprintf(ptfile, " async");
                        }
                        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                            fprintf(ptfile, " loop");
                        }
                        if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                            fprintf(ptfile, " feedback");
                        }
                        if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                            fprintf(ptfile, " memsym");
                        }
                        fprintf(ptfile, " '%s' (Index=%ld)", ptcone1->NAME, ptcone1->INDEX);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
                    fprintf(ptfile, "\n    Ext");
                    ptlocon = ptedge->UEDGE.LOCON;
                    if (ptlocon != NULL) {
                        fprintf(ptfile, " '%s'", ptlocon->NAME);
                    }
                    else {
                        fprintf(ptfile, " ?");
                        error = 1;
                    }
                }
                else {
                    fprintf(ptfile, "\n    ? ?");
                    error = 1;
                }
            }
        }

        /* ------------------------------------------------------------------------
           cone branches
           ------------------------------------------------------------------------ */
        fprintf(ptfile, "\n  BRANCHES");
        counter = 0;
        for (ptbranch = ptcone0->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbrv(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        for (ptbranch = ptcone0->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbrv(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        for (ptbranch = ptcone0->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbrv(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }
        for (ptbranch = ptcone0->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if (treatbrv(ptfile, ptbranch, ++counter)) {
                error = 1;
                counter--;
            }
        }

        /* ------------------------------------------------------------------------
           cells containg the cone
           ------------------------------------------------------------------------ */
        if ((ptchain = ptcone0->CELLS) != NULL) {
            fprintf(ptfile, "\n  CELLS");
            if ((ptptype = getptype(((cell_list *) ptchain->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                fprintf(ptfile, "\n    (%ld", (long) ptptype->DATA);
            }
            else {
                fprintf(ptfile, "\n    (?");
            }
            counter1 = 1;
            for (ptchain = ptchain->NEXT; ptchain != NULL; ptchain = ptchain->NEXT) {
                if (counter1++ == 9) {
                    fprintf(ptfile, "\n");
                    counter1 = 1;
                }
                if (ptchain->DATA != NULL) {
                    if ((ptptype = getptype(((cell_list *) ptchain->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                        fprintf(ptfile, " %ld", (long) ptptype->DATA);
                    }
                    else {
                        fprintf(ptfile, " ?");
                    }
                }
                else {
                    fprintf(ptfile, " ?");
                    error = 1;
                }
            }
            fprintf(ptfile, ")");
        }

        /* ------------------------------------------------------------------------
           cone user fields
           ------------------------------------------------------------------------ */
        if (((getptype(ptcone0->USER, (long) CNS_PARALLEL)) != NULL)
            || ((getptype(ptcone0->USER, (long) CNS_BLEEDER)) != NULL)
            || ((getptype(ptcone0->USER, (long) CNS_PARATRANS)) != NULL)
            || ((getptype(ptcone0->USER, (long) CNS_SWITCH)) != NULL)) {
            fprintf(ptfile, "\n  MISCELLANEOUS");

            /* ---------------------------------------------------------------------
               cone parallel branches
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_PARALLEL)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n    PARALLEL BRANCHES");
                    for (ptchain0 = ptchain0; ptchain0 != NULL; ptchain0 = ptchain0->NEXT) {
                        if (ptchain0->DATA != NULL) {
                            ptchain1 = (chain_list *) ptchain0->DATA;
                            if ((ptptype1 = getptype(((branch_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                fprintf(ptfile, "\n      (%ld", (long) ptptype1->DATA);
                            }
                            else {
                                fprintf(ptfile, "\n      (?");
                            }
                            for (ptchain1 = ptchain1->NEXT; ptchain1 != NULL; ptchain1 = ptchain1->NEXT) {
                                if ((ptptype1 = getptype(((branch_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                    fprintf(ptfile, " %ld", (long) ptptype1->DATA);
                                }
                                else {
                                    fprintf(ptfile, " ?");
                                }
                            }
                            fprintf(ptfile, ")");
                        }
                    }
                }
            }

            /* ---------------------------------------------------------------------
               cone bleeders
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_BLEEDER)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n    BLEEDER BRANCHES");
                    if ((ptptype0 = getptype(((branch_list *) ptchain0->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                        fprintf(ptfile, "\n      (%ld", (long) ptptype0->DATA);
                    }
                    else {
                        fprintf(ptfile, "\n      (?");
                    }
                    for (ptchain1 = ptchain0->NEXT; ptchain1 != NULL; ptchain1 = ptchain1->NEXT) {
                        if ((ptptype1 = getptype(((branch_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                            fprintf(ptfile, " %ld", (long) ptptype1->DATA);
                        }
                        else {
                            fprintf(ptfile, " ?");
                        }
                    }
                    fprintf(ptfile, ")");
                }
            }

            /* ---------------------------------------------------------------------
               cone switches
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_SWITCH)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n    SWITCH TRANSISTOR PAIRS");
                    counter1 = 1;
                    for (ptchain0 = ptchain0; ptchain0 != NULL; ptchain0 = ptchain0->NEXT) {
                        if ((ptchain1 = (chain_list *) ptchain0->DATA) != NULL) {
                            if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                fprintf(ptfile, "\n      (%ld", (long) ptptype1->DATA);
                            }
                            else {
                                fprintf(ptfile, "\n      (?");
                            }
                            if ((ptchain1 = ptchain1->NEXT) != NULL) {
                                if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                    fprintf(ptfile, " %ld)", (long) ptptype1->DATA);
                                }
                                else {
                                    fprintf(ptfile, " ?)");
                                }
                            }
                            else {
                                fprintf(ptfile, " ?)");
                            }
                        }
                        else {
                            fprintf(ptfile, "      (? ?)\n");
                        }
                    }
                }
            }
            /* ---------------------------------------------------------------------
               cone parallel transistors
               --------------------------------------------------------------------- */
            if ((ptptype = getptype(ptcone0->USER, (long) CNS_PARATRANS)) != NULL) {
                if ((ptchain0 = (chain_list *) ptptype->DATA) != NULL) {
                    fprintf(ptfile, "\n    PARALLEL TRANSISTORS");
                    for (ptchain0 = ptchain0; ptchain0 != NULL; ptchain0 = ptchain0->NEXT) {
                        if (ptchain0->DATA != NULL) {
                            ptchain1 = (chain_list *)getptype(((lotrs_list *)ptchain0->DATA)->USER, MBK_TRANS_PARALLEL)->DATA;
                            if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                fprintf(ptfile, "\n      (%ld", (long) ptptype1->DATA);
                            }
                            else {
                                fprintf(ptfile, "\n      (?");
                            }
                            for (ptchain1 = ptchain1->NEXT; ptchain1 != NULL; ptchain1 = ptchain1->NEXT) {
                                if ((ptptype1 = getptype(((lotrs_list *) ptchain1->DATA)->USER, (long) CNS_INDEX)) != NULL) {
                                    fprintf(ptfile, " %ld", (long) ptptype1->DATA);
                                }
                                else {
                                    fprintf(ptfile, " ?");
                                }
                            }
                            fprintf(ptfile, ")");
                        }
                    }
                }
            }

            /* ---------------------------------------------------------------------
               end of cone user fields
               --------------------------------------------------------------------- */
        }

        /* ------------------------------------------------------------------------
           end of cone
           ------------------------------------------------------------------------ */
    }

/* ---------------------------------------------------------------------------
   end of cones
   --------------------------------------------------------------------------- */

  for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {
     if (ptcell->CONES == NULL) continue;
     ptcell->USER = testanddelptype(ptcell->USER, (long) CNS_INDEX);
  }

/* ---------------------------------------------------------------------------
   close file
   --------------------------------------------------------------------------- */
    if (error == 1) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns warning ***");
        (void) fprintf(stderr, " savecnvfig(): unknown fields found (marked by '?')\n");
    }

    if (fclose(ptfile) != 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " savecnvfig() impossible: can't close file %s.cnv\n", ptcnsfig->NAME);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns savecnvfig out\n");
        EXIT(-1);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns savecnvfig out\n");
}

/*============================================================================*
 | function treatbrv();                                                        |
 *============================================================================*/
short 
treatbrv(ptfile, ptbranch, counter)
    FILE           *ptfile;
    branch_list    *ptbranch;
    long            counter;

{
    char            branch_type[256];
    char            lotrs_type[256];
    short           error = 0;
    link_list      *ptlink = NULL;
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptptype = NULL;
    locon_list     *ptlocon = NULL;
    ptype_list     *ptuser;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns treatbr\n");

    if (ptbranch->LINK != NULL) {
        if ((ptptype = getptype(ptbranch->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) counter;
        }
        else {
            ptbranch->USER = addptype(ptbranch->USER, (long) CNS_INDEX, (void *) counter);
        }
        fprintf(ptfile, "\n    %ld) ", counter);

        if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD)
            fprintf(ptfile, "Vdd");
        if ((ptbranch->TYPE & CNS_GND) == CNS_GND)
            fprintf(ptfile, "Gnd");
        if ((ptbranch->TYPE & CNS_VSS) == CNS_VSS)
            fprintf(ptfile, "Vss");
        if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT)
            fprintf(ptfile, "Ext");

        strcpy(branch_type, "");
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            strcat(branch_type, "Non-Functional,");
        if ((ptbranch->TYPE & CNS_NOT_UP) == CNS_NOT_UP)
            strcat(branch_type, "Non-Up,");
        if ((ptbranch->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN)
            strcat(branch_type, "Non-Down,");
        if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER)
            strcat(branch_type, "Bleeder,");
        if ((ptbranch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED)
            strcat(branch_type, "Vdd-Degraded,");
        if ((ptbranch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED)
            strcat(branch_type, "Vss-Degraded,");
        if ((ptbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)
            strcat(branch_type, "Feedback,");
        if ((ptbranch->TYPE & CNS_NOTCONFLICTUAL) == CNS_NOTCONFLICTUAL)
            strcat(branch_type, "Non-Conflictual,");
        if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS)
            strcat(branch_type, "Parallel instance,");
        else if ((ptbranch->TYPE & CNS_PARALLEL) == CNS_PARALLEL)
            strcat(branch_type, "Parallel,");

        if (strlen(branch_type) > 0) {
            branch_type[strlen(branch_type)-1] = '\0';
            fprintf(ptfile, " (%s)", branch_type);
        }

        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                ptlotrs = ptlink->ULINK.LOTRS;
                if (ptlotrs != NULL) {
                    if (strcmp(CNS_TECHNO, "edmesfet") == 0) {
                        if (MLO_IS_TRANSN(ptlotrs->TYPE))
                            fprintf(ptfile, "\n      TE ");
                        else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                            fprintf(ptfile, "\n      TD ");
                        else {
                            fprintf(ptfile, "\n      ?");
                            error = 1;
                        }
                    }
                    else if (strcmp(CNS_TECHNO, "npmosfet") == 0) {
                        if (MLO_IS_TRANSN(ptlotrs->TYPE))
                            fprintf(ptfile, "\n      TN ");
                        else if (MLO_IS_TRANSP(ptlotrs->TYPE))
                            fprintf(ptfile, "\n      TP ");
                        else {
                            fprintf(ptfile, "\n      ? ");
                            error = 1;
                        }
                    }
                    else {
                        fprintf(ptfile, "\n      ? ");
                        error = 1;
                    }
                    if (ptlotrs->TRNAME != NULL) {
                        fprintf(ptfile, "%s ", ptlotrs->TRNAME);
                    }
                    if ((ptptype = getptype(ptlotrs->USER, (long) CNS_INDEX)) != NULL) {
                        fprintf(ptfile, "(Index=%ld) ", (long)ptptype->DATA);
                    }

                    strcpy(lotrs_type, "");
                    if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP)
                        strcat(lotrs_type, "Diode Up,");
                    if ((ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN)
                        strcat(lotrs_type, "Diode Down,");
                    if ((ptlink->TYPE & CNS_RESIST) == CNS_RESIST)
                        strcat(lotrs_type, "Resist,");
                    if ((ptlink->TYPE & CNS_CAPA) == CNS_CAPA)
                        strcat(lotrs_type, "Capa,");
                    if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH)
                        strcat(lotrs_type, "Switch,");
                    if ((ptlink->TYPE & CNS_COMMAND) == CNS_COMMAND)
                        strcat(lotrs_type, "Command,");
                    if ((ptlink->TYPE & CNS_ASYNCLINK) == CNS_ASYNCLINK)
                        strcat(lotrs_type, "Async,");

                    if (strlen(lotrs_type) > 0) {
                        lotrs_type[strlen(lotrs_type)-1] = '\0';
                        fprintf(ptfile, "(%s) ", lotrs_type);
                    }

                    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
                    if (ptuser != NULL) {
                        fprintf(ptfile, "Driven by '%s' (Index=%ld)", ((cone_list *)ptuser->DATA)->NAME, ((cone_list *)ptuser->DATA)->INDEX);
                    }

                }
            }
            else {
                if ((ptlink->TYPE & CNS_IN) == CNS_IN) {
                    fprintf(ptfile, "\n      In");
                }
                else if ((ptlink->TYPE & CNS_INOUT) == CNS_INOUT) {
                    fprintf(ptfile, "\n      InOut");
                }
                else {
                    fprintf(ptfile, "\n      ?");
                    error = 1;
                }
                ptlocon = ptlink->ULINK.LOCON;
                if (ptlocon != NULL) {
                    fprintf(ptfile, " %s, ", ptlocon->NAME);
                }
                else {
                    fprintf(ptfile, " ?, ");
                    error = 1;
                }
            }
        }
    }

    return (error);
}

