/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_detect.c                                                */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : ../../....     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static chain_list *yagSortBranches(chain_list *ptbranches);
static void yagPropClockLatch (cone_list *ptcone, void *ptprev);
static void yagUnmarkCommands (cone_list *ptcone);
static void yagPropNotClockLatch (cone_list *ptcone);
static void yagUnmarkNotCommands (cone_list *ptcone);
static void yagPropClockPrech(cone_list *ptcone, void *ptprev);
static void yagMarkStuck(cone_list *ptcone, pCircuit circuit, chain_list *constraint_list);
static void yagUpdateStuck(cone_list *ptcone, pCircuit circuit, chain_list *constraint_list);
static void yagPropClockGating(cone_list *ptcone, void *ptprev);
static int yagDetectConditionedState(chain_list *gateexpr, char *varname, int *ptoutput_state);

/****************************************************************************
 *                         function yagDetectBadBranches()                  *
 ****************************************************************************/

/* test for branches containing max. number of links */

void yagDetectBadBranches(cone_list *ptcone)
{
    branch_list *brlist[4];
    branch_list *ptbranch;
    int          i;

    /* clean up from previous pass */
    ptcone->TECTYPE &= ~YAG_BADCONE;
    
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if (yagCountLinks(ptbranch->LINK) == YAG_CONTEXT->YAG_MAX_LINKS) {
                ptcone->TECTYPE |= YAG_BADCONE;
            }
        }
    }
}

/****************************************************************************
 *                         function yagDetectParaTrans()                    *
 ****************************************************************************/

/* test for parallel connected transistors */

void yagDetectParaTrans(cone_list *ptcone)
{
    branch_list *brlist[4];
    ptype_list  *ptuser;
    lotrs_list  *pttrans;
    branch_list *ptbranch;
    link_list   *ptlink;
    chain_list  *paralist = NULL;
    int          i;

    /* clean up from previous pass */
    
    ptuser = getptype(ptcone->USER, CNS_PARATRANS);
    if (ptuser != NULL) {
        freechain((chain_list *)ptuser->DATA);
        ptcone->USER = delptype(ptcone->USER, CNS_PARATRANS);
    }

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_EXT) == 0) {
                    pttrans = ptlink->ULINK.LOTRS;
                    if (getptype(pttrans->USER, MBK_TRANS_PARALLEL) != NULL) {
                        if (yagGetChain(paralist, pttrans) == NULL) {
                            paralist = addchain(paralist, pttrans);
                        }
                    }
                    if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                        pttrans = (lotrs_list *)ptuser->DATA;
                        if (getptype(pttrans->USER, MBK_TRANS_PARALLEL) != NULL) {
                            if (yagGetChain(paralist, pttrans) == NULL) {
                                paralist = addchain(paralist, pttrans);
                            }
                        }
                    }
                }
            }
        }
    }
    if (paralist != NULL) ptcone->USER = addptype(ptcone->USER, CNS_PARATRANS, paralist);
}

/****************************************************************************
 *                         function yagAddSwitchInversion()                 *
 ****************************************************************************/

void yagAddSwitchInversion(cnsfig_list *ptcnsfig)
{
    lotrs_list *pttrans1, *pttrans2;
    cone_list  *ptcone1, *ptcone2;
    chain_list *ptloconlist1, *ptloconlist2;
    ptype_list *ptuser;

    for (pttrans1 = ptcnsfig->LOTRS; pttrans1; pttrans1 = pttrans1->NEXT) {
        if ((ptuser = getptype(pttrans1->USER, CNS_SWITCH)) == NULL) continue;
        pttrans2 = (lotrs_list *)ptuser->DATA;
        ptcone1 = (cone_list *)pttrans1->GRID;
        ptcone2 = (cone_list *)pttrans2->GRID;
        if (ptcone1 == NULL || ptcone2 == NULL) continue;
        if ((ptuser = getptype(ptcone1->USER, CNS_EXT)) != NULL) {
            ptloconlist1 = (chain_list *)ptuser->DATA;
        }
        else ptloconlist1 = NULL;
        if ((ptuser = getptype(ptcone2->USER, CNS_EXT)) != NULL) {
            ptloconlist2 = (chain_list *)ptuser->DATA;
        }
        else ptloconlist2 = NULL;
        if (ptloconlist1 == NULL && ptloconlist2 == NULL) {
            cnsAddConeInversion(ptcone1, ptcone2);
        }
        else if (ptloconlist1 == NULL && ptloconlist2 != NULL) {
            cnsAddConeInversion(ptcone2, ptcone1);
        }
        else if (ptloconlist1 != NULL && ptloconlist2 == NULL) {
            cnsAddConeInversion(ptcone1, ptcone2);
        }
        else { /* ptloconlist1 != NULL && ptloconlist2 != NULL */
            cnsAddConeInversion(ptcone1, ptcone2);
        }
    }
}

/****************************************************************************
 *                         function yagCorrectSwitch()                      *
 ****************************************************************************/

/* Use the PMOS in VDD branch and NMOS in VSS branch */
/* For EXT , there is one NMOS branch, and one PMOS */

void yagCorrectSwitch(cone_list *ptcone)
{
    branch_list *ptbranch;
    link_list   *ptlink;
    lotrs_list  *pttrans_n, *pttrans_p, *pttrans, *ptothertrans;
    edge_list   *ptedge, *ptotheredge;
    ptype_list  *ptuser;
    chain_list  *ptchain1, *ptchain2;
    long         inputtype;
    int          changed = FALSE;

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) {
                pttrans_n = ptlink->ULINK.LOTRS;
                pttrans_p = (lotrs_list *)getptype(pttrans_n->USER, CNS_SWITCH)->DATA;
                ptlink->ULINK.LOTRS = pttrans_p;
                ptlink->TYPE&=~CNS_TNLINK;
                ptlink->TYPE|=CNS_TPLINK;
                changed = TRUE;
            }
        }
    }
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) break;
        }
        if (ptlink != NULL) {
            ptcone->BREXT = yagCopyBranch(ptcone->BREXT, ptbranch);
            ptbranch->TYPE |= CNS_NOT_UP;
            if ((ptbranch->TYPE & (CNS_NOT_UP|CNS_NOT_DOWN)) == (CNS_NOT_UP|CNS_NOT_DOWN)) {
                ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                ptbranch->TYPE &= ~(CNS_NOT_UP|CNS_NOT_DOWN);
            }
            ptcone->BREXT->TYPE |= CNS_NOT_DOWN;
            if ((ptcone->BREXT->TYPE & (CNS_NOT_UP|CNS_NOT_DOWN)) == (CNS_NOT_UP|CNS_NOT_DOWN)) {
                ptcone->BREXT->TYPE |= CNS_NOT_FUNCTIONAL;
                ptcone->BREXT->TYPE &= ~(CNS_NOT_UP|CNS_NOT_DOWN);
            }
            for (ptlink = ptcone->BREXT->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) {
                    pttrans_n = ptlink->ULINK.LOTRS;
                    pttrans_p = (lotrs_list *)getptype(pttrans_n->USER, CNS_SWITCH)->DATA;
                    ptlink->ULINK.LOTRS = pttrans_p;
                    ptlink->TYPE&=~CNS_TNLINK;
                    ptlink->TYPE|=CNS_TPLINK;
                    changed = TRUE;
                }
            }
        }
    }
    if (changed) yagChainCone(ptcone);
    
    /* Check incone list to see that all switch inputs exist, add if necessary, and synchronise input markings */
    
    ptuser = getptype(ptcone->USER, CNS_SWITCH);
    if (ptuser) {
        for (ptchain1 = (chain_list *)ptuser->DATA; ptchain1; ptchain1 = ptchain1->NEXT) {
            for (ptchain2 = (chain_list *)ptchain1->DATA; ptchain2; ptchain2 = ptchain2->NEXT) {
                pttrans = (lotrs_list *)ptchain2->DATA;
                ptedge = yagGetEdge(ptcone->INCONE, pttrans->GRID);
                ptothertrans = (lotrs_list *)getptype(pttrans->USER, CNS_SWITCH)->DATA;
                ptotheredge = yagGetEdge(ptcone->INCONE, ptothertrans->GRID);
                if (ptedge == NULL) {
                    if (ptotheredge) inputtype = ptotheredge->TYPE;
                    else inputtype = CNS_CONE;
                    addincone(ptcone, inputtype, pttrans->GRID);
                }
                else {
                    if (ptotheredge && (ptotheredge->TYPE & CNS_HZCOM) == CNS_HZCOM) ptedge->TYPE |= CNS_HZCOM;
                }
            }
        }
    }
}

/****************************************************************************
 *                         function yagDetectSwitch()                       *
 ****************************************************************************/

/* test for a CMOS switch but do not test for gate signal inversion */

