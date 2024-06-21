/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_cells.c                                                 */
/*                                                                          */
/*    (c) copyright 1995 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 05/10/1995     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static int checkCell_MS __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_MS_Separ __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_MSSC __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_MSSX __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_MSDIFF __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_FFT2 __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_FD2R __P((cone_list *ptcone, int fBuildBefig));
static int checkCell_FD2S __P((cone_list *ptcone, int fBuildBefig));
static int checkMasterSlaveConnection(cone_list *ptslave, cone_list *ptmaster);
static int detectAutoFlipFlop(cone_list *ptcone, int fBuildBefig);

static losig_list *getdrivesig __P((cone_list *ptcone, losig_list *ptlatchsig, chain_list *ptcomtranslist));

static chain_list *VISIT_LIST;

void
yagMatchFlipFlopCells(headcone, fBuildBefig)
    cone_list      *headcone;
    int             fBuildBefig;
{
    cone_list      *ptcone;
    int             found = FALSE;

    for (ptcone = headcone; ptcone; ptcone = ptcone->NEXT) {
        if (ptcone->CELLS != NULL) continue;
        if ((ptcone->TYPE & CNS_LATCH) == 0) continue;
        avt_log(LOGYAG,1, "Checking for FFT2 cell at '%s'\n", ptcone->NAME);
        found = checkCell_FFT2(ptcone, fBuildBefig);
        if (found) {
            avt_log(LOGYAG,1, "\tmaster-slave toggle FFT2 found at '%s`\n", ptcone->NAME);
        }
    }

    for (ptcone = headcone; ptcone; ptcone = ptcone->NEXT) {
        VISIT_LIST = addchain(NULL, ptcone);
        checkCell_MS(ptcone, fBuildBefig);
        freechain(VISIT_LIST);
    }

    for (ptcone = headcone; ptcone; ptcone = ptcone->NEXT) {
        VISIT_LIST = addchain(NULL, ptcone);
        checkCell_MS_Separ(ptcone, fBuildBefig);
        freechain(VISIT_LIST);
    }
}

static int
checkCell_MS(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cone_list      *ptincone;
    edge_list      *ptedge;
    int             found = FALSE;

    if (ptcone->CELLS != NULL) return FALSE;
    if ((ptcone->TYPE & CNS_LATCH) == 0) return FALSE;
    avt_log(LOGYAG,1, "Checking for master-slave cell at '%s'\n", ptcone->NAME);

    /* not a slave if latch input is also a slave */

    if (yagGetChain(VISIT_LIST, ptcone) == NULL) {
        for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & CNS_CONE) != 0) {
                ptincone = ptedge->UEDGE.CONE;
                if (ptincone->CELLS != NULL) continue;
                if ((ptcone->TYPE & CNS_LATCH) != 0) {
                    VISIT_LIST = addchain(VISIT_LIST, ptincone);
                    if (checkCell_MS(ptincone, fBuildBefig) == TRUE) {
                        VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
                        return FALSE;
                    }
                    VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
                }
            }
        }
    }

    if (!found) {
        found = checkCell_MSSX(ptcone, fBuildBefig);
        if (found) {
            avt_log(LOGYAG,1, "\tmaster-slave MSSX found at '%s`\n", ptcone->NAME);
        }
    }
    if (!found) {
        found = checkCell_FD2R(ptcone, fBuildBefig);
        if (found) {
            avt_log(LOGYAG,1, "\tmaster-slave FD2R found at '%s`\n", ptcone->NAME);
        }
    }
    if (!found) {
        found = checkCell_FD2S(ptcone, fBuildBefig);
        if (found) {
            avt_log(LOGYAG,1, "\tmaster-slave FD2S found at '%s`\n", ptcone->NAME);
        }
    }
    return found;
}

static int
checkCell_MS_Separ(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cone_list      *ptprevcone;
    edge_list      *ptedge, *ptprevedge;
    int             found = FALSE;

    if (ptcone->CELLS != NULL) return FALSE;
    if ((ptcone->TYPE & CNS_LATCH) == 0) return FALSE;
    avt_log(LOGYAG,1, "Checking for master-slave cell at '%s'\n", ptcone->NAME);

    /* not a slave if latch input is also a slave */

    if (yagGetChain(VISIT_LIST, ptcone) == NULL) {
        for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & CNS_CONE) == 0) continue;
            if ((ptedge->TYPE & CNS_LOOP) == 0) continue;
            if ((ptedge->TYPE & CNS_COMMAND) == 0) continue;
            for (ptprevedge = ptedge->UEDGE.CONE->OUTCONE; ptprevedge; ptprevedge = ptprevedge->NEXT) {
                if ((ptprevedge->TYPE & CNS_LOOP) != 0) {
                    ptprevcone = ptprevedge->UEDGE.CONE;
                    if ((ptprevcone->TYPE & CNS_LATCH) != 0) {
                        VISIT_LIST = addchain(VISIT_LIST, ptprevcone);
                        if (checkCell_MS(ptprevcone, fBuildBefig) == TRUE) {
                            VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
                            return FALSE;
                        }
                        VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
                    }
                }
            }
        }
    }

    if (!found) {
        found = checkCell_MSSC(ptcone, fBuildBefig);
        if (found) {
            avt_log(LOGYAG,1, "\tmaster-slave SCLIB found at '%s`\n", ptcone->NAME);
        }
    }
    if (!found) {
        found = checkCell_MSDIFF(ptcone, fBuildBefig);
        if (found) {
            avt_log(LOGYAG,1, "\tmaster-slave DIFF found at '%s`\n", ptcone->NAME);
        }
    }
    return found;
}

