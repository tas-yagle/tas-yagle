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
 
/* 10/26/95 Cone Netlist Structure functions: cone.c                          */

#include"cnsall.h"

static void    *addconepage();
static cone_list *getfreecone();
static void displaybranch(branch_list *ptbranch, long counter);
static void displaybranchlog(int lib, int loglevel, branch_list *ptbranch, long counter);

/* define number of cones per page                                            */
#define CONESPERPAGE 100

static chain_list *CONEPAGES;   /* chain list of cone pages                  */
static int      PAGECONEINDEX = 0;      /* cone index in current page                */
static cone_list *FREECONELIST; /* list of free cones in current page        */
static cone_list *FREEDCONELIST;        /* list of freed cones in current page       */

/*============================================================================*
 | function initconemem();                                                    |
 | initialize cone memory structure                                           |
 *============================================================================*/
void *
initconemem()
{
    cone_list      *ptcone;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns initconemem\n");

    ptcone = (cone_list *) mbkalloc(CONESPERPAGE * (sizeof(cone_list)));

    CONEPAGES = addchain((chain_list *) NULL, (void *) ptcone);
    FREEDCONELIST = NULL;
    PAGECONEINDEX = 1;
    FREECONELIST = ptcone;

    return NULL;
}

/*============================================================================*
 | function addconepage();                                                    |
 | add a new cone page                                                        |
 *============================================================================*/
static void *
addconepage()
{
    cone_list      *ptcone;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addconepage\n");

    ptcone = (cone_list *) mbkalloc(CONESPERPAGE * (sizeof(cone_list)));

    CONEPAGES = addchain(CONEPAGES, (void *) ptcone);
    PAGECONEINDEX = 1;
    FREECONELIST = ptcone;

    return NULL;
}

/*============================================================================*
 | function getfreecone();                                                    |
 | get a new cone                                                             |
 *============================================================================*/
static cone_list *
getfreecone()
{
    cone_list      *ptcone;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getfreecone in\n");

    if (FREEDCONELIST != NULL) {
        ptcone = FREEDCONELIST;
        FREEDCONELIST = FREEDCONELIST->NEXT;
        ptcone->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecone out\n");
        return (ptcone);
    }
    else if (PAGECONEINDEX < CONESPERPAGE) {
        PAGECONEINDEX++;
        FREECONELIST++;
        FREECONELIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecone out\n");
        return (FREECONELIST);
    }
    else {
        addconepage();
        FREECONELIST->NEXT = NULL;
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns getfreecone out\n");
        return (FREECONELIST);
    }
}

/*============================================================================*
 | function freecone();                                                       |
 | free a cone                                                                |
 *============================================================================*/
void *
freecone(cone_list *ptcone)
{
    edge_list      *pteelist;
    branch_list    *ptbrlist;
    chain_list     *ptchain;
    ptype_list *pt;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecone in\n");

    if (ptcone == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freecone() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns freecone out\n");
        EXIT(-1);
    }

    if ((pteelist = ptcone->INCONE) != NULL)
        freeeelist(pteelist);
    if ((pteelist = ptcone->OUTCONE) != NULL)
        freeeelist(pteelist);
    if ((ptbrlist = ptcone->BREXT) != NULL)
        freebrlist(ptbrlist);
    if ((ptbrlist = ptcone->BRVDD) != NULL)
        freebrlist(ptbrlist);
    if ((ptbrlist = ptcone->BRGND) != NULL)
        freebrlist(ptbrlist);
    if ((ptbrlist = ptcone->BRVSS) != NULL)
        freebrlist(ptbrlist);
    if ((ptchain = ptcone->CELLS) != NULL)
        freechain(ptchain);
    
    if ((pt=getptype(ptcone->USER, CNS_UPEXPR))!=NULL) freeExpr((chain_list *)pt->DATA);
    if ((pt=getptype(ptcone->USER, CNS_DNEXPR))!=NULL) freeExpr((chain_list *)pt->DATA);
    if ((pt=getptype(ptcone->USER, CNS_LOCON))!=NULL) freechain(pt->DATA);
    if ((pt=getptype(ptcone->USER, CNS_EXT))!=NULL) freechain(pt->DATA);

    freeptype(ptcone->USER);
    
    ptcone->NEXT = FREEDCONELIST;
    FREEDCONELIST = ptcone;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecone out\n");

    return NULL;
}

/*============================================================================*
 | function freecelist();                                                     |
 | free a cone list                                                           |
 *============================================================================*/
