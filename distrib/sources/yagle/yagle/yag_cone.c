/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_cone.c                                                  */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 14/05/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static long numcones;
static long numcompleted;

static int  THRULOOP;

static jmp_buf abandon_grow;
static branch_list *new_branches;

static void traverse_cone (inffig_list *ifl, cone_list *ptcone);
static void add_branches (cone_list *ptcone, branch_list *ptbranches);
static branch_list *grow_branch (inffig_list *ifl, cone_list *ptrootcone, branch_list *ptbranch, losig_list *ptsig);

static int traverse_depth;

long yagScanCones(inffig_list *ifl, cone_list *headcone)
{
    cone_list       *ptcone, *ptnextcone;
    int loglvl1=0;

#ifdef DELAY_DEBUG_STAT
    long lasttime=0, tmp;
#endif

    YAG_CONTEXT->YAG_ITERATIONS++;

    loglvl1 = avt_islog(1,LOGYAG);

    numcones = 0;
    numcompleted = 0;

    if ( loglvl1 ) {
        avt_log(LOGYAG,1, "\n\n");
        avt_log(LOGYAG,1, "Starting iteration %d\n\n", YAG_CONTEXT->YAG_ITERATIONS);
    }

/* cone list traversal */
    for (ptcone = headcone; ptcone != NULL; ptcone = ptnextcone) {
        ptnextcone = ptcone->NEXT;

        if ((ptcone->TYPE & YAG_PARTIAL) != 0) {
            traverse_depth = 0;
            traverse_cone(ifl, ptcone);
        }
#ifdef DELAY_DEBUG_STAT
        if (lasttime!=(tmp=time(NULL)))
        {                
          lasttime=tmp;
          fprintf(stdout, "\r[%d: %07ld/%07ld]", YAG_CONTEXT->YAG_ITERATIONS, numcompleted, numcones);
          fflush(stdout);
        }
#endif
    }

    for (ptcone = headcone; ptcone != NULL; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_CONESEEN;
    }

    if ( loglvl1 ) {
        avt_log(LOGYAG,1, "\n");
        avt_log(LOGYAG,1, "In iteration %d out of %ld partial cones :-\n", YAG_CONTEXT->YAG_ITERATIONS, numcones);
        avt_log(LOGYAG,1, "\t%ld cones completed\n", numcompleted);
    }

    return numcones-numcompleted;
}

static
void traverse_cone(inffig_list *ifl, cone_list *ptcone)
{
    edge_list   *ptedge;

    if ((ptcone->TYPE & YAG_LOOPCONF) != 0 && traverse_depth > 1) return;

    if ((ptcone->TYPE & YAG_PARTIAL) == 0 && 
        (ptcone->TECTYPE & CNS_DUAL_CMOS) == 0) return;

    if ((ptcone->TYPE & YAG_CONESEEN) != 0) return;
    ptcone->TYPE |= YAG_CONESEEN;

    if ((ptcone->TYPE & YAG_PARTIAL) != 0) {
        ptedge = (edge_list *)getptype(ptcone->USER, YAG_INPUTS_PTYPE)->DATA;
    }
    else ptedge = ptcone->INCONE;

    for (; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) continue;
        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;

        traverse_depth++;
        traverse_cone(ifl, ptedge->UEDGE.CONE);
        traverse_depth--;
    }

    if ((ptcone->TYPE & YAG_PARTIAL) != 0) {
        numcones++;

        avt_log(LOGYAG,1, "Expanding Cone %ld '%s'\n", ptcone->INDEX, ptcone->NAME);
        if (yagExpandCone(ifl, ptcone) == TRUE) numcompleted++;
    }
}

jmp_buf *yagAbandonGrow()
{
    return &abandon_grow;
}

