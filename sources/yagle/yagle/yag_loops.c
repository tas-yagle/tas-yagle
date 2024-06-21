/****************************************************************************/
/*                                                                          */
/*                      Verification Tools                                  */
/*                                                                          */
/*    Product: Yagle/HiTas                                                  */
/*                                                                          */
/*    (c) copyright AVERTEC                                                 */
/*    All Rights Reserved                                                   */
/*    Support : e-mail support@avertec.com                                  */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static void test_levelhold(cone_list *ptcone, chain_list *looplist);
static void adjust_cone(cone_list *ptcone, int fIgnoreFalse);
static void restore_cone(cone_list *ptcone, int fIgnorefalse);
static void adjust_cone_list(chain_list *ptconelist, int fIgnoreFalse);
static void restore_cone_list(chain_list *ptconelist, int fIgnorefalse);
static int exist_tempbranch(cone_list *ptcone, branch_list *ptbranch);
static int count_possible_data(cone_list *ptcone, cone_list *ptloopcone);
static int check_stronger(cone_list *ptcone1, cone_list *ptcone2, float ratio);
static chain_list *yagGetOppositeNames(char *name);
static pCircuit buildLatchCct(cone_list *ptcone, chain_list *looplist, chain_list *latchexpr);
static chain_list *yagGetAsyncLatchInputs(cone_list *ptcone, chain_list *looplist, pCircuit globalCct, pNode write, pNode *ptasync_write);
static chain_list *yagGetAsyncCommands(cone_list *ptcone, chain_list **command_list, pCircuit globalCct, chain_list *latch_async_commands);
static int checkLoopStability(pCircuit circuit, pNode expr, char *name, pNode *ptmem);
static void yagAutoRSCheck(cone_list *ptcone, chain_list *looplist);
static cone_list *yagAutoMemsymCheck(cone_list *ptcone, chain_list *looplist, pNode condition, chain_list *commandlist, chain_list **ptptloopcommands);
static void yagDisableAsyncTimings(cone_list *ptcone, chain_list *commandlist, chain_list *async_command_list, int fIgnoreFalse);
static int checkMemsymData(pNode dataexpr, pNode loopdataexpr);
static chain_list *getInvList(char *name);

#define YAG_TN_SYNC  ((long) 0x10000000 )
#define YAG_TP_SYNC  ((long) 0x20000000 )
#define YAG_TN_ASYNC ((long) 0x40000000 )
#define YAG_TP_ASYNC ((long) 0x80000000 )

/*************************************************************************
 *                     function yagAnalyseLoop()                         *
 *************************************************************************/