void *
freecelist(cone_list *ptcelist)
{
    cone_list      *ptcone, *last=NULL;
    edge_list      *pteelist;
    branch_list    *ptbrlist;
    chain_list     *ptchain;
    ptype_list *pt;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecelist in\n");

    if (ptcelist == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " freecelist() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns freecelist out\n");
        EXIT(-1);
    }

    for (ptcone = ptcelist; ptcone!= NULL; ptcone = ptcone->NEXT) {
        last=ptcone;
        if ((pteelist = ptcone->INCONE) != NULL)
            freeeelist(pteelist);
        if ((pteelist = ptcone->OUTCONE) != NULL)
            freeeelist(pteelist);
        if ((ptbrlist = ptcone->BREXT) != NULL)
            freebrlist(ptbrlist);
        if ((ptbrlist = ptcone->BRVDD) != NULL)
            freebrlist(ptbrlist);
        if ((ptbrlist = ptcone->BRGND) != NULL)
            freebrlist(ptbrlist);
        if ((ptbrlist = ptcone->BRVSS) != NULL)
            freebrlist(ptbrlist);
        if ((ptchain = ptcone->CELLS) != NULL)
            freechain(ptchain);
        if ((pt=getptype(ptcone->USER, CNS_UPEXPR))!=NULL) freeExpr((chain_list *)pt->DATA);
        if ((pt=getptype(ptcone->USER, CNS_DNEXPR))!=NULL) freeExpr((chain_list *)pt->DATA);
        if ((pt=getptype(ptcone->USER, CNS_LOCON))!=NULL) freechain(pt->DATA);
        if ((pt=getptype(ptcone->USER, CNS_PARATRANS))!=NULL) freechain(pt->DATA);
        if ((pt=getptype(ptcone->USER, CNS_EXT))!=NULL) freechain(pt->DATA);

        freeptype(ptcone->USER);
    }
    last->NEXT = FREEDCONELIST;
    FREEDCONELIST = ptcelist;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns freecelist out\n");
    return NULL;
}

/*============================================================================*
 | function addcone();                                                        |
 | add an element to a cone list                                              |
 *============================================================================*/
cone_list *
addcone(cone_list *ptconehead, long index, char *name, long type, long tectype, long xmax, long xmin, long ymax, long ymin, edge_list *ptincone, edge_list *ptoutcone, branch_list *ptbrext, branch_list *ptbrvdd, branch_list *ptbrgnd, branch_list *ptbrvss, chain_list *ptchain)
{
    cone_list      *ptcone;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcone in\n");

    if (index == 0) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " addcone() impossible: index 0 may not be employed\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns addcone out\n");
        EXIT(-1);
    }

    if (CNS_DEBUG_LEVEL >= 2) {
        for (ptcone = ptconehead; ptcone != NULL; ptcone = ptcone->NEXT) {
            if (ptcone->INDEX == index) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns error ***");
                (void) fprintf(stderr, " addcone() impossible: cone of index %ld already exists\n", ptcone->INDEX);
                if (CNS_TRACE_MODE >= CNS_TRACE)
                    (void) printf("TRA_cns addcone out\n");
                EXIT(-1);
            }
        }
    }

    ptcone = getfreecone();
    ptcone->NEXT = ptconehead;
    ptcone->INDEX = index;
    ptcone->NAME = namealloc(name);
    ptcone->TYPE = type;
    ptcone->TECTYPE = tectype;
#ifdef USEOLDTEMP
    ptcone->XM = xmax;
    ptcone->Xm = xmin;
    ptcone->YM = ymax;
    ptcone->Ym = ymin;
#endif
    ptcone->INCONE = ptincone;
    ptcone->OUTCONE = ptoutcone;
    ptcone->BREXT = ptbrext;
    ptcone->BRVDD = ptbrvdd;
    ptcone->BRGND = ptbrgnd;
    ptcone->BRVSS = ptbrvss;
    ptcone->CELLS = ptchain;
    ptcone->USER = NULL;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns addcone out\n");
    return (ptcone);
}

/*============================================================================*
 | function delcone();                                                        |
 | delete an element from a cone list and free that element                   |
 *============================================================================*/
cone_list *
delcone(cone_list *ptconehead, cone_list *ptcone2del)
{
    cone_list      *ptcone;
    cone_list      *ptcone2sav;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns delcone in\n");

    if ((ptconehead == NULL) || (ptcone2del == NULL)) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " delcone() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcone out\n");
        EXIT(-1);
    }

    if (ptcone2del == ptconehead) {
        ptcone = ptconehead->NEXT;
        ptconehead->NEXT = NULL;
        freecone(ptconehead);
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcone out\n");
        return (ptcone);
    }
    else {
        for (ptcone = ptconehead; ptcone; ptcone = ptcone->NEXT) {
            if (ptcone == ptcone2del) {
                break;
            }
            ptcone2sav = ptcone;
        }
        if (ptcone != NULL) {
            ptcone2sav->NEXT = ptcone->NEXT;
            ptcone2del->NEXT = NULL;
            freecone(ptcone2del);
        }
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns delcone out\n");
        return (ptconehead);
    }
}

/*============================================================================*
 | function appendcone();                                                     |
 | chain two cone lists. WARNING: no consistency check                        |
 *============================================================================*/
cone_list *
appendcone(cone_list *ptcone1, cone_list *ptcone2)
{
    cone_list      *ptcone;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns appendcone\n");

    if (ptcone1 == NULL) {
        return (ptcone2);
    }
    else {
        for (ptcone = ptcone1; ptcone->NEXT != NULL; ptcone = ptcone->NEXT);
        ptcone->NEXT = ptcone2;
        return (ptcone1);
    }
}