void yagDetectSwitch(cone_list *ptcone)
{
    branch_list *brlist[4];
    ptype_list  *ptuser;
    chain_list  *ptchain, *testchain = NULL;
    lotrs_list  *pttrans_n, *pttrans_p;
    branch_list *ptbranch;
    link_list   *ptlink;
    chain_list  *switchpair, *switchlist = NULL;
    int          i;

    /* clean up from previous pass */
    
    ptuser = getptype(ptcone->USER, CNS_SWITCH);
    if (ptuser != NULL) {
        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
            freechain((chain_list *)ptchain->DATA);
        }
        freechain((chain_list *)ptuser->DATA);
        ptcone->USER = delptype(ptcone->USER, CNS_SWITCH);
    }

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) {
                    pttrans_n = ptlink->ULINK.LOTRS;
                    if (yagGetChain(testchain, pttrans_n) == NULL) {
                        pttrans_p = (lotrs_list *)getptype(pttrans_n->USER, CNS_SWITCH)->DATA;
                        testchain = addchain(testchain, pttrans_n);
                        switchpair = addchain(NULL, pttrans_n);
                        switchpair = addchain(switchpair, pttrans_p);
                        switchlist = addchain(switchlist, switchpair);
                    }
                }
            }
        }
    }
    freechain(testchain);
    if (switchlist) ptcone->USER = addptype(ptcone->USER, CNS_SWITCH, switchlist);
}

/****************************************************************************
 *                   function yagDetectParallelBranches()                   *
 ****************************************************************************/

/* test for branches which have identical inputs */

void
yagDetectParallelBranches(cone_list *ptcone)
{
    branch_list *ptbranch;
    chain_list  *ptchain = NULL;
    chain_list  *ptchain0;
    chain_list  *sortedchain;
    ptype_list  *ptuser;

    /* clean up from previous pass */
    
    ptuser = getptype(ptcone->USER, CNS_PARALLEL);
    if (ptuser != NULL) {
        for (ptchain0 = (chain_list *)ptuser->DATA; ptchain0; ptchain0 = ptchain0->NEXT) {
            freechain((chain_list *)ptchain0->DATA);
        }
        freechain((chain_list *)ptuser->DATA);
        ptcone->USER = delptype(ptcone->USER, CNS_PARALLEL);
    }

    ptchain = parabrs(ptcone->BRVDD);
    if (YAG_CONTEXT->YAG_MINIMISE_CONES && ptchain != NULL) {
        for (ptchain0 = ptchain; ptchain0; ptchain0 = ptchain0->NEXT) {
            freechain((chain_list *)ptchain0->DATA);
        }
        freechain(ptchain);
        ptchain = NULL;
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS) {
                yagDestroyBranch(ptcone, ptbranch);
            }
            else ptbranch->TYPE &= ~CNS_PARALLEL;
        }
    }

    ptchain = append(ptchain, parabrs(ptcone->BRVSS));
    if (YAG_CONTEXT->YAG_MINIMISE_CONES && ptchain != NULL) {
        for (ptchain0 = ptchain; ptchain0; ptchain0 = ptchain0->NEXT) {
            freechain((chain_list *)ptchain0->DATA);
        }
        freechain(ptchain);
        ptchain = NULL;
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS) {
                yagDestroyBranch(ptcone, ptbranch);
            }
            else ptbranch->TYPE &= ~CNS_PARALLEL;
        }
    }

    ptchain = append(ptchain, parabrs(ptcone->BREXT));
    if (YAG_CONTEXT->YAG_MINIMISE_CONES && ptchain != NULL) {
        for (ptchain0 = ptchain; ptchain0; ptchain0 = ptchain0->NEXT) {
            freechain((chain_list *)ptchain0->DATA);
        }
        freechain(ptchain);
        ptchain = NULL;
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_PARALLEL_INS) == CNS_PARALLEL_INS) {
                yagDestroyBranch(ptcone, ptbranch);
            }
            else ptbranch->TYPE &= ~CNS_PARALLEL;
        }
    }

    /* sort each list of parallel branches so the the head of the list  */
    /* is the least resistive branch so as to improve the approximation */
    /* due to the equivalent transistors generated for the timing.      */ 
    for (ptchain0 = ptchain; ptchain0; ptchain0 = ptchain0->NEXT) {
        sortedchain = yagSortBranches((chain_list *)ptchain0->DATA);
        freechain((chain_list *)ptchain0->DATA);
        ptchain0->DATA = sortedchain;
    }

    if (ptchain != NULL) {
        ptcone->USER = addptype(ptcone->USER, CNS_PARALLEL, ptchain);
    }
}

static int yagBranchCompare (const void *elem1, const void *elem2)
{
   float res1, res2;

   res1 = yagCalcBranchResistance(*(branch_list **)elem1);
   res2 = yagCalcBranchResistance(*(branch_list **)elem2);

   if (res1 < res2) return 1;
   else if (res2 > res1) return -1;
   
   return 0;
}


static chain_list *
yagSortBranches(chain_list *ptbranches)
{
    branch_list **branchtable;
    branch_list *ptbranch;
    chain_list *ptchain;
    chain_list *reschain = NULL;
    int numbranches, i;
    
    numbranches = yagCountChains(ptbranches);
    branchtable = (branch_list **)mbkalloc(numbranches * sizeof(branch_list *));
    i = 0;
    for (ptchain = ptbranches; ptchain; ptchain = ptchain->NEXT) {
        ptbranch = (branch_list *)ptchain->DATA;
        branchtable[i++] = ptbranch;
    }
    qsort(branchtable, numbranches, sizeof(branch_list *), yagBranchCompare);
    for (i = 0; i < numbranches; i++) reschain = addchain(reschain, branchtable[i]);

    mbkfree(branchtable);
    return reschain;
}

/****************************************************************************
 *                         function yagDetectPullup()                       *
 ****************************************************************************/

/* mark strongly resistive VDD branches (resistance > 10 x max. down resistance */

void yagDetectPullup(cone_list *ptcone)
{
    branch_list *ptbranch;
    link_list   *ptlink;
    float        downres, branchres;
    int          donecalc = FALSE;

    for (ptbranch=ptcone->BRVDD; ptbranch; ptbranch=ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_RESIST) == 0) break;
        }
        if (ptlink != NULL) continue;
        if (!donecalc) {
            downres = yagCalcMaxDownRes(ptcone);
            donecalc = TRUE;
        }
        branchres = yagCalcBranchResistance(ptbranch);
        if ((downres != 0.0 && branchres >= V_FLOAT_TAB[__YAGLE_PULL_RATIO].VALUE * downres)
        || (downres == 0.0 && branchres >= 1.5 && (ptcone->TYPE & CNS_EXT) == CNS_EXT)) {
            ptbranch->TYPE |= CNS_RESBRANCH;
        }
    }
}

/****************************************************************************
 *                         function yagDetectPulldown()                     *
 ****************************************************************************/

/* mark strongly resistive VSS branches (resistance > 10 x max. up resistance */

void
yagDetectPulldown(cone_list *ptcone)
{
    branch_list *ptbranch;
    link_list   *ptlink;
    float        upres, branchres;
    int          donecalc = FALSE;

    for (ptbranch=ptcone->BRVSS; ptbranch; ptbranch=ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_RESIST) == 0) break;
        }
        if (ptlink != NULL) continue;
        if (!donecalc) {
            upres = yagCalcMaxUpRes(ptcone);
            donecalc = TRUE;
        }
        branchres = yagCalcBranchResistance(ptbranch);
        if ((upres != 0.0 && branchres >= V_FLOAT_TAB[__YAGLE_PULL_RATIO].VALUE * upres)
        || (upres == 0.0 && branchres >= 1.5 && (ptcone->TYPE & CNS_EXT) == CNS_EXT)) {
            ptbranch->TYPE |= CNS_RESBRANCH;
        }
    }
}

/****************************************************************************
 *                         function yagDetectLoop()                         *
 ****************************************************************************/