int yagExpandCone(inffig_list *ifl, cone_list *ptcone)
{
    losig_list      *ptsig, *ptnextsig;
    long             current_s, next_s;
    locon_list      *ptcon;
    lotrs_list      *pttrans, *ptothertrans;
    chain_list      *headchain, *ptchain, *ptchain1;
    chain_list      *loconchain, *sigchain, *ptconlist;
    gnode_list      *ptnode;
    ptype_list      *ptuser;
    branch_list     *ptdelbranch;
    link_list       *ptnewlink;
    int             reply, nonblocklatch, is_input;
    short           save_depth, save_usefcf;

    save_depth = YAG_CONTEXT->YAG_DEPTH;
    save_usefcf = YAG_CONTEXT->YAG_USE_FCF;
    ptsig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX);

    /* check for NOFALSEBRANCH directive on signal */
    if ((ptuser = getptype(ptsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & FCL_NOFALSEBRANCH) != 0) YAG_CONTEXT->YAG_USE_FCF = FALSE;
    }
    
    /* To handle a return from BDD explosion */
    setBddCeiling(YAG_CONTEXT->YAG_BDDCEILING);
    if (setjmp(abandon_grow) != 0) {
        if (new_branches != NULL) {
            freebrlist(new_branches);
        }
        YAG_CONTEXT->YAG_DEPTH--;
        if (YAG_CONTEXT->YAG_DEPTH == 0) YAG_CONTEXT->YAG_USE_FCF = FALSE;
    }

    if (YAG_CONTEXT->YAG_CONE_GRAPH != NULL) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    if (YAG_CONTEXT->YAG_SUPPORT_GRAPH != NULL) yagFreeGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
    YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;
    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
    if (!YAG_CONTEXT->YAG_CONE_GRAPH->COMPLETE && YAG_CONTEXT->YAG_USE_FCF && YAG_CONTEXT->YAG_REQUIRE_COMPLETE_GRAPH) {
        yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
        YAG_CONTEXT->YAG_DEPTH = save_depth;
        YAG_CONTEXT->YAG_USE_FCF = save_usefcf;
        return FALSE;
    }

    if ( avt_islog(1,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG,1, "\nBefore Construction :-\n");
        yagDisplayUsedGraphLog(LOGYAG, 1, YAG_CONTEXT->YAG_CONE_GRAPH, 1);
    }

    new_branches = NULL;
    THRULOOP = FALSE;

    /* add a partial branch for each unused transistor */
    /* with source or drain connected to the signal    */
    /* and whose orientation allows traversal          */
    
    headchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {

        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T') {
            if (ptcon->NAME == CNS_GRIDNAME) continue;
            if (ptcon->NAME == CNS_BULKNAME) continue;
            pttrans = (lotrs_list *)ptcon->ROOT;

            /* check for NEVER directive on transistor */
            if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
            }
            
            /* filter transistors without driver */
            if (pttrans->GRID == NULL) continue;

            /* check for autoreference ignoring diodes */
            if ((pttrans->TYPE & DIODE) == 0) {
                if (ptsig == getlosig(YAG_CONTEXT->YAG_CURLOFIG, ((cone_list *)pttrans->GRID)->INDEX)) {
                    continue;
                }
            }

            if ((pttrans->TYPE & USED) != 0) continue;

            /* check transistor orientation */
            if (((pttrans->TYPE & ORIENTED) != 0) && ptcon->DIRECTION == 'I') {
                if (getptype(pttrans->USER, YAG_BIDIR_PTYPE) == NULL) {
                    pttrans->USER = addptype(pttrans->USER, YAG_BIDIR_PTYPE, NULL);
                }
                continue;
            }

            /* Skip PMOS of transfer gate */
            if ((pttrans->TYPE & CNS_TP) == CNS_TP && getptype(pttrans->USER, CNS_SWITCH) != NULL) continue;

            /* Skip parallel transistor instances */
            if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;

            /* traverse first link */
            if (ptcon->NAME == CNS_DRAINNAME)
                ptcon = pttrans->SOURCE;
            else ptcon = pttrans->DRAIN;
            ptnextsig = ptcon->SIG;

            if ((next_s = yagGetVal_s(ptnextsig)) >= 0) {
                current_s = yagGetVal_s(ptsig);
                if (current_s < 0) continue;
                if (current_s > next_s) continue;
                if (current_s == next_s) yagWarning(WAR_SAME_S, ptsig->NAMECHAIN->DATA, ptnextsig->NAMECHAIN->DATA, NULL, 0);
            }

            if (YAG_CONTEXT->YAG_USE_CONNECTOR_DIRECTION==TRUE && !mbk_can_cross_transistor_to(ptsig, pttrans, 'i'))        {
//                printf("(1)blocked: %s from %s to %s\n", pttrans->TRNAME, getsigname(ptsig), getsigname(ptnextsig));
                continue;
            }

            /* check for BLOCKER directive on next signal */
            nonblocklatch = FALSE;
            if ((ptuser = getptype(ptnextsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & FCL_BLOCKER) != 0) continue;
                if (((long)ptuser->DATA & (FCL_LATCH|FCL_MEMSYM|FCL_MASTER|FCL_SLAVE)) != 0) nonblocklatch = TRUE;
            }

            /* check if next signal is a latch or a symmetric memory */
            if (!nonblocklatch && (ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE)) != NULL) {
                if ((((cone_list *)ptuser->DATA)->TYPE & (CNS_LATCH|CNS_MEMSYM)) == CNS_LATCH) continue;
                if (YAG_CONTEXT->YAG_MEMSYM_HEURISTIC && (((cone_list *)ptuser->DATA)->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                    if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) != 0) continue;
                }
            }

            /* check that grid signal is a possible input ignoring bleeders and diodes */
            if ((pttrans->TYPE & (BLEEDER|DIODE)) == 0) {
                ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, (void *)pttrans->GRID);
                if (ptnode == (gnode_list *)EMPTYHT) continue;
                else if ((ptnode->TYPE & ROOT_NODE) == 0) continue;
                new_branches = addbranch(new_branches, 0, NULL);
                reply = yagCheckAddTransLink(new_branches, pttrans);
            }
            else {
                new_branches = addbranch(new_branches, 0, NULL);
                reply = YAG_YES;
            }

            /* add first link */
            if (reply == YAG_YES) {
                new_branches->LINK = yagAddTransLink(NULL, pttrans, ptsig);
                if ((pttrans->TYPE & BLEEDER) != 0) new_branches->TYPE |= CNS_BLEEDER;
            }
            else {
                ptdelbranch = new_branches;
                new_branches = new_branches->NEXT;
                yagDestroyBranch(NULL, ptdelbranch);
                if (YAG_CONTEXT->YAG_SUPPORT_GRAPH != NULL) yagFreeGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
                YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;
                continue;
            }

            if (ptnextsig->TYPE == CNS_SIGVDD || ptnextsig->TYPE == CNS_SIGVSS) {
                new_branches->TYPE = (ptnextsig->TYPE == CNS_SIGVDD ? CNS_VDD : CNS_VSS);
                if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                    ptothertrans = (lotrs_list *)ptuser->DATA;
                }
                else ptothertrans = pttrans;
                if ((pttrans->TYPE & BLEEDER) != 0 || (ptothertrans->TYPE & BLEEDER) != 0) {
                    new_branches->TYPE |= CNS_BLEEDER;
                    if ((ptuser = getptype(ptcone->USER, CNS_BLEEDER)) != NULL) {
                        ptuser->DATA = addchain((chain_list *)ptuser->DATA, new_branches);
                    }
                    else {
                        ptcone->USER = addptype(ptcone->USER, CNS_BLEEDER, addchain(NULL, new_branches));
                    }
                }
            }
            else if ((YAG_CONTEXT->YAG_FLAGS & YAG_HAS_INF_INPUTS)!=0 && ptnextsig->TYPE == CNS_SIGEXT) {
                ptconlist = yagGetExtLoconList(ptnextsig);
                is_input = FALSE;
                for (ptchain1 = ptconlist; ptchain1; ptchain1 = ptchain1->NEXT) {
                    if (getptype(((locon_list *)ptchain1->DATA)->USER, YAG_INPUT_PTYPE) != NULL) is_input = TRUE;
                }
                if (is_input) {
                    for (ptchain1 = ptconlist; ptchain1; ptchain1 = ptchain1->NEXT) {
                        ptcon = (locon_list *)ptchain1->DATA;
                        new_branches->NEXT = yagCopyBranch(new_branches->NEXT, new_branches);
                        new_branches->NEXT->TYPE = CNS_EXT;
                        ptnewlink = addlink(NULL, CNS_IN, ptcon, ptnextsig);
                        appendlink(new_branches->NEXT->LINK, ptnewlink);
                    }
                }
                ptdelbranch = new_branches;
                new_branches = new_branches->NEXT;
                yagDestroyBranch(NULL, ptdelbranch);
                freechain(ptconlist);
            }
            else {
                ptuser = getptype(ptnextsig->USER, LOFIGCHAIN);
                loconchain = yagCopyChainList((chain_list *)ptuser->DATA);
                loconchain = yagRmvChain(loconchain, ptcon);
                new_branches->USER = addptype(new_branches->USER, YAG_LOCONLIST_PTYPE, (void *)loconchain);
                sigchain = addchain(NULL, (void *)ptsig);
                sigchain = addchain(sigchain, (void *)ptnextsig);
                new_branches->USER = addptype(new_branches->USER, YAG_SIGLIST_PTYPE, (void *)sigchain);

                /* recursive call */
                new_branches = grow_branch(ifl, ptcone, new_branches, ptnextsig);
            }

            if (YAG_CONTEXT->YAG_SUPPORT_GRAPH != NULL) yagFreeGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
            YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;
        }
    }
    
    add_branches(ptcone, new_branches);
    ptcone->TYPE &= ~YAG_PARTIAL;

    if (YAG_CONTEXT->YAG_CONE_GRAPH != NULL) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
    if (THRULOOP) ptcone->TYPE |= YAG_FALSECONF;

    unsetBddCeiling();

    avt_log(LOGYAG,1, "\tBDD system used %d/%d reduced nodes\n", numberNodeAllBdd() - YAG_CONTEXT->YAG_LASTNODECOUNT, numberNodeAllBdd());

    YAG_CONTEXT->YAG_USE_FCF = save_usefcf;

    yagAnalyseCone(ptcone);

    YAG_CONTEXT->YAG_DEPTH = save_depth;

    return TRUE;
}