/*============================================================================*
 | function getcone();                                                        |
 | returns a cone of a given index and, or name                               |
 *============================================================================*/
cone_list *
getcone(cnsfig_list *ptcnsfig, long index, char *name)
{
    cone_list      *ptcone;
    short           foundindex = 0;
    short           foundname = 0;
    ht *h;
    ptype_list *pt;
    long l;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns getcone\n");

    if ((ptcnsfig == NULL) || ((index == 0) && (name == NULL))) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns warning ***");
        (void) fprintf(stderr, " getcone() impossible: NULL pointer(s) !\n");
    }

    if ((index != 0) && (name != NULL)) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if (ptcone->INDEX == index)
                foundindex = 1;
            if (strcmp(ptcone->NAME, name) == 0)
                foundname = 1;
            if (foundindex && foundname)
                return (ptcone);
            else if (foundindex) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns warning ***");
                (void) fprintf(stderr, " getcone() conflict: found index but name does not match !\n");
                return (ptcone);
            }
            else if (foundname) {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** cns warning ***");
                (void) fprintf(stderr, " getcone() conflict: found name but index does not match !\n");
                return (ptcone);
            }
        }
    }
    else if (index != 0) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if (ptcone->INDEX == index)
                return (ptcone);
        }
    }
    else if (name != NULL) {
        if ((pt=getptype(ptcnsfig->USER, CNS_CONE_HASHTABLE))==NULL)
        {
          h=addht(1024);
          for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
             addhtitem(h, ptcone->NAME, (long)ptcone);
          }
          ptcnsfig->USER=addptype(ptcnsfig->USER, CNS_CONE_HASHTABLE, h);
          pt=ptcnsfig->USER;
        }
        {
            h=(ht *)pt->DATA;
            if ((l=gethtitem(h, name))!=EMPTYHT) return (cone_list *)l;
        }
    }

    return ((cone_list *) NULL);
}

/*============================================================================*
 | function viewcone();                                                       |
 | display the content of a given cone structure                              |
 *============================================================================*/
void 
viewcone(cone_list *ptcone, int depth)
{
    char           *margin = (char *) mbkalloc(80);
    int             k = 1;
    edge_list      *ptincone;
    edge_list      *ptoutcone;
    branch_list    *ptbranch;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcone in\n");

    if (ptcone == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " viewcone() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns viewcone out\n");
        EXIT(-1);
    }

    if (depth < 0) {
        depth = CNS_CE_VIEW;
    }

    margin = strcpy(margin, " ");
    while (k <= CNS_VIEW_LEVEL) {
        margin = strcat(margin, "   ");
        k++;
    }

    CNS_VIEW_LEVEL++;
    printf("%scone->NEXT    = 0x%lx\n", margin, (unsigned long)ptcone->NEXT);
    printf("%scone->INDEX   = %ld\n", margin, ptcone->INDEX);
    printf("%scone->NAME    = %s\n", margin, ptcone->NAME);
    printf("%scone->TYPE    = 0x%lx\n", margin, (unsigned long)ptcone->TYPE);
    printf("%scone->TECTYPE = 0x%lx\n", margin, (unsigned long)ptcone->TECTYPE);
#ifdef USEOLDTEMP
    printf("%scone->XM      = 0x%lx\n", margin, (unsigned long)ptcone->XM);
    printf("%scone->Xm      = 0x%lx\n", margin, (unsigned long)ptcone->Xm);
    printf("%scone->YM      = 0x%lx\n", margin, (unsigned long)ptcone->YM);
    printf("%scone->ym      = 0x%lx\n", margin, (unsigned long)ptcone->Ym);
#endif
    printf("%scone->INCONE  = 0x%lx\n", margin, (unsigned long)ptcone->INCONE);
    if ((depth >= 1) && (ptcone->INCONE != NULL)) {
        for (ptincone = ptcone->INCONE; ptincone != NULL; ptincone = ptincone->NEXT) {
            viewincone(ptincone, depth - 1);
        }
    }
    printf("%scone->OUTCONE = 0x%lx\n", margin, (unsigned long)ptcone->OUTCONE);
    if ((depth >= 1) && (ptcone->OUTCONE != NULL)) {
        for (ptoutcone = ptcone->OUTCONE; ptoutcone != NULL; ptoutcone = ptoutcone->NEXT) {
            viewoutcone(ptoutcone, depth - 1);
        }
    }
    printf("%scone->BREXT   = 0x%lx\n", margin, (unsigned long)ptcone->BREXT);
    if ((depth >= 1) && (ptcone->BREXT != NULL)) {
        for (ptbranch = ptcone->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            viewbranch(ptbranch, depth - 1);
        }
    }
    printf("%scone->BRVDD   = 0x%lx\n", margin, (unsigned long)ptcone->BRVDD);
    if ((depth >= 1) && (ptcone->BRVDD != NULL)) {
        for (ptbranch = ptcone->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            viewbranch(ptbranch, depth - 1);
        }
    }
    printf("%scone->BRGND   = 0x%lx\n", margin, (unsigned long)ptcone->BRGND);
    if ((depth >= 1) && (ptcone->BRGND != NULL)) {
        for (ptbranch = ptcone->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            viewbranch(ptbranch, depth - 1);
        }
    }
    printf("%scone->BRVSS   = 0x%lx\n", margin, (unsigned long)ptcone->BRVSS);
    if ((depth >= 1) && (ptcone->BRVSS != NULL)) {
        for (ptbranch = ptcone->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            viewbranch(ptbranch, depth - 1);
        }
    }
    printf("%scone->CELLS   = 0x%lx\n", margin, (unsigned long)ptcone->CELLS);
    printf("%scone->USER    = 0x%lx\n", margin, (unsigned long)ptcone->USER);
    if ((depth >= 1) && (ptcone->USER != NULL)) {
        for (ptptype = ptcone->USER; ptptype != NULL; ptptype = ptptype->NEXT) {
            viewcnsptype(ptptype);
        }
    }
    else {
        printf("\n");
    }
    CNS_VIEW_LEVEL--;
    mbkfree((void *) margin);

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns viewcone out\n");
}