void
yagDetectLoops(cone_list *ptcone)
{
    edge_list  *ptinput;
    edge_list  *ptloopin;
    edge_list  *ptoutput;
    edge_list  *ptloopout;
    cone_list  *ptincone;
    
    for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
        if ((ptinput->TYPE & CNS_EXT) == CNS_EXT) continue;
        if ((ptinput->TYPE & YAG_MARK) != 0) {
            ptinput->TYPE &= ~YAG_MARK;
            continue;
        }
        ptincone = ptinput->UEDGE.CONE;
        if ((ptloopin = yagGetEdge(ptincone->INCONE, ptcone)) != NULL) {
            ptinput->TYPE |= CNS_LOOP;
            ptloopin->TYPE |= CNS_LOOP;
            ptoutput = yagGetEdge(ptcone->OUTCONE, ptincone);
            ptoutput->TYPE |= CNS_LOOP;
            ptloopout = yagGetEdge(ptincone->OUTCONE, ptcone);
            ptloopout->TYPE |= CNS_LOOP;
            if ((ptinput->TYPE & CNS_BLEEDER) != CNS_BLEEDER
            && (ptloopin->TYPE & CNS_BLEEDER) != CNS_BLEEDER
            && (ptinput->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK
            && (ptloopin->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK) {
                ptloopin->TYPE |= YAG_MARK;
            }
        }
    }
}

int
yagCountLoops(cone_list *ptcone, int warning)
{
    edge_list  *ptinput;
    edge_list  *ptloopin;
    edge_list  *ptoutput;
    edge_list  *ptloopout;
    cone_list  *ptincone;
    int         numloops;
    
    numloops = 0;
    if ((ptcone->TYPE & CNS_RS) == CNS_RS) return 0;
    for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
        if ((ptinput->TYPE & CNS_EXT) == CNS_EXT) continue;
        if (warning && (ptinput->TYPE & YAG_MARK) != 0) {
            ptinput->TYPE &= ~YAG_MARK;
            continue;
        }
        ptincone = ptinput->UEDGE.CONE;
        if ((ptloopin = yagGetEdge(ptincone->INCONE, ptcone)) != NULL) {
            ptoutput = yagGetEdge(ptcone->OUTCONE, ptincone);
            ptloopout = yagGetEdge(ptincone->OUTCONE, ptcone);
            if ((ptinput->TYPE & CNS_BLEEDER) != CNS_BLEEDER
            && (ptloopin->TYPE & CNS_BLEEDER) != CNS_BLEEDER
            && (ptinput->TYPE & CNS_MEMSYM) != CNS_MEMSYM
            && (ptloopin->TYPE & CNS_MEMSYM) != CNS_MEMSYM
            && (ptinput->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK
            && (ptloopin->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK) {
                if (warning) ptloopin->TYPE |= YAG_MARK;
                if (warning) {
                    if ((ptcone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0 || (ptincone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0) {
                        yagWarning(WAR_STUCK_LOOP, ptcone->NAME, ptincone->NAME, NULL, 0);
                    }
                    else yagWarning(WAR_UNKNOWN_LOOP, ptcone->NAME, ptincone->NAME, NULL, 0);
                }
                numloops++;
            }
        }
    }
    return numloops;
}

/****************************************************************************
 *                         function yagDetectTransfer()                     *
 ****************************************************************************/

/* test for transfer PTYPE on the cone transistors */

int
yagDetectTransfer(cone_list *ptcone)
{
     lotrs_list     *pttrans;
     branch_list    *brlist[4];
     branch_list    *ptbranch;
     link_list      *ptlink;
     edge_list      *ptedge;
     ptype_list     *ptuser;
     chain_list     *bleeders = NULL;
     long            fcltype;
     int             bleeder, not_func, feedback;
     int             done_bleeders;
     int             change_chain = FALSE;
     int             i;

     if (getptype(ptcone->USER, CNS_BLEEDER) == NULL) done_bleeders = FALSE;
     else done_bleeders = TRUE;

     brlist[0] = ptcone->BRVDD;
     brlist[1] = ptcone->BRVSS;
     brlist[2] = ptcone->BREXT;
     brlist[3] = ptcone->BRGND;

     for (i=0; i<4; i++) {
         for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
             if ((ptbranch->TYPE & CNS_EXT) != 0 && ptbranch->LINK->NEXT == NULL) continue;
             bleeder = FALSE;
             not_func = TRUE;
             feedback = FALSE;
             for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                 if ((ptlink->TYPE & CNS_EXT) != CNS_EXT) {
                     pttrans = ptlink->ULINK.LOTRS;
                     if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                         ptedge = yagGetEdge(ptcone->INCONE, pttrans->GRID);
                         if (ptedge == NULL) continue;
                         fcltype = (long)ptuser->DATA;

                         if ((fcltype & FCL_BLEEDER) != 0) {
                             ptedge->TYPE |= CNS_BLEEDER;
                             bleeder = TRUE;
                         }
                         if ((fcltype & FCL_FEEDBACK) != 0) {
                             ptedge->TYPE |= CNS_FEEDBACK;
                             feedback = TRUE;
                         }
                         if ((fcltype & FCL_COMMAND) != 0) {
                             ptlink->TYPE |= CNS_COMMAND;
                             ptedge->TYPE |= CNS_COMMAND;
                         }
                         if ((fcltype & FCL_NOT_FUNCTIONAL) == 0)
                             not_func = FALSE;
                         if ((fcltype & FCL_SHORT) != 0) {
                             ptlink->TYPE |= CNS_SHORT;
                             ptlink->TYPE |= CNS_RESIST;
                             change_chain = TRUE;
                         }
                     }
                     else {
                         not_func = FALSE;
                     }
                 }
             }
             if (bleeder) {
                 ptbranch->TYPE |= CNS_BLEEDER|CNS_NOT_FUNCTIONAL;
                 if (!done_bleeders) bleeders = addchain(bleeders, ptbranch);
             }
             if (not_func) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
             if (feedback) ptbranch->TYPE |= CNS_FEEDBACK;
         }
    }
    if (!done_bleeders && bleeders != NULL) {
        ptcone->USER = addptype(ptcone->USER, CNS_BLEEDER, bleeders);
    }
    return change_chain;
}

/****************************************************************************
 *                         function yagDetectDegraded()                     *
 ****************************************************************************/

/* test for degraded branches */

void
yagDetectDegraded(cone_list *ptcone)
{
    branch_list    *ptbranch;
    branch_list    *brlist[4];
    link_list      *ptlink;
    long            branchtype;
    long            linktype;
    int             i;
    int             has_degraded_vdd = FALSE, has_degraded_vss = FALSE;
    int             bleeder_up = FALSE, bleeder_down = FALSE;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    ptcone->TECTYPE &= ~(CNS_VDD_DEGRADED|CNS_VSS_DEGRADED);
    ptcone->TECTYPE &= ~(CNS_CMOS);

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            branchtype = ptbranch->TYPE & (CNS_VSS|CNS_VDD|CNS_EXT);
            if ((ptbranch->TYPE & CNS_BLEEDER) != 0) {
                if (branchtype == CNS_VDD) bleeder_up = TRUE;
                if (branchtype == CNS_VSS) bleeder_down = TRUE;
            }
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            ptbranch->TYPE &= ~(CNS_VDD_DEGRADED|CNS_VSS_DEGRADED);
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) continue;

                linktype = ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK|CNS_IN|CNS_INOUT);

                if (linktype == CNS_TNLINK || linktype == CNS_TPLINK) {
                    if (linktype == CNS_TNLINK && (branchtype == CNS_VDD || branchtype == CNS_EXT)) {
                        ptbranch->TYPE |= CNS_VDD_DEGRADED;
                        if ((ptbranch->TYPE & CNS_NOT_UP) == 0) has_degraded_vdd = TRUE;
                    }
                    else if (linktype == CNS_TPLINK && (branchtype == CNS_VSS || branchtype == CNS_EXT)) {
                        ptbranch->TYPE |= CNS_VSS_DEGRADED;
                        if ((ptbranch->TYPE & CNS_NOT_DOWN) == 0) has_degraded_vss = TRUE;
                    }
                }
            }
        }
    }
    if ((ptcone->TYPE & CNS_LATCH) == 0) {
        if (has_degraded_vdd && !bleeder_up) ptcone->TECTYPE |= CNS_VDD_DEGRADED;
        if (has_degraded_vss && !bleeder_down) ptcone->TECTYPE |= CNS_VSS_DEGRADED;
    }
    if ((ptcone->TECTYPE & (CNS_VDD_DEGRADED|CNS_VSS_DEGRADED)) == 0) {
        ptcone->TECTYPE |= CNS_CMOS;
    }
}

/****************************************************************************
 *              Functions to update FALSECONF status                        *
 ****************************************************************************/

int yagDetectFalseConf(cone_list *ptcone)
{
    branch_list *ptbranch;
    branch_list *brlist[4];
    losig_list  *ptconesig;
    int         i, changed = FALSE;

    if ((ptcone->TYPE & YAG_FALSECONF) == 0) return 0;
    ptconesig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
    ptcone->TYPE &= ~YAG_FALSECONF;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & YAG_FALSECONF) != 0) {
                if (yagCheckFalseConfBranch(ptbranch, ptconesig)) {
                    ptcone->TYPE |= YAG_FALSECONF;
                }
                else changed = TRUE;
            }
        }
    }
    return changed;
}

int
yagCheckFalseConfBranch(branch_list *ptbranch, losig_list *ptfirstsig)
{
    link_list       *ptlink;
    losig_list      *ptlastsig, *ptnextsig;
    cone_list       *ptsigcone;
    ptype_list      *ptuser;

    ptbranch->TYPE &= ~YAG_FALSECONF;
    ptlastsig = ptfirstsig;

    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK)) == 0) continue;
        if ((ptnextsig = ptlink->ULINK.LOTRS->DRAIN->SIG) == ptlastsig) {
            ptnextsig = ptlink->ULINK.LOTRS->SOURCE->SIG;
        }
        ptlastsig = ptnextsig;
        ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE);
        if (ptuser != NULL) {
            ptsigcone = (cone_list *)ptuser->DATA;
            if ((ptsigcone->TYPE & YAG_LOOPCONF) != 0) break;
        }
    }
    if (ptlink != NULL) {
        ptbranch->TYPE |= YAG_FALSECONF;
        return TRUE;
    }
    return FALSE;
}