static int
checkCell_MSSC(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cell_list      *ptcell;
    cone_list      *ptincone;
    cone_list      *master = NULL;
    cone_list      *slaveout = NULL;
    cone_list      *cominv = NULL, *clock = NULL;
    cone_list      *bridge = NULL, *masterout = NULL;
    cone_list      *scom0 = NULL, *scom1 = NULL;
    cone_list      *mcom0 = NULL, *mcom1 = NULL;
    cone_list      *set = NULL, *reset = NULL;
    cone_list      *nreset = NULL;
    losig_list     *D_sig = NULL;
    losig_list     *mastersig = NULL;
    edge_list      *ptedge;
    befig_list     *ptbefig;
    chain_list     *conelist;
    chain_list     *siglist;
    ptype_list     *ptuser;
    branch_list    *ptbranch;
    branch_list    *brlist[3];
    link_list      *ptlink;
    char           *setname = NULL, *resetname = NULL;
    char           *nresetname = NULL;
    char            auxname[YAGBUFSIZE];
    long            slaveinfo = 0, masterinfo = 0;
    long            celltype;
    int             i, num_commands = 0;
    int             numedges;
    int             falling_edge;

    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) slaveinfo = (long)ptuser->DATA;

    if ((slaveinfo != (FW_INV|FB_INV)) 
    && (slaveinfo != (FW_INV|FB_TRISTATE))
    && (slaveinfo != (FW_INV|FB_SWITCHINV))) return FALSE;

    numedges = 0;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        numedges++;
        if ((ptedge->TYPE & CNS_EXT) != 0) return FALSE;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) slaveout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
        else bridge = ptincone;
    }
    if (num_commands < 1 || num_commands > 4 || numedges != num_commands+2 
    || slaveout == NULL
    || bridge == NULL)
        return FALSE;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
            ptlink = ptbranch->LINK;
            if (ptlink->NEXT == NULL) continue;
            ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;

            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TNLINK)) == (CNS_COMMAND|CNS_TNLINK)) {
                if (scom0 == NULL) scom0 = ptincone;
                else if (scom0 != ptincone) return FALSE;
            }
            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TPLINK)) == (CNS_COMMAND|CNS_TPLINK)) {
                if (scom1 == NULL) scom1 = ptincone;
                else if (scom1 != ptincone) return FALSE;
            }
        }
    }
    if (scom0 == NULL) return FALSE;

    if (yagCountEdges(bridge->INCONE) != 1) return FALSE;
    master = bridge->INCONE->UEDGE.CONE;
    if (yagMatchNOT(bridge, master) == FALSE) return FALSE;

    mastersig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, master->INDEX);
    ptuser = getptype(master->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) {
        masterinfo = (long)ptuser->DATA;
        if ((masterinfo != (FW_INV|FB_INV))
        && (masterinfo != (FW_INV|FB_TRISTATE))
        && (masterinfo != (FW_INV|FB_SWITCHINV))) return FALSE;
    }
    else return FALSE;

    num_commands = 0;
    for (ptedge = master->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) masterout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
    }
    if (num_commands < 1 || masterout != bridge) return FALSE;

    brlist[0] = master->BRVDD;
    brlist[1] = master->BRVSS;
    brlist[2] = master->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
            ptlink = ptbranch->LINK;
            ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            /* set & reset detection */
            if (ptlink->NEXT == NULL && i < 2) {
                if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                    if (i == 0) reset = ptincone;
                    else set = ptincone;
                }
                if ((ptlink->TYPE & CNS_TPLINK) != 0) {
                    if (i == 0) nreset = ptincone;
                }
                continue;
            }

            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TNLINK)) == (CNS_COMMAND|CNS_TNLINK)) {
                if (mcom0 == NULL) mcom0 = ptincone;
                else if (mcom0 != ptincone) return FALSE;
            }
            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TPLINK)) == (CNS_COMMAND|CNS_TPLINK)) {
                if (mcom1 == NULL) mcom1 = ptincone;
                else if (mcom1 != ptincone) return FALSE;
            }

            if (D_sig == NULL) {
                D_sig = ptlink->ULINK.LOTRS->DRAIN->SIG;
                if (D_sig == mastersig) D_sig = ptlink->ULINK.LOTRS->SOURCE->SIG;
            }
        }
    }
    if (D_sig == NULL) return FALSE;
    if (mcom0 == NULL) return FALSE;

    if (yagMatchNOT(scom0, mcom0)) {
        cominv = scom0;
        clock = mcom0;
        falling_edge = TRUE;
    }
    else if (yagMatchNOT(mcom0, scom0)) {
        cominv = mcom0;
        clock = scom0;
        falling_edge = FALSE;
    }
    else return FALSE;

    /* Check that nothing else driven by master */
    if (yagCountEdges(master->OUTCONE) != 1) return FALSE;
    if (yagCountEdges(masterout->OUTCONE) != 2) return FALSE;

    if (!YAG_CONTEXT->YAG_CELL_SHARE) {
        for (ptedge = cominv->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->UEDGE.CONE != ptcone) && (ptedge->UEDGE.CONE != master)) {
                return FALSE;
            }
        }
    }

    /* build the MSSC cell */
    conelist = addchain(NULL, slaveout);
    conelist = addchain(conelist, ptcone);
    conelist = addchain(conelist, masterout);
    conelist = addchain(conelist, master);
    conelist = addchain(conelist, cominv);

    if (set != NULL) {
        setname = set->NAME;
        if (falling_edge) celltype = YAG_CELL_MSS_SC|CNS_UNKNOWN;
        else celltype = YAG_CELL_MSS_SC_RT|CNS_UNKNOWN;
    }
    else if (reset != NULL) {
        resetname = reset->NAME;
        if (falling_edge) celltype = YAG_CELL_MSR_SC|CNS_UNKNOWN;
        else celltype = YAG_CELL_MSR_SC_RT|CNS_UNKNOWN;
    }
    else if (nreset != NULL) {
        nresetname = nreset->NAME;
        if (falling_edge) celltype = YAG_CELL_MSNR_SC|CNS_UNKNOWN;
        else celltype = YAG_CELL_MSNR_SC_RT|CNS_UNKNOWN;
    }
    else {
        if (falling_edge) celltype = YAG_CELL_MS_SC|CNS_UNKNOWN;
        else celltype = YAG_CELL_MS_SC_RT|CNS_UNKNOWN;
    }
    
    sprintf(auxname, "yagaux_%s", ptcone->NAME);
    if (fBuildBefig) {
        ptbefig = yagBuildBefigMSSC(ptcone->NAME, D_sig, master, (char *)D_sig->NAMECHAIN->DATA, clock->NAME, setname, resetname, nresetname, ptcone->NAME, auxname, slaveout->NAME, falling_edge, conelist);
    }
    else ptbefig = NULL;

    YAG_CONTEXT->YAG_CURCNSFIG->CELL = addcell(YAG_CONTEXT->YAG_CURCNSFIG->CELL, celltype, conelist, ptbefig);
    slaveout->CELLS = addchain(slaveout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ptcone->CELLS = addchain(ptcone->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    masterout->CELLS = addchain(masterout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    master->CELLS = addchain(master->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    cominv->CELLS = addchain(cominv->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);

    ptcone->TYPE |= YAG_CELLOUT;
    slaveout->TYPE |= YAG_CELLOUT;

    /* create ordered list of signals for heirarchical net-list */

    siglist = addchain(NULL, D_sig);
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, clock->INDEX));
    if (set != NULL) {
        siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, set->INDEX));
    }
    if (reset != NULL) {
        siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, reset->INDEX));
    }
    if (nreset != NULL) {
        siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, nreset->INDEX));
    }
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, slaveout->INDEX));

    ptcell = YAG_CONTEXT->YAG_CURCNSFIG->CELL;
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, siglist);
    ptcone->TYPE |= CNS_SLAVE;
    master->TYPE |= CNS_MASTER;

    return TRUE;
}

/****************************************************************************
 *                      function checkCell_MSDIFF();                        *
 ****************************************************************************/
