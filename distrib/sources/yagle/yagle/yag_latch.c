/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_latch.c                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 09/08/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static locon_list *yagCheckNextCon(chain_list *loconchain, locon_list *ptprevcon);
static double yagCalcChainResistance(chain_list *transchain);
static chain_list *yagFindBleedTrans(losig_list *ptsig, locon_list *ptcon, char type, cone_list *ptincone);
static chain_list *yagMatchNOTbyLotrs (cone_list *ptcone, cone_list *ptincone, float *up_resistance, float *down_resistance);
static int checkLatch (cone_list *ptcone0, cone_list *ptcone1, int simple);
static void addLatchType (cone_list *ptcone, cone_list *feedbackcone, chain_list *feedbacktrans, int feedbacktype, int forwardtype);
static int checkMemory (cone_list *ptcone0, cone_list *ptcone1);
static int checkDiffLatch (cone_list *ptcone0, cone_list *ptcone1);
static int yagCheckBistable (cone_list *ptcone0, cone_list *ptcone1);
static int isTristate (cone_list *ptcone, cone_list *ptincone, chain_list **pttranslist);
static chain_list *invChain (cone_list *ptcone, chain_list *ptchain);

/*************************************************************************
 *                     function yagExtractBleeder()                      *
 *************************************************************************/

/* test whether a given cone has a bleeder or a level_hold on one of its inputs */

void
yagMarkLevelHold(cone_list *ptbleedcone, cone_list *ptcone, chain_list *transchain)
{
    branch_list *ptbranch;
    edge_list   *ptbleededge;
    link_list   *ptlink;
    cone_list   *ptlinkcone;
    branch_list *brlist[2];
    ptype_list  *ptuser;
    chain_list  *ptchain;
    chain_list  *bleeder_branches = NULL;
    long         linktype, conetype;
    int          i;
    int          isbleed;

    ptbleedcone->TECTYPE &= ~CNS_DUAL_CMOS;
    ptbleedcone->TECTYPE |= YAG_LEVELHOLD;
    if ((ptbleedcone->TYPE & YAG_HASDUAL) != 0) {
        ptbleedcone->TYPE &= ~YAG_HASDUAL;
        ptuser = getptype(ptbleedcone->USER, YAG_DUALEXPR_PTYPE);
        freeExpr((chain_list *)ptuser->DATA);
        ptbleedcone->USER = delptype(ptbleedcone->USER, YAG_DUALEXPR_PTYPE);
    }

    ptbleededge = yagGetEdge(ptbleedcone->INCONE, ptcone);
    if (ptbleededge) ptbleededge->TYPE |= CNS_BLEEDER;
    brlist[0] = ptbleedcone->BRVDD;
    brlist[1] = ptbleedcone->BRVSS;
    for (i=0; i<2; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            linktype = ((i==0)?CNS_TPLINK:CNS_TNLINK);
            conetype = ((i==0)?CNS_VSS:CNS_VDD);
            isbleed = FALSE;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & linktype) == 0) break;
                ptlinkcone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                if (ptlinkcone == ptcone) {
                    isbleed = TRUE;
                }
            }
            if (ptlink == NULL && isbleed) {
                ptbranch->TYPE |= CNS_BLEEDER|CNS_NOT_FUNCTIONAL;
                bleeder_branches = addchain(bleeder_branches, ptbranch);
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    ptlink->ULINK.LOTRS->TYPE |= BLEEDER;
                }
            }
        }
    }
    for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
        ((lotrs_list *)ptchain->DATA)->TYPE |= BLEEDER;
    }
    if (bleeder_branches) {
        ptuser = getptype(ptbleedcone->USER, CNS_BLEEDER);
        if (ptuser) {
            freechain((chain_list *)ptuser->DATA);
            ptuser->DATA = bleeder_branches;
        }
        else ptbleedcone->USER = addptype(ptbleedcone->USER, CNS_BLEEDER, bleeder_branches);
    }
}

static void
yagFclMarkTrans(lotrs_list *ptlotrs, long type)
{
    ptype_list     *ptuser;

    if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        ptuser->DATA = (void *) ((long) ptuser->DATA | type);
    }
    else ptlotrs->USER = addptype(ptlotrs->USER, FCL_TRANSFER_PTYPE, (void *) type);
}

static void
yagFclMarkSig(losig_list *ptlosig, long type)
{
    ptype_list     *ptuser;

    if ((ptuser = getptype(ptlosig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        ptuser->DATA = (void *) ((long) ptuser->DATA | type);
    }
    else ptlosig->USER = addptype(ptlosig->USER, FCL_TRANSFER_PTYPE, (void *) type);
}

/* count connected transistors ignoring parallel instances and bulks*/
int
yagCountConnections(chain_list *loconchain, int only_source_drain)
{
    chain_list *ptchain;
    locon_list *ptlocon;
    lotrs_list *pttrans;
    ptype_list *ptuser;
    int count = 0;
    
    for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        if (ptlocon->TYPE == 'T') {
            if (ptlocon->NAME == CNS_BULKNAME) continue;
            if (ptlocon->NAME == CNS_GRIDNAME && only_source_drain) continue;
            pttrans = (lotrs_list *)ptlocon->ROOT;
            if (pttrans->GRID == NULL) continue;
            if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
            }
            if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;
            count++;
        }
        else if (!only_source_drain) count++;
    }
    return count;
}

/* Return the connector of unique possible next transistor in bleeder chain, null otherwise */
static locon_list *
yagCheckNextCon(chain_list *loconchain, locon_list *ptprevcon)
{
    chain_list *ptchain;
    locon_list *ptlocon, *ptnextcon = NULL;
    lotrs_list *pttrans;
    
    for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        if (ptlocon == ptprevcon) continue;
        if (ptlocon->TYPE == 'T') {
            if (ptlocon->NAME == CNS_BULKNAME) continue;
            if (ptlocon->NAME == CNS_GRIDNAME) return NULL;
            pttrans = (lotrs_list *)ptlocon->ROOT;
            if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;
        }
        else return NULL;
        if (!ptnextcon) ptnextcon = ptlocon;
        else return NULL;
    }
    return ptnextcon;
}

/* Return a list of transistor making bleeder branch of required type */
static chain_list *
yagFindBleedTrans(losig_list *ptsig, locon_list *ptcon, char type, cone_list *ptincone)
{
    chain_list *visit_list = NULL;
    chain_list *transchain = NULL;
    chain_list *loconchain;
    losig_list *ptcurrentsig, *ptnextsig;
    locon_list *ptprevcon;
    cone_list *ptdriver;
    lotrs_list *pttrans;
    ptype_list *ptuser;
    int found = FALSE, done = FALSE;
    int bleeder, resistance;
    char supply_type;
    
    visit_list = addchain(visit_list, ptsig);
    ptcurrentsig = ptsig;
    ptprevcon = NULL;
    supply_type = ((type == CNS_TN)?CNS_SIGVSS:CNS_SIGVDD);

    do {
        bleeder = FALSE; resistance = FALSE;
        pttrans = (lotrs_list *)ptcon->ROOT;
        /* check for NEVER directive on transistor */
        if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) {
                found = FALSE;
                break;
            }
        }
        
        /* filter transistors without driver */
        if (pttrans->GRID == NULL) {
            found = FALSE;
            break;
        }

        if (pttrans->SOURCE->SIG == ptcurrentsig) {
            ptnextsig = pttrans->DRAIN->SIG;
            ptprevcon = pttrans->DRAIN;
        }
        else {
            ptnextsig = pttrans->SOURCE->SIG;
            ptprevcon = pttrans->SOURCE;
        }
        if (yagGetChain(visit_list, ptnextsig) || yagCountChains(visit_list) > 4) {
            found = FALSE;
            break;
        }
        visit_list = addchain(visit_list, ptnextsig);
        ptdriver = (cone_list *)pttrans->GRID;
        if ((pttrans->TYPE & type) == type && ptdriver == ptincone) {
            bleeder = TRUE;
        }
        else if (((pttrans->TYPE & CNS_TN) == CNS_TN && (ptdriver->TYPE & CNS_VDD) == CNS_VDD)
        || ((pttrans->TYPE & CNS_TP) == CNS_TP && (ptdriver->TYPE & CNS_VSS) == CNS_VSS)) {
            resistance = TRUE;
        }
        if (bleeder) found = TRUE;
        if (bleeder || resistance) {
            transchain = addchain(transchain, pttrans);
            if (ptnextsig->TYPE == supply_type) {
                done = TRUE;
            }
            else if (ptnextsig->TYPE != CNS_SIGINT) {
                found = FALSE;
                done = TRUE;
            }
            else {
                ptcurrentsig = ptnextsig;
                loconchain = (chain_list *) getptype(ptcurrentsig->USER, LOFIGCHAIN)->DATA;
                if ((ptcon = yagCheckNextCon(loconchain, ptprevcon)) == NULL) found = FALSE;
            }
        }
        else {
            found = FALSE;
            done = TRUE;
        }
    } while (ptcon && !done);   

    freechain(visit_list);
    if (!found) {
        freechain(transchain);
        transchain = NULL;
    }
    return transchain;
}