/*============================================================================*
 | function displaycone();                                                    |
 | display the content of a given cone structure                              |
 *============================================================================*/
void 
displaycone(cone_list *ptcone0)
{
    long            counter;
    locon_list     *ptlocon;
    char            cone_type[100];
    char            cone_tectype[100];
    char            edgetype[100];
    edge_list      *ptedge;
    cone_list      *ptcone1;
    branch_list    *ptbranch;
    ptype_list     *ptuser;

    printf("Name:    %s\n", ptcone0->NAME);
    printf("Type:    ");
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
    printf("%s\n", cone_type);

    printf("TecType: ");
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
    printf("%s\n\n", cone_tectype);

    /* ------------------------------------------------------------------------
       cone inputs
       ------------------------------------------------------------------------ */
    printf("INPUTS:\n");
    for (ptedge = ptcone0->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                printf("    %s", ptcone1->NAME);
                strcpy(edgetype, "");
                if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    strcat(edgetype, "Command ");
                }
                if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                    strcat(edgetype, "Async ");
                }
                if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                    strcat(edgetype, "Bleeder ");
                }
                if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                    strcat(edgetype, "Feedback ");
                }
                if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                    strcat(edgetype, "MemSym ");
                }
                if ((ptedge->TYPE & CNS_HZCOM) == CNS_HZCOM) {
                    strcat(edgetype, "HZ_Command ");
                }
                if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                    strcat(edgetype, "Loop ");
                }
                if (strlen(edgetype) > 0) {
                    edgetype[strlen(edgetype)-1] = '\0';
                    printf(" (%s)", edgetype);
                }
                printf("\n");
            }
        }
        else if ((ptedge->TYPE & CNS_VDD) == CNS_VDD) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                printf("    %s (Vdd)\n", ptcone1->NAME);
            }
        }
        else if ((ptedge->TYPE & CNS_GND) == CNS_GND) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                printf("    %s (Gnd)\n", ptcone1->NAME);
            }
        }
        else if ((ptedge->TYPE & CNS_VSS) == CNS_VSS) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                printf("    %s (Vss)\n", ptcone1->NAME);
            }
        }
        else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptlocon = ptedge->UEDGE.LOCON;
            if (ptlocon != NULL) {
                printf("    %s (External)\n", ptlocon->NAME);
            }
        }
    }

    /* ------------------------------------------------------------------------
       cone outputs
       ------------------------------------------------------------------------ */
    printf("OUTPUTS:\n");
    for (ptedge = ptcone0->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                printf("    %s", ptcone1->NAME);
                strcpy(edgetype, "");
                if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    strcat(edgetype, "Command ");
                }
                if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                    strcat(edgetype, "Async ");
                }
                if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                    strcat(edgetype, "Bleeder ");
                }
                if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                    strcat(edgetype, "Feedback ");
                }
                if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                    strcat(edgetype, "MemSym ");
                }
                if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                    strcat(edgetype, "Loop ");
                }
                if (strlen(edgetype) > 0) {
                    edgetype[strlen(edgetype)-1] = '\0';
                    printf(" (%s)", edgetype);
                }
                printf("\n");
            }
        }
        else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptlocon = ptedge->UEDGE.LOCON;
            if (ptlocon != NULL) {
                printf("    %s (External)\n", ptlocon->NAME);
            }
        }
    }

    /* ------------------------------------------------------------------------
       cone branches
       ------------------------------------------------------------------------ */
    printf("\nBRANCHES:\n");
    counter = 0;
    for (ptbranch = ptcone0->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranch(ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranch(ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranch(ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranch(ptbranch, ++counter);
    }

    /* ------------------------------------------------------------------------
       cone signature
       ------------------------------------------------------------------------ */
    if ((ptuser = getptype(ptcone0->USER, CNS_SIGNATURE)) != NULL) {
        printf("\nSIGNATURE: %s\n", (char *)ptuser->DATA);
    }
}

static void 
displaybranch(branch_list *ptbranch, long counter)
{
    long           *ptlong;
    char            branch_type[100];
    char            lotrs_type[100];
    link_list      *ptlink = NULL;
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptptype = NULL;
    locon_list     *ptlocon = NULL;
    ptype_list     *ptuser;

    if (ptbranch->LINK != NULL) {
        ptlong = (long *) mbkalloc(sizeof(long));
        *ptlong = counter;
        if ((ptptype = getptype(ptbranch->USER, (long) CNS_INDEX)) != NULL) {
            ptptype->DATA = (void *) ptlong;
            mbkfree(ptlong);
        }
        else {
            ptbranch->USER = addptype(ptbranch->USER, (long) CNS_INDEX, (void *) ptlong);
        }
        printf("  %ld) ", counter);

        if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD)
            printf("Vdd");
        else if ((ptbranch->TYPE & CNS_GND) == CNS_GND)
            printf("Gnd");
        else if ((ptbranch->TYPE & CNS_VSS) == CNS_VSS)
            printf("Vss");
        else if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT)
            printf("Ext");

        strcpy(branch_type, "");
        if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS)
            strcat(branch_type, "Parallel instance,");
        else if ((ptbranch->TYPE & CNS_PARALLEL) == CNS_PARALLEL)
            strcat(branch_type, "Parallel,");
        if ((ptbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED)
            strcat(branch_type, "Degraded,");
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            strcat(branch_type, "Non-Functional,");
        if ((ptbranch->TYPE & CNS_NOT_UP) == CNS_NOT_UP)
            strcat(branch_type, "Non-Up,");
        if ((ptbranch->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN)
            strcat(branch_type, "Non-Down,");
        if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER)
            strcat(branch_type, "Bleeder,");
        if ((ptbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)
            strcat(branch_type, "Feedback,");
        if ((ptbranch->TYPE & (long) 0x40000000) != 0)
            strcat(branch_type, "PROBABLY FALSE,");

        if (strlen(branch_type) > 0) {
            branch_type[strlen(branch_type)-1] = '\0';
            printf(" (%s)", branch_type);
        }
        printf("\n");
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                ptlotrs = ptlink->ULINK.LOTRS;
                if (ptlotrs != NULL) {
                    if (MLO_IS_TRANSN(ptlotrs->TYPE)) {
                        printf("    TN ");
                    }
                    else if (MLO_IS_TRANSP(ptlotrs->TYPE)) {
                        printf("    TP ");
                    }
                    if (ptlotrs->TRNAME != NULL) {
                        if (isdigit((int)*(ptlotrs->TRNAME))) printf("tr_%s ", ptlotrs->TRNAME);
                        else printf("%s ", ptlotrs->TRNAME);
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
                        printf("(%s) ", lotrs_type);
                    }
                    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
                    if (ptuser != NULL) {
                        printf("Driven by '%s', ", ((cone_list *)ptuser->DATA)->NAME);
                    }
                    printf("W=%ld, L=%ld\n", ptlotrs->WIDTH, ptlotrs->LENGTH);
                }
            }
            else {
                if ((ptlink->TYPE & CNS_IN) == CNS_IN) {
                    printf("    In ");
                }
                else if ((ptlink->TYPE & CNS_INOUT) == CNS_INOUT) {
                    printf("    InOut ");
                }
                ptlocon = ptlink->ULINK.LOCON;
                if (ptlocon != NULL) {
                    printf("%s\n", ptlocon->NAME);
                }
            }
        }
    }
}