static int 
checkCell_MSDIFF(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cell_list      *ptcell;
    cone_list      *master = NULL;
    cone_list      *slaveout = NULL;
    cone_list      *masterout = NULL;
    cone_list      *clock = NULL, *nclock = NULL;
    cone_list      *data = NULL, *ndata = NULL;
    branch_list    *ptbranch;
    befig_list     *ptbefig;
    chain_list     *conelist;
    chain_list     *siglist;
    char            auxname[YAGBUFSIZE];

    if ((ptcone->TYPE & CNS_MEMSYM) == 0) return FALSE;
    if (yagCountEdges(ptcone->INCONE) != 3) return FALSE;
    slaveout = (cone_list *)getptype(ptcone->USER, YAG_MEMORY_PTYPE)->DATA;
    if (yagCountEdges(slaveout->INCONE) != 3) return FALSE;

    if (ptcone->BRVDD->NEXT != NULL) return FALSE;
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_FEEDBACK) == 0) break;
    }
    if (ptbranch == NULL) return FALSE;
    if (yagCountLinks(ptbranch->LINK) != 2) return FALSE;
    masterout = (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID;
    nclock = (cone_list *)ptbranch->LINK->NEXT->ULINK.LOTRS->GRID;
    
    if (slaveout->BRVDD->NEXT != NULL) return FALSE;
    for (ptbranch = slaveout->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_FEEDBACK) == 0) break;
    }
    if (ptbranch == NULL) return FALSE;
    if (yagCountLinks(ptbranch->LINK) != 2) return FALSE;
    master = (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID;

    if ((master->TYPE & (CNS_MEMSYM|CNS_LATCH)) != (CNS_MEMSYM|CNS_LATCH)) return FALSE;
    if (yagCountEdges(master->INCONE) != 3) return FALSE;
    if ((masterout->TYPE & (CNS_MEMSYM|CNS_LATCH)) != CNS_MEMSYM) return FALSE;
    if (masterout != (cone_list *)getptype(master->USER, YAG_MEMORY_PTYPE)->DATA) return FALSE;
    if (yagCountEdges(masterout->INCONE) != 3) return FALSE;

    if (master->BRVDD->NEXT != NULL) return FALSE;
    for (ptbranch = master->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_FEEDBACK) == 0) break;
    }
    if (ptbranch == NULL) return FALSE;
    if (yagCountLinks(ptbranch->LINK) != 2) return FALSE;
    ndata = (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID;
    clock = (cone_list *)ptbranch->LINK->NEXT->ULINK.LOTRS->GRID;
    
    if (masterout->BRVDD->NEXT != NULL) return FALSE;
    for (ptbranch = masterout->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_FEEDBACK) == 0) break;
    }
    if (ptbranch == NULL) return FALSE;
    if (yagCountLinks(ptbranch->LINK) != 2) return FALSE;
    data = (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID;

    if (yagMatchNOT(nclock, clock) == FALSE) return FALSE;
    if (!YAG_CONTEXT->YAG_CELL_SHARE) {
        if (yagCountEdges(nclock->OUTCONE) != 1) return FALSE;
    }
    if (yagMatchNOT(ndata, data) == FALSE) return FALSE;
    if (!YAG_CONTEXT->YAG_CELL_SHARE) {
        if (yagCountEdges(ndata->OUTCONE) != 1) return FALSE;
    }

    conelist = addchain(NULL, slaveout);
    conelist = addchain(conelist, ptcone);
    conelist = addchain(conelist, masterout);
    conelist = addchain(conelist, master);
    conelist = addchain(conelist, nclock);
    conelist = addchain(conelist, ndata);
    
    sprintf(auxname, "yagaux_%s", ptcone->NAME);
    if (fBuildBefig) {
        ptbefig = yagBuildBefigMSDIFF(ptcone->NAME, data->NAME, clock->NAME, ptcone->NAME, auxname, slaveout->NAME, conelist);
    }
    else ptbefig = NULL;

    YAG_CONTEXT->YAG_CURCNSFIG->CELL = addcell(YAG_CONTEXT->YAG_CURCNSFIG->CELL, YAG_CELL_MSDIFF|CNS_UNKNOWN, conelist, ptbefig);
    slaveout->CELLS = addchain(slaveout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ptcone->CELLS = addchain(ptcone->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    masterout->CELLS = addchain(masterout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    master->CELLS = addchain(master->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    nclock->CELLS = addchain(nclock->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ndata->CELLS = addchain(ndata->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    
    ptcone->TYPE |= YAG_CELLOUT;
    slaveout->TYPE |= YAG_CELLOUT;
    
    ptcone->TYPE |= CNS_SLAVE;
    master->TYPE |= CNS_MASTER;

    siglist = addchain(NULL, getlosig(YAG_CONTEXT->YAG_CURLOFIG, data->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, clock->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, slaveout->INDEX));
    
    ptcell = YAG_CONTEXT->YAG_CURCNSFIG->CELL;
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, siglist);
    
    return TRUE;
}

static int
checkCell_MSSX(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cell_list      *ptcell;
    cone_list      *ptincone;
    cone_list      *master = NULL;
    cone_list      *slaveout = NULL;
    cone_list      *cominv = NULL, *clock = NULL, *nclock = NULL;
    cone_list      *masterout = NULL;
    cone_list      *scom0 = NULL, *scom1 = NULL;
    cone_list      *mcom0 = NULL, *mcom1 = NULL;
    cone_list      *datainput = NULL;
    losig_list     *mastersig = NULL;
    edge_list      *ptedge;
    befig_list     *ptbefig;
    chain_list     *conelist;
    chain_list     *siglist;
    ptype_list     *ptuser;
    branch_list    *ptbranch;
    branch_list    *brlist[3];
    link_list      *ptlink;
    long            slaveinfo = 0, masterinfo = 0;
    long            celltype;
    int             i, num_commands = 0;
    int             numedges;
    int             falling_edge;

    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) slaveinfo = (long)ptuser->DATA;

    if ((slaveinfo != (FW_INV|FB_INV)) 
    && (slaveinfo != (FW_INV|FB_TRISTATE))
    && (slaveinfo != (FW_INV|FB_SWITCHINV))) return FALSE;

    numedges = 0;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        numedges++;
        if ((ptedge->TYPE & CNS_EXT) != 0) return FALSE;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) slaveout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
        else master = ptincone;
    }
    if (num_commands < 1 || num_commands > 4 || numedges != num_commands+2 
    || slaveout == NULL
    || master == NULL)
        return FALSE;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
            ptlink = ptbranch->LINK;
            if (ptlink->NEXT == NULL) continue;
            ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;

            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TNLINK)) == (CNS_COMMAND|CNS_TNLINK)) {
                if (scom0 == NULL) scom0 = ptincone;
                else if (scom0 != ptincone) return FALSE;
            }
            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TPLINK)) == (CNS_COMMAND|CNS_TPLINK)) {
                if (scom1 == NULL) scom1 = ptincone;
                else if (scom1 != ptincone) return FALSE;
            }
        }
    }
    if (scom0 == NULL) return FALSE;

    mastersig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, master->INDEX);
    ptuser = getptype(master->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) {
        masterinfo = (long)ptuser->DATA;
        if ((masterinfo != (FW_INV|FB_INV))
        && (masterinfo != (FW_INV|FB_TRISTATE))
        && (masterinfo != (FW_INV|FB_SWITCHINV))) return FALSE;
    }
    else return FALSE;

    num_commands = 0;
    numedges = 0;
    for (ptedge = master->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) return FALSE;
        numedges++;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) masterout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
        else datainput = ptincone;
    }
    if (num_commands != 2) return FALSE;
    if (numedges != 4) return FALSE;
    if (datainput == NULL) return FALSE;

    brlist[0] = master->BRVDD;
    brlist[1] = master->BRVSS;
    brlist[2] = master->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if ((ptbranch->TYPE & CNS_DEGRADED) != 0) return FALSE;
            ptlink = ptbranch->LINK;
            ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;

            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TNLINK)) == (CNS_COMMAND|CNS_TNLINK)) {
                if (mcom0 == NULL) mcom0 = ptincone;
                else if (mcom0 != ptincone) return FALSE;
            }
            if ((ptlink->TYPE & (CNS_COMMAND|CNS_TPLINK)) == (CNS_COMMAND|CNS_TPLINK)) {
                if (mcom1 == NULL) mcom1 = ptincone;
                else if (mcom1 != ptincone) return FALSE;
            }
        }
    }
    if (mcom0 == NULL) return FALSE;

    if (yagMatchNOT(scom0, mcom0)) {
        cominv = scom0;
        clock = mcom0;
        falling_edge = TRUE;
    }
    else if (yagMatchNOT(mcom0, scom0)) {
        cominv = mcom0;
        clock = scom0;
        falling_edge = FALSE;
    }
    else return FALSE;

    if (yagCountEdges(clock->INCONE) == 1) {
        ptincone = clock->INCONE->UEDGE.CONE;
        if (yagMatchNOT(clock, ptincone) && yagCountEdges(ptincone->OUTCONE) == 1) {
            nclock = clock;
            clock = ptincone;
            falling_edge = !falling_edge;
        }
    }
 
    /* Check that nothing else driven by master */
    if (yagCountEdges(master->OUTCONE) != 2) return FALSE;
    if (yagCountEdges(masterout->OUTCONE) != 1) return FALSE;

    if (!YAG_CONTEXT->YAG_CELL_SHARE) {
        for (ptedge = cominv->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->UEDGE.CONE != ptcone) && (ptedge->UEDGE.CONE != master)) {
                return FALSE;
            }
        }
    }

    /* build the MSSX cell */

    conelist = addchain(NULL, slaveout);
    conelist = addchain(conelist, ptcone);
    conelist = addchain(conelist, masterout);
    conelist = addchain(conelist, master);
    conelist = addchain(conelist, cominv);
    if (nclock != NULL) {
        conelist = addchain(conelist, nclock);
    }

    if (falling_edge) celltype = YAG_CELL_MS_SX|CNS_UNKNOWN;
    else celltype = YAG_CELL_MS_SX_RT|CNS_UNKNOWN;
    
    if (fBuildBefig) {
        ptbefig = yagBuildBefigMSSX(ptcone->NAME, datainput->NAME, clock->NAME, master->NAME, slaveout->NAME, falling_edge, conelist);
    }
    else ptbefig = NULL;

    YAG_CONTEXT->YAG_CURCNSFIG->CELL = addcell(YAG_CONTEXT->YAG_CURCNSFIG->CELL, celltype, conelist, ptbefig);
    slaveout->CELLS = addchain(slaveout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ptcone->CELLS = addchain(ptcone->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    masterout->CELLS = addchain(masterout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    master->CELLS = addchain(master->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    cominv->CELLS = addchain(cominv->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    if (nclock != NULL) {
        nclock->CELLS = addchain(nclock->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    }

    ptcone->TYPE |= YAG_CELLOUT;
    slaveout->TYPE |= YAG_CELLOUT;

    ptcone->TYPE |= CNS_SLAVE;
    master->TYPE |= CNS_MASTER;

    /* create ordered list of signals for heirarchical net-list */

    siglist = addchain(NULL, getlosig(YAG_CONTEXT->YAG_CURLOFIG, datainput->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, clock->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, slaveout->INDEX));

    ptcell = YAG_CONTEXT->YAG_CURCNSFIG->CELL;
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, siglist);
    
    return TRUE;
}

static int
checkCell_FFT2(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cell_list      *ptcell;
    cone_list      *ptincone;
    cone_list      *master = NULL;
    cone_list      *reset = NULL;
    cone_list      *data = NULL;
    cone_list      *slaveout = NULL;
    cone_list      *masterout = NULL;
    cone_list      *clock = NULL, *nclock = NULL;
    cone_list      *ld = NULL, *nld = NULL;
    cone_list      *inc = NULL, *ninc = NULL;
    cone_list      *c = NULL, *nc = NULL;
    edge_list      *ptedge;
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    losig_list     *mastersig;
    losig_list     *D_sig = NULL;
    befig_list     *ptbefig;
    chain_list     *translist = NULL;
    chain_list     *templist = NULL;
    chain_list     *conelist;
    chain_list     *siglist;
    ptype_list     *ptuser;
    char            auxname[YAGBUFSIZE];
    long            slaveinfo = 0, masterinfo = 0;
    int             num_commands = 0;
    int             num_branch = 0;
    int             i;

    if ((ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) == 0) return FALSE;
    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) slaveinfo = (long)ptuser->DATA;

    if (slaveinfo != (FW_INV|FB_SWITCHNAND)) return FALSE;

    if (yagCountEdges(ptcone->INCONE) != 10) return FALSE;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) return FALSE;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) {
            slaveout = ptincone;
            continue;
        }
        if ((ptedge->TYPE & CNS_COMMAND) != 0) {
            num_commands++;
            continue;
        }
        if ((ptincone->TYPE & CNS_LATCH) != 0) {
            master = ptincone;
            continue;
        }
        if (data == NULL) {
            if (yagMatchSwitchedInversion(ptcone, ptincone, &translist) == PM_SWITCHINV) {
                data = ptincone;
                continue;
            }
            else {
                freechain(translist);
                translist = NULL;
            }
        }
        if (reset == NULL) {
            if (yagMatchSwitchedInversion(ptcone, ptincone, &templist) == PM_SWITCHNAND) {
                reset = ptincone;
                freechain(templist);
                templist = NULL;
                continue;
            }
        }
    }
    if (num_commands != 4
    || slaveout == NULL 
    || master == NULL
    || reset == NULL
    || data == NULL)
        return FALSE;

    mastersig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, master->INDEX);

    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        num_branch++;
        ptlink = ptbranch->LINK;
        if ((ptbranch->TYPE & CNS_BLEEDER) != 0) {
            if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                inc = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            }
            else ninc = (cone_list *)ptlink->ULINK.LOTRS->GRID;
        }
        else if (yagGetChain(translist, ptlink->ULINK.LOTRS) != NULL) {
            if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                ld = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            }
            else nld = (cone_list *)ptlink->ULINK.LOTRS->GRID;
        }
        else {
            if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                clock = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            }
            else nclock = (cone_list *)ptlink->ULINK.LOTRS->GRID;
        }
    }
    if (num_branch != 6
    || inc == NULL || ninc == NULL
    || ld == NULL || nld == NULL
    || clock == NULL || nclock == NULL)
        return FALSE;

    freechain(translist);
    translist = NULL;

    ptuser = getptype(master->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) masterinfo = (long)ptuser->DATA;

    if (masterinfo != (FW_NAND|FB_SWITCHINV)) return FALSE;

    num_commands = 0;
    for (ptedge = master->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) masterout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
    }
    if (num_commands != 4) return FALSE;

    if (yagMatchSwitchedInversion(master, slaveout, &translist) != PM_SWITCHNAND) return FALSE;

    brlist[0] = master->BRVDD;
    brlist[1] = master->BRVSS;
    brlist[2] = master->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            ptlink = ptbranch->LINK;
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) {
                if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                    c = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                }
                else nc = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                continue;
            }
            if (yagGetChain(translist, ptlink->ULINK.LOTRS) != NULL) {
                if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                    if ((cone_list *)ptlink->ULINK.LOTRS->GRID != inc) return FALSE;
                }
                else if ((cone_list *)ptlink->ULINK.LOTRS->GRID != ninc) return FALSE;
            }
            else {
                if (D_sig == NULL) {
                    D_sig = ptlink->ULINK.LOTRS->DRAIN->SIG;
                    if (D_sig == mastersig) D_sig = ptlink->ULINK.LOTRS->SOURCE->SIG;
                }
                if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                    if ((cone_list *)ptlink->ULINK.LOTRS->GRID != ld) return FALSE;
                }
                else if ((cone_list *)ptlink->ULINK.LOTRS->GRID != nld) return FALSE;
            }
        }
    }

    /* build the FFT2 cell */

    conelist = addchain(NULL, slaveout);
    conelist = addchain(conelist, ptcone);
    conelist = addchain(conelist, masterout);
    conelist = addchain(conelist, master);
    conelist = addchain(conelist, inc);
    conelist = addchain(conelist, ninc);
    conelist = addchain(conelist, c);
    conelist = addchain(conelist, nc);

    sprintf(auxname, "yagaux_%s", ptcone->NAME);
    if (fBuildBefig) {
        ptbefig = yagBuildBefigFFT2(ptcone->NAME, D_sig, master, (char *)D_sig->NAMECHAIN->DATA, nclock->NAME, ld->NAME, reset->NAME, ptcone->NAME, slaveout->NAME, auxname, conelist);
    }
    else ptbefig = NULL;

    YAG_CONTEXT->YAG_CURCNSFIG->CELL = addcell(YAG_CONTEXT->YAG_CURCNSFIG->CELL, YAG_CELL_FFT2|CNS_UNKNOWN, conelist, ptbefig);
    slaveout->CELLS = addchain(slaveout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ptcone->CELLS = addchain(ptcone->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    masterout->CELLS = addchain(masterout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    master->CELLS = addchain(master->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    inc->CELLS = addchain(inc->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ninc->CELLS = addchain(ninc->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    c->CELLS = addchain(c->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    nc->CELLS = addchain(nc->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);

    ptcone->TYPE |= YAG_CELLOUT;
    slaveout->TYPE |= YAG_CELLOUT;
    
    ptcone->TYPE |= CNS_SLAVE;
    master->TYPE |= CNS_MASTER;

    siglist = addchain(NULL, D_sig);
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, nclock->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ld->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, reset->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, slaveout->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX));
    
    ptcell = YAG_CONTEXT->YAG_CURCNSFIG->CELL;
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, siglist);
    
    return TRUE;
}

static int
checkCell_FD2R(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cell_list      *ptcell;
    cone_list      *ptincone;
    cone_list      *master = NULL;
    cone_list      *reset = NULL;
    cone_list      *clock = NULL;
    cone_list      *nclock = NULL;
    cone_list      *slaveout = NULL;
    cone_list      *masterout = NULL;
    edge_list      *ptedge;
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    losig_list     *mastersig;
    losig_list     *D_sig = NULL;
    befig_list     *ptbefig;
    chain_list     *conelist;
    chain_list     *siglist;
    ptype_list     *ptuser;
    char            auxname[YAGBUFSIZE];
    long            slaveinfo = 0, masterinfo = 0;
    int             num_commands = 0;
    int             i;

    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) slaveinfo = (long)ptuser->DATA;

    if (slaveinfo != (FW_INV|FB_SWITCHNAND)) return FALSE;

    if (yagCountEdges(ptcone->INCONE) != 5) return FALSE;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) return FALSE;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) slaveout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
        else if ((ptincone->TYPE & CNS_LATCH) != 0) master = ptincone;
        else reset = ptincone;
    }
    if (num_commands != 2
    || slaveout == NULL 
    || reset == NULL 
    || master == NULL)
        return FALSE;

    mastersig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, master->INDEX);

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptlink->TYPE & CNS_TNLINK) != 0) {
            clock = (cone_list *)ptlink->ULINK.LOTRS->GRID;
        }
        else nclock = (cone_list *)ptlink->ULINK.LOTRS->GRID;
    }
    if (clock == NULL || nclock == NULL) return FALSE;

    ptuser = getptype(master->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) masterinfo = (long)ptuser->DATA;

    if (masterinfo != (FW_NAND|FB_SWITCHINV)) return FALSE;

    num_commands = 0;
    for (ptedge = master->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) masterout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
    }
    if (num_commands != 2) return FALSE;

    brlist[0] = master->BRVDD;
    brlist[1] = master->BRVSS;
    brlist[2] = master->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
            ptlink = ptbranch->LINK;
            if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                if ((cone_list *)ptlink->ULINK.LOTRS->GRID != nclock) return FALSE;
            }
            else if ((cone_list *)ptlink->ULINK.LOTRS->GRID != clock) return FALSE;
            if (D_sig == NULL) {
                D_sig = ptlink->ULINK.LOTRS->DRAIN->SIG;
                if (D_sig == mastersig) D_sig = ptlink->ULINK.LOTRS->SOURCE->SIG;
            }
        }
    }

    /* build the FD2R cell */

    conelist = addchain(NULL, slaveout);
    conelist = addchain(conelist, ptcone);
    conelist = addchain(conelist, masterout);
    conelist = addchain(conelist, master);
    
    sprintf(auxname, "yagaux_%s", ptcone->NAME);
    if (fBuildBefig) {
        ptbefig = yagBuildBefigFD2R(ptcone->NAME, D_sig, master, (char *)D_sig->NAMECHAIN->DATA, nclock->NAME, reset->NAME, ptcone->NAME, slaveout->NAME, auxname, conelist);
    }
    else ptbefig = NULL;

    YAG_CONTEXT->YAG_CURCNSFIG->CELL = addcell(YAG_CONTEXT->YAG_CURCNSFIG->CELL, YAG_CELL_FD2R|CNS_UNKNOWN, conelist, ptbefig);
    slaveout->CELLS = addchain(slaveout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ptcone->CELLS = addchain(ptcone->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    masterout->CELLS = addchain(masterout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    master->CELLS = addchain(master->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    
    ptcone->TYPE |= YAG_CELLOUT;
    slaveout->TYPE |= YAG_CELLOUT;
    
    ptcone->TYPE |= CNS_SLAVE;
    master->TYPE |= CNS_MASTER;

    siglist = addchain(NULL, D_sig);
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, nclock->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, reset->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, slaveout->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX));
    
    ptcell = YAG_CONTEXT->YAG_CURCNSFIG->CELL;
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, siglist);
    
    return TRUE;
}