static double
yagCalcChainResistance(chain_list *transchain)
{
    double resistance = 0.0;
    lotrs_list *ptlotrs;
    chain_list *ptchain;
    
    for (ptchain = transchain; ptchain; ptchain = ptchain->NEXT) {
        ptlotrs = (lotrs_list *)ptchain->DATA;
        resistance += (ptlotrs->LENGTH / ptlotrs->WIDTH);
    }
    return resistance;
}

static chain_list *
yagMatchNOTbyLotrs(cone_list *ptcone, cone_list *ptincone, float *up_resistance, float *down_resistance)
{
    losig_list     *ptsig;
    locon_list     *ptcon;
    edge_list      *ptinput;
    cone_list      *testinput;
    branch_list    *ptbranch;
    link_list      *ptlink;
    float           up_conductance = 0.0, down_conductance = 0.0;
    chain_list     *loconchain, *ptchain, *bleedtranschain, *transchain = NULL;
    chain_list     *up_branches = NULL, *down_branches = NULL;
    
    int             count;
    int             found_nmos = FALSE, found_pmos = FALSE;

    ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
    if (ptsig->TYPE == CNS_SIGVDD || ptsig->TYPE == CNS_SIGVSS) return NULL;

    /* Dual cone already extracted */
    if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) {
        count = 0;
        for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
            if ((ptinput->TYPE & CNS_EXT) == CNS_EXT) continue;
            if ((ptinput->TYPE & CNS_POWER) == CNS_POWER) continue;
            count++;
            testinput = ptinput->UEDGE.CONE;
        }
        if (count == 1 && testinput == ptincone) {
            for (ptbranch =  ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    transchain = addchain(transchain, ptlink->ULINK.LOTRS);
                }
                up_branches = addchain(up_branches, ptbranch);
            }
            for (ptbranch =  ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    transchain = addchain(transchain, ptlink->ULINK.LOTRS);
                }
                down_branches = addchain(down_branches, ptbranch);
            }
            *up_resistance = yagCalcParallelResistance(up_branches);
            *down_resistance = yagCalcParallelResistance(down_branches);
            freechain(up_branches);
            freechain(down_branches);
            return transchain;
        }
        return NULL;
    }

    /* General case of no cone */
    loconchain = (chain_list *) getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T') {
            if (ptcon->NAME == CNS_GRIDNAME) continue;
            if (ptcon->NAME == CNS_BULKNAME) continue;
            bleedtranschain = yagFindBleedTrans(ptsig, ptcon, CNS_TN, ptincone);
            if (bleedtranschain) {
                found_nmos = TRUE;
                transchain = append(transchain, bleedtranschain);
                down_conductance += 1 / yagCalcChainResistance(bleedtranschain);
            }
            bleedtranschain = yagFindBleedTrans(ptsig, ptcon, CNS_TP, ptincone);
            if (bleedtranschain) {
                found_pmos = TRUE;
                transchain = append(transchain, bleedtranschain);
                up_conductance += 1 / (2.3 * yagCalcChainResistance(bleedtranschain));
            }
        }
    }
    if (found_nmos && found_pmos) {
        *up_resistance = 1/up_conductance;
        *down_resistance = 1/down_conductance;
        return transchain;
    }
    if (transchain) freechain(transchain);
    return NULL;
}

static int
checkStrictLevelHold(cone_list *ptcone, chain_list *transchain)
{
    losig_list     *ptsig;
    lotrs_list     *pttrans;
    locon_list     *ptlocon;
    chain_list     *loconchain, *ptchain;
    ptype_list     *ptuser;

    /* Cone on external pin cannot be a strict level-hold */
    if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) return FALSE;
    
    /* Check that all gate connections are due to feedback */
    ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
    loconchain = (chain_list *) getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    
    for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        if (ptlocon->NAME != CNS_GRIDNAME) continue;
        pttrans = ptlocon->ROOT;
        if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
        }
        if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;
        if (yagGetChain(transchain, pttrans) == NULL) return FALSE;
    }

    /* Check number of source/drain connections */
    if (yagCountConnections(loconchain, TRUE) == yagCountBranches(ptcone)) return TRUE;
    return FALSE;
}