void 
yagAnalyseLoop(cone_list *ptcone, int fIgnoreFalse)
{
    cone_list      *ptloopcone, *ptsymcone = NULL;
    abl_pair        local_abl;
    chain_list     *resconf_abl;
    chain_list     *local_expr_abl;
    chain_list     *local_conf_abl, *local_hz_abl;
    chain_list     *latch_write_support;
    chain_list     *looplist;
    chain_list     *support;
    chain_list     *tempabl;
    chain_list     *command_list = NULL;
    chain_list     *symcommand_list = NULL;
    chain_list     *ptchain;
    pCircuit        directCct;
    pCircuit        latchCct;
    pCircuit        savecircuit;
    edge_list      *ptedge;
    branch_list    *ptbranch;
    graph          *savegraph;
    pNode           sup, sdn;
    pNode           local_sup, local_sdn;
    pNode           local_conflict, global_conflict, res_local_conflict, res_global_conflict, local_hz;
    pNode           latch_conflict, latch_hz;
    pNode           local_resconf, global_resconf;
    pNode           global_expr, global_hz;
    pNode           local_expr;
    pNode           memorise, sure_memorise, sure_write, async_write = NULL;
    long            branchmask;
    int             numvars;
    short           save_prop_hz, save_depth;
    chain_list     *datacommands = NULL;
    chain_list     *latch_async_inputs = NULL;
    chain_list     *async_command_list = NULL;
    chain_list     *sym_async_command_list = NULL;
    int             loglvl3 = 0;
    int             restored;
    int             trylevelhold = FALSE;
    int             has_conflict = FALSE, has_hz = FALSE;
    int             count_forward, count_reverse;
    int             res;

    loglvl3 = avt_islog(3,LOGYAG);

    if ((ptcone->TECTYPE & YAG_NOTLATCH) == YAG_NOTLATCH) return;
    if ((ptcone->TYPE & CNS_PRECHARGE) == CNS_PRECHARGE) return;
    if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) return;
    if ((ptcone->TYPE & CNS_MEMSYM) == CNS_MEMSYM) return;
    if ((ptcone->TYPE & CNS_RS) == CNS_RS) return;
    if ((ptcone->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP) return;
    if ((ptcone->TYPE & CNS_MASTER) == CNS_MASTER) return;
    if ((ptcone->TYPE & CNS_SLAVE) == CNS_SLAVE) return;

    if ((looplist = yagCheckLoop(ptcone, fIgnoreFalse)) != NULL) {
        freechain(looplist);
        adjust_cone(ptcone, fIgnoreFalse);
        restored = FALSE;
        looplist = yagCheckLoop(ptcone, fIgnoreFalse);
        if (YAG_CONTEXT->YAG_AUTO_MEMSYM) adjust_cone_list(looplist, fIgnoreFalse);

        /* If loop between 2 CMOS dual cones then privilege the one */
        /* with the greatest number of inputs                       */
        /* If loop between 1 CMOS dual cone and 1 non CMOS DUAL     */
        /* then privilege the non CMOS DUAL                         */
        if (yagCountChains(looplist) == 1) {
            ptloopcone = (cone_list *)looplist->DATA;
            if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS
            && (ptloopcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) {
                if (yagCountEdges(ptloopcone->INCONE) > yagCountEdges(ptcone->INCONE)) {
                    restore_cone(ptcone, fIgnoreFalse);
                    if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
                    freechain(looplist);
                    return;
                }
            }
            if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS
            && (ptloopcone->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS) {
                restore_cone(ptcone, fIgnoreFalse);
                if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
                freechain(looplist);
                return;
            }
            if ((ptcone->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS
            && (ptloopcone->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS) {
                res = 0;
                if (V_FLOAT_TAB[__YAGLE_LATCHLOOP_RATIO].VALUE > 1) {
                    res = check_stronger(ptloopcone, ptcone, V_FLOAT_TAB[__YAGLE_LATCHLOOP_RATIO].VALUE);
                }
                if (res != 1) {
                    count_forward = count_possible_data(ptcone, ptloopcone);
                    count_reverse = count_possible_data(ptloopcone, ptcone);
                    if (res == -1 || count_reverse > count_forward || (count_reverse == count_forward && yagCountEdges(ptcone->OUTCONE) > 1 && yagCountEdges(ptloopcone->OUTCONE) == 1)) {
                        restore_cone(ptcone, fIgnoreFalse);
                        if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
                        freechain(looplist);
                        return;
                    }
                }
            }
        }

        /* remove electrical conflicts in loop cones */

        for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
            ptloopcone = (cone_list *)ptchain->DATA;
            if ((ptloopcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT && (ptloopcone->TYPE & YAG_HASDUAL) != YAG_HASDUAL) {
                ptcone->TYPE |= YAG_PARTIAL;
                yagAnalyseElectric(ptloopcone, NULL, fIgnoreFalse);
                ptcone->TYPE &= ~YAG_PARTIAL;
            }
            if ((ptloopcone->TYPE & (CNS_CONFLICT|CNS_TRI)) != 0 && (ptloopcone->TYPE & YAG_HASDUAL) == 0 && (ptloopcone->TECTYPE & YAG_RESCONF) == 0) {
                restore_cone(ptcone, fIgnoreFalse);
                if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
                test_levelhold(ptcone, looplist);
                freechain(looplist);
                return;
            }
        }

        /* Need to build graph for cone which has itself as a primary */
        /* variable so we mark cone as constrained.                   */
        /* We never exploit beyond HZ cones                           */

        savegraph = YAG_CONTEXT->YAG_CONE_GRAPH;
        savecircuit = YAG_CONTEXT->YAG_CURCIRCUIT;
        
        save_prop_hz = YAG_CONTEXT->YAG_PROP_HZ;
        save_depth = YAG_CONTEXT->YAG_DEPTH;
        YAG_CONTEXT->YAG_DEPTH = YAG_CONTEXT->YAG_AUTOLOOP_DEPTH;
        YAG_CONTEXT->YAG_PROP_HZ = FALSE;
        ptcone->TYPE |= YAG_FORCEPRIM;
        for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
            ptloopcone = (cone_list *)ptchain->DATA;
            if ((ptloopcone->TYPE & YAG_STOP) == YAG_STOP) {
                ptloopcone->TYPE &= ~YAG_STOP;
                ptloopcone->USER = addptype(ptloopcone->USER, YAG_STOP_PTYPE, NULL);
            }
        }
        YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
        for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
            ptloopcone = (cone_list *)ptchain->DATA;
            if (getptype(ptloopcone->USER, YAG_STOP_PTYPE) != NULL) {
                ptloopcone->TYPE |= YAG_STOP;
                ptloopcone->USER = delptype(ptloopcone->USER, YAG_STOP_PTYPE);
            }
        }
        YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;
        ptcone->TYPE &= ~YAG_FORCEPRIM;

        directCct = yagBuildDirectCct(ptcone);

        /* check that cone has itself as a primary variable */
        if (searchInputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, ptcone->NAME) == EMPTYTH) {
            yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
            YAG_CONTEXT->YAG_CURCIRCUIT = savecircuit;
            YAG_CONTEXT->YAG_CONE_GRAPH = savegraph;
            YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;
            YAG_CONTEXT->YAG_DEPTH = save_depth;
            for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
                if (ptbranch->LINK->NEXT == NULL) {
                    ptbranch->TYPE &= ~CNS_BLEEDER;
                    ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
                    ptedge->TYPE &= ~CNS_BLEEDER;
                }
            }
            restore_cone(ptcone, fIgnoreFalse);
            if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
            test_levelhold(ptcone, looplist);
            destroyCct(directCct);
            freechain(looplist);
            return;
        }
        if (directCct->countI <= YAG_CONTEXT->YAG_AUTOLOOP_CEILING) {
            if (fIgnoreFalse) avt_log(LOGYAG,1, "Attempting loop analysis on cone '%s' ignoring suspected false branches\n", ptcone->NAME);
            else avt_log(LOGYAG,1, "Attempting loop analysis on cone '%s'  with all branches\n", ptcone->NAME);

            /* Generate expression for cone as a function of its direct inputs */

            branchmask = CNS_NOT_FUNCTIONAL|CNS_IGNORE|CNS_MASK_PARA|YAG_IGNORE;
            if (fIgnoreFalse) branchmask |= YAG_FALSECONF;
            cnsMakeConeExpr(ptcone, &local_abl, branchmask, 0, FALSE);
            if (local_abl.UP == NULL) local_abl.UP = createAtom("'0'");
            if (local_abl.DN == NULL) local_abl.DN = createAtom("'0'");
     
            /* Build BDDs for direct inputs in terms of primary variables */
            /* created as outputs for the current circuit                 */

            yagBuildDirectInputBdds(ptcone);

            /* Generate up and down expressions for cone in terms of direct inputs */

            local_sup = ablToBddCct(directCct, local_abl.UP);
            local_sdn = ablToBddCct(directCct, local_abl.DN);
            tempabl = createExpr(OR);
            addQExpr(tempabl, copyExpr(local_abl.UP));
            addQExpr(tempabl, copyExpr(local_abl.DN));
            support = supportChain_listExpr(tempabl);
            numvars = yagCountChains(support);
            freeExpr(tempabl);
            freechain(support);

            /* Generate up and down expressions for cone in terms of primary variables */

            sup = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, local_abl.UP);
            sdn = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, local_abl.DN);

            /* apply the contraints */
            sup = yagApplyConstraints(sup, NULL);
            sdn = yagApplyConstraints(sdn, NULL);

            freeExpr(local_abl.UP);
            freeExpr(local_abl.DN);

            /* Generate condition for electrical conflict */

            local_conflict = applyBinBdd(AND, local_sup, local_sdn);
            global_conflict = applyBinBdd(AND, sup, sdn);
            global_conflict = yagApplyConstraints(global_conflict, NULL);
            global_hz = notBdd(applyBinBdd(OR, sup, sdn));
            global_hz = yagApplyConstraints(global_hz, NULL);
            if (global_hz != BDD_zero) {
                if ((loglvl3 || YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) && global_hz != BDD_zero) {
                    avt_log(LOGYAG,1,"Warning: Latch can be HZ when : ");
                    tempabl = bddToAblCct(YAG_CONTEXT->YAG_CURCIRCUIT, global_hz);
                    displayInfExprLog(LOGYAG, 1, tempabl);
                    freeExpr(tempabl);
                    avt_log(LOGYAG,1," = '1'\n");
                }
                local_hz = notBdd(applyBinBdd(OR, local_sup, local_sdn));
                has_hz = TRUE;
            }
            else local_hz = BDD_zero;

            /* Generate conflict resolved expression for cone */

            if (global_conflict != BDD_zero) {
                res_local_conflict = local_conflict;
                if (directCct->countI <= YAG_CONTEXT->YAG_AUTOLOOP_CEILING) {
                    local_resconf = yagResolveConflicts(directCct, YAG_CONTEXT->YAG_CURCIRCUIT, &res_local_conflict, ptcone);
                }
                else local_resconf = BDD_zero;
                if (res_local_conflict != BDD_zero) {
                    tempabl = bddToAblCct(directCct, res_local_conflict);
                    res_global_conflict = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, tempabl);
                    if ( loglvl3 || YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME ) {
                        avt_log(LOGYAG,1,"Conflict when : ");
                        displayInfExprLog(LOGYAG, 1, tempabl);
                        avt_log(LOGYAG,1," = '1'\n");
                    }
                    freeExpr(tempabl);
                    has_conflict = TRUE;
                }
                else res_global_conflict = BDD_zero;
            }
            else {
                local_resconf = BDD_zero;
                res_local_conflict = BDD_zero;
                res_global_conflict = BDD_zero;
            }
            
            if (local_resconf != NULL) {
                resconf_abl = bddToAblCct(directCct, local_resconf);
                global_resconf = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, resconf_abl);
                freeExpr(resconf_abl);
                /* Local and global expression calculation */
                local_expr = applyBinBdd(OR, local_resconf, applyBinBdd(AND, local_sup, notBdd(local_sdn)));
                global_expr = applyBinBdd(OR, global_resconf, applyBinBdd(AND, sup, notBdd(sdn)));
                /* apply the contraints */
                global_expr = yagApplyConstraints(global_expr, NULL);
                if ( avt_islog(2,LOGYAG) ) {
                    yagDisplayBddExprLog(LOGYAG, 2, "Local sup", directCct, local_sup);
                    yagDisplayBddExprLog(LOGYAG, 2, "Local sdn", directCct, local_sdn);
                    yagDisplayBddExprLog(LOGYAG, 2, "Local resconf", directCct, local_resconf);
                    yagDisplayBddExprLog(LOGYAG, 2, "Local expression", directCct, local_expr);
                    yagDisplayBddExprLog(LOGYAG, 2, "Global sup", YAG_CONTEXT->YAG_CURCIRCUIT, sup);
                    yagDisplayBddExprLog(LOGYAG, 2, "Global sdn", YAG_CONTEXT->YAG_CURCIRCUIT, sdn);
                    yagDisplayBddExprLog(LOGYAG, 2, "Global resconf", YAG_CONTEXT->YAG_CURCIRCUIT, global_resconf);
                    yagDisplayBddExprLog(LOGYAG, 2, "Global expression", YAG_CONTEXT->YAG_CURCIRCUIT, global_expr);
                }
                local_conf_abl = bddToAblCct(directCct, res_local_conflict);
                local_hz_abl = bddToAblCct(directCct, local_hz);

                /* check loop stability and obtain memorisation condition */

                if (checkLoopStability(YAG_CONTEXT->YAG_CURCIRCUIT, global_expr, ptcone->NAME, &memorise) == FALSE) {
                    avt_log(LOGYAG,1, "Unstable loop at '%s'\n", ptcone->NAME);
                }
                else if (memorise != BDD_zero && memorise != BDD_one) {
                    sure_memorise = yagSimplestBdd(memorise, applyBinBdd(AND, memorise, notBdd(res_global_conflict)));
                    sure_memorise = yagApplyConstraints(sure_memorise, NULL);
                    sure_write = yagSimplestBdd(notBdd(memorise), applyBinBdd(AND, notBdd(memorise), notBdd(res_global_conflict)));
                    sure_write = yagApplyConstraints(sure_write, NULL);

                    avt_log(LOGYAG,1, "Stable Latch loop at %ld '%s'\n", ptcone->INDEX, ptcone->NAME);
                    if ( loglvl3 || YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME ) {
                        yagDisplayBddExprLog(LOGYAG, 1, "Global memory condition", YAG_CONTEXT->YAG_CURCIRCUIT, memorise);
                        yagDisplayBddExprLog(LOGYAG, 1, "Sure memory condition", YAG_CONTEXT->YAG_CURCIRCUIT, sure_memorise);
                        support = supportChain_listBdd(sure_memorise);
                        avt_log(LOGYAG,1, "Support size = %d\n", yagCountChains(support));
                        freechain(support);
                    }
                    local_expr_abl = bddToAblCct(directCct, local_expr);
                    latchCct = buildLatchCct(ptcone, looplist, local_expr_abl);
                    freeExpr(local_expr_abl);

                    latch_write_support = yagGetWriteSupport(YAG_CONTEXT->YAG_CURCIRCUIT, latchCct, sure_write, NULL);
                    if (latch_write_support != NULL) {
                        if (directCct->countI <= YAG_CONTEXT->YAG_AUTOLOOP_CEILING) {
                            latch_conflict = ablToBddCct(latchCct, local_conf_abl);
                            latch_hz = ablToBddCct(latchCct, local_hz_abl);
                            yagExtractLatchDrivers(latchCct, ptcone, latch_write_support, latch_conflict, latch_hz, sure_write, looplist);
                        }
                        if (YAG_CONTEXT->YAG_AUTO_RS > 0) {
                            yagAutoRSCheck(ptcone, looplist);
                        }
                        if (YAG_CONTEXT->YAG_AUTO_ASYNC > 0) {
                            latch_async_inputs = yagGetAsyncLatchInputs(ptcone, looplist, YAG_CONTEXT->YAG_CURCIRCUIT, sure_write, &async_write);
                        }
                        freechain(latch_write_support);
                        command_list = yagGetCommands(YAG_CONTEXT->YAG_CURCIRCUIT, ptcone, sure_memorise, TRUE, &datacommands);
                        if (YAG_CONTEXT->YAG_AUTO_MEMSYM > 0) {
                            ptsymcone = yagAutoMemsymCheck(ptcone, looplist, sure_memorise, command_list, &symcommand_list);
                        }
                        restore_cone(ptcone, fIgnoreFalse);
                        if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
                        restored = TRUE;
                        if (latch_async_inputs) {
                            async_command_list = yagGetAsyncCommands(ptcone, &command_list, YAG_CONTEXT->YAG_CURCIRCUIT, latch_async_inputs);
                            if (ptsymcone) sym_async_command_list = yagGetAsyncCommands(ptsymcone, &symcommand_list, YAG_CONTEXT->YAG_CURCIRCUIT, latch_async_inputs);
                        }
                        yagMarkMemory(YAG_CONTEXT->YAG_CURCIRCUIT, ptcone, command_list, async_command_list, looplist, sure_memorise, sure_write, global_conflict, async_write, fIgnoreFalse);
                        if (ptsymcone) yagMarkMemory(YAG_CONTEXT->YAG_CURCIRCUIT, ptsymcone, symcommand_list, sym_async_command_list, looplist, sure_memorise, sure_write, global_conflict, async_write, fIgnoreFalse);
                        ptcone->TYPE &= ~(CNS_CONFLICT|CNS_TRI);
                        if (ptsymcone) ptsymcone->TYPE &= ~(CNS_CONFLICT|CNS_TRI);
                        if (has_conflict) {
                            ptcone->TYPE |= CNS_CONFLICT;
                            if (ptsymcone) ptsymcone->TYPE |= CNS_CONFLICT;
                        }
                        if (has_hz) {
                            ptcone->TYPE |= CNS_TRI;
                            if (ptsymcone) ptsymcone->TYPE |= CNS_TRI;
                        }
                        freechain(async_command_list);
                        freechain(sym_async_command_list);
                        destroyCct(latchCct);
                        freechain(command_list);
                        freechain(symcommand_list);
                        freechain(datacommands);
                        freechain(latch_async_inputs);
                    }
                }
                else if (memorise == BDD_zero) {
                    trylevelhold = TRUE;
                }

                freeExpr(local_conf_abl);
                freeExpr(local_hz_abl);
            }
        }
        else trylevelhold = TRUE;

        if (!restored) {
            restore_cone(ptcone, fIgnoreFalse);
            if (YAG_CONTEXT->YAG_AUTO_MEMSYM) restore_cone_list(looplist, fIgnoreFalse);
        }
        if (trylevelhold) {
            test_levelhold(ptcone, looplist);
        }
        freechain(looplist);
        destroyCct(directCct);
        yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = savegraph;
        YAG_CONTEXT->YAG_CURCIRCUIT = savecircuit;
        YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;
        YAG_CONTEXT->YAG_DEPTH = save_depth;
    }
}

static void
test_levelhold(cone_list *ptcone, chain_list *looplist)
{
    losig_list     *ptloopsig;
    chain_list     *ptchain;
    cone_list      *ptloopcone;

    if (looplist->NEXT == NULL) {
        ptloopcone = (cone_list *)looplist->DATA;
        if (yagMatchNOT(ptcone, ptloopcone) && yagMatchNOT(ptloopcone, ptcone)) {
            ptcone->TYPE |= YAG_AUTOLATCH;
            yagMarkLevelHold(ptcone, ptloopcone, NULL);
            cnsConeFunction(ptcone, FALSE);
        }
    }
}

/* Adjust and restore functions to allow ignoring of possible false */
/* branches and to treat level-hold nodes as external connectors       */

static void
adjust_cone_list(chain_list *ptconelist, int fIgnoreFalse)
{
    chain_list *ptchain;
    
    for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) {
        adjust_cone((cone_list *)ptchain->DATA, fIgnoreFalse);
    }
}

static void
restore_cone_list(chain_list *ptconelist, int fIgnoreFalse)
{
    chain_list *ptchain;
    
    for (ptchain = ptconelist; ptchain; ptchain = ptchain->NEXT) {
        restore_cone((cone_list *)ptchain->DATA, fIgnoreFalse);
    }
}