static int
checkCell_FD2S(ptcone, fBuildBefig)
    cone_list      *ptcone;
    int             fBuildBefig;
{
    cell_list      *ptcell;
    cone_list      *ptincone;
    cone_list      *master = NULL;
    cone_list      *reset = NULL;
    cone_list      *clock = NULL;
    cone_list      *nclock = NULL;
    cone_list      *slaveout = NULL;
    cone_list      *masterout = NULL;
    edge_list      *ptedge;
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    losig_list     *mastersig;
    losig_list     *D_sig = NULL;
    befig_list     *ptbefig;
    chain_list     *conelist;
    chain_list     *siglist;
    ptype_list     *ptuser;
    char            auxname[YAGBUFSIZE];
    long            slaveinfo = 0, masterinfo = 0;
    int             num_commands = 0;
    int             num_inputs = 0;
    int             i;

    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) slaveinfo = (long)ptuser->DATA;

    if (slaveinfo != (FW_NAND|FB_SWITCHINV)) return FALSE;

    if (yagCountEdges(ptcone->INCONE) != 4) return FALSE;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) return FALSE;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) slaveout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
        else if ((ptincone->TYPE & CNS_LATCH) != 0) master = ptincone;
    }
    if (num_commands != 2
    || slaveout == NULL 
    || master == NULL)
        return FALSE;

    for (ptedge = slaveout->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        num_inputs++;
        ptincone = ptedge->UEDGE.CONE;
        if (ptincone != ptcone) reset = ptincone;
    }
    if (num_inputs != 2) return FALSE;

    mastersig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, master->INDEX);

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptlink->TYPE & CNS_TNLINK) != 0) {
            clock = (cone_list *)ptlink->ULINK.LOTRS->GRID;
        }
        else nclock = (cone_list *)ptlink->ULINK.LOTRS->GRID;
    }
    if (clock == NULL || nclock == NULL) return FALSE;

    ptuser = getptype(master->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) masterinfo = (long)ptuser->DATA;

    if (masterinfo != (FW_INV|FB_SWITCHNAND)) return FALSE;

    num_commands = 0;
    for (ptedge = master->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        ptincone = ptedge->UEDGE.CONE;
        if ((ptedge->TYPE & CNS_LOOP) != 0) masterout = ptincone;
        else if ((ptedge->TYPE & CNS_COMMAND) != 0) num_commands++;
    }
    if (num_commands != 2) return FALSE;

    brlist[0] = master->BRVDD;
    brlist[1] = master->BRVSS;
    brlist[2] = master->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
            ptlink = ptbranch->LINK;
            if ((ptlink->TYPE & CNS_TNLINK) != 0) {
                if ((cone_list *)ptlink->ULINK.LOTRS->GRID != nclock) return FALSE;
            }
            else if ((cone_list *)ptlink->ULINK.LOTRS->GRID != clock) return FALSE;
            if (D_sig == NULL) {
                D_sig = ptlink->ULINK.LOTRS->DRAIN->SIG;
                if (D_sig == mastersig) D_sig = ptlink->ULINK.LOTRS->SOURCE->SIG;
            }
        }
    }

    /* build the FD2S cell */

    conelist = addchain(NULL, slaveout);
    conelist = addchain(conelist, ptcone);
    conelist = addchain(conelist, masterout);
    conelist = addchain(conelist, master);

    sprintf(auxname, "yagaux_%s", ptcone->NAME);
    if (fBuildBefig) {
        ptbefig = yagBuildBefigFD2S(ptcone->NAME, D_sig, master, (char *)D_sig->NAMECHAIN->DATA, nclock->NAME, reset->NAME, ptcone->NAME, slaveout->NAME, auxname, conelist);
    }
    else ptbefig = NULL;

    YAG_CONTEXT->YAG_CURCNSFIG->CELL = addcell(YAG_CONTEXT->YAG_CURCNSFIG->CELL, YAG_CELL_FD2S|CNS_UNKNOWN, conelist, ptbefig);
    slaveout->CELLS = addchain(slaveout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    ptcone->CELLS = addchain(ptcone->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    masterout->CELLS = addchain(masterout->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);
    master->CELLS = addchain(master->CELLS, YAG_CONTEXT->YAG_CURCNSFIG->CELL);

    ptcone->TYPE |= YAG_CELLOUT;
    slaveout->TYPE |= YAG_CELLOUT;
    
    ptcone->TYPE |= CNS_SLAVE;
    master->TYPE |= CNS_MASTER;

    siglist = addchain(NULL, D_sig);
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, nclock->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, reset->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, slaveout->INDEX));
    siglist = addchain(siglist, getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX));
    
    ptcell = YAG_CONTEXT->YAG_CURCNSFIG->CELL;
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, siglist);
    
    return TRUE;
}