static
void add_branches(cone_list *ptcone, branch_list *ptbranches)
{
    branch_list     *ptbranch, *ptnextbranch;
    ptype_list      *ptuser;

    /* add the new branches to the cone branch_list */
    
    if (ptbranches != NULL) {
        for (ptbranch = ptbranches; ptbranch != NULL; ptbranch = ptnextbranch) {
            ptnextbranch = ptbranch->NEXT;
            ptbranch->NEXT = NULL;
            switch (ptbranch->TYPE & (CNS_VSS|CNS_VDD|CNS_EXT|CNS_GND)) {
                case CNS_VSS:
                    ptcone->BRVSS = yagAppendBranch(ptbranch, ptcone->BRVSS);
                    break;
                case CNS_VDD:
                    ptcone->BRVDD = yagAppendBranch(ptbranch, ptcone->BRVDD);
                    break;
                case CNS_EXT:
                    ptcone->BREXT = yagAppendBranch(ptbranch, ptcone->BREXT);
                    break;
                case CNS_GND:
                    ptcone->BRGND = yagAppendBranch(ptbranch, ptcone->BRGND);
                    break;
            }
            /* update edge lists from new branch */
            yagChainBranch(ptcone, ptbranch);

            /* cone can no longer be CMOS DUAL */
        }
        ptcone->TECTYPE &= ~CNS_DUAL_CMOS;
    }

    if ((ptuser = getptype(ptcone->USER, YAG_INPUTS_PTYPE)) != NULL) {
        if (ptuser->DATA != NULL) yagFreeEdgeList((edge_list *)ptuser->DATA);
        ptcone->USER = delptype(ptcone->USER, YAG_INPUTS_PTYPE);
    }
}