static void
adjust_cone(cone_list *ptcone, int fIgnoreFalse)
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    cone_list      *ptsigcone;
    losig_list     *ptsig, *ptnextsig, *ptlastsig;
    ptype_list     *ptuser;
    branch_list    *brlist[4];
    edge_list      *save_inputs, *ptedge;
    link_list      *ptlink1, *ptlinklist;
    locon_list     *ptcon;
    int             i, changed = FALSE;

    ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i = 0; i < 4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            ptlastsig = ptsig;
            if (fIgnoreFalse && (ptbranch->TYPE & YAG_FALSECONF) != 0) ptbranch->TYPE |= YAG_IGNORE;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK)) == 0) continue;
                if ((ptnextsig = ptlink->ULINK.LOTRS->DRAIN->SIG) == ptlastsig) {
                    ptnextsig = ptlink->ULINK.LOTRS->SOURCE->SIG;
                }
                ptlastsig = ptnextsig;
                ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE);
                if (ptuser != NULL) {
                    ptsigcone = (cone_list *)ptuser->DATA;
                    if ((ptsigcone->TECTYPE & YAG_LEVELHOLD) != 0) {
                        ptbranch->TYPE |= YAG_IGNORE;
                        if (exist_tempbranch(ptcone, ptbranch) == FALSE) {
                            changed = TRUE;
                            ptlinklist = NULL;
                            for (ptlink1 = ptbranch->LINK; ptlink1 && ptlink1 != ptlink->NEXT; ptlink1 = ptlink1->NEXT) {
                                ptlinklist = addlink(ptlinklist, ptlink1->TYPE, ptlink1->ULINK.PTR, ptlink1->SIG);
                            }
                            ptcon = (locon_list *)mbkalloc(sizeof(locon_list));
                            ptcon->NAME   = ptsigcone->NAME;
                            ptcon->SIG    = ptnextsig;
                            ptcon->ROOT   = NULL;
                            ptcon->TYPE   = 'I';
                            ptcon->DIRECTION = 'I';
                            ptcon->USER   = NULL;
                            ptcon->PNODE  = NULL;
                            ptlinklist = addlink(ptlinklist, CNS_EXT|CNS_IN, ptcon, ptnextsig);
                            ptcone->BREXT = addbranch(ptcone->BREXT, CNS_EXT|YAG_TEMPBRANCH, (link_list *)reverse((chain_list *)ptlinklist)); 
                        }
                        break;
                    }
                }
            }
        }
    }
    
    if (fIgnoreFalse || changed) {
        save_inputs = ptcone->INCONE;
        ptcone->USER = addptype(ptcone->USER, YAG_SAVEINPUTS_PTYPE, save_inputs);
        ptcone->INCONE = NULL;
        if (changed) yagChainCone(ptcone);
        else {
            for (ptedge = save_inputs; ptedge; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & YAG_FALSECONF) == 0) {
                    addincone(ptcone, ptedge->TYPE, ptedge->UEDGE.PTR);
                }
            }
        }
    }

    /* Ignore simple external connector branch and input */
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) {
            ptbranch->TYPE |= CNS_BLEEDER;
            ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
            ptedge->TYPE |= CNS_BLEEDER;
        }
    }
}

static int
exist_tempbranch(cone_list *ptcone, branch_list *ptbranch)
{
    branch_list *pttestbranch;
    link_list *ptlink, *pttestlink;

    for (pttestbranch = ptcone->BREXT; pttestbranch; pttestbranch = pttestbranch->NEXT) {
        if ((pttestbranch->TYPE & YAG_TEMPBRANCH) == 0) continue;
        for (ptlink = ptbranch->LINK, pttestlink = pttestbranch->LINK; ptlink && pttestlink->NEXT; ptlink = ptlink->NEXT, pttestlink = pttestlink->NEXT) {
            if (ptlink->ULINK.PTR != pttestlink->ULINK.PTR) break;
        }
        if (pttestlink->NEXT == NULL) break;
    }
    if (ptcone->BREXT != NULL && pttestbranch != NULL) return TRUE;
    return FALSE;
}

static void
restore_cone(cone_list *ptcone, int fIgnoreFalse)
{
    branch_list    *ptbranch, *ptnextbranch;
    branch_list    *brlist[4];
    link_list      *ptlink;
    edge_list      *ptedge;
    int             i, changed = FALSE;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT; 
    brlist[3] = ptcone->BRGND;

    for (i = 0; i < 4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & YAG_IGNORE) != 0) {
                ptbranch->TYPE &= ~YAG_IGNORE;
                changed = TRUE;
            }
        }
    }
    
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptnextbranch) {
        ptnextbranch = ptbranch->NEXT;
        if ((ptbranch->TYPE & YAG_TEMPBRANCH) != 0) {
            for (ptlink = ptbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
            mbkfree(ptlink->ULINK.LOCON);
            yagDestroyBranch(ptcone, ptbranch);
        }
    }
    
    if (fIgnoreFalse || changed) {
        if (ptcone->INCONE) yagFreeEdgeList(ptcone->INCONE);
        ptcone->INCONE = (edge_list *)getptype(ptcone->USER, YAG_SAVEINPUTS_PTYPE)->DATA;
        ptcone->USER = delptype(ptcone->USER, YAG_SAVEINPUTS_PTYPE);
    }

    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) {
            ptbranch->TYPE &= ~CNS_BLEEDER;
            ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
            ptedge->TYPE &= ~CNS_BLEEDER;
        }
    }
}

static int
count_possible_data(cone_list *ptcone, cone_list *ptloopcone)
{
    branch_list    *ptbranch;
    branch_list    *brlist[3];
    link_list      *ptlink;
    edge_list      *ptedge;
    lotrs_list     *pttrans;
    cone_list      *ptincone;
    chain_list     *tmpchain;
    chain_list     *list[3];
    int             i, count = 0;
    

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    list[0] = NULL;
    list[1] = NULL;
    list[2] = NULL;

    for (i = 0; i < 3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            tmpchain = NULL;
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if ((ptbranch->TYPE & YAG_IGNORE) != 0) continue;
            if (i == 2 && ptbranch->LINK->NEXT == NULL) continue;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_EXT) == CNS_EXT) {
                    ptedge = yagGetEdge(ptcone->INCONE, ptlink->ULINK.LOCON);
                }
                else {
                    pttrans = ptlink->ULINK.LOTRS;
                    ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                    ptedge = yagGetEdge(ptcone->INCONE, ptincone);
                }
                tmpchain = addchain(tmpchain, ptedge);
            }
            if (ptlink != NULL) freechain (tmpchain);
            else list[i] = append(list[i], tmpchain);
        }
    }
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if (yagGetChain(list[2], ptedge) || (yagGetChain(list[0], ptedge) && yagGetChain(list[1], ptedge))) count++;
    }
    freechain(list[0]);
    freechain(list[1]);
    freechain(list[2]);
    
    return count;
}

static int
check_stronger(cone_list *ptcone1, cone_list *ptcone2, float ratio)
{
    branch_list *ptbranch;
    link_list *ptlink;
    chain_list *ptchain;
    lotrs_list *pttrans;
    float resistance1, resistance2;

    ptchain = NULL;
    for (ptbranch = ptcone1->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
        if ((ptbranch->TYPE & YAG_IGNORE) != 0) continue;
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            pttrans = ptlink->ULINK.LOTRS;
            if ((cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA == ptcone2) {
                ptchain = addchain(ptchain, ptbranch);
                break;
            }
        }
    }
    resistance1 = yagCalcParallelResistance(ptchain);
    freechain(ptchain);

    ptchain = NULL;
    for (ptbranch = ptcone2->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
        if ((ptbranch->TYPE & YAG_IGNORE) != 0) continue;
        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
            pttrans = ptlink->ULINK.LOTRS;
            if ((cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA == ptcone1) {
                ptchain = addchain(ptchain, ptbranch);
                break;
            }
        }
    }
    resistance2 = yagCalcParallelResistance(ptchain);
    freechain(ptchain);
    
    if (resistance2 > ratio * resistance1) return 1;
    if (resistance1 > ratio * resistance2) return -1;
    return 0;
}

/*************************************************************************
 *                     function yagAnalyseElectric()                     *
 *************************************************************************/

pNode
yagAnalyseElectric(cone_list *ptcone, pCircuit conecct, int fIgnoreFalse)
{
    pCircuit        directCct;
    pCircuit        savecircuit;
    graph          *savegraph;
    edge_list      *save_inputs, *ptedge;
    abl_pair        local_abl;
    chain_list     *local_expr_abl;
    pNode           local_sup, local_sdn;
    pNode           local_conflict;
    pNode           local_resconf;
    pNode           local_expr;
    long            branchmask;
    short           save_prop_hz;
    ptype_list     *ptuser;

    savegraph = YAG_CONTEXT->YAG_CONE_GRAPH;
    savecircuit = YAG_CONTEXT->YAG_CURCIRCUIT;

    if (fIgnoreFalse) {
        save_inputs = ptcone->INCONE;
        ptcone->INCONE = NULL;
        for (ptedge = save_inputs; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & YAG_FALSECONF) == 0) {
                addincone(ptcone, ptedge->TYPE, ptedge->UEDGE.PTR);
            }
        }
    }
    
    save_prop_hz = YAG_CONTEXT->YAG_PROP_HZ;
    YAG_CONTEXT->YAG_PROP_HZ = FALSE;
    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
    YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;

    /* Generate expression for cone as a function of its direct inputs */

    branchmask = CNS_NOT_FUNCTIONAL|CNS_IGNORE|CNS_MASK_PARA;
    if (fIgnoreFalse) branchmask |= YAG_FALSECONF;
    cnsMakeConeExpr(ptcone, &local_abl, branchmask, 0, FALSE);

    /* Build BDDs for direct inputs in terms of primary variables */
    /* created as outputs for the current circuit                 */

    yagBuildDirectInputBdds(ptcone);

    if (conecct == NULL) directCct = yagBuildDirectCct(ptcone);
    else directCct = conecct;

    if (directCct->countI <= YAG_CONTEXT->YAG_AUTOLOOP_CEILING) {
        /* Generate up and down expressions for cone in terms of direct inputs */

        if (local_abl.UP != NULL) {
            local_sup = ablToBddCct(directCct, local_abl.UP);
            freeExpr(local_abl.UP);
        }
        else local_sup = BDD_zero;
        if (local_abl.DN != NULL) {
            local_sdn = ablToBddCct(directCct, local_abl.DN);
            freeExpr(local_abl.DN);
        }
        else local_sdn = BDD_zero;

        /* Generate condition for electrical conflict */

        local_conflict = applyBinBdd(AND, local_sup, local_sdn);

        /* Generate conflict resolved expression for cone */

        local_resconf = yagResolveConflicts(directCct, YAG_CONTEXT->YAG_CURCIRCUIT, &local_conflict, ptcone);
        if (local_conflict == BDD_zero) {
            ptcone->TECTYPE |= YAG_RESCONF;
        }
        local_expr = applyBinBdd(OR, local_resconf, applyBinBdd(AND, local_sup, notBdd(local_sdn)));
        local_expr_abl = bddToAblCct(directCct, local_expr);
        if ((ptuser = getptype(ptcone->USER, CNS_UPEXPR)) != NULL) {
            freeExpr(ptuser->DATA);
            ptuser->DATA = local_expr_abl;
        }
        else ptcone->USER = addptype(ptcone->USER, CNS_UPEXPR, local_expr_abl);
    }
    
    if (conecct == NULL) destroyCct(directCct);
    yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    YAG_CONTEXT->YAG_CONE_GRAPH = savegraph;
    YAG_CONTEXT->YAG_CURCIRCUIT = savecircuit;

    if (fIgnoreFalse) {
        yagFreeEdgeList(ptcone->INCONE);
        ptcone->INCONE = save_inputs;
    }
    if (conecct != NULL) return local_conflict;
    else return NULL;
}