befig_list *
yagMakeLatchBehaviour(ptcone, fUseSigNames)
    cone_list      *ptcone;
    int             fUseSigNames;
{
    cone_list      *ptcomcone;
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    losig_list     *latchsig;
    losig_list     *ptdrivesig;
    lotrs_list     *pttrans;
    befig_list     *ptbefig;
    chain_list     *ptchain;
    chain_list     *ptcondition;
    chain_list     *pttranslist;
    chain_list     *driverlist = NULL;
    chain_list     *condlist = NULL;
    ptype_list     *ptuser;
    char            auxname[YAGBUFSIZE];
    long            latchinfo = 0;
    int             i;

    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) latchinfo = (long)ptuser->DATA;

    if (latchinfo != (FW_INV|FB_INV)
    && latchinfo != (FW_INV|FB_PASSINV)
    && latchinfo != (FW_INV|FB_SWITCHINV)
    && latchinfo != (FW_INV|FB_TRISTATE))
        return NULL;

    latchsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
            if ((ptbranch->TYPE & YAG_TREATED) != 0) {
                ptbranch->TYPE &= ~YAG_TREATED;
                continue;
            }
            if ((ptbranch->LINK->TYPE & CNS_COMMAND) != 0) {
                pttranslist = NULL;
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    if ((ptlink->TYPE & CNS_COMMAND) == 0) break;
                    pttranslist = addchain(pttranslist, ptlink->ULINK.LOTRS);
                }
                ptdrivesig = getdrivesig(ptcone, latchsig, pttranslist);
                if (ptdrivesig != NULL) {
                    if (pttranslist->NEXT == NULL) {
                        pttrans = (lotrs_list *)pttranslist->DATA;
                        ptcomcone = (cone_list *)pttrans->GRID;
                        if ((pttrans->TYPE & CNS_TN) != 0) {
                            ptcondition = createAtom(ptcomcone->NAME);
                        }
                        else ptcondition = notExpr(createAtom(ptcomcone->NAME));
                    }
                    else {
                        ptcondition = createExpr(AND);
                        for (ptchain = pttranslist; ptchain; ptchain = ptchain->NEXT) {
                            pttrans = (lotrs_list *)ptchain->DATA;
                            ptcomcone = (cone_list *)pttrans->GRID;
                            if ((pttrans->TYPE & CNS_TN) != 0) {
                                addQExpr(ptcondition, createAtom(ptcomcone->NAME));
                            }
                            else addQExpr(ptcondition, notExpr(createAtom(ptcomcone->NAME)));
                        }
                    }
                    driverlist = addchain(driverlist, ptdrivesig);
                    condlist = addchain(condlist, ptcondition);
                }
                else {
                    for (i=0; i<3; i++) {
                        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                            ptbranch->TYPE &= ~YAG_TREATED;
                        }
                    }
                    for (ptchain = condlist; ptchain; ptchain = ptchain->NEXT) {
                        freeExpr((chain_list *)ptchain->DATA);
                    }
                    freechain(condlist);
                    freechain(pttranslist);
                    freechain(driverlist);
                    return NULL;
                }
                freechain(pttranslist);
            }
        }
    }

    if (fUseSigNames) {
        sprintf(auxname, "yagaux_%s", ptcone->NAME);
        if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
            ptbefig = yagBuildBefigSimpleLatch(ptcone->NAME, driverlist, condlist, ptcone, auxname, ptcone->NAME, fUseSigNames);
        }
        else {
            ptbefig = yagBuildBefigSimpleLatch(ptcone->NAME, driverlist, condlist, ptcone, ptcone->NAME, auxname, fUseSigNames);
        }
    }
    else {
        ptbefig = yagBuildBefigSimpleLatch(ptcone->NAME, driverlist, condlist, ptcone, "mem", "f", fUseSigNames);
    }
    
    freechain(driverlist);
    freechain(condlist);
    
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            ptbranch->TYPE &= ~YAG_TREATED;
        }
    }

    return ptbefig;
}