void 
displayconelog(int lib, int loglevel, cone_list *ptcone0)
{
    long            counter;
    locon_list     *ptlocon;
    char            cone_type[100];
    char            cone_tectype[100];
    char            edgetype[100];
    edge_list      *ptedge;
    cone_list      *ptcone1;
    branch_list    *ptbranch;
    ptype_list     *ptuser;

    avt_log(lib, loglevel, "Name:    %s\n", ptcone0->NAME);
    avt_log(lib, loglevel, "Type:    ");
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
    avt_log(lib, loglevel, "%s\n", cone_type);

    avt_log(lib, loglevel, "TecType: ");
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
    avt_log(lib, loglevel, "%s\n\n", cone_tectype);

    /* ------------------------------------------------------------------------
       cone inputs
       ------------------------------------------------------------------------ */
    avt_log(lib, loglevel, "INPUTS:\n");
    for (ptedge = ptcone0->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                avt_log(lib, loglevel, "    %s", ptcone1->NAME);
                strcpy(edgetype, "");
                if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    strcat(edgetype, "Command ");
                }
                if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                    strcat(edgetype, "Async ");
                }
                if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                    strcat(edgetype, "Bleeder ");
                }
                if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                    strcat(edgetype, "Feedback ");
                }
                if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                    strcat(edgetype, "MemSym ");
                }
                if ((ptedge->TYPE & CNS_HZCOM) == CNS_HZCOM) {
                    strcat(edgetype, "HZ_Command ");
                }
                if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                    strcat(edgetype, "Loop ");
                }
                if (strlen(edgetype) > 0) {
                    edgetype[strlen(edgetype)-1] = '\0';
                    avt_log(lib, loglevel, " (%s)", edgetype);
                }
                avt_log(lib, loglevel, "\n");
            }
        }
        else if ((ptedge->TYPE & CNS_VDD) == CNS_VDD) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                avt_log(lib, loglevel, "    %s (Vdd)\n", ptcone1->NAME);
            }
        }
        else if ((ptedge->TYPE & CNS_GND) == CNS_GND) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                avt_log(lib, loglevel, "    %s (Gnd)\n", ptcone1->NAME);
            }
        }
        else if ((ptedge->TYPE & CNS_VSS) == CNS_VSS) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                avt_log(lib, loglevel, "    %s (Vss)\n", ptcone1->NAME);
            }
        }
        else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptlocon = ptedge->UEDGE.LOCON;
            if (ptlocon != NULL) {
                avt_log(lib, loglevel, "    %s (External)\n", ptlocon->NAME);
            }
        }
    }

    /* ------------------------------------------------------------------------
       cone outputs
       ------------------------------------------------------------------------ */
    avt_log(lib, loglevel, "OUTPUTS:\n");
    for (ptedge = ptcone0->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == CNS_CONE) {
            ptcone1 = ptedge->UEDGE.CONE;
            if (ptcone1 != NULL) {
                avt_log(lib, loglevel, "    %s", ptcone1->NAME);
                strcpy(edgetype, "");
                if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    strcat(edgetype, "Command ");
                }
                if ((ptedge->TYPE & CNS_ASYNC) == CNS_ASYNC) {
                    strcat(edgetype, "Async ");
                }
                if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                    strcat(edgetype, "Bleeder ");
                }
                if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                    strcat(edgetype, "Feedback ");
                }
                if ((ptedge->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                    strcat(edgetype, "MemSym ");
                }
                if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
                    strcat(edgetype, "Loop ");
                }
                if (strlen(edgetype) > 0) {
                    edgetype[strlen(edgetype)-1] = '\0';
                    avt_log(lib, loglevel, " (%s)", edgetype);
                }
                avt_log(lib, loglevel, "\n");
            }
        }
        else if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptlocon = ptedge->UEDGE.LOCON;
            if (ptlocon != NULL) {
                avt_log(lib, loglevel, "    %s (External)\n", ptlocon->NAME);
            }
        }
    }

    /* ------------------------------------------------------------------------
       cone branches
       ------------------------------------------------------------------------ */
    avt_log(lib, loglevel, "\nBRANCHES:\n");
    counter = 0;
    for (ptbranch = ptcone0->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranchlog(lib, loglevel,     ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BRGND; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranchlog(lib, loglevel,     ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranchlog(lib, loglevel,     ptbranch, ++counter);
    }
    for (ptbranch = ptcone0->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        displaybranchlog(lib, loglevel,     ptbranch, ++counter);
    }

    /* ------------------------------------------------------------------------
       cone signature
       ------------------------------------------------------------------------ */
    if ((ptuser = getptype(ptcone0->USER, CNS_SIGNATURE)) != NULL) {
        avt_log(lib, loglevel, "\nSIGNATURE: %s\n", (char *)ptuser->DATA);
    }
}

static void 
displaybranchlog(int lib, int loglevel, branch_list *ptbranch, long counter)
{
    long           *ptlong;
    char            branch_type[100];
    char            lotrs_type[100];
    link_list      *ptlink = NULL;
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptptype = NULL;
    locon_list     *ptlocon = NULL;
    ptype_list     *ptuser;

    if (ptbranch->LINK != NULL) {
        avt_log(lib, loglevel, "  %ld) ", counter);

        if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD)
            avt_log(lib, loglevel, "Vdd");
        else if ((ptbranch->TYPE & CNS_GND) == CNS_GND)
            avt_log(lib, loglevel, "Gnd");
        else if ((ptbranch->TYPE & CNS_VSS) == CNS_VSS)
            avt_log(lib, loglevel, "Vss");
        else if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT)
            avt_log(lib, loglevel, "Ext");

        strcpy(branch_type, "");
        if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS)
            strcat(branch_type, "Parallel instance,");
        else if ((ptbranch->TYPE & CNS_PARALLEL) == CNS_PARALLEL)
            strcat(branch_type, "Parallel,");
        if ((ptbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED)
            strcat(branch_type, "Degraded,");
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
            strcat(branch_type, "Non-Functional,");
        if ((ptbranch->TYPE & CNS_NOT_UP) == CNS_NOT_UP)
            strcat(branch_type, "Non-Up,");
        if ((ptbranch->TYPE & CNS_NOT_DOWN) == CNS_NOT_DOWN)
            strcat(branch_type, "Non-Down,");
        if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER)
            strcat(branch_type, "Bleeder,");
        if ((ptbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK)
            strcat(branch_type, "Feedback,");
        if ((ptbranch->TYPE & (long) 0x40000000) != 0)
            strcat(branch_type, "PROBABLY FALSE,");

        if (strlen(branch_type) > 0) {
            branch_type[strlen(branch_type)-1] = '\0';
            avt_log(lib, loglevel, " (%s)", branch_type);
        }
        avt_log(lib, loglevel, "\n");
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                ptlotrs = ptlink->ULINK.LOTRS;
                if (ptlotrs != NULL) {
                    if (MLO_IS_TRANSN(ptlotrs->TYPE)) {
                        avt_log(lib, loglevel, "    TN ");
                    }
                    else if (MLO_IS_TRANSP(ptlotrs->TYPE)) {
                        avt_log(lib, loglevel, "    TP ");
                    }
                    if (ptlotrs->TRNAME != NULL) {
                        if (isdigit((int)*(ptlotrs->TRNAME))) avt_log(lib, loglevel, "tr_%s ", ptlotrs->TRNAME);
                        else avt_log(lib, loglevel, "%s ", ptlotrs->TRNAME);
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
                        avt_log(lib, loglevel, "(%s) ", lotrs_type);
                    }
                    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
                    if (ptuser != NULL) {
                        avt_log(lib, loglevel, "Driven by '%s', ", ((cone_list *)ptuser->DATA)->NAME);
                    }
                    avt_log(lib, loglevel, "W=%ld, L=%ld\n", ptlotrs->WIDTH, ptlotrs->LENGTH);
                }
            }
            else {
                if ((ptlink->TYPE & CNS_IN) == CNS_IN) {
                    avt_log(lib, loglevel, "    In ");
                }
                else if ((ptlink->TYPE & CNS_INOUT) == CNS_INOUT) {
                    avt_log(lib, loglevel, "    InOut ");
                }
                ptlocon = ptlink->ULINK.LOCON;
                if (ptlocon != NULL) {
                    avt_log(lib, loglevel, "%s\n", ptlocon->NAME);
                }
            }
        }
    }
}