/*************************************************************************
 *                    function buildDirectInputBdds()                    *
 *************************************************************************/

/* build BDDs for roots of the current graph  */
/* store as outputs of the current circuit    */

void
yagBuildDirectInputBdds(cone_list *ptcone)
{
    char        *name;
    pNode        bdd;
    edge_list   *ptedge;
    chain_list  *expr;
    chain_list  *tempexpr;
    gnode_list  *ptnode;

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptedge->TYPE & CNS_EXT) == 0) {
            ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptedge->UEDGE.CONE);
            name = ptnode->OBJECT.CONE->NAME;
        }
        else {
            /* node is an external connector */
            ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptedge->UEDGE.LOCON);
            name = ptnode->OBJECT.LOCON->NAME;
        }
        tempexpr = createAtom(name);
        expr = yagExpandExpr(ptnode, tempexpr, 0);
        freeExpr(tempexpr);
        bdd = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, expr);
        addOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name, bdd);
        freeExpr(expr);
    }
}

/*************************************************************************
 *                    function buildDirectCct()                          *
 *************************************************************************/

/* build circuit for cone direct inputs  */

pCircuit
yagBuildDirectCct(cone_list *ptcone)
{
    pCircuit    circuit;
    edge_list  *ptedge;
    char       *name;
    int         numinputs;

    numinputs = yagCountEdges(ptcone->INCONE);
    circuit = initializeCct(ptcone->NAME, numinputs, 10);
    
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_IGNORE) != 0) continue;
        if ((ptedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptedge->TYPE & CNS_EXT) != CNS_EXT) {
            name = ptedge->UEDGE.CONE->NAME;
        }
        else name = ptedge->UEDGE.LOCON->NAME;
        addInputCct_no_NA(circuit, name);
    }

    avt_log(LOGYAG,1, "Number of direct inputs : %d\n", circuit->countI);
    return circuit;
}

/*************************************************************************
 *                    function buildLatchCct()                           *
 *************************************************************************/

/* build circuit for latch analysis */

static pCircuit
buildLatchCct(cone_list *ptcone, chain_list *looplist, chain_list *latchexpr)
{
    pCircuit    circuit;
    pNode       bdd;
    edge_list  *ptedge;
    cone_list  *ptloopcone;
    gnode_list *ptloopnode;
    gnode_list *ptnode;
    chain_list *ptchain0, *ptchain1;
    chain_list *tempexpr, *expr;
    chain_list *nodelist;
    ptype_list *ptuser;
    char       *name;
    int         numinputs;
    int         flag;
    int         stuck;

    numinputs = yagCountEdges(ptcone->INCONE) - yagCountChains(looplist) + 1;
    circuit = initializeCct(ptcone->NAME, numinputs, 10);

    /* Add all inputs which are not on the loop */

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_IGNORE) != 0) continue;
        if ((ptedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptedge->TYPE & CNS_EXT) != CNS_EXT) {
            if (yagGetChain(looplist, ptedge->UEDGE.CONE) != NULL) continue;
            ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptedge->UEDGE.CONE);
            nodelist = yagGetPrimVars(YAG_CONTEXT->YAG_CONE_GRAPH, ptnode);
            flag = FALSE;
            stuck = ((ptedge->UEDGE.CONE->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0);
            for (ptchain0 = nodelist; ptchain0; ptchain0 = ptchain0->NEXT) {
                if ((gnode_list *)ptchain0->DATA != ptnode) flag = TRUE;
                if ((((gnode_list *)ptchain0->DATA)->TYPE & ROOT_NODE) != ROOT_NODE) break;
            }
            if (stuck || (flag == TRUE && ptchain0 == NULL)) {
                expr = bddToAblCct(YAG_CONTEXT->YAG_CURCIRCUIT, searchOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, ptedge->UEDGE.CONE->NAME));
                addOutputCct_no_NA(circuit, ptedge->UEDGE.CONE->NAME, ablToBddCct(circuit, expr));
                freeExpr(expr);
            }
            else addInputCct_no_NA(circuit, ptedge->UEDGE.CONE->NAME);
            freechain(nodelist);
        }
        else addInputCct_no_NA(circuit, ptedge->UEDGE.LOCON->NAME);
    }

    /* Add union of inputs of all cones on the loop */

    for (ptchain0 = looplist; ptchain0; ptchain0 = ptchain0->NEXT) {

        /* Add loop cone inputs to latch circuit and */
        /* add BDDs for them to global circuit       */

        ptloopnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptchain0->DATA);
        for (ptchain1 = ptloopnode->FATHERS; ptchain1; ptchain1 = ptchain1->NEXT) {
            ptnode = (gnode_list *)ptchain1->DATA;
            if ((ptnode->TYPE & CONE_TYPE) != 0) {
                name = ptnode->OBJECT.CONE->NAME;
            }
            else {
                /* node is an external connector */
                name = ptnode->OBJECT.LOCON->NAME;
            }
            if (searchInputCct_no_NA(circuit, name) == EMPTYTH) {
                addInputCct_no_NA(circuit, name);
            }
            if (searchOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name) != NULL) continue;
            tempexpr = createAtom(name);
            expr = yagExpandExpr(ptnode, tempexpr, 0);
            freeExpr(tempexpr);
            bdd = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, expr);
            addOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name, bdd);
            freeExpr(expr);
        }

        /* Add loop cone expr to output of latch circuit */

        ptloopcone = (cone_list *)ptchain0->DATA;
        if ((ptloopcone->TYPE & CNS_CONFLICT) != 0 && (ptloopcone->TYPE & YAG_HASDUAL) != 0) {
           ptuser = getptype(ptloopcone->USER, YAG_DUALEXPR_PTYPE);
        }
        else ptuser = getptype(ptloopcone->USER, CNS_UPEXPR);
        if (ptuser == NULL) {
            yagBug(DBG_NO_ABL, "buildLatchCct", ptloopcone->NAME, NULL, 0);
        }
        addOutputCct_no_NA(circuit, ptloopcone->NAME, ablToBddCct(circuit, (chain_list *)ptuser->DATA));
    } 

    /* Add bdd for latch output */
    addOutputCct_no_NA(circuit, ptcone->NAME, ablToBddCct(circuit, latchexpr));

    if ( avt_islog(2,LOGYAG) ) {
        displayCctLog(LOGYAG,2,circuit, 1);
    }
    return circuit;
}

/*************************************************************************
 *                    function checkLoopStability()                      *
 *************************************************************************/

/* given a conflict resolved global expression for a cone */
/* on a two cone loop, returns whether the loop can be    */
/* unstable and the condition for loop memorisation       */

static int
checkLoopStability(pCircuit circuit, pNode expr, char *name, pNode *ptmem)
{
    pNode       f0, f1;
    pNode       loopvar;
    int         index;

    index = searchInputCct_no_NA(circuit, name); // called with a cone name => already namealloced
    if (index <= 1) {
        avt_log(LOGYAG,1, "loopname: %s\n", name);
        displayCctLog(LOGYAG,1,circuit, 0);
    }
    loopvar = createNodeTermBdd(index);
    f1 = constraintBdd(expr, loopvar);
    f0 = constraintBdd(expr, notBdd(loopvar));
    if ( avt_islog(3,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        yagDisplayBddExprLog(LOGYAG, 1, "f0", circuit, f0);
        yagDisplayBddExprLog(LOGYAG, 1, "f1", circuit, f1);
    }
    *ptmem = applyBinBdd(AND, f1, notBdd(f0));

    if (applyBinBdd(AND, f0, notBdd(f1)) != BDD_zero) return FALSE;
    else return TRUE;
}

/*************************************************************************
 *                    function isInversion()                             *
 *************************************************************************/

/* check for inverse relation between given input and output */

int
yagIsInversion(pCircuit circuit, char *input, char *output)
{
    pNode       f0, f1;
    pNode       loopvar;
    pNode       expr;
    int         index;

    index = searchInputCct_no_NA(circuit, input); // called with a cone name => already namealloced
    if (index <= 1) {
        avt_log(LOGYAG,1, "input: %s\n", input);
        displayCctLog(LOGYAG,1,circuit, 0);
        return TRUE;
    }
    loopvar = createNodeTermBdd(index);
    if ((expr = searchOutputCct_no_NA(circuit, output)) == NULL) {
        avt_log(LOGYAG,1, "output: %s\n", output);
        displayCctLog(LOGYAG,1,circuit, 0);
        return TRUE;
    }
    f1 = constraintBdd(expr, loopvar);
    f0 = constraintBdd(expr, notBdd(loopvar));

    if (applyBinBdd(AND, f0, notBdd(f1)) != BDD_zero) return TRUE;
    else return FALSE;
}

/*************************************************************************
 *                    function getWriteSupport()                         *
 *************************************************************************/

/* given a global write condition obtain list of */
/* all constrained inputs of the latch circuit   */

chain_list *
yagGetCorrelatedWriteSupport(pCircuit globalCct, pCircuit latchCct, pNode write, chain_list *ptallsupport)
{
    chain_list *result = NULL, *name_result = NULL;
    chain_list *global_support;
    chain_list *ptchain;
    pNode       globalvar;
    char       *varname, *opposite_name;
    pNode       varbdd;
    short       index, opposite_index;
    
    global_support = supportChain_listBdd(write);
    
    for (index = latchCct->countI - 1; index > 1; index--) {
        varname = searchIndexCct(latchCct, index);
        varbdd = searchOutputCct_no_NA(globalCct, varname);
        if (varbdd == NULL) continue;
        for (ptchain = global_support; ptchain; ptchain = ptchain->NEXT) {
            globalvar = createNodeTermBdd(((pNode)ptchain->DATA)->index);
            if (constraintBdd(varbdd, globalvar) != varbdd) break;
        }
        if (ptchain != NULL) {
            if (yagGetChain(name_result, varname) == NULL) {
                result = addchain(result, createNodeTermBdd(index));
                name_result = addchain(name_result, varname);
            }
            if ((ptchain = yagGetOppositeNames(varname))) {
                for (; ptchain; ptchain = ptchain->NEXT) {
                    opposite_name = (char *)ptchain->DATA;
                    if ((opposite_index = searchInputCct_no_NA(latchCct, opposite_name)) != (short)EMPTYHT) {
                        if (yagGetChain(name_result, opposite_name) == NULL) {
                            result = addchain(result, createNodeTermBdd(opposite_index));
                            name_result = addchain(name_result, opposite_name);
                        }
                    }
                }
                freechain(ptchain);
            }
        }
    }
    /* Test whether all support are commands */
    if (ptallsupport) {
        for (ptchain = ptallsupport; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(name_result, ptchain->DATA) == NULL) break;
        }
        if (ptchain == NULL) {
            freechain(result);
            result = NULL;
        }
    }
    freechain(global_support);
    freechain(name_result);
    return result;
}