void 
yagExtractBleeder(inffig_list *ifl, cone_list *ptcone)
{
    losig_list     *ptsig, *ptbleedsig;
    edge_list      *ptinput;
    cone_list      *ptbleedcone;
    locon_list     *ptcon;
    losig_list     *ptsig1, *ptsig2;
    lotrs_list     *pttrans, *ptcomtrans1, *ptcomtrans2, *ptcuttrans;
    chain_list     *loconchain, *bleedconchain, *ptchain, *transchain, *looptranschain;
    float           upres, dnres, loopupres, loopdnres, ratio;
    chain_list     *bleedtranslist = NULL;
    chain_list     *newchain;
    long            bleedtype = 0;
    ht             *translist;
    int             found = FALSE;
    int             isLevelHold = FALSE;


    avt_log(LOGYAG,1, "Checking for bled input on cone '%s' : ", ptcone->NAME);
    
    ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
    loconchain = (chain_list *) getptype(ptsig->USER, LOFIGCHAIN)->DATA;

    if (YAG_CONTEXT->YAG_DETECT_LEVELHOLD || YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM) {
        for (ptinput = ptcone->INCONE; ptinput && (ptinput->TYPE & CNS_POWER) == CNS_POWER; ptinput = ptinput->NEXT);
        if (!ptinput) return;
        ptbleedcone = ptinput->UEDGE.CONE;
        looptranschain = yagMatchNOTbyLotrs(ptcone, ptbleedcone, &loopupres, &loopdnres);
        if (looptranschain == NULL) return;
        freechain(looptranschain);
        if ((transchain = yagMatchNOTbyLotrs(ptbleedcone, ptcone, &upres, &dnres)) != NULL) {
            isLevelHold = checkStrictLevelHold(ptcone, transchain);
            if (!isLevelHold && YAG_CONTEXT->YAG_DETECT_LEVELHOLD != 2) {
                ratio = (V_FLOAT_TAB[__YAGLE_LATCHLOOP_RATIO].VALUE > 1)?V_FLOAT_TAB[__YAGLE_LATCHLOOP_RATIO].VALUE:1.5;
                isLevelHold = (yagCountConnections(loconchain, TRUE) == yagCountBranches(ptcone)) && (upres > ratio*loopupres);
            }
            if (isLevelHold) {
                yagMarkLevelHold(ptbleedcone, ptcone, transchain);
                freechain(transchain);
                avt_log(LOGYAG,1, "LEVEL-HOLD FOUND\n");
                return;
            }
            else if (YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM) {
                ptcomtrans1 = NULL;
                ptcomtrans2 = NULL;
                ptcuttrans = NULL;
                ptbleedsig = (losig_list *)getptype(ptbleedcone->USER, CNS_SIGNAL)->DATA;
                for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
                    ptcon = (locon_list *) ptchain->DATA;
                    if (ptcon->TYPE != 'T') continue;
                    if (ptcon->NAME == CNS_GRIDNAME) continue;
                    if (ptcon->NAME == CNS_BULKNAME) continue;
                    pttrans = (lotrs_list *) ptcon->ROOT;
                    if ((cone_list *)pttrans->GRID == ptbleedcone) continue;
                    if (!ptcomtrans1) ptcomtrans1 = pttrans;
                    else break;
                }
                if (!ptchain && ptcomtrans1) {
                    if (ptcomtrans1->DRAIN->SIG == ptsig) ptsig1 = ptcomtrans1->SOURCE->SIG;
                    else ptsig1 = ptcomtrans1->DRAIN->SIG;
                    for (ptchain = (chain_list *)getptype(ptbleedsig->USER, LOFIGCHAIN)->DATA; ptchain; ptchain = ptchain->NEXT) {
                        ptcon = (locon_list *) ptchain->DATA;
                        if (ptcon->TYPE != 'T') continue;
                        if (ptcon->NAME == CNS_GRIDNAME) continue;
                        if (ptcon->NAME == CNS_BULKNAME) continue;
                        pttrans = (lotrs_list *) ptcon->ROOT;
                        if ((cone_list *)pttrans->GRID == ptcone) continue;
                        if (!ptcomtrans2) ptcomtrans2 = pttrans;
                        else break;
                    }
                    if (!ptchain && ptcomtrans2 && (ptcomtrans2->GRID == ptcomtrans1->GRID)) {
                        if (ptcomtrans2->DRAIN->SIG == ptsig) ptsig2 = ptcomtrans2->SOURCE->SIG;
                        else ptsig2 = ptcomtrans2->DRAIN->SIG;
                        for (ptchain = (chain_list *)getptype(ptsig1->USER, LOFIGCHAIN)->DATA; ptchain; ptchain = ptchain->NEXT) {
                            ptcon = (locon_list *) ptchain->DATA;
                            if (ptcon->TYPE != 'T') continue;
                            if (ptcon->NAME == CNS_GRIDNAME) continue;
                            if (ptcon->NAME == CNS_BULKNAME) continue;
                            pttrans = (lotrs_list *) ptcon->ROOT;
                            if ((pttrans->SOURCE->SIG == ptsig1 && pttrans->DRAIN->SIG == ptsig2)
                            || (pttrans->DRAIN->SIG == ptsig1 && pttrans->SOURCE->SIG == ptsig2)) {
                                ptcuttrans = pttrans;
                            }
                        }
                        ptcone->TYPE |= CNS_LATCH|CNS_MEMSYM;
                        ptinput = yagGetEdge(ptcone->INCONE, ptbleedcone);
                        if (ptinput) ptinput->TYPE |= CNS_MEMSYM;
                        ptinput = yagGetEdge(ptbleedcone->INCONE, ptcone);
                        if (ptinput) ptinput->TYPE |= CNS_MEMSYM;
                        ptbleedcone->TYPE |= CNS_LATCH|CNS_MEMSYM;
                        ptcone->USER = addptype(ptcone->USER, YAG_MEMORY_PTYPE, ptbleedcone);
                        ptbleedcone->USER = addptype(ptbleedcone->USER, YAG_MEMORY_PTYPE, ptcone);
                        yagFclMarkTrans(ptcomtrans1, FCL_COMMAND);
                        yagFclMarkTrans(ptcomtrans2, FCL_COMMAND);
                        if (ptcuttrans) yagFclMarkTrans(ptcuttrans, FCL_NEVER);
                        avt_log(LOGYAG,1, "SIMPLE MEMSYM FOUND\n");
                        freechain(transchain);
                        return;
                    }
                }
            }
            freechain(transchain);
        }
    }

    for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
        if ((ptinput->TYPE & CNS_EXT) == CNS_EXT) continue;
        ptbleedcone = ptinput->UEDGE.CONE;
        if ((ptbleedcone->TYPE & YAG_PARTIAL) != YAG_PARTIAL) continue;
        ptbleedsig = (losig_list *)getptype(ptbleedcone->USER, CNS_SIGNAL)->DATA;
        translist = yagGetTransList(ifl, ptbleedcone, ptbleedsig);
        bleedconchain = (chain_list *) getptype(ptbleedsig->USER, LOFIGCHAIN)->DATA;
        for (ptchain = bleedconchain; ptchain; ptchain = ptchain->NEXT) {
            ptcon = (locon_list *) ptchain->DATA;
            if (ptcon->TYPE != 'T') continue;
            if (ptcon->NAME == CNS_GRIDNAME) continue;
            if (ptcon->NAME == CNS_BULKNAME) continue;
            pttrans = (lotrs_list *) ptcon->ROOT;
            if (gethtitem(translist, pttrans) == EMPTYHT) continue;
            if ((newchain = yagFindBleedTrans(ptbleedsig, ptcon, CNS_TN, ptcone)) != NULL) {
                bleedtype |= CNS_VSS;
                bleedtranslist = append(bleedtranslist, newchain);
            }
            if ((newchain = yagFindBleedTrans(ptbleedsig, ptcon, CNS_TP, ptcone)) != NULL) {
                bleedtype |= CNS_VDD;
                bleedtranslist = append(bleedtranslist, newchain);
            }
        }

        for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
            ptcon = (locon_list *) ptchain->DATA;
            if (ptcon->TYPE != 'T') continue;
            if (ptcon->NAME != CNS_GRIDNAME) continue;
            pttrans = (lotrs_list *) ptcon->ROOT;
            if (gethtitem(translist, pttrans) == EMPTYHT) continue;
            if (yagGetChain(bleedtranslist, pttrans) == NULL) break;
        }

        if (ptchain == NULL) {
            if (bleedtype == CNS_VDD || bleedtype == CNS_VSS) {
                for (ptchain = bleedtranslist; ptchain; ptchain = ptchain->NEXT) {
                    ((lotrs_list *) ptchain->DATA)->TYPE |= BLEEDER;
                    found = TRUE;
                }
            }
        }
        delht(translist);
        freechain(bleedtranslist);
        bleedtranslist = NULL;
    }
    if (found)
        avt_log(LOGYAG,1, "FOUND\n");
    else
        avt_log(LOGYAG,1, "NOT FOUND\n");
}

/****************************************************************************
 *                         function yagDetectBleeder()                      *
 ****************************************************************************/

/* test for any bleeders missed in the initial bleeder detection phase */

void
yagDetectBleeder(cone_list *ptcone)
{
    branch_list *ptbranch;
    link_list   *ptlink;
    cone_list   *loopcone;
    chain_list  *bleeders = NULL;
    chain_list  *maybe = NULL;
    chain_list  *ptchain;
    edge_list   *ptedge;
    branch_list *brlist[4];
    long        looptype;
    int         i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_CONE) == 0) continue;
        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
        loopcone = ptedge->UEDGE.CONE;
        looptype = 0;
        maybe = NULL;
        if (yagGetEdge(loopcone->INCONE, ptcone) != NULL) {
            if ((yagMatchInversion(loopcone, ptcone, NULL, NULL) != PM_UNKNOWN)
            || (YAG_CONTEXT->YAG_BLEEDER_STRICTNESS == 0 && (loopcone->TECTYPE & CNS_CMOS) != 0)) {
                if (YAG_CONTEXT->YAG_BLEEDER_STRICTNESS > 1 && yagCountConeEdges(loopcone->INCONE) != 1) continue;
                for (i=0; i<4; i++) {
                    for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                        if ((ptbranch->TYPE & (CNS_VDD|CNS_VSS)) != 0 && yagCountActiveLinks(ptbranch->LINK) == 1) {
                            for (ptlink = ptbranch->LINK; (ptlink->TYPE & CNS_RESIST) == CNS_RESIST; ptlink = ptlink->NEXT);
                            if ((cone_list *)(ptlink->ULINK.LOTRS->GRID) == loopcone) {
                                if ((ptbranch->TYPE & looptype) != looptype) {
                                    looptype = -1L;
                                    break;
                                }
                                looptype |= (ptbranch->TYPE & (CNS_VDD|CNS_VSS));
                                maybe = addchain(maybe, ptbranch);
                            }
                        }
                        else {
                            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                                if ((cone_list *)(ptlink->ULINK.LOTRS->GRID) == loopcone) {
                                    if ((ptbranch->TYPE & looptype) != looptype) {
                                        looptype = -1L;
                                        break;
                                    }
                                    looptype |= (ptbranch->TYPE & (CNS_VDD|CNS_VSS));
                                }
                            }
                            if (looptype == -1L) break;
                        }
                    }
                    if (looptype == -1L) break;
                }
                if ((looptype == CNS_VDD || looptype == CNS_VSS) && maybe != NULL) {
                    bleeders = append(bleeders, maybe);
                    ptedge->TYPE |= CNS_BLEEDER;
                }
                else freechain(maybe);
            }
        }
    }

    if (bleeders != NULL) {
        for (ptchain = bleeders; ptchain; ptchain = ptchain->NEXT) {
            ptbranch = (branch_list *)(ptchain->DATA);
            ptbranch->TYPE |= CNS_BLEEDER|CNS_NOT_FUNCTIONAL;
            ptlink = ptbranch->LINK;
            ptlink->ULINK.LOTRS->TYPE |= BLEEDER;
        }
        ptcone->USER = addptype(ptcone->USER, CNS_BLEEDER, bleeders);
        ptcone->TYPE |= YAG_MARK;
    }
}

/****************************************************************************
 *                         function yagRmvThruBleed()                       *
 ****************************************************************************/

/* remove branches terminating with bleed transistors     */
/* and branches whose final link input is a bleeder input */