void
yagDetectLoopConf(cone_list *ptcone)
{
    edge_list  *ptedgelist0, *ptedgelist1;
    edge_list  *ptedge0, *ptedge1;
    cone_list  *ptincone;

    ptcone->TYPE &= ~YAG_LOOPCONF;
    
    ptedgelist0 = ptcone->INCONE;
    for (ptedge0 = ptedgelist0; ptedge0; ptedge0 = ptedge0->NEXT) {
        if ((ptedge0->TYPE & CNS_EXT) != 0) continue;
        if ((ptedge0->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptedge0->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
        ptincone = ptedge0->UEDGE.CONE;
        if ((ptincone->TYPE & YAG_PARTIAL) != 0) {
            ptedgelist1 = (edge_list *)getptype(ptincone->USER, YAG_INPUTS_PTYPE)->DATA;
        }
        else ptedgelist1 = ptincone->INCONE;
        ptedge1 = yagGetEdge(ptedgelist1, ptcone);
        if (ptedge1 != NULL) {
            if ((ptedge1->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
            if ((ptedge1->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
            ptcone->TYPE |= YAG_LOOPCONF;
            break;
        }
    }
}  

/****************************************************************************
 *                function yagRemoveRedundantBranches()                     *
 ****************************************************************************/

/* test for redundant branches */

void
yagRemoveRedundantBranches(cone_list *ptcone)
{
    branch_list    *ptbranch;
    branch_list    *ptnextbranch;
    branch_list    *brlist[4];
    int             changes = FALSE;
    int             i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    if ((ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) != 0) return;
    if ((ptcone->TYPE & YAG_HASDUAL) == 0) return;
    if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_FLIP_FLOP|CNS_MASTER|CNS_SLAVE|CNS_BLEEDER)) != 0) return;
    
    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptnextbranch) {
            ptnextbranch = ptbranch->NEXT;
            if ((ptbranch->TYPE & YAG_DUALBRANCH) == 0) {
                yagDestroyBranch(ptcone, ptbranch);
                changes = TRUE;
            }
        }
    }
    if (changes) {
        yagChainCone(ptcone);
        yagDetectParallelBranches(ptcone);
        yagDetectSwitch(ptcone);
        yagDetectDegraded(ptcone);
        ptcone->TECTYPE |= CNS_DUAL_CMOS;
    }
}

/****************************************************************************
 *                         function yagTransferParallel()                   *
 ****************************************************************************/

/* test for transfer PTYPE on parallel transistors */
/* (to be called before parallel restoration) */

void
yagTransferParallel(lofig_list *ptlofig)
{
    long transtype;
    lotrs_list *ptlotrs, *ptparatrans;
    ptype_list *ptuser;
    chain_list *transchain, *ptchain;
    
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            transtype = (long)ptuser->DATA;
            if ((ptuser = getptype(ptlotrs->USER, MBK_TRANS_PARALLEL)) != NULL) {
                transchain = (chain_list *)ptuser->DATA;
                for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
                    ptparatrans = (lotrs_list *)ptchain->DATA;
                    if (ptparatrans == ptlotrs) continue;
                    if ((ptuser = getptype(ptparatrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)(transtype | (long)ptuser->DATA);
                    }
                    else ptparatrans->USER = addptype(ptparatrans->USER, FCL_TRANSFER_PTYPE, (void *)transtype);
                }
            }
        }
    }
}

/****************************************************************************
 *                         function yagDetectGlitcher()                     *
 ****************************************************************************/

/* detect two transistor branches which serve no purpose */
/* unless considered to be dynamically active */

static int
unconnectedcommon(lotrs_list *pttrans1, lotrs_list *pttrans2)
{
    losig_list        *ptcommonsig = NULL;
    chain_list        *loconchain;

    if (pttrans1->DRAIN->SIG == pttrans2->DRAIN->SIG) ptcommonsig = pttrans1->DRAIN->SIG;
    else if (pttrans1->SOURCE->SIG == pttrans2->SOURCE->SIG) ptcommonsig = pttrans1->SOURCE->SIG;
    else if (pttrans1->DRAIN->SIG == pttrans2->SOURCE->SIG) ptcommonsig = pttrans1->DRAIN->SIG;
    else if (pttrans1->SOURCE->SIG == pttrans2->DRAIN->SIG) ptcommonsig = pttrans1->SOURCE->SIG;
    if (ptcommonsig) {
        loconchain = (chain_list *)getptype(ptcommonsig->USER, LOFIGCHAIN)->DATA;
        if (yagCountChains(loconchain) == 2) return TRUE;
    }
    return FALSE;
}    

int
yagDetectGlitcher(branch_list *ptbranch, lotrs_list *pttrans, int mark)
{
    lotrs_list        *ptprevtrans;
    cone_list         *ptcone;
    link_list         *ptlink;
    int                numlinks;
    
    numlinks = yagCountLinks(ptbranch->LINK);
    if ((ptbranch->TYPE & CNS_EXT) != 0) numlinks--;
    if (pttrans != NULL) {
        if (numlinks > V_INT_TAB[__YAGLE_GLITCH_LINKS].VALUE - 1 || numlinks < 1) return FALSE;
        for (ptlink = ptbranch->LINK; ptlink->NEXT; ptlink = ptlink->NEXT);
        ptprevtrans = ptlink->ULINK.LOTRS;
    }
    else {
        if (numlinks > V_INT_TAB[__YAGLE_GLITCH_LINKS].VALUE || numlinks < 2) return FALSE;
        pttrans = ptbranch->LINK->ULINK.LOTRS;
        for (ptlink = ptbranch->LINK->NEXT; ptlink; ptlink = ptlink->NEXT) {
            if ((ptbranch->TYPE & CNS_EXT) != 0 && !ptlink->NEXT) break; 
            ptprevtrans = pttrans;
            pttrans = ptlink->ULINK.LOTRS;
        }
    }
    if (!unconnectedcommon(pttrans, ptprevtrans)) return FALSE;
    
    if (mark) {
        ptcone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
        ptcone->TYPE |= YAG_STOP;
        ptcone = (cone_list *)getptype(ptprevtrans->USER, CNS_DRIVINGCONE)->DATA;
        ptcone->TYPE |= YAG_STOP;
    }
    return TRUE;
}

/****************************************************************************
 *                         function yagDetectClockLatch()                     *
 ****************************************************************************/

/* unmark latches with no command on a clock path */

void
yagDetectClockLatch(inffig_list *ifl, cnsfig_list *ptcnsfig)
{
    locon_list     *ptlocon;
    cone_list      *ptcone;
    cone_list      *ptextcone;
    edge_list      *ptout;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    int             real_latch;
    int             count;
    chain_list     *cklatchlist = NULL;

    if (YAG_CONTEXT->YAG_LATCH_REQUIRE_CLOCK) {
        cklatchlist = inf_GetEntriesByType(ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
        cklatchlist = append(cklatchlist, inf_GetEntriesByType(ifl, INF_CKLATCH, INF_YES));
    }
    else if ((cklatchlist = inf_GetEntriesByType(ifl, INF_CKLATCH, INF_YES))==NULL) return;
    
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_MARK;
    }
    count = 0;
    /* Propagate from input connectors */
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (yagGetChain(cklatchlist, ptlocon->NAME) != NULL) {
            count++;
            ptuser = getptype(ptlocon->USER, CNS_EXT);
            if (ptuser != NULL) ptextcone = (cone_list *)ptuser->DATA;
            for (ptout = ptextcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                yagPropClockLatch(ptout->UEDGE.CONE, ptextcone);
            }
            ptuser = getptype(ptlocon->USER, CNS_CONE);
            if (ptuser != NULL) {
                for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptcone = (cone_list *)ptchain->DATA;
                    if (ptcone != ptextcone) yagPropClockLatch(ptcone, ptlocon);
                }
            }
        }   
    }
    /* Propagate from internal cones */
    if (count != yagCountChains(cklatchlist)) {
        for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
            if (yagGetChain(cklatchlist, ptcone->NAME) != NULL) {
                for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                    if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                    yagPropClockLatch(ptout->UEDGE.CONE, ptcone);
                }
            }
        }
    }
    /* Perform the unmarking */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        real_latch = FALSE;
        ptcone->TYPE &= ~YAG_VISITED;
        if ((ptcone->TYPE & YAG_MARK) == YAG_MARK) {
            real_latch = TRUE;
            ptcone->TYPE &= ~YAG_MARK;
        }
        if ((ptcone->TYPE & (CNS_FLIP_FLOP|CNS_MEMSYM)) != 0) continue;
        if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
            if ((ptcone->TYPE & CNS_TRI) == CNS_TRI
            || (ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
                continue;
            }
        }
        if (!real_latch && (ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
            yagUnmarkLatch(ptcone, TRUE, TRUE);
            yagWarning(WAR_UNMARKEDLATCH_LOOP, NULL, ptcone->NAME, (char *)ptcone->INDEX, 0);
        }
        else if (YAG_CONTEXT->YAG_STRICT_CKLATCH && real_latch && (ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
            yagUnmarkCommands(ptcone);
        }
    }

    freechain(cklatchlist);
}