static losig_list *
getdrivesig(ptcone, ptlatchsig, ptcomtranslist)
    cone_list   *ptcone;
    losig_list  *ptlatchsig;
    chain_list  *ptcomtranslist;
{
    lotrs_list  *pttrans;
    losig_list  *ptsig = NULL;
    losig_list  *ptdatasig = NULL;
    branch_list *ptbranch;
    branch_list *brlist[3];
    link_list   *ptlink;
    char         typeflag;
    int         i;
    
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    typeflag = 0;
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
            if ((ptbranch->LINK->TYPE & CNS_COMMAND) == 0) continue;
            ptsig = ptlatchsig;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_COMMAND) == 0) break;
                pttrans = ptlink->ULINK.LOTRS;
                if (yagGetChain(ptcomtranslist, pttrans) == NULL) break;
                if (pttrans->DRAIN->SIG == ptsig) ptsig = pttrans->SOURCE->SIG;
                else ptsig = pttrans->DRAIN->SIG;
            }
            if (ptlink != NULL) {
                if ((ptlink->TYPE & CNS_COMMAND) != 0) continue;
            }
            ptbranch->TYPE |= YAG_TREATED;
            ptdatasig = ptsig;
            typeflag |= (char)(0x1 << i);
        }
    }
    if (typeflag > 2) return ptdatasig;
    return NULL;
}