void
yagRmvThruBleed(cone_list *ptcone)
{
    branch_list *ptbranch;
    branch_list *ptnextbranch;
    link_list   *ptlink;
    branch_list *brlist[2];
    cone_list   *ptincone;
    edge_list   *ptinedge;
    int          changed = FALSE;
    int          i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;

    for (i=0; i<2; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptnextbranch) {
            ptnextbranch = ptbranch->NEXT;
            if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
            ptlink = ptbranch->LINK;
            if (ptlink->NEXT == NULL) continue;
            for (ptlink = ptlink->NEXT; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
            if ((ptlink->ULINK.LOTRS->TYPE & BLEEDER) != 0) {
                yagDestroyBranch(ptcone, ptbranch);
                changed = TRUE;
                continue;
            }
            ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            ptinedge = yagGetEdge(ptcone->INCONE, ptincone);
            if (ptinedge != NULL && (ptinedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) {
                yagDestroyBranch(ptcone, ptbranch);
                changed = TRUE;
            }
        }
    }
    if (changed) ptcone->TYPE |= YAG_MARK;
}

/*************************************************************************
 *                     function yagMarkLoopConf()                        *
 *************************************************************************/

void
yagMarkLoopConf(cone_list *ptcone)
{
    edge_list  *ptedgelist0, *ptedgelist1;
    edge_list  *ptedge0, *ptedge1;
    cone_list  *ptincone;

    if ((ptcone->TYPE & YAG_LOOPCONF) != 0) return;
    if ((ptcone->TYPE & YAG_PARTIAL) == 0) return;
    
    ptedgelist0 = (edge_list *)getptype(ptcone->USER, YAG_INPUTS_PTYPE)->DATA;
    for (ptedge0 = ptedgelist0; ptedge0; ptedge0 = ptedge0->NEXT) {
        if ((ptedge0->TYPE & CNS_EXT) != 0) continue;
        if ((ptedge0->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        ptincone = ptedge0->UEDGE.CONE;
        if ((ptincone->TYPE & YAG_PARTIAL) != 0) {
            ptedgelist1 = (edge_list *)getptype(ptincone->USER, YAG_INPUTS_PTYPE)->DATA;
        }
        else ptedgelist1 = ptincone->INCONE;
        ptedge1 = yagGetEdge(ptedgelist1, ptcone);
        if (ptedge1 != NULL) {
            if ((ptedge1->TYPE & CNS_BLEEDER) != CNS_BLEEDER) {
	            ptcone->TYPE |= YAG_LOOPCONF;
	            if ((ptincone->TYPE & YAG_PARTIAL) != 0) ptincone->TYPE |= YAG_LOOPCONF;
	            break;
            }
        }
    }
}  

/*************************************************************************
 *                     function yagMatchLatch()                          *
 *************************************************************************/

/* general latch pattern-matching entry point */

void
yagMatchLatch(cone_list *ptcone)
{
    chain_list *looplist;
    chain_list *ptchain;
    cone_list  *loopcone;
    int         found;
    
    if ((looplist = yagCheckLoop(ptcone, FALSE)) != NULL) {
        for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
            loopcone = (cone_list *)ptchain->DATA;
            if ((loopcone->TECTYPE & YAG_NOTLATCH) == YAG_NOTLATCH) continue;
            avt_log(LOGYAG,1, "Loop detected between '%s' and '%s'\n",ptcone->NAME,loopcone->NAME);
            found = checkLatch(ptcone, loopcone, FALSE);
            if (found) avt_log(LOGYAG,1, "\tLatch found\n");
            if (!found) {
                found = checkMemory(ptcone, loopcone);
                if (found) avt_log(LOGYAG,1, "\tMemory found\n");
            }
            if (!found) {
                found = checkDiffLatch(ptcone, loopcone);
                if (found) avt_log(LOGYAG,1, "\tMemory found\n");
            }
            if (!found) {
                avt_log(LOGYAG,1, "\tNothing found\n");
            }
        }
    }
    freechain(looplist);
}

void
yagMatchSimpleLatch(cone_list *ptcone)
{
    chain_list *looplist;
    chain_list *ptchain;
    cone_list  *loopcone;
    int         found;
    
    if ((looplist = yagCheckLoop(ptcone, FALSE)) != NULL) {
        for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
            loopcone = (cone_list *)ptchain->DATA;
            if ((loopcone->TECTYPE & YAG_NOTLATCH) == YAG_NOTLATCH) continue;
            avt_log(LOGYAG,1, "Loop detected between '%s' and '%s'\n",ptcone->NAME,loopcone->NAME);
            found = checkLatch(ptcone, loopcone, TRUE);
            if (!found) {
                avt_log(LOGYAG,1, "\tNothing found\n");
            }
        }
    }
    freechain(looplist);
}

/*************************************************************************
 *                       function checkLoop()                            *
 *************************************************************************/

/* check if given cone is part of a two cone loop */
/* and return the other cone of the loop.         */

chain_list *
yagCheckLoop(ptcone, fIgnoreFalse)
    cone_list  *ptcone;
    int         fIgnoreFalse;
{
    edge_list  *ptinput;
    edge_list  *ptloopin;
    cone_list  *ptincone;
    chain_list *looplist = NULL;
    
    for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
        if (fIgnoreFalse && (ptinput->TYPE & YAG_FALSECONF) == YAG_FALSECONF) continue;
        if ((ptinput->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptinput->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
        if ((ptinput->TYPE & CNS_EXT) == CNS_EXT) continue;
        ptincone = ptinput->UEDGE.CONE;
        if ((ptincone->TYPE & (CNS_LATCH|CNS_MEMSYM)) != 0) continue;
        if ((ptloopin = yagGetEdge(ptincone->INCONE, ptcone)) != NULL) {
            if ((ptloopin->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
            looplist = addchain(looplist, ptincone);
        }
    }
    return looplist;
}

/*************************************************************************
 *                       function checkLatch()                           *
 *************************************************************************/

/* test whether two cones form a standard latch */

static int
checkLatch(cone_list *ptcone0, cone_list *ptcone1, int simple)
{
    float       strength0, strength1;
    chain_list *translist0 = NULL;
    chain_list *translist1 = NULL;
    chain_list *translist = NULL;
    int         type0, type1;
    
    type1 = yagMatchInversion(ptcone1, ptcone0, &strength1, &translist1);
    type0 = yagMatchInversion(ptcone0, ptcone1, &strength0, &translist0);
    if (type0 == PM_UNKNOWN && type1 == PM_UNKNOWN) {
        freechain(translist0);
        freechain(translist1);
        return FALSE;
    }
    
    if (simple) {
        if (type0 == PM_INV && type1 == PM_INV && (ptcone1->TYPE & YAG_PARTIAL) == 0 && yagCountEdges(ptcone1->INCONE) == 1) {
            addLatchType(ptcone0, ptcone1, translist0, type0, type1);
            freechain(translist0);
            freechain(translist1);
            return TRUE;
        }
        else {
            freechain(translist0);
            freechain(translist1);
            return FALSE;
        }
    }

    /* check for simple double inversion case (beware if other cone is precharge) */
    if (type0 != PM_UNKNOWN && type1 != PM_UNKNOWN) {
        if ((strength0/strength1) < 0.8) {
            addLatchType(ptcone0, ptcone1, translist0, type0, type1);
        }
        else if ((strength1/strength0) > 1.2 && (ptcone1->TYPE & CNS_PRECHARGE) == 0) {
            addLatchType(ptcone1, ptcone0, translist1, type1, type0);
        }
        else if (yagCountEdges(ptcone0->INCONE) > 1 && type0 == PM_INV && yagCountEdges(ptcone1->INCONE) == 1 && type1 == PM_INV) {
            addLatchType(ptcone0, ptcone1, translist0, type0, type1);
        }
        else if (yagCountEdges(ptcone0->INCONE) == 1 && type0 == PM_INV && yagCountEdges(ptcone1->INCONE) > 1 && type1 == PM_INV && (ptcone1->TYPE & CNS_PRECHARGE) == 0) {
            addLatchType(ptcone1, ptcone0, translist1, type1, type0);
        }
        else {
            freechain(translist0);
            freechain(translist1);
            return FALSE;
        }
    }
    /* check for switched or tri-state inverter feedback */
    else if (type0 != PM_UNKNOWN) {
        if ((type1 = yagMatchSwitchedInversion(ptcone1, ptcone0, &translist)) != FB_UNKNOWN) {
            addLatchType(ptcone1, ptcone0, translist, type1, type0);
        }
        else {
            freechain(translist0);
            freechain(translist1);
            freechain(translist);
            return FALSE;
        }
    }
    else {
        if ((type0 = yagMatchSwitchedInversion(ptcone0, ptcone1, &translist)) != FB_UNKNOWN) {
            addLatchType(ptcone0, ptcone1, translist, type0, type1);
        }
        else {
            freechain(translist0);
            freechain(translist1);
            freechain(translist);
            return FALSE;
        }
    }

    freechain(translist0);
    freechain(translist1);
    freechain(translist);
    return TRUE;
}                    

/*************************************************************************
 *                       function addLatchType()                         *
 *************************************************************************/

/* add the necessary type fields to latch cone, branches and links */

static void
addLatchType(cone_list *ptcone, cone_list *feedbackcone, chain_list *feedbacktrans, int feedbacktype, int forwardtype)
{
    branch_list    *ptbranch;
    branch_list    *brlist[4];
    link_list      *ptlink;
    edge_list      *ptedge;
    long            latchtype;
    int             i;

    ptcone->TYPE |= CNS_LATCH;
    ptcone->TYPE |= YAG_MARK;

    switch (forwardtype) {
        case PM_INV: latchtype = FW_INV; break;
        case PM_NAND: latchtype = FW_NAND; break;
        case PM_NOR: latchtype = FW_NOR; break;
    }
    switch (feedbacktype) {
        case PM_INV: latchtype |= FB_INV; break;
        case PM_NAND: latchtype |= FB_NAND; break;
        case PM_NOR: latchtype |= FB_NOR; break;
        case PM_TRISTATE: latchtype |= FB_TRISTATE; break;
        case PM_PASSINV: latchtype |= FB_PASSINV; break;
        case PM_SWITCHINV: latchtype |= FB_SWITCHINV; break;
        case PM_PASSNAND: latchtype |= FB_PASSNAND; break;
        case PM_SWITCHNAND: latchtype |= FB_SWITCHNAND; break;
        case PM_PASSNOR: latchtype |= FB_PASSNOR; break;
        case PM_SWITCHNOR: latchtype |= FB_SWITCHNOR; break;
    }

    ptcone->USER = addptype(ptcone->USER, YAG_LATCHINFO_PTYPE, (void *)latchtype);

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<2; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if (yagGetChain(feedbacktrans, ptlink->ULINK.LOTRS) == NULL) break;
            }
            if (ptlink == NULL) ptbranch->TYPE |= (CNS_NOT_FUNCTIONAL|CNS_FEEDBACK);
        }
    }

    ptedge = yagGetEdge(ptcone->INCONE, feedbackcone);
    ptedge->TYPE |= CNS_FEEDBACK;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_FEEDBACK) != 0) continue;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT) continue;
                if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP) continue;
                if ((ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN) continue;
                if ((ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK)) != 0) {
                    ptlink->TYPE |= CNS_COMMAND;
                    ptedge = yagGetEdge(ptcone->INCONE, (cone_list *)ptlink->ULINK.LOTRS->GRID);
                    ptedge->TYPE |= CNS_COMMAND;
                    break;
                }
            }
        }
    }
}
  