static void
yagPropClockLatch(cone_list *ptcone, void *ptprev)
{
    edge_list      *ptout, *ptin;
    
    if ((ptcone->TYPE & (CNS_FLIP_FLOP|CNS_MEMSYM)) != 0) return;
    if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
        if ((ptcone->TYPE & CNS_TRI) == CNS_TRI
        || (ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
            return;
        }
    }
    if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
        for (ptin = ptcone->INCONE; ptin; ptin = ptin->NEXT) {
            if (ptin->UEDGE.PTR == ptprev) break;
        }
        if (ptin != NULL && (ptin->TYPE & CNS_COMMAND) != CNS_COMMAND) {
            /* PROPAGATE THROUGH */
        }
        else {
            ptin->TYPE |= YAG_MARK;
            ptcone->TYPE |= YAG_MARK;
            return;
        }
    }
    if ((ptcone->TYPE & YAG_VISITED) == YAG_VISITED) return;
    ptcone->TYPE |= YAG_VISITED;
    for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
        if ((ptout->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
        if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
        yagPropClockLatch(ptout->UEDGE.CONE, ptcone);
    }
}

void
yagUnmarkLatch(cone_list *ptcone, int cutloop, int warn)
{
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    edge_list      *ptinedge;
    cone_list      *ptloopcone;
    chain_list     *new_bleeders = NULL;
    ptype_list     *ptuser;
    int             i;

    if ((ptcone->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
        if ((ptuser = getptype(ptcone->USER, YAG_MEMORY_PTYPE)) != NULL) {
            ptloopcone = (cone_list *)ptuser->DATA;
            ptcone->USER = delptype(ptcone->USER, YAG_MEMORY_PTYPE);
            ptloopcone->USER = delptype(ptloopcone->USER, YAG_MEMORY_PTYPE);
            yagUnmarkLatch(ptloopcone, cutloop, warn);
        }
    }

    if ((ptcone->TYPE & CNS_RS) != 0) {
        if ((ptuser = getptype(ptcone->USER, YAG_BISTABLE_PTYPE)) != NULL) {
            ptloopcone = (cone_list *)ptuser->DATA;
            ptcone->USER = delptype(ptcone->USER, YAG_BISTABLE_PTYPE);
            ptloopcone->USER = delptype(ptloopcone->USER, YAG_BISTABLE_PTYPE);
            yagUnmarkLatch(ptloopcone, cutloop, warn);
        }
        cutloop = FALSE;
    }
    ptcone->TYPE &= ~(CNS_LATCH|CNS_RS|CNS_MASTER|CNS_SLAVE|CNS_FLIP_FLOP|CNS_MEMSYM|YAG_AUTOLATCH);
    ptcone->TECTYPE &= ~(CNS_NAND|CNS_NOR);
    
    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        ptinedge->TYPE &= ~CNS_COMMAND;
        ptinedge->TYPE &= ~CNS_MEMSYM;
        ptinedge->TYPE &= ~CNS_ASYNC;
        if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0) ptinedge->TYPE &= ~CNS_BLEEDER;
        if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) {
            ptinedge->TYPE &= ~CNS_FEEDBACK;
            if (cutloop) ptinedge->TYPE |= CNS_BLEEDER;
        }
    }
    
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & YAG_LATCH_NF) == YAG_LATCH_NF) ptbranch->TYPE &= ~(CNS_NOT_FUNCTIONAL|YAG_LATCH_NF);
            if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0) {
                ptbranch->TYPE &= ~CNS_NOT_FUNCTIONAL;
                ptbranch->TYPE &= ~CNS_BLEEDER;
            }
            if ((ptbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) {
                ptbranch->TYPE &= ~CNS_FEEDBACK;
                ptbranch->TYPE &= ~CNS_NOTCONFLICTUAL;
                if (cutloop) {
                    ptbranch->TYPE |= CNS_BLEEDER;
                    new_bleeders = addchain(new_bleeders, ptbranch);
                }
                else ptbranch->TYPE &= ~CNS_NOT_FUNCTIONAL;
            }
            ptbranch->TYPE &= ~YAG_ASYNC;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                ptlink->TYPE &= ~CNS_COMMAND;
                ptlink->TYPE &= ~CNS_ASYNCLINK;
            }
        }
    }
    if (new_bleeders != NULL) {
        ptuser = getptype(ptcone->USER, CNS_BLEEDER);
        if (ptuser != NULL) {
            ptuser->DATA = append(ptuser->DATA, new_bleeders);
        }
        else ptcone->USER = addptype(ptcone->USER, CNS_BLEEDER, (void *)new_bleeders);
    }
    if (!new_bleeders && (ptcone->TECTYPE & YAG_LEVELHOLD) != 0) {
        ptuser = getptype(ptcone->USER, CNS_BLEEDER);
        if (ptuser) {
            freechain((chain_list *)ptuser->DATA);
            ptcone->USER = delptype(ptcone->USER, CNS_BLEEDER);
        }
    }
    /* remove stuck for latches but not LHs */
    if ((ptcone->TECTYPE & YAG_LEVELHOLD) == 0) {
        ptcone->TECTYPE &= ~(CNS_ONE|CNS_ZERO);
    }
    ptcone->TECTYPE &= ~YAG_LEVELHOLD;
    if (warn) yagWarning(WAR_UNMARKEDLATCH_LOOP, NULL, ptcone->NAME, (char *)ptcone->INDEX, 0);
}

static void
yagUnmarkCommands(cone_list *ptcone)
{
    cone_list      *ptdrivingcone;
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    edge_list      *ptinedge;
    ptype_list     *ptuser;
    int             i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                if ((ptlink->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
                    if (ptuser == NULL) continue;
                    ptdrivingcone = (cone_list *)ptuser->DATA;
                    ptinedge = yagGetEdge(ptcone->INCONE, ptdrivingcone);
                    if (ptinedge != NULL && (ptinedge->TYPE & YAG_MARK) == 0) {
                        ptlink->TYPE &= ~CNS_COMMAND;
                    }
                }
            }
        }
    }

    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        if ((ptinedge->TYPE & YAG_MARK) == 0) {
            ptinedge->TYPE &= ~CNS_COMMAND;
        }
        else {
            ptinedge->TYPE &= ~YAG_MARK;
        }
    }
}

/****************************************************************************
 *                         function yagDetectNotClockLatch()                *
 ****************************************************************************/

/* unmark latch commands on a not-clock path */

void
yagDetectNotClockLatch(inffig_list *ifl, cnsfig_list *ptcnsfig)
{
    locon_list     *ptlocon;
    cone_list      *ptcone;
    cone_list      *ptextcone;
    edge_list      *ptout;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    int             count;
    chain_list     *notcklatchlist;

    if ((notcklatchlist=inf_GetEntriesByType(ifl, INF_CKLATCH, INF_NO))==NULL) return;

    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_MARK;
    }
    count = 0;
    /* Propagate from input connectors */
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (yagGetChain(notcklatchlist, ptlocon->NAME) != NULL) {
            count++;
            ptuser = getptype(ptlocon->USER, CNS_EXT);
            if (ptuser != NULL) ptextcone = (cone_list *)ptuser->DATA;
            for (ptout = ptextcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                yagPropNotClockLatch(ptout->UEDGE.CONE);
            }
            ptuser = getptype(ptlocon->USER, CNS_CONE);
            if (ptuser != NULL) {
                for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptcone = (cone_list *)ptchain->DATA;
                    if (ptcone != ptextcone) yagPropNotClockLatch(ptcone);
                }
            }
        }   
    }
    /* Propagate from internal cones */
    if (count != yagCountChains(notcklatchlist)) {
        for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
            if (yagGetChain(notcklatchlist, ptcone->NAME) != NULL) {
                for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                    if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                    yagPropNotClockLatch(ptout->UEDGE.CONE);
                }
            }
        }
    }
    /* Perform the unmarking */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_VISITED;
        if ((ptcone->TYPE & YAG_MARK) == YAG_MARK) {
            ptcone->TYPE &= ~YAG_MARK;
            yagUnmarkNotCommands(ptcone);
        }
    }

    freechain(notcklatchlist);
}

static void
yagPropNotClockLatch(cone_list *ptcone)
{
    edge_list      *ptout;
    
    if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
        if ((ptcone->TYPE & CNS_TRI) == CNS_TRI
        || (ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
            return;
        }
    }
    if ((ptcone->TYPE & (CNS_LATCH|CNS_FLIP_FLOP|CNS_MEMSYM|CNS_MASTER|CNS_SLAVE)) != 0) {
        ptcone->TYPE |= YAG_MARK;
        return;
    }
    if ((ptcone->TYPE & YAG_VISITED) == YAG_VISITED) return;
    ptcone->TYPE |= YAG_VISITED;
    for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
        if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
        yagPropNotClockLatch(ptout->UEDGE.CONE);
    }
}

static void
yagUnmarkNotCommands(cone_list *ptcone)
{
    cone_list      *ptdrivingcone;
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    edge_list      *ptinedge;
    ptype_list     *ptuser;
    int             i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                if ((ptlink->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
                    if (ptuser == NULL) continue;
                    ptdrivingcone = (cone_list *)ptuser->DATA;
                    ptinedge = yagGetEdge(ptcone->INCONE, ptdrivingcone);
                    if (ptinedge != NULL && (ptinedge->TYPE & YAG_MARK) != 0) {
                        ptlink->TYPE &= ~CNS_COMMAND;
                    }
                }
            }
        }
    }

    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        if ((ptinedge->TYPE & YAG_MARK) != 0) {
            ptinedge->TYPE &= ~CNS_COMMAND;
            ptinedge->TYPE &= ~YAG_MARK;
        }
    }
}