/*============================================================================*
 | function coneparabrs();                                                    |
 | search cone for parallel branches and eventually update it's user field    |
 | and the types of it's branches                                             |
 *============================================================================*/
void 
coneparabrs(cone_list *ptcone)
{
    chain_list     *ptchain0;
    chain_list     *ptchain1;
    chain_list     *ptchain2;
    chain_list     *ptchain3;
    chain_list     *ptchain;
    ptype_list     *ptptype;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns coneparabrs in\n");

    ptchain0 = parabrs(ptcone->BREXT);
    ptchain1 = parabrs(ptcone->BRVDD);
    ptchain2 = parabrs(ptcone->BRGND);
    ptchain3 = parabrs(ptcone->BRVSS);

    if (ptchain2 != NULL) {
        for (ptchain = ptchain2; ptchain->NEXT != NULL; ptchain = ptchain->NEXT);
        ptchain->NEXT = ptchain3;
    }

    if (ptchain1 != NULL) {
        for (ptchain = ptchain1; ptchain->NEXT != NULL; ptchain = ptchain->NEXT);
        if (ptchain2 != NULL)
            ptchain->NEXT = ptchain2;
        else
            ptchain->NEXT = ptchain3;
    }

    if (ptchain0 != NULL) {
        for (ptchain = ptchain0; ptchain->NEXT != NULL; ptchain = ptchain->NEXT);
        if (ptchain1 != NULL)
            ptchain->NEXT = ptchain1;
        else if (ptchain2 != NULL)
            ptchain->NEXT = ptchain2;
        else
            ptchain->NEXT = ptchain3;
    }
    else if (ptchain1 != NULL)
        ptchain0 = ptchain1;
    else if (ptchain2 != NULL)
        ptchain0 = ptchain2;
    else
        ptchain0 = ptchain3;

    if (ptchain0 != NULL) {
        if ((ptptype = getptype(ptcone->USER, (long) CNS_PARALLEL)) != NULL) {
            ptptype->DATA = (void *) ptchain0;
        }
        else {
            ptcone->USER = addptype(ptcone->USER, (long) CNS_PARALLEL, (void *) ptchain0);
        }
    }
    else if (getptype(ptcone->USER, (long) CNS_PARALLEL) != NULL) {
        ptcone->USER = delptype(ptcone->USER, (long) CNS_PARALLEL);
    }

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns coneparabrs out\n");
}

