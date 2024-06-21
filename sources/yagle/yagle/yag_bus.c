/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Produit : YAGLE  v3.0                                                 */
/*    Fichier : yag_bus.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 Avertec                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 30/01/2002     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

/*************************************************************************
 *                     function yagAnalyseBus()                          *
 *************************************************************************/

void 
yagAnalyseBus(ptcone, fIgnoreFalse)
    cone_list      *ptcone;
    int             fIgnoreFalse;
{
    ptype_list     *ptuser;
    chain_list     *ptsupport, *ptallsupport, *tmpabl, *ptchain;
    ptype_list     *ptptype;
    chain_list     *ptcommands;
    chain_list     *tempabl;
    chain_list     *resconf_abl;
    edge_list      *save_inputs, *ptedge;
    branch_list    *ptbranch;
    biabl_list     *ptdrivers;
    graph          *savegraph;
    pCircuit        directCct;
    pCircuit        savecircuit;
    long            branchmask;
    abl_pair        local_abl;
    pNode           local_sup, local_sdn;
    pNode           sup, sdn, conflict, write;
    pNode           contraint;
    pNode           local_conflict, local_resconf, global_resconf;
    pNode           local_expr, global_expr;
    short           save_prop_hz, save_depth;
    chain_list     *datacommands = NULL;
    int             has_conflict = FALSE;

    avt_log(LOGYAG,1, "Analysing Bus at '%s'\n", ptcone->NAME);

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
    /* Ignore simple external connector branch and input */
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) {
            ptbranch->TYPE |= CNS_BLEEDER;
            ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
            ptedge->TYPE |= CNS_BLEEDER;
        }
    }
        
    save_prop_hz = YAG_CONTEXT->YAG_PROP_HZ;
    save_depth = YAG_CONTEXT->YAG_DEPTH;
    YAG_CONTEXT->YAG_DEPTH = YAG_CONTEXT->YAG_BUS_DEPTH;
    YAG_CONTEXT->YAG_PROP_HZ = FALSE;
    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
    YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;

    directCct = yagBuildDirectCct(ptcone);
    if (directCct->countI > YAG_CONTEXT->YAG_AUTOLOOP_CEILING) {
        destroyCct(directCct);
        yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = savegraph;
        YAG_CONTEXT->YAG_CURCIRCUIT = savecircuit;
        YAG_CONTEXT->YAG_DEPTH = save_depth;
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            if (ptbranch->LINK->NEXT == NULL) {
                ptbranch->TYPE &= ~CNS_BLEEDER;
                ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON); {
                    ptedge->TYPE &= ~CNS_BLEEDER;
                }
            }
        }
        ptcone->TYPE |= CNS_TRI;
        ptcone->TYPE &= ~CNS_LATCH;
        if (fIgnoreFalse) {
            yagFreeEdgeList(ptcone->INCONE);
            ptcone->INCONE = save_inputs;
        }
        return;
    }

    /* Generate expression for cone as a function of its direct inputs */

    branchmask = CNS_RESBRANCH|CNS_NOT_FUNCTIONAL|CNS_IGNORE|CNS_MASK_PARA;
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

    /* Generate up and down expressions for cone in terms of primary variables */

    sup = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, local_abl.UP);
    sdn = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, local_abl.DN);

    /* apply the contraints */
    sup = yagApplyConstraints(sup, NULL);
    sdn = yagApplyConstraints(sdn, NULL);

    conflict = applyBinBdd(AND, sup, sdn);
    write = applyBinBdd(OR, sup, sdn);

    freeExpr(local_abl.UP);
    freeExpr(local_abl.DN);

    /* Generate condition for electrical conflict */

    local_conflict = applyBinBdd(AND, local_sup, local_sdn);

    if (conflict != BDD_zero) {
        local_resconf = yagResolveConflicts(directCct, YAG_CONTEXT->YAG_CURCIRCUIT, &local_conflict, ptcone);
        if (local_conflict != BDD_zero) {
            if ( avt_islog(2,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
                avt_log(LOGYAG,1,"Conflict when : ");
                tempabl = bddToAblCct(directCct, local_conflict);
                displayInfExprLog(LOGYAG, 1, tempabl);
                freeExpr(tempabl);
                avt_log(LOGYAG,1," = '1'\n");
            }
            has_conflict = TRUE;
        }
    }
    else {
        local_resconf = BDD_zero;
        local_conflict = BDD_zero;
    }

    resconf_abl = bddToAblCct(directCct, local_resconf);
    global_resconf = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, resconf_abl);
    freeExpr(resconf_abl);
    local_expr = applyBinBdd(OR, local_resconf, applyBinBdd(AND, local_sup, notBdd(local_sdn)));
    global_expr = applyBinBdd(OR, global_resconf, applyBinBdd(AND, sup, notBdd(sdn)));

    if ((ptuser = getptype(ptcone->USER, YAG_BUSBEFIG_PTYPE)) != NULL) {
        beh_delbefig((befig_list *)ptuser->DATA, (befig_list *)ptuser->DATA, 'Y');
        ptcone->USER = delptype(ptcone->USER, YAG_BUSBEFIG_PTYPE);
    }
    addOutputCct_no_NA(directCct, ptcone->NAME, local_expr);
    tmpabl = bddToAblCct(directCct, local_expr);
    if ((ptcone->TYPE & (CNS_LATCH|CNS_EXT)) == 0) ptallsupport = supportChain_listExpr(tmpabl);
    else ptallsupport = NULL;
    freeExpr(tmpabl);

    ptsupport = yagGetWriteSupport(YAG_CONTEXT->YAG_CURCIRCUIT, directCct, write, ptallsupport);
    if (ptsupport) {
        ptdrivers = yagExtractBusDrivers(directCct, ptcone, ptsupport, local_conflict, write);
        if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH || (ptcone->TYPE & CNS_EXT) == CNS_EXT) {
            yagBuildDirectInputBdds(ptcone);
            ptcommands = yagGetCommands(YAG_CONTEXT->YAG_CURCIRCUIT, ptcone, write, FALSE, &datacommands);
            if (fIgnoreFalse) {
                yagFreeEdgeList(ptcone->INCONE);
                ptcone->INCONE = save_inputs;
                fIgnoreFalse = FALSE;
            }
            if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
                yagMarkMemory(NULL, ptcone, ptcommands, NULL, NULL, notBdd(write), write, NULL, NULL, fIgnoreFalse);
                ptcone->USER = addptype(ptcone->USER, YAG_LATCHINFO_PTYPE, (void *)DLATCH);
                freechain(datacommands);
            }
            else if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
                yagMarkTristateCommands(ptcone, ptcommands);
            }
            freechain(ptcommands);
        }
        freechain(ptsupport);
        ptcone->TYPE &= ~CNS_CONFLICT;
        if (has_conflict) ptcone->TYPE |= CNS_CONFLICT;
    }
    else {
        if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
            ptcone->TYPE &= ~CNS_LATCH;
            ptcone->TYPE |= CNS_TRI;
        }
    }

    freechain(ptallsupport);
    destroyCct(directCct);
    yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    YAG_CONTEXT->YAG_CONE_GRAPH = savegraph;
    YAG_CONTEXT->YAG_CURCIRCUIT = savecircuit;
    YAG_CONTEXT->YAG_DEPTH = save_depth;
    if (fIgnoreFalse) {
        yagFreeEdgeList(ptcone->INCONE);
        ptcone->INCONE = save_inputs;
    }
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) {
            ptbranch->TYPE &= ~CNS_BLEEDER;
            ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON); {
                ptedge->TYPE &= ~CNS_BLEEDER;
            }
        }
    }
}

void yagMarkTristateCommands(ptcone, ptcommands)
    cone_list  *ptcone;
    chain_list *ptcommands;
{
    edge_list  *ptedge;
    char       *name;

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) != CNS_EXT) {
            name = ptedge->UEDGE.CONE->NAME;
        }
        else name = ptedge->UEDGE.LOCON->NAME;
        if (yagGetChain(ptcommands, name) != NULL) ptedge->TYPE |= CNS_HZCOM;
    }
}