/****************************************************************************
 *                         function yagDetectClockPrecharge()               *
 ****************************************************************************/

/* unmark any precharge whose clock is not a precharge clock path */

void
yagDetectClockPrech(inffig_list *ifl, cnsfig_list *ptcnsfig)
{
    locon_list     *ptlocon;
    cone_list      *ptcone;
    cone_list      *ptextcone;
    edge_list      *ptout;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    int             count;
    chain_list     *ckprechlist;

    if ((ckprechlist=inf_GetEntriesByType(ifl, INF_CKPRECH, INF_ANY_VALUES))==NULL) return;

    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_MARK;
    }
    count = 0;
    /* Propagate from input connectors */
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (yagGetChain(ckprechlist, ptlocon->NAME) != NULL) {
            count++;
            ptuser = getptype(ptlocon->USER, CNS_EXT);
            if (ptuser != NULL) ptextcone = (cone_list *)ptuser->DATA;
            for (ptout = ptextcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                yagPropClockLatch(ptout->UEDGE.CONE, ptextcone);
            }
            ptuser = getptype(ptlocon->USER, CNS_CONE);
            if (ptuser != NULL) {
                for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptcone = (cone_list *)ptchain->DATA;
                    if (ptcone != ptextcone) yagPropClockPrech(ptcone, ptlocon);
                }
            }
        }   
    }
    /* Propagate from internal cones */
    if (count != yagCountChains(ckprechlist)) {
        for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
            if (yagGetChain(ckprechlist, ptcone->NAME) != NULL) {
                for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                    if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                    yagPropClockPrech(ptout->UEDGE.CONE, ptcone);
                }
            }
        }
    }
    /* Perform the unmarking */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_VISITED;
        if ((ptcone->TYPE & YAG_MARK) != YAG_MARK && (ptcone->TYPE & CNS_PRECHARGE) == CNS_PRECHARGE) {
            ptcone->TYPE &= ~CNS_PRECHARGE;
            yagWarning(WAR_UNMARKED_PRECHARGE, NULL, ptcone->NAME, (char *)ptcone->INDEX, 0);
        }
        ptcone->TYPE &= ~YAG_MARK;
    }

    freechain(ckprechlist);
}

static void
yagPropClockPrech(cone_list *ptcone, void *ptprev)
{
    edge_list      *ptout, *ptin;
    
    if ((ptcone->TYPE & YAG_VISITED) == YAG_VISITED) return;
    ptcone->TYPE |= YAG_VISITED;
    if ((ptcone->TYPE & (CNS_FLIP_FLOP|CNS_MEMSYM)) != 0) return;
    if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
        if ((ptcone->TYPE & CNS_TRI) == CNS_TRI
        || (ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
            return;
        }
    }
    if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
        for (ptin = ptcone->INCONE; ptin; ptin = ptin->NEXT) {
            if (ptin->UEDGE.PTR == ptprev) break;
        }
        if (ptin != NULL && (ptin->TYPE & CNS_COMMAND) == CNS_COMMAND) return;
    }
    if ((ptcone->TYPE & CNS_PRECHARGE) == CNS_PRECHARGE) {
        for (ptin = ptcone->INCONE; ptin; ptin = ptin->NEXT) {
            if (ptin->UEDGE.PTR == ptprev) break;
        }
        if (ptin != NULL && (ptin->TYPE & YAG_PRECHCOM) == YAG_PRECHCOM) {
            ptcone->TYPE |= YAG_MARK;
            return;
        }
    }
    for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
        if ((ptout->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
        if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
        yagPropClockPrech(ptout->UEDGE.CONE, ptcone);
    }
}

/****************************************************************************
 *                         function yagPropagateStuck()                     *
 ****************************************************************************/

int
yagPropagateStuck(cone_list *ptcone)
{
    edge_list *ptedge;
    cone_list *ptnextcone;
    int        changes = FALSE;
    
    for (ptedge = ptcone->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) continue;
        ptnextcone = ptedge->UEDGE.CONE;
        if (yagGetEdge(ptnextcone->INCONE, ptcone) == NULL) continue;
        if (yagDetectStuck(ptnextcone)) {
            changes = TRUE;
            yagPropagateStuck(ptnextcone);
        }
    }
    return changes;
}

int
yagDetectStuck(cone_list *ptcone)
{
    edge_list *ptedge;
    pCircuit    circuit;
    ptype_list *ptuser;
    pNode       upbdd, dnbdd, ptconstraintbdd;
    cone_list  *ptincone;
    locon_list *ptinlocon;
    chain_list *ptupexpr;
    chain_list *ptdnexpr;
    chain_list *ptconstraint;
    chain_list *constraint_list = NULL;
    int         result = TRUE;
    char        freeup = FALSE, freedown = FALSE;

    avt_log(LOGYAG, 2,  "\tChecking stuck at cone '%s'\n", ptcone->NAME);
    if ((ptcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) return FALSE;
    if ((ptcone->TYPE & YAG_PARTIAL) != 0 && (ptcone->TECTYPE & CNS_DUAL_CMOS) == 0) return FALSE;
    if ((ptcone->TYPE & CNS_LATCH) == 0 && (ptuser = getptype(ptcone->USER, YAG_DUALEXPR_PTYPE)) != NULL) {
        ptupexpr = (chain_list *)ptuser->DATA;
        ptdnexpr = NULL;
    }
    else {
        if ((ptuser = getptype(ptcone->USER, CNS_UPEXPR)) != NULL) {
            ptupexpr = (chain_list *)ptuser->DATA;
            if (!ptupexpr) ptupexpr = createAtom("'0'"), freeup=TRUE;
        } else return FALSE;
        if ((ptuser = getptype(ptcone->USER, CNS_DNEXPR)) != NULL) {
            ptdnexpr = (chain_list *)ptuser->DATA;
            if (!ptdnexpr) ptdnexpr = createAtom("'0'"), freedown=TRUE;
        } else return FALSE;
    }

    circuit = yagBuildDirectCct(ptcone);
    upbdd = ablToBddCct(circuit, ptupexpr);
    if (ptdnexpr) dnbdd = ablToBddCct(circuit, ptdnexpr);
    else dnbdd = NULL;
    
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        ptconstraint = NULL;
        if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptinlocon = ptedge->UEDGE.LOCON;
            ptuser = getptype(ptinlocon->USER, CNS_TYPELOCON);
            if (ptuser == NULL) continue;
            if (((long)ptuser->DATA & CNS_ZERO) == CNS_ZERO) {
                ptconstraint = createAtom(ptinlocon->NAME);
                ptconstraintbdd = notBdd(ablToBddCct(circuit, ptconstraint));
            }
            else if (((long)ptuser->DATA & CNS_ONE) == CNS_ONE) {
                ptconstraint = createAtom(ptinlocon->NAME);
                ptconstraintbdd = ablToBddCct(circuit, ptconstraint);
            }
        }
        else {
            ptincone = ptedge->UEDGE.CONE;
            if ((ptincone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
                ptconstraint = createAtom(ptincone->NAME);
                ptconstraintbdd = notBdd(ablToBddCct(circuit, ptconstraint));
            }
            else if ((ptincone->TECTYPE & CNS_ONE) == CNS_ONE) {
                ptconstraint = createAtom(ptincone->NAME);
                ptconstraintbdd = ablToBddCct(circuit, ptconstraint);
            }
        }
        if (ptconstraint != NULL) {
            constraint_list = addchain(constraint_list, ptconstraintbdd);
            upbdd = constraintBdd(upbdd, ptconstraintbdd);
            if (ptdnexpr) dnbdd = constraintBdd(dnbdd, ptconstraintbdd);
            freeExpr(ptconstraint);
        }
    }
    
    if (upbdd == BDD_one && (dnbdd == BDD_zero || dnbdd == NULL)) {
        ptcone->TECTYPE |= CNS_ONE;
        ptcone->TYPE |= YAG_FORCEPRIM;
        avt_log(LOGYAG, 2,  "\t\tStuck ONE\n", ptcone->NAME);
        if (V_BOOL_TAB[__YAGLE_ANALYSE_STUCK].VALUE) yagMarkStuck(ptcone, circuit, constraint_list);
    }
    else if (upbdd == BDD_zero && (dnbdd == BDD_one || dnbdd == NULL)) {
        ptcone->TECTYPE |= CNS_ZERO;
        ptcone->TYPE |= YAG_FORCEPRIM;
        avt_log(LOGYAG, 2,  "\t\tStuck ZERO\n", ptcone->NAME);
        if (V_BOOL_TAB[__YAGLE_ANALYSE_STUCK].VALUE) yagMarkStuck(ptcone, circuit, constraint_list);
    }
    else if (YAG_CONTEXT->YAG_STUCK_LATCH && (ptcone->TYPE & CNS_LATCH) != 0 && dnbdd != NULL) {
        if (upbdd != BDD_zero && dnbdd == BDD_zero) {
            ptcone->TECTYPE |= CNS_ONE;
            ptcone->TYPE |= YAG_FORCEPRIM;
            avt_log(LOGYAG, 2,  "\t\tLatch Stuck ONE\n", ptcone->NAME);
            if (V_BOOL_TAB[__YAGLE_ANALYSE_STUCK].VALUE) yagMarkStuck(ptcone, circuit, constraint_list);
        }
        else if (upbdd == BDD_zero && dnbdd != BDD_zero) {
            ptcone->TECTYPE |= CNS_ZERO;
            ptcone->TYPE |= YAG_FORCEPRIM;
            avt_log(LOGYAG, 2,  "\t\tLatch Stuck ONE\n", ptcone->NAME);
            if (V_BOOL_TAB[__YAGLE_ANALYSE_STUCK].VALUE) yagMarkStuck(ptcone, circuit, constraint_list);
        }
        else result =  FALSE;
    }
    else result = FALSE;

    if (result == FALSE && constraint_list != NULL && V_BOOL_TAB[__YAGLE_ANALYSE_STUCK].VALUE) {
        yagUpdateStuck(ptcone, circuit, constraint_list);
    }
    
    freechain(constraint_list);
    destroyCct(circuit);

    if (freeup) freeExpr(ptupexpr);
    if (freedown) freeExpr(ptdnexpr);
    
    return result;
}