/*============================================================================*
 | function cnbrlotrsnum();                                                   |
 | returns the maximum number of series transistor found in the input cone    |
 *============================================================================*/
short 
cnbrlotrsnum(cone_list *ptcone)
{
    short           lotrsnum1;
    short           lotrsnum2;
    short           lotrsnum3;
    short           lotrsnum4;
    short           lotrsmax1;
    short           lotrsmax2;
    short           lotrsmax;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns cnbrlotrsnum in\n");

    if (ptcone == NULL) {
        (void) fflush(stdout);
        (void) fprintf(stderr, "*** cns error ***");
        (void) fprintf(stderr, " cnbrlotrsnum() impossible: NULL pointer !\n");
        if (CNS_TRACE_MODE >= CNS_TRACE)
            (void) printf("TRA_cns cnbrlotrsnum out\n");
        EXIT(-1);
    }

    lotrsnum1 = brlotrsnum(ptcone->BREXT);
    lotrsnum2 = brlotrsnum(ptcone->BRVDD);
    lotrsnum3 = brlotrsnum(ptcone->BRGND);
    lotrsnum4 = brlotrsnum(ptcone->BRVSS);

    if (lotrsnum1 > lotrsnum2)
        lotrsmax1 = lotrsnum1;
    else
        lotrsmax1 = lotrsnum2;

    if (lotrsnum3 > lotrsnum4)
        lotrsmax2 = lotrsnum3;
    else
        lotrsmax2 = lotrsnum4;

    if (lotrsmax1 > lotrsmax2)
        lotrsmax = lotrsmax1;
    else
        lotrsmax = lotrsmax2;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns cnbrlotrsnum out\n");

    return (lotrsmax);
}