chain_list *
yagGetWriteSupport(pCircuit globalCct, pCircuit latchCct, pNode write, chain_list *ptallsupport)
{
    chain_list *result = NULL, *name_result = NULL;
    chain_list *write_support;
    chain_list *support;
    chain_list *ptchain;
    char       *varname, *opposite_name;
    pNode       varbdd;
    short       index, opposite_index;
    
    write_support = supportChain_listBddExpr(globalCct, write);
    
    for (index = latchCct->countI - 1; index > 1; index--) {
        varname = searchIndexCct(latchCct, index);
        varbdd = searchOutputCct_no_NA(globalCct, varname);
        if (varbdd == NULL) continue;
        /* apply the contraints */
        varbdd = yagApplyConstraints(varbdd, NULL);

        support = supportChain_listBddExpr(globalCct, varbdd);
        for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(write_support, ptchain->DATA) == NULL) break;
        }
        if (ptchain == NULL && support) {
            if (yagGetChain(name_result, varname) == NULL) {
                result = addchain(result, createNodeTermBdd(index));
                name_result = addchain(name_result, varname);
            }
            if ((ptchain = yagGetOppositeNames(varname))) {
                for (; ptchain; ptchain = delchain(ptchain, ptchain)) {
                    opposite_name = (char *)ptchain->DATA;
                    if ((opposite_index = searchInputCct_no_NA(latchCct, opposite_name)) != (short)EMPTYHT) {
                        if (yagGetChain(name_result, opposite_name) == NULL) {
                            result = addchain(result, createNodeTermBdd(opposite_index));
                            name_result = addchain(name_result, opposite_name);
                        }
                    }
                }
            }
        }
        freechain(support);
    }
    /* try correlated support */
    if (result == NULL) {
        result = yagGetCorrelatedWriteSupport(globalCct, latchCct, write, ptallsupport);
    }
    /* else Test whether all support are commands */
    else if (ptallsupport) {
        for (ptchain = ptallsupport; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(name_result, ptchain->DATA) == NULL) break;
        }
        if (ptchain == NULL) {
            freechain(result);
            result = NULL;
        }
    }
    freechain(write_support);
    freechain(name_result);
    return result;
}

static chain_list *
yagGetOppositeNames(char *name)
{
    ptype_list *ptptype;
    chain_list *support;
    chain_list *reschain = NULL;
    char       *opposite_name;
    
    for (ptptype = YAG_CONTEXT->YAG_CONE_GRAPH->EXTRA_CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
        support = (chain_list *)ptptype->TYPE;
        if (yagGetChain(support, name) != NULL && yagCountChains(support) == 2) {
            if ((char *)support->DATA == name) opposite_name = (char *)support->NEXT->DATA;
            else opposite_name = (char *)support->DATA;
            if (yagGetChain(reschain, opposite_name) == NULL) reschain = addchain(reschain, opposite_name);
        }
    }
    return reschain;
}

/*************************************************************************
 *                    function checkBranchBlocked()                      *
 *************************************************************************/

static int
checkBranchBlocked(branch_list *ptbranch, pNode check)
{
    pNode           branchexpr, branch_check;

    branchexpr = (pNode)getptype(ptbranch->USER, YAG_BRANCHEXPR_PTYPE)->DATA;
    branch_check = applyBinBdd(AND, branchexpr, check);
    branch_check = yagApplyConstraints(branch_check, NULL);
    if (branch_check == BDD_zero) return TRUE;
    return FALSE;
}

/*************************************************************************
 *                    function checkFeedbackFunctional()                 *
 *************************************************************************/

static void
checkFeedbackFunctional(branch_list *ptbranch, pNode write, cone_list *ptcone)
{
    branch_list    *ptbranchlist, *testbranch;
    pNode           branchexpr, feedback_write, testexpr;
    ptype_list     *ptuser;

    if ((ptbranch->TYPE & CNS_VSS) == CNS_VSS) ptbranchlist = ptcone->BRVSS;
    else ptbranchlist = ptcone->BRVDD;

    branchexpr = (pNode)getptype(ptbranch->USER, YAG_BRANCHEXPR_PTYPE)->DATA;
    feedback_write = applyBinBdd(AND, branchexpr, write);
    feedback_write = yagApplyConstraints(feedback_write, NULL);
    if (feedback_write != BDD_zero) {
        for (testbranch = ptbranchlist; testbranch; testbranch = testbranch->NEXT) {
            if ((testbranch->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;
            ptuser = getptype(testbranch->USER, YAG_BRANCHEXPR_PTYPE);
            if (ptuser == NULL) continue;
            testexpr = (pNode)ptuser->DATA;
            if (applyBinBdd(AND, testexpr, feedback_write) == feedback_write) break;
        }
        if (testbranch == NULL) ptbranch->TYPE &= ~(CNS_NOT_FUNCTIONAL|YAG_LATCH_NF);
    }
}

/*************************************************************************
 *                    function markMemory()                              *
 *************************************************************************/

void
yagMarkMemory(pCircuit ptcircuit, cone_list *ptcone, chain_list *commandlist, chain_list *async_command_list, chain_list *looplist, pNode memorize, pNode write, pNode conflict, pNode async_write, int fIgnoreFalse)
{
    link_list      *ptlink;
    branch_list    *ptbranch;
    branch_list    *brlist[3];
    cone_list      *loopcone;
    cone_list      *ptincone;
    edge_list      *ptinedge;
    edge_list      *ptoutedge;
    lotrs_list     *pttrans;
    chain_list     *ptchain;
    chain_list     *branchabl;
    ptype_list     *ptuser;
    pNode           sync_write, branchexpr, branch_mem;
    int             i, hasdata, ismemsym;
    char           *asyncname;

    ptcone->TYPE |= YAG_AUTOLATCH;
    ptcone->TYPE &= ~YAG_LOOPCONF;
    
    ismemsym = ((ptcone->TYPE & CNS_MEMSYM) == CNS_MEMSYM);

    if ((ptcone->TYPE & CNS_RS) == CNS_RS) return;
    ptcone->TYPE |= CNS_LATCH;
    
    if (async_write && write) sync_write = applyBinBdd(AND, write, notBdd(async_write));
    else sync_write = NULL;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        if ((ptinedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptinedge->TYPE & CNS_EXT) != 0) continue;
        ptincone = ptinedge->UEDGE.CONE;
        ptincone->TYPE &= ~YAG_LOOPCONF;
        if (yagGetChain(looplist, ptincone) != NULL) {
            ptinedge->TYPE |= CNS_FEEDBACK;
            if ((ptoutedge = yagGetEdge(ptincone->OUTCONE, ptcone)) != NULL) {
                ptoutedge->TYPE |= CNS_FEEDBACK;
            }
        }
        if (yagGetChain(commandlist, ptincone->NAME) != NULL) {
            ptinedge->TYPE |= CNS_COMMAND;
        }
        if (yagGetChain(async_command_list, ptincone->NAME) != NULL) {
            ptinedge->TYPE |= CNS_ASYNC|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
        }
    }
    /* If a branch is opened by the memorising condition then that   */
    /* branch is marked as feedback and not-functional, if there     */
    /* exists a non-memorising condition where only a feedback       */
    /* branch is open then that branch must be considered functional */
    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if (fIgnoreFalse && (ptbranch->TYPE & YAG_FALSECONF) != 0) continue;
            if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
            if (i < 2 && ptcircuit != NULL) {
                branchabl = cnsMakeBranchExpr(ptbranch, 0, FALSE);
                branchexpr = ablToBddCct(ptcircuit, branchabl);
                ptbranch->USER = addptype(ptbranch->USER, YAG_BRANCHEXPR_PTYPE, branchexpr);
                freeExpr(branchabl);
                branch_mem = applyBinBdd(AND, branchexpr, memorize);
                branch_mem = yagApplyConstraints(branch_mem, NULL);
                if (branch_mem != BDD_zero) {
                    ptbranch->TYPE |= CNS_FEEDBACK|CNS_NOT_FUNCTIONAL|YAG_LATCH_NF;
                }
            }
            asyncname = NULL;
            hasdata = FALSE;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if (i == 2 && ptlink->NEXT == NULL) continue;
                pttrans = ptlink->ULINK.LOTRS;
                ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                if (ptincone == NULL) continue;
                if (yagGetChain(async_command_list, ptincone->NAME) == NULL && yagGetChain(commandlist, ptincone->NAME) == NULL) hasdata = TRUE;
                if (!asyncname && (ptchain = yagGetChain(async_command_list, ptincone->NAME))) asyncname = ptchain->DATA;
            }
            /* asynchronous driver branch */
            if (asyncname && sync_write && checkBranchBlocked(ptbranch, sync_write)) {
                ptbranch->TYPE |= YAG_ASYNC;
                continue;
            }
            /* synchronous driver branch */
            else if (asyncname && hasdata && async_write && checkBranchBlocked(ptbranch, async_write) && (ptbranch->TYPE & (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) != (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) {
                ptbranch->TYPE |= YAG_SYNC;
            }
            if ((ptbranch->TYPE & (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) == (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) {
                continue;
            }
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if (i == 2 && ptlink->NEXT == NULL) continue;
                if ((ptbranch->TYPE & YAG_ASYNC) == YAG_ASYNC) continue;
                pttrans = ptlink->ULINK.LOTRS;
                ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                if (ptincone == NULL) continue;
                if ((ptbranch->TYPE & YAG_ASYNC) == YAG_ASYNC) {
                    if (yagGetChain(async_command_list, ptincone->NAME) != NULL) {
                        ptlink->TYPE |= CNS_ASYNCLINK|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
                    }
                    else if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                        pttrans = (lotrs_list *)ptuser->DATA;
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        if (yagGetChain(async_command_list, ptincone->NAME) != NULL) {
                            ptlink->TYPE |= CNS_ASYNCLINK|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
                        }
                    }
                }
                else {
                    if (yagGetChain(commandlist, ptincone->NAME) != NULL) {
                        ptlink->TYPE |= CNS_COMMAND;
                    }
                    else if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                        pttrans = (lotrs_list *)ptuser->DATA;
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        if (yagGetChain(commandlist, ptincone->NAME) != NULL) {
                            ptlink->TYPE |= CNS_COMMAND;
                        }
                    }
                }
            }
        }
    }
    if (ptcircuit != NULL) {
        for (i=0; i<2; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                if (fIgnoreFalse && (ptbranch->TYPE & YAG_FALSECONF) != 0) continue;
                if (conflict != NULL && conflict == BDD_zero) {
                    checkFeedbackFunctional(ptbranch, write, ptcone);
                }
                if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
                if ((ptbranch->TYPE & YAG_ASYNC) == YAG_ASYNC) {
                    loopcone = NULL;
                    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                        pttrans = ptlink->ULINK.LOTRS;
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        if (yagGetChain(async_command_list, ptincone->NAME) != NULL) {
                            ptlink->TYPE |= CNS_ASYNCLINK|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
                        }
                        else if (ptlink == ptbranch->LINK && ptlink->NEXT == NULL) {
                            ptlink->TYPE |= CNS_ASYNCLINK|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
                            ptinedge = yagGetEdge(ptcone->INCONE, ptincone);
                            ptinedge->TYPE |= CNS_ASYNC|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
                        }
                        else if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                            pttrans = (lotrs_list *)ptuser->DATA;
                            ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                            if (ptincone == NULL) continue;
                            if (yagGetChain(async_command_list, ptincone->NAME) != NULL) {
                                ptlink->TYPE |= CNS_ASYNCLINK|((V_BOOL_TAB[__YAGLE_ASYNC_COMMAND].VALUE)?CNS_COMMAND:0);
                            }
                        }
                        /* no cut of feedback loop in case of set/reset on loop (need to be strict) */
                        ptinedge = yagGetEdge(ptcone->INCONE, ptincone);
                        if ((ptinedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK && yagGetChain(async_command_list, ptincone->NAME)) {
                            if (yagCheckUniqueConnection(ptincone, ptcone)) {
                                ptinedge->TYPE &= ~CNS_FEEDBACK;
                                if ((ptoutedge = yagGetEdge(ptincone->OUTCONE, ptcone)) != NULL) {
                                    ptoutedge->TYPE &= ~CNS_FEEDBACK;
                                }
                                ptbranch->TYPE &= ~CNS_FEEDBACK;
                            }
                        }
                    }
                }
                else {
                    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                        pttrans = ptlink->ULINK.LOTRS;
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (yagGetChain(commandlist, ptincone->NAME) != NULL) {
                            ptlink->TYPE |= CNS_COMMAND;
                        }
                        else if (ptlink == ptbranch->LINK && ptlink->NEXT == NULL) {
                            ptlink->TYPE |= CNS_COMMAND;
                            ptinedge = yagGetEdge(ptcone->INCONE, ptincone);
                            ptinedge->TYPE |= CNS_COMMAND;
                        }
                        else if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
                            pttrans = (lotrs_list *)ptuser->DATA;
                            ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                            if (ptincone == NULL) continue;
                            if (yagGetChain(commandlist, ptincone->NAME) != NULL) {
                                ptlink->TYPE |= CNS_COMMAND;
                            }
                        }
                    }
                }
            }
        }
        for (i=0; i<2; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                if (fIgnoreFalse && (ptbranch->TYPE & YAG_FALSECONF) != 0) continue;
                if ((ptbranch->TYPE & CNS_BLEEDER) != 0) continue;
                ptbranch->USER = delptype(ptbranch->USER, YAG_BRANCHEXPR_PTYPE);
            }
        }
    }
    
    if (YAG_CONTEXT->YAG_AUTO_ASYNC > 1) {
        for (i=0; i<2; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                if ((ptbranch->TYPE & YAG_ASYNC) == YAG_ASYNC) ptbranch->TYPE |= (CNS_NOT_FUNCTIONAL|YAG_LATCH_NF);
            }
        }
    }
    if (YAG_CONTEXT->YAG_AUTO_ASYNC > 0) {
        yagDisableAsyncTimings(ptcone, commandlist, async_command_list, fIgnoreFalse);
    }
    if (ismemsym) {
        for (i=0; i<3; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                ptbranch->TYPE &= ~(CNS_FEEDBACK|CNS_NOT_FUNCTIONAL|YAG_LATCH_NF);
            }
        }
        for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
            ptinedge->TYPE &= ~CNS_FEEDBACK;
        }
    }
}