static void
yagMarkStuck(cone_list *ptcone, pCircuit circuit, chain_list *constraint_list)
{
    branch_list *ptbranch;
    branch_list *ptfalsebranch;
    branch_list *ptresbranch;
    chain_list  *ptchain;
    chain_list  *branch_expr;
    link_list   *ptlink;
    abl_pair    ablXt;
    pNode        branch_bdd;

    if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
        ptfalsebranch = ptcone->BRVSS;
        ptresbranch = ptcone->BRVDD;
    }
    else if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
        ptfalsebranch = ptcone->BRVDD;
        ptresbranch = ptcone->BRVSS;
    }
    else return;
    
    for (ptbranch = ptfalsebranch; ptbranch; ptbranch = ptbranch->NEXT) {
        ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
    }
    if (avt_islog(3,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG, 3,  "\nAnalysing stuck cone structure :-\n\n");
        displayconelog(LOGYAG, 3, ptcone);
    }
    avt_log(LOGYAG, 3,  "\t\tDetecting non-functional up/down branches\n");
    for (ptbranch = ptresbranch; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
        if ((branch_expr = cnsMakeBranchExpr(ptbranch, CNS_COMMAND, FALSE)) == NULL) continue;
        branch_bdd = ablToBddCct(circuit, branch_expr);
        freeExpr(branch_expr);
        for (ptchain = constraint_list; ptchain; ptchain = ptchain->NEXT) {
            branch_bdd = constraintBdd(branch_bdd, (pNode)ptchain->DATA);
        }
        if (branch_bdd == BDD_zero) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
        else {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_COMMAND) == 0) ptlink->TYPE |= CNS_RESIST;
            }
        }
    }
    avt_log(LOGYAG, 3,  "\t\tDetecting non-functional external branches\n");
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
        cnsMakeExtBranchExpr(ptbranch, &ablXt, CNS_COMMAND, FALSE);
        freeExpr(ablXt.DN);
        if ((branch_expr = ablXt.UP) == NULL) continue;
        branch_bdd = ablToBddCct(circuit, branch_expr);
        freeExpr(branch_expr);
        for (ptchain = constraint_list; ptchain; ptchain = ptchain->NEXT) {
            branch_bdd = constraintBdd(branch_bdd, (pNode)ptchain->DATA);
        }
        if (branch_bdd == BDD_zero) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
        else {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_COMMAND) == 0) ptlink->TYPE |= CNS_RESIST;
            }
        }
    }
}

static void
yagUpdateStuck(cone_list *ptcone, pCircuit circuit, chain_list *constraint_list)
{
    branch_list *brlist[3];
    branch_list *ptbranch;
    chain_list  *ptchain;
    chain_list  *branch_expr;
    link_list   *ptlink;
    cone_list   *ptlinkcone;
    ptype_list  *ptuser;
    abl_pair    ablXt;
    pNode        branch_bdd;
    int          i;

    if (avt_islog(3,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG, 3,  "\nUpdating cone structure from stuck inputs :-\n\n");
        displayconelog(LOGYAG, 3, ptcone);
    }
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
            if (i != 2) {
                if ((branch_expr = cnsMakeBranchExpr(ptbranch, CNS_COMMAND, FALSE)) == NULL) continue;
            }
            else {
                cnsMakeExtBranchExpr(ptbranch, &ablXt, CNS_COMMAND, FALSE);
                freeExpr(ablXt.DN);
                if ((branch_expr = ablXt.UP) == NULL) continue;
            }
            branch_bdd = ablToBddCct(circuit, branch_expr);
            freeExpr(branch_expr);
            for (ptchain = constraint_list; ptchain; ptchain = ptchain->NEXT) {
                branch_bdd = constraintBdd(branch_bdd, (pNode)ptchain->DATA);
            }
            if (branch_bdd == BDD_zero) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
            else {
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    if (i == 2 && !ptlink->NEXT) break;
                    ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
                    if (ptuser == NULL || ptuser->DATA == NULL) continue;
                    else ptlinkcone = (cone_list*)ptuser->DATA;
                    if ((ptlink->TYPE & CNS_TNLINK) == CNS_TNLINK && (ptlinkcone->TECTYPE & CNS_ONE) == CNS_ONE) {
                        ptlink->TYPE |= CNS_RESIST;
                    }
                    else if ((ptlink->TYPE & CNS_TPLINK) == CNS_TNLINK && (ptlinkcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
                        ptlink->TYPE |= CNS_RESIST;
                    }
                }
            }
        }
    }
}

/****************************************************************************
 *                         function yagInfUnmarkLatches()                   *
 ****************************************************************************/

void
yagInfUnmarkLatches(inffig_list *ifl, cnsfig_list *ptcnsfig)
{
    chain_list *ptchain, *ptnotlatchchain;
    cone_list *ptcone;

    ptnotlatchchain=inf_GetEntriesByType(ifl, INF_NOTLATCH, INF_ANY_VALUES);
    if (ptnotlatchchain != NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TYPE & CNS_LATCH) != CNS_LATCH) continue;
            for (ptchain = ptnotlatchchain; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_TestREGEX(ptcone->NAME, (char *)ptchain->DATA)) {
                    yagUnmarkLatch(ptcone, TRUE, TRUE);
                    break;
                }
            }
        }
        freechain(ptnotlatchchain);
    }
}

/****************************************************************************
 *                         function yagDetectClockGating()                  *
 ****************************************************************************/

/* detect convergence between clock and non-clock paths */