/*============================================================================*
 | function locon2cone();                                                     |
 | add a connector to a cone's CNS_LOCON USER field if it isn't already in    |
 | the list (unicity check)                                                   |
 *============================================================================*/
void 
locon2cone(locon_list *ptlocon, cone_list *ptcone)
{
    ptype_list     *ptptype;
    chain_list     *ptchain;
    short           found = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns locon2cone\n");

    if ((ptlocon != NULL) && (ptcone != NULL)) {
        if ((ptptype = getptype(ptcone->USER, (long) CNS_LOCON)) != NULL) {
            for (ptchain = (chain_list *) ptptype->DATA; ptchain != NULL; ptchain = ptchain->NEXT) {
                if ((locon_list *) ptchain->DATA == ptlocon) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ptptype->DATA = addchain((chain_list *) ptptype->DATA, (void *) ptlocon);
            }
        }
        else {
            ptcone->USER = addptype(ptcone->USER, CNS_LOCON, (void *) addchain((chain_list *) NULL, (void *) ptlocon));
        }
    }
}

/*============================================================================*
 | function lotrs2cone();                                                     |
 | add a transistor to a cone's CNS_LOTRS USER field if it isn't already in   |
 | the list (unicity check)                                                   |
 *============================================================================*/
void 
lotrs2cone(lotrs_list *ptlotrs, cone_list *ptcone)
{
    ptype_list     *ptptype;
    chain_list     *ptchain;
    short           found = 0;

    if (CNS_TRACE_MODE >= CNS_TRACE)
        (void) printf("TRA_cns lotrs2cone\n");

    if ((ptlotrs != NULL) && (ptcone != NULL)) {
        if ((ptptype = getptype(ptcone->USER, (long) CNS_LOTRS)) != NULL) {
            for (ptchain = (chain_list *) ptptype->DATA; ptchain != NULL; ptchain = ptchain->NEXT) {
                if ((lotrs_list *) ptchain->DATA == ptlotrs) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ptptype->DATA = (void *) addchain((chain_list *) ptptype->DATA, (void *) ptlotrs);
            }
        }
        else {
            ptcone->USER = addptype(ptcone->USER, CNS_LOTRS, (void *) addchain((chain_list *) NULL, (void *) ptlotrs));
        }
    }
}

/*============================================================================*
 | function addconelotrs();                                                   |
 | complete a cone's CNS_LOTRS USER field                                     |
 *============================================================================*/
void 
addconelotrs(cone_list *ptcone)
{
    link_list   *ptlink;
    lotrs_list  *ptlotrs;
    branch_list *ptbranch;
    branch_list *brlist[4];
    ptype_list  *ptuser;
    chain_list  *ptchain0, *ptchain1;
    int          i;

    freeconelotrs(ptcone);

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                    ptlotrs = ptlink->ULINK.LOTRS;
                    lotrs2cone(ptlotrs, ptcone);
                }
            }
        }
    }
    if ((ptuser = getptype(ptcone->USER, CNS_SWITCH)) != NULL) {
        for (ptchain0 = (chain_list *)ptuser->DATA; ptchain0; ptchain0 = ptchain0->NEXT) {
            for (ptchain1 = (chain_list *)ptuser->DATA; ptchain1; ptchain1 = ptchain1->NEXT) {
                lotrs2cone((lotrs_list *)ptchain1->DATA, ptcone);
            }
        }
    }
    if ((ptuser = getptype(ptcone->USER, CNS_PARATRANS)) != NULL) {
        for (ptchain0 = (chain_list *)ptuser->DATA; ptchain0; ptchain0 = ptchain0->NEXT) {
            for (ptchain1 = (chain_list *)ptuser->DATA; ptchain1; ptchain1 = ptchain1->NEXT) {
                lotrs2cone((lotrs_list *)ptchain1->DATA, ptcone);
            }
        }
    }
}

/*============================================================================*
 | function freeconelotrs();                                                  |
 | delete a cone's CNS_LOTRS USER field                                       |
 *============================================================================*/
void 
freeconelotrs(cone_list *ptcone)
{
    ptype_list  *ptuser;

    ptuser = getptype(ptcone->USER, CNS_LOTRS);
    if (ptuser != NULL) {
        if (ptuser->DATA != NULL) freechain((chain_list *)ptuser->DATA);
        ptcone->USER = delptype(ptcone->USER, CNS_LOTRS);
    }
}