befig_list *
yagBuildBefigSimpleLatch(figname, datasiglist, conditionlist, latchcone, reg, Q, fUseSigNames)
    char           *figname;
    chain_list     *datasiglist;
    chain_list     *conditionlist;
    cone_list      *latchcone;
    char           *reg;
    char           *Q;
    int             fUseSigNames;
{
    befig_list     *ptbefig;
    cone_list      *ptdrivecone;
    //cone_list      *ptlinkcone;
    edge_list      *ptin;
    losig_list     *ptdatasig;
    chain_list     *ptcndabl, *ptvalabl;
    biabl_list     *ptbiabl = NULL;
    chain_list     *ptsigchain, *ptcndchain;
    ptype_list     *ptuser;
    char           *name;
    //char           *oldname;
    char            buf[16];
    char            label[16];
    int             deleteDriveCone;
    int             nameindex;

    ptbefig = yagInitBefig(figname);

    for (ptsigchain = datasiglist, ptcndchain = conditionlist, nameindex = 1;
     ptsigchain != NULL;
     ptsigchain = ptsigchain->NEXT, ptcndchain = ptcndchain->NEXT, nameindex++) {
        deleteDriveCone = FALSE;
        ptdatasig = (losig_list *)ptsigchain->DATA;
        if (fUseSigNames) name = ptdatasig->NAMECHAIN->DATA;
        else {
            sprintf(buf, "yagle_%d", nameindex);
            name = namealloc(buf);
        }
        if (ptdatasig->TYPE == 'I') {
            ptuser = getptype(ptdatasig->USER, YAG_CONE_PTYPE);
            if (ptuser != NULL) ptdrivecone = (cone_list *)ptuser->DATA;
            if (ptuser == NULL || (ptdrivecone->TYPE & (YAG_TEMPCONE|YAG_DELETED)) != 0) {
                ptdrivecone = yagAddGlueCone(ptdatasig, latchcone);
            }
            ptvalabl = createAtom(name);
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
            ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
        }
        else if (ptdatasig->TYPE == CNS_SIGVDD) ptvalabl = createAtom("'1'");
        else if (ptdatasig->TYPE == CNS_SIGVSS) ptvalabl = createAtom("'0'");
        else {
            ptvalabl = createAtom(name);
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
            ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
        }

        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        ptcndabl = (chain_list *)ptcndchain->DATA;
        ptbiabl = beh_addbiabl(ptbiabl, label, ptcndabl, ptvalabl);
        if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
    }

    for (ptin = latchcone->INCONE; ptin; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
        if ((ptin->TYPE & (CNS_POWER|CNS_CONE)) != 0) {
            name = ptin->UEDGE.CONE->NAME;
        }
        else name = ptin->UEDGE.LOCON->NAME;
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
    }

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, Q, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, Q, createAtom(reg), NULL, 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, reg, ptbiabl, yagMakeBinode(ptbiabl, latchcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, reg);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    yagCleanBehVect();
    return ptbefig;
}

static int
yagCheckAutoFlipFlop(cone_list *ptslave, cone_list *ptmaster)
{
    edge_list *ptedge, *ptgraphedges = NULL;
    chain_list *ptconechain = NULL;
    chain_list *ptextranodes, *ptchain;
    cone_list *ptcone;
    graph *ptgraph;
    gnode_list *ptnode;
    befig_list *ptbefig, *tmp_befig;
    cbhseq     *ptcbhseq;
    ptype_list *ptuser;
    char        buf[1024];
    char       *name;
    short save_depth, save_prop_hz;
    int         existOne = FALSE;
    int         existZero = FALSE;
    
    sprintf(buf, "flipflop_%s", ptmaster->NAME);
    ptbefig = yagInitBefig(namealloc(buf));
    yagInitBehDefined();

    ptgraphedges = addedge(ptgraphedges, CNS_CONE, ptslave);
    ptgraphedges = addedge(ptgraphedges, CNS_CONE, ptmaster);
    for (ptedge = ptslave->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == 0) continue;
        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
            ptgraphedges = addedge(ptgraphedges, CNS_CONE, ptedge->UEDGE.CONE);
        }
    }
    for (ptedge = ptmaster->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == 0) continue;
        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) {
            ptgraphedges = addedge(ptgraphedges, CNS_CONE, ptedge->UEDGE.CONE);
        }
    }
    
    save_prop_hz = YAG_CONTEXT->YAG_PROP_HZ;
    save_depth = YAG_CONTEXT->YAG_DEPTH;
    YAG_CONTEXT->YAG_DEPTH = 2;
    YAG_CONTEXT->YAG_PROP_HZ = FALSE;
    
    ptgraph = yagBuildGraph(ptgraphedges, NULL, TRUE);
    yagTraverseGraph(ptgraph);
    
    /* Obtain list of cones to add to cell befig */
    for (ptedge = ptgraphedges; ptedge; ptedge = ptedge->NEXT) {
        ptconechain = addchain(ptconechain, ptedge->UEDGE.CONE);
    }
    ptextranodes = yagGetNonPrimaryUsedNodes(ptgraph);
    for (ptchain = ptextranodes; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *)ptchain->DATA;
        if ((ptnode->TYPE & EXT) != 0) continue;
        if ((ptnode->TYPE & ROOT_NODE) == 0) {
            ptconechain = addchain(ptconechain, ptnode->OBJECT.CONE);
        }
    }
    freechain(ptextranodes);
    
    /* Obtain list of inputs of cell befig */
    for (ptchain = ptgraph->PRIMVARS; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *) ptchain->DATA;
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) name = ptnode->OBJECT.CONE->NAME;
        else name = ptnode->OBJECT.LOCON->NAME;
        if ((ptnode->TYPE & ROOT_NODE) == 0) {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
            ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
        }
        else {
            ptcone = ptnode->OBJECT.CONE;
            for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & CNS_LOOP) != 0) continue;
                if ((ptedge->TYPE & CNS_EXT) != 0) {
                    name = ptedge->UEDGE.LOCON->NAME;
                }
                else {
                    ptcone = ptedge->UEDGE.CONE;
                    if (ptcone == ptmaster || ptcone == ptslave) continue;
                    name = ptcone->NAME;
                }
                ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
                ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
            }
        }
    }
    yagFreeGraph(ptgraph);
    YAG_CONTEXT->YAG_DEPTH = save_depth;
    YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;
    
    /* Add an artificial output */
    sprintf(buf,"flipflop_%s_out", ptslave->NAME);
    name = namealloc(buf);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, name, createAtom(ptslave->NAME), NULL, 0);
    
    /* add all the cone behaviours */
    for (ptchain = ptconechain; ptchain; ptchain = ptchain->NEXT) {
        ptcone = (cone_list *)ptchain->DATA;
        if ((ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE)) != NULL) {
            yagAddBefigBehaviour(tmp_befig=beh_duplicate((befig_list *)ptuser->DATA), ptbefig, FALSE);
            beh_frebefig(tmp_befig);
        }
        else if ((ptcone->TYPE & (CNS_VDD|CNS_VSS)) != 0) {
            if ((ptcone->TYPE & CNS_VDD) == CNS_VDD) existOne = TRUE;
            else existZero = TRUE;
        }
        else if ((ptcone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0) {
            if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) existOne = TRUE;
            else existZero = TRUE;
        }
        else yagAddConeBehaviour(ptcone, ptbefig, ptcone, NULL);
    }
    if (existZero) {
        yagAddBeauxVect(ptbefig, namealloc("yag_zero"), createAtom("'0'"), NULL, NULL, 0);
        yagAddBerinVect(ptbefig, namealloc("yag_zero"));
    }
    if (existOne) {
        yagAddBeauxVect(ptbefig, namealloc("yag_one"), createAtom("'1'"), NULL, NULL, 0);
        yagAddBerinVect(ptbefig, namealloc("yag_one"));
    }
    
    /* Check using CBH */
    if ((ptcbhseq = cbh_getseqfunc(ptbefig, 0)) != NULL) {
        ptslave->TYPE |= CNS_SLAVE;
        ptmaster->TYPE |= CNS_MASTER;
    }
    yagCleanBehVect();
    yagCleanBehDefined();
    beh_delbefig(ptbefig, ptbefig, 'Y');

    freechain(ptconechain);

    if (ptcbhseq) {
        cbh_delseq(ptcbhseq);
        return TRUE;
    }
    else return FALSE;
}