/*************************************************************************
 *                    function yagDisableAsyncTimings()                  *
 *************************************************************************/

static void
yagDisableAsyncTimings(cone_list *ptcone, chain_list *commandlist, chain_list *async_command_list, int fIgnoreFalse)
{
    link_list      *ptlink;
    branch_list    *ptbranch;
    branch_list    *brlist[3];
    cone_list      *ptincone;
    lotrs_list     *pttrans;
    ptype_list     *ptuser, *ptptype, *ptptype2;
    ptype_list     *comtypelist[2], *asynctypelist[2];
    long            linktype, comtype, asynctype;
    int             i, j;
    char           *inname;
    inffig_list    *ifl;

    if (async_command_list) {
        ifl=getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);
        if (ifl == NULL) ifl = addinffig(YAG_CONTEXT->YAG_FIGNAME);

        brlist[0] = ptcone->BRVDD;
        brlist[1] = ptcone->BRVSS;
        brlist[2] = ptcone->BREXT;

        /* disable access timing arcs from opposite command edge if necessary */
        comtypelist[0] = NULL;
        comtypelist[1] = NULL;
        asynctypelist[0] = NULL;
        asynctypelist[1] = NULL;
        for (i=0; i<3; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                if (fIgnoreFalse && (ptbranch->TYPE & YAG_FALSECONF) != 0) continue;
                if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    if (i == 2 && ptlink->NEXT == NULL) continue;
                    linktype = ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK);
                    pttrans = ptlink->ULINK.LOTRS;
                    if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL && ((i == 0 && linktype == CNS_TNLINK) || (i == 1 && linktype == CNS_TPLINK))) {
                        pttrans = (lotrs_list *)ptuser->DATA;
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        linktype = (linktype == CNS_TNLINK) ? CNS_TPLINK : CNS_TNLINK;
                        inname = ptincone->NAME;
                    }
                    else {
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        inname = ptincone->NAME;
                    }
                    if (yagGetChain(commandlist, inname) == NULL) continue;
                    j = (i == 2) ? 0 : i;
                    if ((ptptype = getptype(comtypelist[j], (long)inname)) == NULL) {
                        ptptype = addptype(comtypelist[j], (long)inname, NULL);
                        comtypelist[j] = ptptype;
                    }
                    if (i == 2) {
                        if ((ptptype2 = getptype(comtypelist[1], (long)inname)) == NULL) {
                            ptptype2 = addptype(comtypelist[1], (long)inname, NULL);
                            comtypelist[1] = ptptype2;
                        }
                    }
                    else ptptype2 = NULL;
                    if ((ptbranch->TYPE & YAG_ASYNC) == YAG_ASYNC) {
                        ptptype->DATA = (char *)(((long)ptptype->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_ASYNC : YAG_TP_ASYNC));
                        if (ptptype2) ptptype2->DATA = (char *)(((long)ptptype2->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_ASYNC : YAG_TP_ASYNC));
                    }
                    else {
                        ptptype->DATA = (char *)(((long)ptptype->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_SYNC : YAG_TP_SYNC));
                        if (ptptype2) ptptype2->DATA = (char *)(((long)ptptype2->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_SYNC : YAG_TP_SYNC));
                    }
                }
            }
        }
        for (i=0; i<2; i++) {
            for (ptptype = comtypelist[i]; ptptype; ptptype = ptptype->NEXT) {
                comtype = (long)ptptype->DATA;
                if ((comtype & (YAG_TN_SYNC|YAG_TP_SYNC)) == (YAG_TN_SYNC|YAG_TP_SYNC)) continue;
                if ((comtype & (YAG_TP_SYNC|YAG_TP_ASYNC)) == YAG_TP_ASYNC) {
                    if (i == 0) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x12, 0, NULL);
                    if (i == 1) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x11, 0, NULL);
                }
                if ((comtype & (YAG_TN_SYNC|YAG_TN_ASYNC)) == YAG_TN_ASYNC) {
                    if (i == 0) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x22, 0, NULL);
                    if (i == 1) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x21, 0, NULL);
                }
            }
            freeptype(comtypelist[i]);
        }
        /* disable async timing arcs from opposite async event if necessary */
        for (i=0; i<3; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                if (fIgnoreFalse && (ptbranch->TYPE & YAG_FALSECONF) != 0) continue;
                if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    if (i == 2 && ptlink->NEXT == NULL) continue;
                    linktype = ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK);
                    pttrans = ptlink->ULINK.LOTRS;
                    if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL && ((i == 0 && linktype == CNS_TNLINK) || (i == 1 && linktype == CNS_TPLINK))) {
                        pttrans = (lotrs_list *)ptuser->DATA;
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        linktype = (linktype == CNS_TNLINK) ? CNS_TPLINK : CNS_TNLINK;
                        inname = ptincone->NAME;
                    }
                    else {
                        ptincone = (cone_list *)getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA;
                        if (ptincone == NULL) continue;
                        inname = ptincone->NAME;
                    }
                    if (yagGetChain(async_command_list, inname) == NULL) continue;
                    j = (i == 2) ? 0 : i;
                    if ((ptptype = getptype(asynctypelist[j], (long)inname)) == NULL) {
                        ptptype = addptype(asynctypelist[j], (long)inname, NULL);
                        asynctypelist[j] = ptptype;
                    }
                    if (i == 2) {
                        if ((ptptype2 = getptype(asynctypelist[1], (long)inname)) == NULL) {
                            ptptype2 = addptype(asynctypelist[1], (long)inname, NULL);
                            asynctypelist[1] = ptptype2;
                        }
                    }
                    else ptptype2 = NULL;
                    if ((ptbranch->TYPE & YAG_ASYNC) == YAG_ASYNC) {
                        ptptype->DATA = (char *)(((long)ptptype->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_ASYNC : YAG_TP_ASYNC));
                        if (ptptype2) ptptype2->DATA = (char *)(((long)ptptype2->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_ASYNC : YAG_TP_ASYNC));
                    }
                    else {
                        ptptype->DATA = (char *)(((long)ptptype->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_SYNC : YAG_TP_SYNC));
                        if (ptptype2) ptptype2->DATA = (char *)(((long)ptptype2->DATA)|((linktype == CNS_TNLINK) ? YAG_TN_SYNC : YAG_TP_SYNC));
                    }
                }
            }
        }
        for (i=0; i<2; i++) {
            for (ptptype = asynctypelist[i]; ptptype; ptptype = ptptype->NEXT) {
                asynctype = (long)ptptype->DATA;
                if ((asynctype & (YAG_TN_ASYNC|YAG_TP_ASYNC)) == (YAG_TN_ASYNC|YAG_TP_ASYNC)) continue;
                if ((asynctype & (YAG_TP_ASYNC|YAG_TP_SYNC)) == YAG_TP_SYNC) {
                    if (i == 0) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x12, 0, NULL);
                    if (i == 1) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x11, 0, NULL);
                }
                if ((asynctype & (YAG_TN_ASYNC|YAG_TN_SYNC)) == YAG_TN_SYNC) {
                    if (i == 0) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x22, 0, NULL);
                    if (i == 1) inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone->NAME), INF_DISABLE_GATE_DELAY, infTasVectName((char *)ptptype->TYPE), NULL, 0x21, 0, NULL);
                }
            }
            freeptype(asynctypelist[i]);
        }
    }
}

static chain_list *
yagGetAsyncCommands(cone_list *ptcone, chain_list **command_list, pCircuit globalCct, chain_list *latch_async_inputs)
{
    edge_list  *ptedge;
    chain_list *async_commands = NULL;
    chain_list *ptchain;
    chain_list *tmp_expr;
    chain_list *support;
    pNode       expr;
    int         added;
    char       *name, *invname;
    chain_list *invlist, *invchain;

    if ((ptcone->TYPE & CNS_RS) == CNS_RS) return NULL;
    
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        added = FALSE;
        if ((ptedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptedge->TYPE & CNS_EXT) != CNS_EXT) {
            name = ptedge->UEDGE.CONE->NAME;
        }
        else name = ptedge->UEDGE.LOCON->NAME;
        if (yagGetChain(*command_list, name) == NULL) continue;
        tmp_expr = createAtom_no_NA(name);
        expr = ablToBddCct(globalCct, tmp_expr);
        freeExpr(tmp_expr);
        if (expr == NULL) continue;
        
        support = supportChain_listBddExpr(globalCct, expr);
        if (support == NULL) continue;
        /* async command if at all influenced by latch async list */
        for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(latch_async_inputs, ptchain->DATA) != NULL) break;
        }
        if (ptchain != NULL) {
            async_commands = addchain(async_commands, name);
            *command_list = delchaindata(*command_list, name);
            added = TRUE;
        }
        freechain(support);
        if (added) continue;
        invlist = getInvList(name);
        for (invchain = invlist; invchain; invchain = invchain->NEXT) {
            invname = (char *)invchain->DATA;
            tmp_expr = createAtom_no_NA(invname);
            expr = ablToBddCct(globalCct, tmp_expr);
            freeExpr(tmp_expr);
            if (expr == NULL) continue;
            support = supportChain_listBddExpr(globalCct, expr);
            if (support == NULL) continue;
            /* async command if at all influenced by latch async list */
            for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                if (yagGetChain(latch_async_inputs, ptchain->DATA) != NULL) break;
            }
            freechain(support);
            if (ptchain != NULL) {
                async_commands = addchain(async_commands, name);
                *command_list = delchaindata(*command_list, name);
                break;
            }
        }
        freechain(invlist);
    }
    return async_commands;
}