/*************************************************************************
 *                       function checkMemory()                          *
 *************************************************************************/

/* test whether two cones form a symmetric memory point */

static int
checkMemory(cone_list *ptcone0, cone_list *ptcone1)
{
    cone_list      *ptincone;
    branch_list    *ptbranch;
    link_list      *ptlink;
    chain_list     *ptpossiblecommands = NULL;
    chain_list     *ptcommands = NULL;
    cone_list      *ptcommand = NULL;
    edge_list      *command;
    edge_list      *ptinedge;
    chain_list     *ptchain;
    branch_list    *brlist0[4];
    branch_list    *brlist1[4];
    int             commanded = FALSE;
    float           ratio;
    int             i;

    if (yagMatchNOT(ptcone0, ptcone1) == TRUE && yagMatchNOT(ptcone1, ptcone0) == TRUE) {
        ratio = yagInverterStrength(ptcone1, ptcone0)/yagInverterStrength(ptcone0, ptcone1);
        if (ratio > 0.8 && ratio < 1.2) {

            brlist0[0] = ptcone0->BRVDD;
            brlist0[1] = ptcone0->BRVSS;
            brlist0[2] = ptcone0->BREXT;
            brlist0[3] = ptcone0->BRGND;

            brlist1[0] = ptcone1->BRVDD;
            brlist1[1] = ptcone1->BRVSS;
            brlist1[2] = ptcone1->BREXT;
            brlist1[3] = ptcone1->BRGND;

            /* Generate list of possible command inputs on Cone 0 */

            for (i=0; i<4; i++) {
                for (ptbranch = brlist0[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                    ptlink = ptbranch->LINK;
                    if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                    ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                    if (ptincone != ptcone1) {
                        if (yagGetChain(ptpossiblecommands, ptincone) == NULL) {
                            ptpossiblecommands = addchain(ptpossiblecommands, ptincone);
                        }
                    }
                }
            }

            /* Check for command on Cone 1 identical to one on Cone 0 */

            for (i=0; i<4; i++) {
                for (ptbranch = brlist1[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                    ptlink = ptbranch->LINK;
                    if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                    ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                    if (ptincone != ptcone0) {
                        if (yagGetChain(ptpossiblecommands, ptincone) != NULL) {
                            commanded = TRUE;
                        }
                    }
                }
            }
            freechain(ptpossiblecommands);
            if (commanded == FALSE) return FALSE;

            /* list of commands is intersection of INCONE lists */

            for (ptinedge = ptcone0->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
                if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
                if (yagGetEdge(ptcone1->INCONE, ptinedge->UEDGE.CONE) != NULL) {
                    ptcommands = addchain(ptcommands, ptinedge->UEDGE.CONE);
                }
            }

            /* memory detected so mark accordingly */

            ptcone0->TYPE |= CNS_MEMSYM;
            ptcone1->TYPE |= CNS_MEMSYM;
            ptinedge = yagGetEdge(ptcone0->INCONE, ptcone1);
            if (ptinedge) ptinedge->TYPE |= CNS_MEMSYM;
            ptinedge = yagGetEdge(ptcone1->INCONE, ptcone0);
            if (ptinedge) ptinedge->TYPE |= CNS_MEMSYM;
            ptcone0->USER = addptype(ptcone0->USER, YAG_LATCHINFO_PTYPE, (void *)(FW_INV|FB_INV));
            ptcone1->USER = addptype(ptcone1->USER, YAG_LATCHINFO_PTYPE, (void *)(FW_INV|FB_INV));

            for (ptchain = ptcommands; ptchain; ptchain = ptchain->NEXT) {
                ptcommand = (cone_list *)ptchain->DATA;
                command = yagGetEdge(ptcone0->INCONE, ptcommand);
                command->TYPE |= CNS_COMMAND;
                command = yagGetEdge(ptcone1->INCONE, ptcommand);
                command->TYPE |= CNS_COMMAND;
            }

            for (i=0; i<4; i++) {
                for (ptbranch = brlist0[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                        if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                        ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                        if (ptincone != ptcone1 && yagGetChain(ptcommands, ptlink->ULINK.LOTRS->GRID) != NULL) {
                            ptlink->TYPE |= CNS_COMMAND;
                        }
                    }
                }
            }

            for (i=0; i<4; i++) {
                for (ptbranch = brlist1[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                        if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                        ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                        if (ptincone != ptcone0 && yagGetChain(ptcommands, ptlink->ULINK.LOTRS->GRID) != NULL) {
                            ptlink->TYPE |= CNS_COMMAND;
                        }
                    }
                }
            }

            ptcone0->TYPE |= YAG_MARK;
            ptcone1->TYPE |= YAG_MARK;

            freechain(ptcommands);
            return TRUE;
        }
    }
    return FALSE;
}

/*************************************************************************
 *                       function checkDiffLatch()                       *
 *************************************************************************/

/* test whether two cones form a differential latch */

static int
checkDiffLatch(cone_list *ptcone0, cone_list *ptcone1)
{
    cone_list      *ptincone;
    branch_list    *ptbranch;
    link_list      *ptlink;
    chain_list     *ptpossiblecommands = NULL;
    chain_list     *ptcommands = NULL;
    cone_list      *ptcommand = NULL;
    edge_list      *command, *ptinedge;
    chain_list     *ptchain;
    branch_list    *brlist0[2];
    branch_list    *brlist1[2];
    float           ratio;
    int             i;

    if (yagMatchNOT(ptcone0, ptcone1) == TRUE && yagMatchNOT(ptcone1, ptcone0) == TRUE) {
        ratio = yagInverterStrength(ptcone1, ptcone0)/yagInverterStrength(ptcone0, ptcone1);
        if (ratio > 0.8 && ratio < 1.2) {

            brlist0[0] = ptcone0->BRVDD;
            brlist0[1] = ptcone0->BRVSS;

            brlist1[0] = ptcone1->BRVDD;
            brlist1[1] = ptcone1->BRVSS;

            /* Generate list of possible command inputs on Cone 0 */

            for (ptbranch = ptcone0->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
                ptlink = ptbranch->LINK;
                if (ptlink->NEXT == NULL) continue;
                ptlink = ptlink->NEXT;
                if (ptlink->NEXT != NULL) continue;
                if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                if (ptincone != ptcone1) {
                    if (yagGetChain(ptpossiblecommands, ptincone) == NULL) {
                        ptpossiblecommands = addchain(ptpossiblecommands, ptincone);
                    }
                }
            }

            /* Check for command on Cone 1 identical to one on Cone 0 */

            for (ptbranch = ptcone1->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
                ptlink = ptbranch->LINK;
                if (ptlink->NEXT == NULL) continue;
                ptlink = ptlink->NEXT;
                if (ptlink->NEXT != NULL) continue;
                if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                if (ptincone != ptcone0) {
                    if (yagGetChain(ptpossiblecommands, ptincone) != NULL) {
                        ptcommands = addchain(ptcommands, ptincone);
                    }
                }
            }
            freechain(ptpossiblecommands);

            if (ptcommands == NULL) return FALSE;

            /* memory detected so mark accordingly */

            ptcone0->TYPE |= CNS_MEMSYM;
            ptcone1->TYPE |= CNS_MEMSYM;
            ptinedge = yagGetEdge(ptcone0->INCONE, ptcone1);
            if (ptinedge) ptinedge->TYPE |= CNS_MEMSYM;
            ptinedge = yagGetEdge(ptcone1->INCONE, ptcone0);
            if (ptinedge) ptinedge->TYPE |= CNS_MEMSYM;
            ptcone0->USER = addptype(ptcone0->USER, YAG_LATCHINFO_PTYPE, (void *)(FW_INV|FB_INV|DIFF));
            ptcone1->USER = addptype(ptcone1->USER, YAG_LATCHINFO_PTYPE, (void *)(FW_INV|FB_INV|DIFF));

            for (ptchain = ptcommands; ptchain; ptchain = ptchain->NEXT) {
                ptcommand = (cone_list *)ptchain->DATA;
                command = yagGetEdge(ptcone0->INCONE, ptcommand);
                command->TYPE |= CNS_COMMAND;
                command = yagGetEdge(ptcone1->INCONE, ptcommand);
                command->TYPE |= CNS_COMMAND;
            }

            for (i=0; i<2; i++) {
                for (ptbranch = brlist0[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                    ptlink = ptbranch->LINK;
                    if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                    ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                    if (ptincone == ptcone1) continue;
                    if (ptlink->NEXT == NULL) continue;
                    ptlink = ptlink->NEXT;
                    if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                    if (yagGetChain(ptcommands, ptlink->ULINK.LOTRS->GRID) != NULL) {
                        ptlink->TYPE |= CNS_COMMAND;
                    }
                }
            }

            for (i=0; i<2; i++) {
                for (ptbranch = brlist1[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                    ptlink = ptbranch->LINK;
                    if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                    ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                    if (ptincone == ptcone0) continue;
                    if (ptlink->NEXT == NULL) continue;
                    ptlink = ptlink->NEXT;
                    if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) continue;
                    if (yagGetChain(ptcommands, ptlink->ULINK.LOTRS->GRID) != NULL) {
                        ptlink->TYPE |= CNS_COMMAND;
                    }
                }
            }

            ptcone0->TYPE |= YAG_MARK;
            ptcone1->TYPE |= YAG_MARK;

            freechain(ptcommands);
            return TRUE;
        }
    }
    return FALSE;
}

/****************************************************************************
 *                     function yagPairMemsym();                            *
 ****************************************************************************/

void
yagPairMemsym(inffig_list *ifl, cone_list *ptcone)
{
    cone_list  *ptsymcone, *pttestcone;
    losig_list *ptsig;
    chain_list *looplist = NULL;
    chain_list *ptchain;
    edge_list  *ptinput;
    edge_list  *ptloopin;
    cone_list  *ptincone;
    chain_list *ptinflist;
    char       *ptsymname = NULL;
    int         numfound = 0;
    
    if (inf_GetPointer(ifl, INF_MEMSYM, "", (void **)&ptinflist)) {
        ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
        for (ptchain = ptinflist; ptchain; ptchain = ptchain->NEXT) {
            if (mbk_LosigTestREGEX(ptsig, ((inf_assoc *)ptchain->DATA)->orig)) {
                ptsymname = ((inf_assoc *)ptchain->DATA)->dest;
            }
            if (mbk_LosigTestREGEX(ptsig, ((inf_assoc *)ptchain->DATA)->dest)) {
                ptsymname = ((inf_assoc *)ptchain->DATA)->orig;
            }
        }
    }

    for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
        if ((ptinput->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptinput->TYPE & CNS_EXT) == CNS_EXT) continue;
        ptincone = ptinput->UEDGE.CONE;
        if ((ptloopin = yagGetEdge(ptincone->INCONE, ptcone)) != NULL) {
            if ((ptloopin->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
            looplist = addchain(looplist, ptincone);
        }
    }

    if (looplist != NULL) {
        for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
            pttestcone = (cone_list *)ptchain->DATA;
            if ((pttestcone->TYPE & CNS_MEMSYM) != 0) {
                if (ptsymname) {
                    ptsig = (losig_list *)getptype(pttestcone->USER, CNS_SIGNAL)->DATA;
                    if (mbk_LosigTestREGEX(ptsig, ptsymname)) {
                        ptsymcone = pttestcone;
                        numfound = 1;
                        break;
                    }
                }
                numfound++;
                ptsymcone = pttestcone;
            }
        }
        if (numfound == 1) {
            ptcone->USER = addptype(ptcone->USER, YAG_MEMORY_PTYPE, ptsymcone);
            ptsymcone->USER = addptype(ptsymcone->USER, YAG_MEMORY_PTYPE, ptcone);
            ptcone->TYPE |= CNS_LATCH;
            ptsymcone->TYPE |= CNS_LATCH;
            ptloopin = yagGetEdge(ptcone->INCONE, ptsymcone);
            ptloopin->TYPE |= CNS_MEMSYM;
            ptloopin = yagGetEdge(ptsymcone->INCONE, ptcone);
            ptloopin->TYPE |= CNS_MEMSYM;
        }
        freechain(looplist);
    }
}
    
/*************************************************************************
 *                       function yagCheckBistable()                     *
 *************************************************************************/

/* test whether two cones form a double NAND/NOR bistable */

static int
yagCheckBistable(cone_list *ptcone0, cone_list *ptcone1)
{
    if (yagMatchNAND(ptcone0,ptcone1) > 0) {
        if (yagMatchNAND(ptcone1, ptcone0) > 0) {
            ptcone0->TYPE |= CNS_RS;
            ptcone0->TECTYPE |= CNS_NAND;
            ptcone0->USER = addptype(ptcone0->USER, YAG_BISTABLE_PTYPE, ptcone1);
            ptcone0->USER = addptype(ptcone0->USER, YAG_LATCHINFO_PTYPE, (void *)BISTABLE_NAND);
            ptcone1->TYPE |= CNS_RS;
            ptcone1->TECTYPE |= CNS_NAND;
            ptcone1->USER = addptype(ptcone1->USER, YAG_BISTABLE_PTYPE, ptcone0);
            ptcone1->USER = addptype(ptcone1->USER, YAG_LATCHINFO_PTYPE, (void *)BISTABLE_NAND);
            return TRUE;
        }
    }
    if (yagMatchNOR(ptcone0,ptcone1) > 0) {
        if (yagMatchNOR(ptcone1, ptcone0) > 0) {
            ptcone0->TYPE |= CNS_RS;
            ptcone0->TECTYPE |= CNS_NOR;
            ptcone0->USER = addptype(ptcone0->USER, YAG_BISTABLE_PTYPE, ptcone1);
            ptcone0->USER = addptype(ptcone0->USER, YAG_LATCHINFO_PTYPE, (void *)BISTABLE_NOR);
            ptcone1->TYPE |= CNS_RS;
            ptcone1->TECTYPE |= CNS_NOR;
            ptcone1->USER = addptype(ptcone1->USER, YAG_BISTABLE_PTYPE, ptcone0);
            ptcone1->USER = addptype(ptcone1->USER, YAG_LATCHINFO_PTYPE, (void *)BISTABLE_NOR);
            return TRUE;
        }
    }
    return FALSE;
}

/*************************************************************************
 *                     function yagMatchNAND()                           *
 *************************************************************************/

/* check if NAND exists between two cones and return # of inputs */

int
yagMatchNAND(cone_list *ptcone, cone_list *ptincone)
{
    chain_list    *ptconechain = NULL;
    chain_list    *ptchain;
    branch_list   *ptbranch;
    link_list     *ptlink, *ptdrivenlink;
    int            numinputs = 0;

    /* search for down branch driven by incone */
    /* and containing only N type transistors  */

    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) continue;
        ptdrivenlink = NULL;
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_TPLINK) != 0) break;
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                ptdrivenlink = ptlink;
            }
        }
        if (ptlink == NULL && ptdrivenlink != NULL) break;
    }

    if (ptbranch == NULL) return 0;

    /* build list if NAND inputs */
    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        ptconechain = addchain(ptconechain, ptlink->ULINK.LOTRS->GRID);
        numinputs++;
    }

    /* search for single link up branches for each input */
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if (ptlink->NEXT != NULL) continue;
        if ((ptlink->TYPE & CNS_TPLINK) == 0) continue;
        if ((ptchain = yagGetChain(ptconechain, ptlink->ULINK.LOTRS->GRID)) != NULL) {
            ptconechain = delchain(ptconechain, ptchain);
        }
    }

    if (ptconechain == NULL) return numinputs;
    else {
        freechain(ptconechain);
        return 0;
    }
}