static
branch_list *grow_branch(inffig_list *ifl, cone_list *ptrootcone, branch_list *ptbranch, losig_list *ptsig)
{
    branch_list *ptnewbranch, *ptdelbranch;
    cone_list   *ptcone;
    locon_list  *ptcon;
    lotrs_list  *pttrans, *ptothertrans;
    losig_list  *ptinputsig;
    losig_list  *ptnextsig;
    chain_list  *headchain;
    chain_list  *ptchain, *ptnextchain, *ptchain1;
    chain_list  *ptnextloconchain;
    chain_list  *headsigchain, *sigchain;
    chain_list  *ptconlist;
    gnode_list  *ptnode;
    ptype_list  *ptuser;
    link_list   *ptlink, *ptnewlink;
    long        current_s, next_s;
    int         reply, has_support, nonblocklatch, is_input;
    
    if (yagCountLinks(ptbranch->LINK) == YAG_CONTEXT->YAG_MAX_LINKS) {
        ptnewbranch = ptbranch->NEXT;
        ptuser = getptype(ptbranch->USER, YAG_LOCONLIST_PTYPE);
        freechain((chain_list *)ptuser->DATA);
        ptuser = getptype(ptbranch->USER, YAG_SIGLIST_PTYPE);
        freechain((chain_list *)ptuser->DATA);
        yagDestroyBranch(NULL, ptbranch);
        return ptnewbranch;
    }

    headchain = (chain_list *)getptype(ptbranch->USER, YAG_LOCONLIST_PTYPE)->DATA;

    ptuser = getptype(ptsig->USER, YAG_CONE_PTYPE);
    if (ptuser != NULL) {
        ptcone = (cone_list *)ptuser->DATA;
        if ((ptcone->TYPE & YAG_LOOPCONF) != 0) {
            THRULOOP = TRUE;
            ptbranch->TYPE |= YAG_FALSECONF;
        }
    }
    else ptcone = NULL;
    
    for (ptchain = headchain; ptchain != NULL; ptchain = ptnextchain) {
        ptnextchain = ptchain->NEXT;

        ptcon = (locon_list *)ptchain->DATA;

        if (ptcon->TYPE == 'T') {
            
            /* ignore gate & bulk connectors */
            if (ptcon->NAME == CNS_GRIDNAME) continue;
            if (ptcon->NAME == CNS_BULKNAME) continue;
            
            pttrans = (lotrs_list *)ptcon->ROOT;
            
            /* check for NEVER or BLOCKER directive on transistor */
            if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & (FCL_NEVER|FCL_BLOCKER|FCL_BLEEDER)) != 0) continue;
            }

            /* filter transistors without driver */
            if (pttrans->GRID == NULL) continue;

            headsigchain = (chain_list *)getptype(ptbranch->USER, YAG_SIGLIST_PTYPE)->DATA;

            /* check for autoreference ignoring diodes */
            if ((pttrans->TYPE & DIODE) == 0) {
                ptinputsig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, ((cone_list *)pttrans->GRID)->INDEX);
                if (yagGetChain(headsigchain, (void *)ptinputsig) != NULL) {
                    continue;
                }
            }

            /* check that grid signal is a possible input ignoring diodes */
            if ((pttrans->TYPE & (DIODE|BLEEDER)) == 0) {
                ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, (void *)pttrans->GRID);
                if (ptnode == (gnode_list *)EMPTYHT) {
                    continue;
                }
                else if ((ptnode->TYPE & ROOT_NODE) == 0) {
                    continue;
                }
            }
            
            /* check transistor orientation */
            if (((pttrans->TYPE & ORIENTED) != 0) && ptcon->DIRECTION == 'I') {
                yagWarning(WAR_BIDIR_BLOCKED,NULL,"N",(char*)pttrans,0);
                continue;
            }

            /* Skip PMOS of transfer gate */
            if ((pttrans->TYPE & CNS_TP) == CNS_TP && getptype(pttrans->USER, CNS_SWITCH) != NULL) continue;

            /* Skip parallel transistor instances */
            if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;

            /* check if bleeder */
            if((pttrans->TYPE & BLEEDER) != 0 && (ptbranch->TYPE & CNS_BLEEDER) == 0) continue;
                        
            /* traverse new link */
            if (ptcon->NAME == CNS_SOURCENAME)
                ptcon = pttrans->DRAIN;
            else ptcon = pttrans->SOURCE;
            ptnextsig = ptcon->SIG;
                
            if ((next_s = yagGetVal_s(ptnextsig)) >= 0) {
                current_s = yagGetVal_s(ptsig);
                if (current_s < 0) continue;
                if (current_s > next_s) continue;
                if (current_s == next_s) yagWarning(WAR_SAME_S, ptsig->NAMECHAIN->DATA, ptnextsig->NAMECHAIN->DATA, NULL, 0);
            }
            
            if (YAG_CONTEXT->YAG_USE_CONNECTOR_DIRECTION==TRUE && !mbk_can_cross_transistor_to(ptsig, pttrans, 'i'))        {
//                printf("(2)blocked: %s from %s to %s\n", pttrans->TRNAME, getsigname(ptsig), getsigname(ptnextsig));
                continue;
            }

            /* test for BLOCKER directive on signal */
            nonblocklatch = FALSE;
            if ((ptuser = getptype(ptnextsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & FCL_BLOCKER) != 0) continue;
                if (((long)ptuser->DATA & (FCL_LATCH|FCL_MEMSYM|FCL_MASTER|FCL_SLAVE)) != 0) nonblocklatch = TRUE;
            }

            /* check if next signal is a latch or a symmetric memory */
            if (!nonblocklatch && (ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE)) != NULL) {
                if ((((cone_list *)ptuser->DATA)->TYPE & (CNS_LATCH|CNS_MEMSYM)) == CNS_LATCH) continue;
                if (YAG_CONTEXT->YAG_MEMSYM_HEURISTIC && (((cone_list *)ptuser->DATA)->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
                    if ((ptrootcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) != 0) continue;
                }
            }

            /* test if signal already encountered */
            if (yagGetChain(headsigchain, (void *)ptnextsig) != NULL) {
                continue;
            }
            
            /* test for inverse autoreference ignoring diodes */
            if ((pttrans->TYPE & DIODE) == 0) {
                if (ptnextsig->TYPE != CNS_SIGVDD && ptnextsig->TYPE != CNS_SIGVSS) {
                    if ((ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE)) != NULL) {
                        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT ) {
                            if (ptuser->DATA == (void *)ptlink->ULINK.LOTRS->GRID) break;
                        }
                        if (ptlink != NULL) {
                            continue;
                        }
                    }
                }
            }
            
            if ((pttrans->TYPE & (BLEEDER|DIODE)) != 0) {
                reply = YAG_YES;
                has_support = FALSE;
            }
            else {
                reply = yagCheckAddTransLink(ptbranch, pttrans);
                has_support = TRUE;
            }
            if (reply == YAG_YES) {

                /* create a duplicate branch with the new link appended */
                /* and attempt to grow this branch recursively unless */
                /* a power supply connector has been reached */

                if (ptnextsig->TYPE == CNS_SIGEXT) ptconlist = yagGetExtLoconList(ptnextsig);
                else ptconlist=NULL;

                ptbranch->NEXT = yagCopyBranch(ptbranch->NEXT, ptbranch);
                ptnewbranch = ptbranch->NEXT;
                ptnewlink = yagAddTransLink(NULL, pttrans, ptsig);
                ptnewbranch->LINK = appendlink(ptnewbranch->LINK, ptnewlink);
                
                if (ptnextsig->TYPE == CNS_SIGVDD || ptnextsig->TYPE == CNS_SIGVSS) {
                    ptnewbranch->TYPE |= (ptnextsig->TYPE == CNS_SIGVDD ? CNS_VDD : CNS_VSS);
                    if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                        ptothertrans = (lotrs_list *)ptuser->DATA;
                    }
                    else ptothertrans = pttrans;
                    if ((pttrans->TYPE & BLEEDER) != 0 || (ptothertrans->TYPE & BLEEDER) != 0) {
                        ptnewbranch->TYPE |= CNS_BLEEDER;
                        if ((ptuser = getptype(ptrootcone->USER, CNS_BLEEDER)) != NULL) {
                            ptuser->DATA = addchain((chain_list *)ptuser->DATA, ptnewbranch);
                        }
                        else {
                            ptrootcone->USER = addptype(ptrootcone->USER, CNS_BLEEDER, addchain(NULL, ptnewbranch));
                        }
                    }
                    if (yagCountLinks(ptnewbranch->LINK) == YAG_CONTEXT->YAG_MAX_LINKS) {
                        ptrootcone->TECTYPE |= YAG_BADCONE;
                    }
                }
                else if ((YAG_CONTEXT->YAG_FLAGS & YAG_HAS_INF_INPUTS)!=0 && ptnextsig->TYPE == CNS_SIGEXT) {
                    is_input = FALSE;
                    for (ptchain1 = ptconlist; ptchain1; ptchain1 = ptchain1->NEXT) {
                        if (getptype(((locon_list *)ptchain1->DATA)->USER, YAG_INPUT_PTYPE) != NULL) is_input = TRUE;
                    }
                    if (is_input) {
                        for (ptchain1 = ptconlist; ptchain1; ptchain1 = ptchain1->NEXT) {
                            ptcon = (locon_list *)ptchain1->DATA;
                            if (yagCountLinks(ptnewbranch->LINK) == YAG_CONTEXT->YAG_MAX_LINKS) {
                                ptrootcone->TECTYPE |= YAG_BADCONE;
                            }
                            ptnewbranch->NEXT = yagCopyBranch(ptnewbranch->NEXT, ptnewbranch);
                            ptnewbranch->NEXT->TYPE = CNS_EXT;
                            ptnewlink = addlink(NULL, CNS_IN, ptcon, ptnextsig);
                            appendlink(ptnewbranch->NEXT->LINK, ptnewlink);
                        }
                    }
                    ptdelbranch = ptnewbranch;
                    ptnewbranch = ptnewbranch->NEXT;
                    ptbranch->NEXT = ptnewbranch;
                    yagDestroyBranch(NULL, ptdelbranch);
                }
                else {
                    ptnextloconchain = yagCopyChainList((chain_list *)getptype(ptnextsig->USER, LOFIGCHAIN)->DATA);
                    ptnextloconchain = yagRmvChain(ptnextloconchain, ptcon);
                    ptnewbranch->USER = addptype(ptnewbranch->USER, YAG_LOCONLIST_PTYPE, (void *)ptnextloconchain);
                    sigchain = yagCopyChainList(headsigchain);
                    sigchain = addchain(sigchain, (void *)ptnextsig);
                    ptnewbranch->USER = addptype(ptnewbranch->USER, YAG_SIGLIST_PTYPE, (void *)sigchain);

                    ptbranch->NEXT = grow_branch(ifl, ptrootcone, ptnewbranch, ptnextsig);
                }
                freechain(ptconlist);
            }

            /* reduce the graph */
            if (has_support && YAG_CONTEXT->YAG_SUPPORT_GRAPH != NULL) yagReduceSupportGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
        }
    }

    /* partial branch no longer required */
        
    /* check for external branch */
    if (ptsig->TYPE == CNS_SIGEXT) {
        ptconlist = yagGetExtLoconList(ptsig);
        for (ptchain = ptconlist; ptchain; ptchain = ptchain->NEXT) {
            ptcon = (locon_list *)ptchain->DATA;
            reply = yagCheckAddExtLink(ptcone, ptcon, ptrootcone);

            /* create a duplicate branch with an end link */
            /* pointing to the connector */
       
            if (reply == YAG_YES) {
                ptbranch->NEXT = yagCopyBranch(ptbranch->NEXT, ptbranch);
                ptnewlink = addlink(NULL, CNS_IN, ptcon, ptsig);
                appendlink(ptbranch->NEXT->LINK, ptnewlink);
                ptbranch->NEXT->TYPE |= CNS_EXT;
            }
            else if (reply == YAG_NO) {
                ptcon->USER = addptype(ptcon->USER, YAG_INOUT_PTYPE, 0);
            }
        }
        freechain(ptconlist);
    }

    ptnewbranch = ptbranch->NEXT;

    /* delete the partial branch */
    freechain(headchain);
    ptuser = getptype(ptbranch->USER, YAG_SIGLIST_PTYPE);
    freechain((chain_list *)ptuser->DATA);
    yagDestroyBranch(NULL, ptbranch);
    
    return ptnewbranch;
}