static chain_list *
yagGetAsyncLatchInputs(cone_list *ptcone, chain_list *looplist, pCircuit globalCct, pNode write, pNode *ptasync_write)
{
    chain_list *check_exprs = NULL, *cmd_exprs = NULL;
    chain_list *expr, *command_expr;
    chain_list *ptchain, *ptchain1, *ptnextchain;
    chain_list *support;
    chain_list *clock_commands = NULL, *async_one = NULL, *async_zero = NULL, *async_bdd_list = NULL;
    biabl_list *ptbiabl;
    biabl_list *drivers = NULL;
    befig_list *ptbefig;
    cone_list  *ptloopcone;
    chain_list *write_support;
    ptype_list *ptuser;
    pNode       async_check, command, check;
    char       *command_name;
    int         changes, abandon = FALSE;

    *ptasync_write = BDD_zero;
    if ((ptcone->TYPE & CNS_RS) != 0) return NULL;

    ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
    if (ptuser) {
        ptbefig = (befig_list *)ptuser->DATA;
        if (ptbefig->BEREG) drivers = ptbefig->BEREG->BIABL;
    }
    if (drivers == NULL) return NULL;
    
    ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
    if (ptuser) {
        ptbefig = (befig_list *)ptuser->DATA;
        if (ptbefig->BEREG) drivers = ptbefig->BEREG->BIABL;
    }
    if (drivers == NULL) return NULL;
    
    write_support = supportChain_listBddExpr(globalCct, write);

    /* generate list of expressions which must be non-constant */
    /* and condition for non-asynchronous write                */
    async_check = BDD_one;
    for (ptbiabl = drivers; ptbiabl; ptbiabl = ptbiabl->NEXT) {
        if (yagOneExpr(ptbiabl->VALABL) || yagZeroExpr(ptbiabl->VALABL)) {
            async_check = applyBinBdd(AND, async_check, notBdd(ablToBddCct(globalCct, ptbiabl->CNDABL)));
            *ptasync_write = applyBinBdd(OR, *ptasync_write, ablToBddCct(globalCct, ptbiabl->CNDABL));
        }
        else cmd_exprs = addchain(cmd_exprs, yagApplyConstraints(ablToBddCct(globalCct, ptbiabl->CNDABL), NULL));
    }
    if (oneBdd(async_check)) {
      freechain(write_support);
      freechain(cmd_exprs);
      return NULL;
    }
    async_check = yagApplyConstraints(async_check, NULL);
    *ptasync_write = yagApplyConstraints(*ptasync_write, NULL);
    for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
        ptloopcone = (cone_list *)ptchain->DATA;
        if ((ptuser = getptype(ptloopcone->USER, YAG_DUALEXPR_PTYPE)) != NULL) {
            expr = (chain_list *)ptuser->DATA;
            check_exprs = addchain(check_exprs, ablToBddCct(globalCct, expr));
        }
        else {
            if ((ptuser = getptype(ptloopcone->USER, CNS_UPEXPR)) != NULL) {
                expr = (chain_list *)ptuser->DATA;
                check_exprs = addchain(check_exprs, ablToBddCct(globalCct, expr));
            }
            if ((ptuser = getptype(ptloopcone->USER, CNS_DNEXPR)) != NULL) {
                expr = (chain_list *)ptuser->DATA;
                check_exprs = addchain(check_exprs, yagApplyConstraints(ablToBddCct(globalCct, expr), NULL));
            }
        }
    }
    
    /* Separate commands into list of clock commands and list of asynchronous commands */ 
    do {
        changes = FALSE;
        for (ptchain = write_support; ptchain; ptchain = ptchain->NEXT) {
            command_name = (char *)ptchain->DATA;
            expr = createAtom(command_name);
            command = ablToBddCct(globalCct, expr);
            freeExpr(expr);
            if (yagGetChain(clock_commands, command_name) != NULL) continue;
            if (zeroBdd(constraintBdd(async_check, command))) {
                async_zero = addchain(async_zero, command_name);
                async_check = constraintBdd(async_check, notBdd(command));
                async_bdd_list = addchain(async_bdd_list, notBdd(command));
                changes = TRUE;
            }
            else if (zeroBdd(constraintBdd(async_check, notBdd(command)))) {
                async_one = addchain(async_one, command_name);
                async_check = constraintBdd(async_check, command);
                async_bdd_list = addchain(async_bdd_list, command);
                changes = TRUE;
            }
        }
        for (ptchain = check_exprs; ptchain; ptchain = ptnextchain) {
            ptnextchain = ptchain->NEXT;
            check = (pNode)ptchain->DATA;
            for (ptchain1 = async_bdd_list; ptchain1; ptchain1 = ptchain1->NEXT) {
                check = constraintBdd(check, (pNode)ptchain1->DATA);
            }
            ptchain->DATA = (chain_list *)check;
            if (zeroBdd(check) || oneBdd(check)) {
                abandon = TRUE;
                break;
            }
        }
        for (ptchain = cmd_exprs; ptchain; ptchain = ptnextchain) {
            ptnextchain = ptchain->NEXT;
            check = (pNode)ptchain->DATA;
            for (ptchain1 = async_bdd_list; ptchain1; ptchain1 = ptchain1->NEXT) {
                check = constraintBdd(check, (pNode)ptchain1->DATA);
            }
            ptchain->DATA = (chain_list *)check;
            if (zeroBdd(check) || oneBdd(check)) {
                abandon = TRUE;
                break;
            }
            support = supportChain_listBddExpr(globalCct, check);
            if (yagCountChains(support) == 1) {
                command_name = (char *)support->DATA;
                clock_commands = addchain(clock_commands, command_name);
                command_expr = createAtom(command_name);
                command = ablToBddCct(globalCct, command_expr);
                freeExpr(command_expr);
                cmd_exprs = delchain(cmd_exprs, ptchain);
                changes = TRUE;
                /* adapt the async_check expr with info of sure clock */
                async_check = applyBinBdd(AND, constraintBdd(async_check, command), constraintBdd(async_check, notBdd(command)));
            }
            freechain(support);
        }
    } while (changes && (yagCountChains(async_bdd_list) + yagCountChains(clock_commands) < yagCountChains(write_support)));

    freechain(cmd_exprs);
    freechain(check_exprs);
    freechain(async_bdd_list);
    freechain(clock_commands);
    freechain(write_support);
    if (abandon) {
        freechain(async_one);
        freechain(async_zero);
        return NULL;
    }
    else return append(async_one, async_zero);
}

chain_list *
yagGetCommands(pCircuit globalCct, cone_list *ptcone, pNode condition, int strict, chain_list **ptdatacommands)
{
    edge_list  *ptedge;
    chain_list *ptcommands = NULL;
    chain_list *ptchain;
    chain_list *support;
    chain_list *condition_support;
    pNode       expr;
    int         added;
    char       *name, *invname;
    chain_list *invlist, *invchain;

    if ((ptcone->TYPE & CNS_RS) == CNS_RS) return NULL;
    condition_support = supportChain_listBddExpr(globalCct, condition);
    
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        added = FALSE;
        if ((ptedge->TYPE & CNS_BLEEDER) != 0) continue;
        if ((ptedge->TYPE & CNS_EXT) != CNS_EXT) {
            name = ptedge->UEDGE.CONE->NAME;
        }
        else name = ptedge->UEDGE.LOCON->NAME;
        expr = searchOutputCct_no_NA(globalCct, name);
        if (expr == NULL) continue;
        /* apply the contraints */
        expr = yagApplyConstraints(expr, NULL);

        support = supportChain_listBddExpr(globalCct, expr);
        if (support == NULL) continue;
        if (strict) { /* a command must only be influenced by write support */
            for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                if (yagGetChain(condition_support, ptchain->DATA) == NULL) {
                    if (ptchain->DATA != ptcone->NAME || support->NEXT == NULL) break;
                }
            }
            if (ptchain == NULL) {
                ptcommands = addchain(ptcommands, name);
                added = TRUE;
            }
        }
        else { /* command if at all influenced by write support */
            for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                if (yagGetChain(condition_support, ptchain->DATA) != NULL) break;
            }
            if (ptchain != NULL) {
                ptcommands = addchain(ptcommands, name);
                added = TRUE;
            }
        }
        if (ptdatacommands != NULL && strict) { /* datacommand if at all influenced but not in strict list */
            for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                if (yagGetChain(condition_support, ptchain->DATA) != NULL) break;
            }
            if (ptchain != NULL) {
                if (ptcommands == NULL || ptcommands->DATA != name) {
                    *ptdatacommands = addchain(*ptdatacommands, name);
                    added = TRUE;
                }
            }
        }
        freechain(support);
        if (added) continue;
        invlist = getInvList(name);
        for (invchain = invlist; invchain; invchain = invchain->NEXT) {
            invname = (char *)invchain->DATA;
            expr = searchOutputCct_no_NA(globalCct, invname);
            if (expr == NULL) continue;
            /* apply the contraints */
            expr = yagApplyConstraints(expr, NULL);
            
            support = supportChain_listBddExpr(globalCct, expr);
            if (support == NULL) continue;
            if (strict) { /* a command must only be influenced by write support */
                for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                    if (yagGetChain(condition_support, ptchain->DATA) == NULL) {
                        if (ptchain->DATA != ptcone->NAME || support->NEXT == NULL) break;
                    }
                }
                if (ptchain == NULL) {
                    ptcommands = addchain(ptcommands, name);
                    freechain(support);
                    break;
                }
            }
            else { /* command if at all influenced by write support */
                for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                    if (yagGetChain(condition_support, ptchain->DATA) != NULL) break;
                }
                if (ptchain != NULL) {
                    ptcommands = addchain(ptcommands, name);
                    freechain(support);
                    break;
                }
            }
            if (ptdatacommands != NULL && strict) { /* datacommand if at all influenced but not in strict list */
                for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                    if (yagGetChain(condition_support, ptchain->DATA) != NULL) break;
                }
                if (ptchain != NULL) {
                    if (ptcommands == NULL || ptcommands->DATA != name) {
                        *ptdatacommands = addchain(*ptdatacommands, name);
                        freechain(support);
                        break;
                    }
                }
            }
            freechain(support);
        }
        freechain(invlist);
    }
    freechain(condition_support);
    return ptcommands;
}