/*************************************************************************
 *                       function yagMatchNOR()                          *
 *************************************************************************/

/* check if NOR exists between two cones and return # of inputs */

int
yagMatchNOR(cone_list *ptcone, cone_list *ptincone)
{
    chain_list    *ptconechain = NULL;
    chain_list    *ptchain;
    branch_list   *ptbranch;
    link_list     *ptlink, *ptdrivenlink;
    int            numinputs = 0;

    /* search for up branch driven by incone   */
    /* and containing only P type transistors  */

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) continue;
        ptdrivenlink = NULL;
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_TNLINK) != 0) break;
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                ptdrivenlink = ptlink;
            }
        }
        if (ptlink == NULL && ptdrivenlink != NULL) break;
    }

    if (ptbranch == NULL) return 0;

    /* build list if NOR inputs */
    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        ptconechain = addchain(ptconechain, ptlink->ULINK.LOTRS->GRID);
        numinputs++;
    }

    /* search for single link down branches for each input */
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if (ptlink->NEXT != NULL) continue;
        if ((ptlink->TYPE & CNS_TNLINK) == 0) continue;
        if ((ptchain = yagGetChain(ptconechain, ptlink->ULINK.LOTRS->GRID)) != NULL) {
            ptconechain = delchain(ptconechain, ptchain);
        }
    }

    if (ptconechain == NULL) return numinputs;
    else {
        freechain(ptconechain);
        return 0;
    }
}