cone_list *yagAddGlueCone(losig_list *ptsig, cone_list *ptcone)
{
    cone_list     *ptgluecone;
    cone_list     *headgluecone;
    ptype_list    *ptuser;

    ptuser = getptype(YAG_CONTEXT->YAG_CURCNSFIG->USER, YAG_GLUECONE_PTYPE);
    if (ptuser == NULL) {
        YAG_CONTEXT->YAG_CURCNSFIG->USER = addptype(YAG_CONTEXT->YAG_CURCNSFIG->USER, YAG_GLUECONE_PTYPE, NULL);
        ptuser = YAG_CONTEXT->YAG_CURCNSFIG->USER;
    }
    headgluecone = (cone_list *)ptuser->DATA;
    ptgluecone = yagMakeGlueCone(headgluecone, ptsig, ptcone);
    ptuser->DATA = ptgluecone;

    return ptgluecone;
}

cone_list *yagMakeGlueCone(cone_list *headgluecone, losig_list *ptsig, cone_list *ptcone)
{
    cone_list     *ptgluecone;
    branch_list   *brlist[3];
    branch_list  **ptgluebrlist[3];
    branch_list   *ptbranch;
    branch_list   *ptgluebranch;
    link_list     *ptlink;
    losig_list    *ptconesig;
    losig_list    *ptlastsig;
    losig_list    *ptnextsig;
    long           typemask;
    int            i;

    typemask = CNS_VDD|CNS_VSS|CNS_EXT|
               CNS_BLEEDER|CNS_NOT_FUNCTIONAL|CNS_FEEDBACK;

    ptgluecone = yagMakeCone(headgluecone, ptsig, NULL, YAG_GLUECONE, 0);
    ptgluecone->USER = addptype(ptgluecone->USER, YAG_CONE_PTYPE, ptcone);

    ptgluebrlist[0] = &(ptgluecone->BRVDD);
    ptgluebrlist[1] = &(ptgluecone->BRVSS);
    ptgluebrlist[2] = &(ptgluecone->BREXT);

    ptconesig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            ptlastsig = ptconesig;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                ptnextsig = ptlink->ULINK.LOTRS->DRAIN->SIG;
                if (ptnextsig == ptlastsig) {
                    ptnextsig = ptlink->ULINK.LOTRS->SOURCE->SIG;
                }
                if (ptnextsig == ptsig) break;
                ptlastsig = ptnextsig;
            }
            if (ptlink != NULL) {
                *ptgluebrlist[i] = addbranch(*ptgluebrlist[i], 0, NULL);
                ptgluebranch = *ptgluebrlist[i];
                ptgluebranch->TYPE = ptbranch->TYPE & typemask;
                ptgluebranch->LINK = yagCopyLinkList(ptlink->NEXT);
            }
        }
    }
    yagChainCone(ptgluecone);
    yagAnalyseCone(ptgluecone);

    return ptgluecone;
}