void
yagDisableRSArcs(cone_list *ptcone1, cone_list *ptcone2, cone_list *ptloopcone1, cone_list *ptloopcone2, long rstype, short cuttype)
{
    chain_list *ptinlist1 = NULL, *ptinlist2 = NULL;
    chain_list *ptchain;
    cone_list *ptincone1, *ptincone2;
    edge_list *ptedge;
    inffig_list *ifl;
    ptype_list *ptuser;
    long infotype;
    int count1 = 0, count2 = 0;

    for (ptedge = ptcone1->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        if (ptedge->UEDGE.CONE != ptloopcone2) ptinlist1 = addchain(ptinlist1, ptedge->UEDGE.CONE);
        count1++;
    }
    for (ptedge = ptcone2->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != 0) continue;
        if (ptedge->UEDGE.CONE != ptloopcone1) ptinlist2 = addchain(ptinlist2, ptedge->UEDGE.CONE);
        count2++;
    }

    infotype = 0;
    ptuser = getptype(ptcone1->USER, YAG_INFO_PTYPE);
    if (ptuser != NULL) infotype |= (long)ptuser->DATA & (YAG_INFORSMARK|YAG_INFORSLEGAL|YAG_INFORSILLEGAL);
    ptuser = getptype(ptcone2->USER, YAG_INFO_PTYPE);
    if (ptuser != NULL) infotype |= (long)ptuser->DATA & (YAG_INFORSMARK|YAG_INFORSLEGAL|YAG_INFORSILLEGAL);
    switch (infotype) {
        case 0: break;
        case YAG_INFORSMARK:
                freechain(ptinlist1);
                freechain(ptinlist2);
                return;
        case YAG_INFORSLEGAL: cuttype = YAG_RS_LEGAL; break;
        case YAG_INFORSILLEGAL: cuttype = YAG_RS_ILLEGAL; break;
        default:
                freechain(ptinlist1);
                freechain(ptinlist2);
                return;
    }

    if (cuttype == 0)
    {
      freechain(ptinlist1);
      freechain(ptinlist2);
      return;
    }

    ifl=getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);
    if (ifl == NULL) ifl = addinffig(YAG_CONTEXT->YAG_FIGNAME);
    
    if (rstype == CNS_NOR) {
        inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone1->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptloopcone2->NAME), NULL, 0x21, 0, NULL);
        inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone2->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptloopcone1->NAME), NULL, 0x21, 0, NULL);
        if ((cuttype & YAG_RS_LEGAL) != 0) {
            for (ptchain = ptinlist1; ptchain; ptchain = ptchain->NEXT) {
                ptincone1 = (cone_list *)ptchain->DATA;
                inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone1->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptincone1->NAME), NULL, 0x12, 0, NULL);
            }
            for (ptchain = ptinlist2; ptchain; ptchain = ptchain->NEXT) {
                ptincone2 = (cone_list *)ptchain->DATA;
                inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone2->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptincone2->NAME), NULL, 0x12, 0, NULL);
            }
        }
    }
    if (rstype == CNS_NAND) {
        inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone1->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptloopcone2->NAME), NULL, 0x12, 0, NULL);
        inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone2->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptloopcone1->NAME), NULL, 0x12, 0, NULL);
        if ((cuttype & YAG_RS_LEGAL) != 0) {
            for (ptchain = ptinlist1; ptchain; ptchain = ptchain->NEXT) {
                ptincone1 = (cone_list *)ptchain->DATA;
                inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone1->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptincone1->NAME), NULL, 0x21, 0, NULL);
            }
            for (ptchain = ptinlist2; ptchain; ptchain = ptchain->NEXT) {
                ptincone2 = (cone_list *)ptchain->DATA;
                inf_AddAssociation(ifl, INF_LOADED_LOCATION, infTasVectName(ptcone2->NAME), INF_DISABLE_GATE_DELAY, infTasVectName(ptincone2->NAME), NULL, 0x21, 0, NULL);
            }
        }
    }
    freechain(ptinlist1);
    freechain(ptinlist2);
}

static void
yagAutoRSCheck(cone_list *ptcone, chain_list *looplist)
{
    cone_list      *loopcone;
    biabl_list     *drivers = NULL;
    biabl_list     *ptbiabl, *weakdriver = NULL;
    befig_list     *ptbefig = NULL;
    ptype_list     *ptuser;
    long            rstype = 0;
    int             rslatch = FALSE, has_set = FALSE, has_reset = FALSE;

    ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
    if (ptuser) {
        ptbefig = (befig_list *)ptuser->DATA;
        if (ptbefig->BEREG) drivers = ptbefig->BEREG->BIABL;
    }
    if (drivers == NULL) return;
    
    if (YAG_CONTEXT->YAG_AUTO_RS > 0) {
        rslatch = TRUE;
        has_set = FALSE;
        has_reset = FALSE;
        for (ptbiabl = drivers; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if ((ptbiabl->FLAG & BEH_CND_WEAK) == BEH_CND_WEAK) {
                weakdriver = ptbiabl;
                continue;
            }
            if (!yagOneExpr(ptbiabl->VALABL) && !yagZeroExpr(ptbiabl->VALABL)) {
                rslatch = FALSE;
            }
            if (yagOneExpr(ptbiabl->VALABL)) has_set = TRUE;
            if (yagZeroExpr(ptbiabl->VALABL)) has_reset = TRUE;
        }
        if (has_set == FALSE || has_reset == FALSE) rslatch = FALSE;
        if (yagCountChains(looplist) != 1) rslatch = FALSE;
        else {
            loopcone = (cone_list *)looplist->DATA;
            if (yagMatchNAND(ptcone, loopcone) > 0 && yagMatchNAND(loopcone, ptcone) > 0) {
                rstype = CNS_NAND;
            }
            if (yagMatchNOR(ptcone, loopcone) > 0 && yagMatchNOR(loopcone, ptcone) > 0) {
                rstype = CNS_NOR;
            }
            if (rstype == 0 && (YAG_CONTEXT->YAG_AUTO_RS & YAG_RS_TOLERANT) == 0) rslatch = FALSE;
            if (rslatch) {
                if ((YAG_CONTEXT->YAG_AUTO_RS & YAG_RS_MODEL) == 0) {
                    beh_delbefig(ptbefig, ptbefig, 'Y');
                    ptcone->USER = delptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
                }
                else ptbefig->BEREG->BIABL = beh_delbiabl(ptbefig->BEREG->BIABL, weakdriver, 'Y');
                ptcone->USER = addptype(ptcone->USER, YAG_BISTABLE_PTYPE, loopcone);
                loopcone->USER = addptype(loopcone->USER, YAG_BISTABLE_PTYPE, ptcone);
                ptcone->TECTYPE |= rstype;
                loopcone->TECTYPE |= rstype;
                ptcone->TYPE |= CNS_RS;
                loopcone->TYPE |= CNS_RS;
            }
        }
    }
}

static cone_list *
yagAutoMemsymCheck(cone_list *ptcone, chain_list *looplist, pNode condition, chain_list *commandlist, chain_list **ptptloopcommands)
{
    chain_list *ptchain, *ptchain1;
    chain_list *ptdata, *ptloopdata;
    chain_list *ptdatachain = NULL, *ptloopdatachain = NULL;
    chain_list *data_abl, *loopdata_abl;
    cone_list  *ptloopcone;
    edge_list  *ptedge;
    pNode       dataexpr, loopdataexpr;
    char       *name;

    for (ptchain = looplist; ptchain; ptchain = ptchain->NEXT) {
        ptloopcone = (cone_list *)ptchain->DATA;
        *ptptloopcommands = yagGetCommands(YAG_CONTEXT->YAG_CURCIRCUIT, ptloopcone, condition, TRUE, NULL);
        
        /* check there is at least one command in common */
        for (ptchain1 = commandlist; ptchain1; ptchain1 = ptchain1->NEXT) {
            if (yagGetChain(*ptptloopcommands, ptchain1->DATA) != NULL) break;
        }
        if (ptchain1 == NULL) continue;
        
        /* build list of data inputs for cone and loopcone */
        for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
            if (ptedge->UEDGE.PTR == ptloopcone) continue;
            if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) name = ptedge->UEDGE.LOCON->NAME;
            else name = ptedge->UEDGE.CONE->NAME;
            if (yagGetChain(commandlist, name) == NULL) ptdatachain = addchain(ptdatachain, name);
        }
        for (ptedge = ptloopcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
            if (ptedge->UEDGE.PTR == ptcone) continue;
            if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) name = ptedge->UEDGE.LOCON->NAME;
            else name = ptedge->UEDGE.CONE->NAME;
            if (yagGetChain(*ptptloopcommands, name) == NULL) ptloopdatachain = addchain(ptloopdatachain, name);
        }
        
        for (ptdata = ptdatachain; ptdata; ptdata = ptdata->NEXT) {
            for (ptloopdata = ptloopdatachain; ptloopdata; ptloopdata = ptloopdata->NEXT) {
                data_abl = createAtom(ptdata->DATA);
                dataexpr = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, data_abl);
                loopdata_abl = createAtom(ptloopdata->DATA);
                loopdataexpr = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, loopdata_abl);
                freeExpr(data_abl);
                freeExpr(loopdata_abl);
                if (checkMemsymData(dataexpr, loopdataexpr)) break;
            }
            if (ptloopdata) break;
        }
        if (ptdata) break;
    }
    if (ptchain) { /* Memsym found */
        ptcone->TYPE |= CNS_MEMSYM;
        ptloopcone->TYPE |= CNS_MEMSYM;
        ptedge = yagGetEdge(ptcone->INCONE, ptloopcone);
        ptedge->TYPE |= CNS_MEMSYM;
        ptedge = yagGetEdge(ptloopcone->INCONE, ptcone);
        ptedge->TYPE |= CNS_MEMSYM;
        ptcone->USER = addptype(ptcone->USER, YAG_MEMORY_PTYPE, ptloopcone);
        ptloopcone->USER = addptype(ptloopcone->USER, YAG_MEMORY_PTYPE, ptcone);
        return ptloopcone;
    }
    return NULL;
}

static int
checkMemsymData(pNode dataexpr, pNode loopdataexpr)
{
    chain_list *datasupport, *loopdatasupport;
    chain_list *ptchain;
    pNode xor_expr, test_expr; 

    xor_expr = applyBinBdd(OR, applyBinBdd(AND, dataexpr, notBdd(loopdataexpr)), applyBinBdd(AND, notBdd(dataexpr), loopdataexpr));
    test_expr = yagApplyConstraints(xor_expr, NULL);
    if (!zeroBdd(test_expr)) {
        datasupport = supportChain_listBddExpr(YAG_CONTEXT->YAG_CURCIRCUIT, dataexpr);
        loopdatasupport = supportChain_listBddExpr(YAG_CONTEXT->YAG_CURCIRCUIT, loopdataexpr);
        for (ptchain = datasupport; ptchain; ptchain = ptchain->NEXT) {
            if (yagGetChain(loopdatasupport, ptchain->DATA) != NULL) break;
        }
        freechain(datasupport);
        freechain(loopdatasupport);
        if (ptchain || (xor_expr != test_expr)) return TRUE;
    }
    return FALSE;
}

static chain_list *
getInvList(char *name)
{
    ptype_list *ptptype;
    chain_list *ptsupport;
    chain_list *result = NULL;
    
    for (ptptype = YAG_CONTEXT->YAG_CONE_GRAPH->EXTRA_CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
        ptsupport = (chain_list *)ptptype->TYPE;
        if (ptsupport->DATA == name) result = addchain(result, ptsupport->NEXT->DATA);
        else if (ptsupport->NEXT->DATA == name) result = addchain(result, ptsupport->DATA);
    }
    return result;
}