/****************************************************************************
 *                     function yagSelectRSLatch();                         *
 ****************************************************************************/

void
yagSelectRSLatch(cone_list *ptcone0, cone_list *ptcone1)
{
    cone_list  *ptlatchcone;
    cone_list  *ptfeedbackcone;
    edge_list  *ptedge;

    if (yagCountEdges(ptcone0->OUTCONE) >= yagCountEdges(ptcone1->OUTCONE)) {
        ptlatchcone = ptcone0;
        ptfeedbackcone = ptcone1;
    }
    else {
        ptlatchcone = ptcone1;
        ptfeedbackcone = ptcone0;
    }

    ptlatchcone->TYPE |= CNS_LATCH;
    ptedge = yagGetEdge(ptlatchcone->INCONE, ptfeedbackcone);
    ptedge->TYPE |= CNS_FEEDBACK;
    ptedge = yagGetEdge(ptfeedbackcone->OUTCONE, ptlatchcone);
    ptedge->TYPE |= CNS_FEEDBACK;
}

/*************************************************************************
 *                       function yagInverterStrength()                  *
 *************************************************************************/

/* returns the W/L of a CMOS inverter cone */

float
yagInverterStrength(cone_list *ptcone0, cone_list *ptcone1)
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    lotrs_list     *pttrans;
    float           test_strength;

    /* search for required link in down branches */
    for (ptbranch = ptcone1->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptcone0) {
                pttrans = ptlink->ULINK.LOTRS;
                test_strength = ((float)pttrans->WIDTH / (float)pttrans->LENGTH);
                return test_strength;
            }
        }
    }
    return 0.0;
}

/*************************************************************************
 *                      function yagMatchNOT()                           *
 *************************************************************************/

/* test whether a simple inverter   */
/* lies between the two given cones */

int
yagMatchNOT(cone_list *ptcone, cone_list *ptincone)
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    cone_list      *ptlinkcone;

    /* search for up branch */
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_TPLINK) == 0) break;
            ptlinkcone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            if (ptlinkcone != ptincone && (ptlinkcone->TYPE & CNS_VSS) == 0) break;
        }
        if (ptlink == NULL) break;
    }

    if (ptbranch == NULL) return FALSE;

    /* search for corresponding down branch */
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_TNLINK) == 0) break;
            ptlinkcone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            if (ptlinkcone != ptincone && (ptlinkcone->TYPE & CNS_VDD) == 0) break;
        }
        if (ptlink == NULL) return TRUE;
    }
    return FALSE;
}

/*************************************************************************
 *                       function isTristate()                           *
 *************************************************************************/

/* test whether a simple tristate   */
/* lies between the two given cones */

static int
isTristate(cone_list *ptcone, cone_list *ptincone, chain_list **pttranslist)
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    chain_list     *templist = NULL;
    chain_list     *ptchain;
    int             found = FALSE;

    /* search for up branch */
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if (ptlink->NEXT == NULL) continue;
        if ((ptlink->TYPE & CNS_TPLINK) == 0) continue;
        ptlink = ptlink->NEXT;
        if (ptlink->NEXT != NULL) continue;
        if ((ptlink->TYPE & CNS_TPLINK) == 0) continue;
        if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                templist = addchain(templist, ptlink->ULINK.LOTRS);
            }
            found = TRUE;
        }
    }

    if (found == FALSE) return FALSE;

    found = FALSE;
    /* search for corresponding down branch */
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if (ptlink->NEXT == NULL) continue;
        if ((ptlink->TYPE & CNS_TNLINK) == 0) continue;
        ptlink = ptlink->NEXT;
        if (ptlink->NEXT != NULL) continue;
        if ((ptlink->TYPE & CNS_TNLINK) == 0) continue;
        if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                templist = addchain(templist, ptlink->ULINK.LOTRS);
            }
            found = TRUE;
        }
    }
    if (found) {
        for (ptchain = templist; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(*pttranslist, ptchain->DATA) == NULL) {
                *pttranslist = addchain(*pttranslist, ptchain->DATA);
            }
        }
        freechain(templist);
        return TRUE;
    }
    else return FALSE;
}

/*************************************************************************
 *                     function yagMatchInversion()                      *
 *************************************************************************/

/* check for simple inversion (INV, NAND or NOR)  */
/* between two cones and return corresponding W/L */