static int
checkMasterSlaveConnection(cone_list *ptslave, cone_list *ptmaster)
{
    cone_list *ptoutput;
    edge_list *ptedge;
    chain_list *ptoutputlist = NULL;
    chain_list *ptchain;

    ptoutput = NULL;
    for (ptedge = ptmaster->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == 0) continue;
        ptoutputlist = addchain(ptoutputlist, ptedge->UEDGE.CONE);
        if ((ptedge->TYPE & CNS_LOOP) == CNS_LOOP) continue;
        /* only one non-loop output allowed */
        if (ptoutput != NULL) {
            freechain(ptoutputlist);
            return FALSE;
        }
        ptoutput = ptedge->UEDGE.CONE;
    }
    if (ptoutput == ptslave) {
        freechain(ptoutputlist);
        return TRUE;
    }
    
    for (ptchain = ptoutputlist; ptchain; ptchain = ptchain->NEXT) {
        ptoutput = (cone_list *)ptchain->DATA;
        for (ptedge = ptoutput->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & CNS_CONE) == 0) continue;
            if ((ptedge->TYPE & (CNS_LOOP|CNS_FEEDBACK)) == (CNS_LOOP|CNS_FEEDBACK)) continue;
            if (ptedge->UEDGE.CONE != ptslave) {
                freechain(ptoutputlist);
                return FALSE;
            }
        }
    }
    freechain(ptoutputlist);
    return TRUE;
}

static int
detectAutoFlipFlop(cone_list *ptcone, int fBuildBefig)
{
    cone_list *ptincone, *ptmidcone, *ptmaster;
    edge_list *ptedge, *ptmidedge;

    if (ptcone->CELLS != NULL) return FALSE;
    if ((ptcone->TYPE & CNS_LATCH) == 0) return FALSE;
    if ((ptcone->TYPE & (CNS_MASTER|CNS_SLAVE)) != 0) return FALSE;
    avt_log(LOGYAG,1, "Checking for flip-flop at '%s'\n", ptcone->NAME);

    /* Identify a pair of directly connected latches */
    ptmaster = NULL;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == 0) continue;
        if ((ptedge->TYPE & YAG_NOT_FUNCTIONAL) != 0) continue;
        if ((ptedge->TYPE & CNS_LOOP) != 0) continue;
        if ((ptedge->TYPE & CNS_COMMAND) != 0) continue;
        ptincone = ptedge->UEDGE.CONE;
        /* Only one possible data which is master */
        if (ptmaster != NULL) {
            ptmaster = NULL;
            break;
        }
        if ((ptincone->TYPE & CNS_LATCH) == CNS_LATCH) {
            ptmaster = ptincone;
        }
    }
    if (ptmaster != NULL) {
        if ((ptmaster->TYPE & (CNS_MASTER|CNS_SLAVE)) != 0) return FALSE;
        /* First check that potential master is not a slave */
        if (yagGetChain(VISIT_LIST, ptmaster) == NULL) {
            VISIT_LIST = addchain(VISIT_LIST, ptmaster);
            if (detectAutoFlipFlop(ptmaster, fBuildBefig) == TRUE) {
                VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
                return FALSE;
            }
            VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
        }
        /* OK let's check this pair */
        if (checkMasterSlaveConnection(ptcone, ptmaster)) {
            return yagCheckAutoFlipFlop(ptcone, ptmaster);
        }
        else return FALSE;
    }

    /* Identify a pair of latches separated by one cone */
    ptmidcone = NULL;
    ptmaster = NULL;
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == 0) continue;
        if ((ptedge->TYPE & YAG_NOT_FUNCTIONAL) != 0) continue;
        if ((ptedge->TYPE & CNS_LOOP) != 0) continue;
        if ((ptedge->TYPE & CNS_COMMAND) != 0) continue;
        /* only one possible midcone */
        if (ptmidcone != NULL) {
            ptmaster = NULL;
            break;
        }
        ptmidcone = ptedge->UEDGE.CONE;
        if ((ptmidcone->TYPE & CNS_LATCH) == CNS_LATCH) break;
        for (ptmidedge = ptmidcone->INCONE; ptmidedge; ptmidedge = ptmidedge->NEXT) {
            if ((ptmidedge->TYPE & CNS_CONE) == 0) continue;
            if ((ptedge->TYPE & YAG_NOT_FUNCTIONAL) != 0) continue;
            if ((ptmidedge->TYPE & CNS_LOOP) == 0) continue;
            ptincone = ptmidedge->UEDGE.CONE;
            /* Only one possible master */
            if (ptmaster != NULL) {
                ptmaster = NULL;
                break;
            }
            if ((ptincone->TYPE & CNS_LATCH) == CNS_LATCH) {
                ptmaster = ptincone;
            }
        }
        if (ptmidedge != NULL) break;
    }
    if (ptmaster != NULL) {
        if ((ptmaster->TYPE & (CNS_MASTER|CNS_SLAVE)) != 0) return FALSE;
        /* First check that potential master is not a slave */
        if (yagGetChain(VISIT_LIST, ptmaster) == NULL) {
            VISIT_LIST = addchain(VISIT_LIST, ptmaster);
            if (detectAutoFlipFlop(ptmaster, fBuildBefig) == TRUE) {
                VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
                return FALSE;
            }
            VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
        }
        /* OK let's check this pair */
        if (checkMasterSlaveConnection(ptcone, ptmaster)) {
            return yagCheckAutoFlipFlop(ptcone, ptmaster);
        }
        else return FALSE;
    }
    return FALSE;
}

void
yagAutoFlipFlop(cone_list *headcone, int fBuildBefig)
{
    cone_list *ptcone;
    
    for (ptcone = headcone; ptcone; ptcone = ptcone->NEXT) {
        VISIT_LIST = addchain(NULL, ptcone);
        detectAutoFlipFlop(ptcone, fBuildBefig);
        freechain(VISIT_LIST);
    }

}