void
yagDetectClockGating(inffig_list *ifl, cnsfig_list *ptcnsfig)
{
    locon_list     *ptlocon, *ptinlocon;
    cone_list      *ptcone, *ptincone;
    cone_list      *ptextcone;
    edge_list      *ptout, *ptin;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    chain_list     *clocklist = NULL;
    chain_list     *datalist = NULL;
    chain_list     *gateexpr;
    char           *clockname;
    int             numclock, numdata, conditioned_state, output_state;

    clocklist = inf_GetEntriesByType(ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
    if (clocklist == NULL) return;
    
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_MARK;
    }
    /* Propagate from input connectors */
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        for (ptchain = clocklist; ptchain; ptchain = ptchain->NEXT) {
            if (mbk_TestREGEX(ptlocon->NAME, (char *)ptchain->DATA)) break;
        }
        if (ptchain != NULL) {
            ptuser = getptype(ptlocon->USER, CNS_EXT);
            if (ptuser != NULL) {
                ptextcone = (cone_list *)ptuser->DATA;
                if ((ptextcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) continue;
                for (ptout = ptextcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                    if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                    yagPropClockGating(ptout->UEDGE.CONE, ptextcone);
                }
            }
            ptuser = getptype(ptlocon->USER, CNS_CONE);
            if (ptuser != NULL) {
                for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptcone = (cone_list *)ptchain->DATA;
                    if ((ptcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) continue;
                    if (ptcone != ptextcone) yagPropClockGating(ptcone, ptlocon);
                }
            }
        }   
    }
    /* Propagate from internal cones */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) continue;
        if ((ptcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) continue;
        for (ptchain = clocklist; ptchain; ptchain = ptchain->NEXT) {
            if (mbk_TestREGEX(ptcone->NAME, (char *)ptchain->DATA)) break;
        }
        if (ptchain != NULL) {
            for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
                if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
                yagPropClockGating(ptout->UEDGE.CONE, ptcone);
            }
        }
    }
    /* add the clock gating check */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        datalist = NULL;
        if ((ptcone->TYPE & YAG_MARK) == YAG_MARK) {
            ptcone->TYPE &= ~YAG_MARK;
            if ((ptcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) continue;
            numclock = 0;
            numdata = 0;
            for (ptin = ptcone->INCONE; ptin; ptin = ptin->NEXT) {
                if ((ptin->TYPE & YAG_MARK) != YAG_MARK) {
                    ptin->TYPE &= ~YAG_MARK;
                    numdata++;
                    if ((ptin->TYPE & CNS_EXT) == CNS_EXT) {
                        ptinlocon = ptin->UEDGE.LOCON;
                        /* ignore external connector link */
                        ptuser = getptype(ptinlocon->USER, CNS_EXT);
                        if (ptuser && (cone_list *)ptuser->DATA == ptcone) continue;
                        /* ignore stuck data */
                        ptuser = getptype(ptinlocon->USER, CNS_TYPELOCON);
                        if (ptuser && (((long)ptuser->DATA) & (CNS_ZERO|CNS_ONE)) != 0) continue;
                        datalist = addchain(datalist, ptinlocon->NAME);
                    }
                    else {
                        ptincone = ptin->UEDGE.CONE;
                        if ((ptincone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) continue;
                        datalist = addchain(datalist, ptincone->NAME);
                    }
                }
                else {
                    if ((ptin->TYPE & CNS_EXT) == CNS_EXT) clockname = ptin->UEDGE.LOCON->NAME;
                    else clockname = ptin->UEDGE.CONE->NAME;
                    numclock++;
                }
            }
            if (numclock == 1 && numdata > 0 && (ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) == 0 && (ptcone->TECTYPE & (CNS_ONE|CNS_ZERO)) == 0) {
                if ((ptuser = getptype(ptcone->USER, YAG_DUALEXPR_PTYPE)) != NULL) {
                    gateexpr = ptuser->DATA;
                }
                else if ((ptuser = getptype(ptcone->USER, CNS_UPEXPR)) != NULL) {
                    gateexpr = ptuser->DATA;
                }
                else continue;
                conditioned_state = yagDetectConditionedState(gateexpr, clockname, &output_state);
                if (conditioned_state >= 0) {
                    if (YAG_CONTEXT->YAG_CLOCK_GATE & (short)0x1) {
                        for (ptchain = datalist; ptchain; ptchain = ptchain->NEXT) {
                            if (yagAddCheckDirective(ifl, (char *)ptchain->DATA, clockname, conditioned_state)) {
                                yagWarning(WAR_CLOCK_GATING, (char *)ptchain->DATA, clockname, NULL, conditioned_state);
                            }
                        }
                    }
                    if (YAG_CONTEXT->YAG_CLOCK_GATE & (short)0x2) yagAddFilterDirective(ifl, ptcone->NAME, output_state);
                }
            }
            freechain(datalist);
        }
    }

    freechain(clocklist);
    yagEndAddCheckDirective();
}

static void
yagPropClockGating(cone_list *ptcone, void *ptprev)
{
    edge_list      *ptout, *ptin;
    
    if ((ptcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) return;
    if ((ptcone->TYPE & (CNS_FLIP_FLOP|CNS_MEMSYM)) != 0) return;
    if ((ptcone->TYPE & (CNS_LATCH|CNS_PRECHARGE|CNS_RS)) != 0) return;
    if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
        if ((ptcone->TYPE & CNS_TRI) == CNS_TRI
        || (ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
            return;
        }
    }
    for (ptin = ptcone->INCONE; ptin; ptin = ptin->NEXT) {
        if (ptin->UEDGE.PTR == ptprev) ptin->TYPE |= YAG_MARK;
    }
    if ((ptcone->TYPE & YAG_MARK) == YAG_MARK) return;
    ptcone->TYPE |= YAG_MARK;
    for (ptout = ptcone->OUTCONE; ptout; ptout = ptout->NEXT) {
        if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;
        yagPropClockGating(ptout->UEDGE.CONE, ptcone);
    }
}

static int
yagDetectConditionedState(chain_list *gateexpr, char *varname, int *ptoutput_state)
{
    pCircuit    circuit;
    chain_list *ptsupport;
    chain_list *ptchain;
    pNode       var, expr, f0, f1, blocking_condition, output_state;
    int         index;

    ptsupport = supportChain_listExpr(gateexpr);
    circuit = initializeCct("temp", yagCountChains(ptsupport), 10);
    for (ptchain = ptsupport; ptchain; ptchain = ptchain->NEXT) {
        addInputCct_no_NA(circuit, (char *)ptchain->DATA);
    }
    
    index = searchInputCct_no_NA(circuit, varname);
    if (index <= 1) return -1;
    var = createNodeTermBdd(index);
    expr = ablToBddCct(circuit, gateexpr);
    f1 = constraintBdd(expr, var);
    f0 = constraintBdd(expr, notBdd(var));
    
    blocking_condition = notBdd(applyBinBdd(OR, applyBinBdd(AND, f0, notBdd(f1)), applyBinBdd(AND, notBdd(f0), f1)));
    
    output_state = constraintBdd(expr, blocking_condition);
    
    destroyCct(circuit);
    freechain(ptsupport);
    if (zeroBdd(output_state)) {
        *ptoutput_state = 1;
        if (zeroBdd(f0) && !zeroBdd(f1)) return 1;
        if (!zeroBdd(f0) && zeroBdd(f1)) return 0;
    }
    else if (oneBdd(output_state)) {
        *ptoutput_state = 0;
        if (oneBdd(f0) && !oneBdd(f1)) return 1;
        if (!oneBdd(f0) && oneBdd(f1)) return 0;
    }
    
    return -1;
}

void yagDetectDelayedRS(cnsfig_list *ptcnsfig)
{
    cone_list *ptcone1, *ptdelaycone1, *ptcone2, *ptdelaycone2, *ptinput, *ptprevcone;
    edge_list *ptedge;
    long rstype;
    int numdelaycones;

    for (ptcone1 = ptcnsfig->CONE; ptcone1; ptcone1 = ptcone1->NEXT) {
        rstype = 0;
        if ((ptcone1->TYPE & CNS_RS) == CNS_RS) continue;
        if ((ptcone1->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS) continue;
        if (yagCountConeEdges(ptcone1->INCONE) <= 1) continue;
        ptinput = yagGetNextConeEdge(ptcone1->INCONE);
        if (!ptinput) continue;
        if (yagMatchNAND(ptcone1, ptinput)) rstype = CNS_NAND;
        if (yagMatchNOR(ptcone1, ptinput)) rstype = CNS_NOR;
        if (rstype == 0) continue;
        if (yagCountConeEdges(ptcone1->OUTCONE) != 1) continue;
        ptdelaycone1 = yagGetNextConeEdge(ptcone1->OUTCONE);
        ptprevcone = ptcone1;
        numdelaycones = 1;
        while (yagCountConeEdges(ptdelaycone1->OUTCONE) == 1 && (ptdelaycone1->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS && yagCountConeEdges(ptdelaycone1->INCONE) == 1) {
            ptprevcone = ptdelaycone1;
            ptdelaycone1 = yagGetNextConeEdge(ptdelaycone1->OUTCONE);
            numdelaycones++;
        }
        if ((ptdelaycone1->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS || yagCountConeEdges(ptdelaycone1->INCONE) != 1) {
            ptdelaycone1 = ptprevcone;
            numdelaycones--;
        }
        if (numdelaycones == 0 || numdelaycones % 2 != 0) continue;
        for (ptedge = ptdelaycone1->OUTCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) continue;
            ptcone2 = ptedge->UEDGE.CONE;
            if (rstype == CNS_NAND) {
                if (!yagMatchNAND(ptcone2, ptdelaycone1)) continue;
            }
            if (rstype == CNS_NOR) {
                if (!yagMatchNOR(ptcone2, ptdelaycone1)) continue;
            }
            if (yagCountConeEdges(ptcone2->OUTCONE) != 1) continue;
            ptdelaycone2 = yagGetNextConeEdge(ptcone2->OUTCONE);
            ptprevcone = ptcone2;
            numdelaycones = 1;
            while (yagCountConeEdges(ptdelaycone2->OUTCONE) == 1 && (ptdelaycone2->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS && yagCountConeEdges(ptdelaycone2->INCONE) == 1) {
                ptprevcone = ptdelaycone2;
                ptdelaycone2 = yagGetNextConeEdge(ptdelaycone2->OUTCONE);
                numdelaycones++;
            }
            if ((ptdelaycone2->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS || yagCountConeEdges(ptdelaycone2->INCONE) != 1) {
                ptdelaycone2 = ptprevcone;
                numdelaycones--;
            }
            if (numdelaycones == 0 || numdelaycones % 2 != 0) continue;
            if (yagGetEdge(ptcone1->INCONE, ptdelaycone2) != NULL) { /* found */
                yagDisableRSArcs(ptcone1, ptcone2, ptdelaycone1, ptdelaycone2, rstype , YAG_RS_LEGAL);
                yagWarning(WAR_DELAYED_RS, ptcone1->NAME, ptcone2->NAME, NULL, 0);
                ptcone1->TYPE |= CNS_RS;
                ptcone2->TYPE |= CNS_RS;
                ptcone1->USER = addptype(ptcone1->USER, YAG_BISTABLE_PTYPE, ptcone2);
                ptcone2->USER = addptype(ptcone2->USER, YAG_BISTABLE_PTYPE, ptcone1);
            }
        }
    }
}