int
yagMatchInversion(cone_list *ptcone, cone_list *ptincone, float *ptstrength, chain_list **pttranslist)
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    lotrs_list     *pttrans = NULL;
    cone_list      *ptgatecone = NULL;
    chain_list     *templist = NULL;
    chain_list     *ptchain;
    int             fConfirmNOR, fConfirmNAND;
    int             matchtype;

    /* search for up branch */
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        fConfirmNOR = FALSE;
        ptlink = ptbranch->LINK;
        if ((ptlink->TYPE & CNS_TPLINK) == 0) continue;
        if (ptlink->NEXT != NULL) {
            if ((ptlink->NEXT->TYPE & CNS_TPLINK) == 0) continue;
            if (ptlink->NEXT->NEXT != NULL) continue;
            fConfirmNOR = TRUE;
        }
        if (fConfirmNOR == FALSE) {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) break;
        }
        else {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                ptgatecone = (cone_list *)ptlink->NEXT->ULINK.LOTRS->GRID;
                break;
            }
            else if ((cone_list *)ptlink->NEXT->ULINK.LOTRS->GRID == ptincone) {
                ptgatecone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                break;
            }
        }
    }
    if (ptbranch == NULL) {
        if (ptstrength != NULL) *ptstrength = 0.0;
        return PM_UNKNOWN;
    }

    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        templist = addchain(templist, ptlink->ULINK.LOTRS);
    }

    /* search for down branch */
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        fConfirmNAND = FALSE;
        ptlink = ptbranch->LINK;
        if ((ptlink->TYPE & CNS_TNLINK) == 0) continue;
        if (ptlink->NEXT != NULL) {
            if (fConfirmNOR == TRUE) continue;
            if ((ptlink->NEXT->TYPE & CNS_TNLINK) == 0) continue;
            if (ptlink->NEXT->NEXT != NULL) continue;
            fConfirmNAND = TRUE;
        }
        if (fConfirmNAND == FALSE) {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                pttrans = ptlink->ULINK.LOTRS;
                break;
            }
        }
        else {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                pttrans = ptlink->ULINK.LOTRS;
                ptgatecone = (cone_list *)ptlink->NEXT->ULINK.LOTRS->GRID;
                break;
            }
            else if ((cone_list *)ptlink->NEXT->ULINK.LOTRS->GRID == ptincone) {
                pttrans = ptlink->NEXT->ULINK.LOTRS;
                ptgatecone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                break;
            }
        } 
    }
    if (ptbranch == NULL) {
        if (ptstrength != NULL) *ptstrength = 0.0;
        freechain(templist);
        return PM_UNKNOWN;
    }

    /* confirm gatecone in down branches for NOR */
    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        templist = addchain(templist, ptlink->ULINK.LOTRS);
    }

    if (fConfirmNOR == TRUE) {
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            ptlink = ptbranch->LINK;
            if (ptlink->NEXT != NULL) continue;
            if ((ptlink->TYPE & CNS_TNLINK) == 0) continue;
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptgatecone) break;
        }
        if (ptbranch == NULL) {
            if (ptstrength != NULL) *ptstrength = 0.0;
            freechain(templist);
            return PM_UNKNOWN;
        }
        matchtype = PM_NOR;

        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            templist = addchain(templist, ptlink->ULINK.LOTRS);
        }
    }

    /* confirm gatecone in up branches for NAND */
    else if (fConfirmNAND == TRUE) {
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            ptlink = ptbranch->LINK;
            if (ptlink->NEXT != NULL) continue;
            if ((ptlink->TYPE & CNS_TPLINK) == 0) continue;
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptgatecone) break;
        }
        if (ptbranch == NULL) {
            if (ptstrength != NULL) *ptstrength = 0.0;
            freechain(templist);
            return PM_UNKNOWN;
        }
        matchtype = PM_NAND;

        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            templist = addchain(templist, ptlink->ULINK.LOTRS);
        }
    }

    else matchtype = PM_INV;

    if (ptstrength != NULL) {
        *ptstrength = ((float)pttrans->WIDTH / (float)pttrans->LENGTH);
    }
    if (pttranslist != NULL) {
        for (ptchain = templist; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(*pttranslist, ptchain->DATA) == NULL) {
                *pttranslist = addchain(*pttranslist, ptchain->DATA);
            }
        }
    }
    freechain(templist);
    return matchtype;
}

/*************************************************************************
 *                  function yagMatchSwitchedInversion()                 *
 *************************************************************************/

/* check for inverted feedback (INV, NAND, NOR) */
/* via a pass-transistor, a switch or tri-state */

int
yagMatchSwitchedInversion(cone_list *ptcone, cone_list *ptincone, chain_list **pttranslist)
{
    branch_list       *ptbranch;
    link_list         *ptlink;
    lotrs_list        *pttrans;
    losig_list        *ptsig, *ptnextsig;
    cone_list         *ptdualcone;
    ptype_list        *ptuser;
    float              strength;
    long               linktype = 0;
    int                type, dualtype = PM_UNKNOWN;

    ptsig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX);
    
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        ptlink = ptbranch->LINK;
        if (ptlink->NEXT == NULL) continue;
        pttrans = ptlink->ULINK.LOTRS;
        if (pttrans->DRAIN->SIG != ptsig) ptnextsig = pttrans->DRAIN->SIG;
        else ptnextsig = pttrans->SOURCE->SIG;
        ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE);
        if (ptuser == NULL) continue;
        ptdualcone = (cone_list *)ptuser->DATA;
        type = yagMatchInversion(ptdualcone, ptincone, &strength, pttranslist);
        if (type == PM_UNKNOWN) continue;
        dualtype = type;
        linktype |= (ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK));
        if (yagGetChain(*pttranslist, ptlink->ULINK.LOTRS) == NULL) {
            *pttranslist = addchain(*pttranslist, ptlink->ULINK.LOTRS);
        }
    }

    if (dualtype != PM_UNKNOWN) {
        if (linktype == (CNS_TNLINK|CNS_TPLINK)) {
            switch (dualtype) {
                case PM_INV: return PM_SWITCHINV;
                case PM_NAND: return PM_SWITCHNAND;
                case PM_NOR: return PM_SWITCHNOR;
            }
        }
        else if (linktype == CNS_TNLINK) {
            switch (dualtype) {
                case PM_INV: return PM_PASSINV;
                case PM_NAND: return PM_PASSNAND;
                case PM_NOR: return PM_PASSNOR;
            }
        }
        else return PM_UNKNOWN;
    }

    if (isTristate(ptcone, ptincone, pttranslist)) return PM_TRISTATE;
    else return PM_UNKNOWN;
}

/*************************************************************************
 *                       function yagIsInverse()                         *
 *************************************************************************/

int
yagIsInverse(cone_list *ptcone, cone_list *ptincone)
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    int             found = FALSE;
    int             bad = FALSE;

    /* search for up branch */
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                found = TRUE;
                if ((ptlink->TYPE & CNS_TPLINK) == 0) bad = TRUE;
            }
        }
    }
                
    if (found == FALSE || bad == TRUE) return FALSE;

    found = FALSE;
    bad = FALSE;
    /* search for corresponding down branch */
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            if ((cone_list *)ptlink->ULINK.LOTRS->GRID == ptincone) {
                found = TRUE;
                if ((ptlink->TYPE & CNS_TNLINK) == 0) bad = TRUE;
            }
        }
    }
                
    if (found == FALSE || bad == TRUE) return FALSE;
    return TRUE;
}

/*************************************************************************
 *                       function yagRmvThruLatch()                      *
 *************************************************************************/

/* delete the branches which pass through latches */

void 
yagRmvThruLatch(cone_list *ptcone)
{
    branch_list    *ptbranch, *ptnextbranch = NULL;
    link_list      *ptlink;
    cone_list      *ptsigcone;
    losig_list     *ptsig, *ptnextsig, *ptlastsig;
    ptype_list     *ptuser;
    branch_list    *brlist[4];
    int             i, changed = FALSE;

    ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    avt_log(LOGYAG,1, "Check for latch branches on cone '%s'\n", ptcone->NAME);

    for (i = 0; i < 4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptnextbranch) {
            ptlastsig = ptsig;
            ptnextbranch = ptbranch->NEXT;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK)) == 0) continue;
                if ((ptnextsig = ptlink->ULINK.LOTRS->DRAIN->SIG) == ptlastsig) {
                    ptnextsig = ptlink->ULINK.LOTRS->SOURCE->SIG;
                }
                ptlastsig = ptnextsig;
                ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE);
                if (ptuser != NULL) {
                    ptsigcone = (cone_list *)ptuser->DATA;
                    if ((ptsigcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) == CNS_LATCH
                    || ((ptcone->TYPE & YAG_HASDUAL) != 0 && (ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) == 0 && (ptsigcone->TYPE & CNS_MEMSYM) != 0)
                    || (YAG_CONTEXT->YAG_MEMSYM_HEURISTIC && (ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) != 0 && (ptsigcone->TYPE & CNS_MEMSYM) != 0)) {
                        yagDestroyBranch(ptcone, ptbranch);
                        changed = TRUE;
                        break;
                    }
                }
            }
        }
    }
    /* re-verify the cone if branches have been removed */
    if (changed) {
        ptcone->TYPE |= YAG_MARK;
    }
}

/****************************************************************************
 *                         function invChain();                             *
 ****************************************************************************/
 /*-------------------------------------------------------------------------+
 | remonte les couches de cones  tant que ceux ci sont des inverseurs. Au   |
 | fur et a mesure, on constitue une liste chainee des cones remontes       |
 +-------------------------------------------------------------------------*/
static chain_list *
invChain(cone_list *ptcone, chain_list *ptchain)
{
    cone_list      *ptincone;
    edge_list      *ptin;
    int             numinputs = 0;

    for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_CONE) == CNS_CONE) {
            numinputs++;
            ptincone = ptin->UEDGE.CONE;
        }
    }

    if (numinputs != 1) return ptchain;

    if (yagGetChain(ptchain, ptincone) != NULL) return ptchain;
    if (yagMatchNOT(ptcone, ptincone) == FALSE) return ptchain;

    ptchain = addchain(ptchain, (void *)ptincone);
    ptchain = invChain(ptincone, ptchain);
    return  ptchain;
}

