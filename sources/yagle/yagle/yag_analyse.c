/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_analyse.c                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 10/06/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static jmp_buf abandon_analyse;

static int get_opinion(void);
static chain_list *add_node_bdd(chain_list *bddlist, gnode_list *ptnode);
static void build_circuit(branch_list *ptbranch, jmp_buf abandon_env);
static pNode extend_circuit(cone_list *ptcone, long type, jmp_buf abandon_env);
static void add_mutex_pair(chain_list **ptmutex, char *name1, char *name2);
static void check_cone(cone_list *ptcone);
static void yagCheckGlobalDuality (cone_list *ptcone);
static void check_big_cone(cone_list *ptcone, gnode_list *rootnode);
static ht *order(branch_list *ptbranchlist, table *ptnames);
static void clean_precharge_marks(cone_list *ptcone, cone_list *ptloopcone);

jmp_buf *yagAbandonAnalyse()
{
    return &abandon_analyse;
}

int
yagCheckAddTransLink(branch_list *ptbranch, lotrs_list  *pttrans)
{
    cone_list   *ptcone;
    long        nodetype;
    pNode       bdd;
    int         result, transblocked = FALSE;

    if (!YAG_CONTEXT->YAG_USE_FCF) return YAG_YES;

    /* recreate support graph and circuit if nonexistent */

    if (YAG_CONTEXT->YAG_SUPPORT_GRAPH == NULL) {
        YAG_CONTEXT->YAG_SUPPORT_GRAPH = yagNewGraph();
        YAG_CONTEXT->YAG_SUPPORT_GRAPH->COMPLETE = TRUE;
        build_circuit(ptbranch, *yagAbandonGrow());
    }

    /* extend support graph and circuit */
    ptcone = (cone_list *)pttrans->GRID;
    nodetype = ((pttrans->TYPE & CNS_TN) == CNS_TN ? TN_NODE : TP_NODE);

    if (YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        avt_log(LOGYAG, 3,  "Try to add transistor '%s' type %ld driven by '%s'\n", (pttrans->TRNAME != NULL) ? pttrans->TRNAME : "", nodetype, ptcone->NAME);
    }

    if ((bdd = extend_circuit(ptcone, nodetype, *yagAbandonGrow())) == NULL) return YAG_NO;

    /* detect constants to mark resistive links */
    if ((bdd == BDD_one && nodetype == TN_NODE) || (bdd == BDD_zero && nodetype == TP_NODE)) {
        pttrans->TYPE |= RESIST;
    }

    if ((bdd == BDD_one && nodetype == TP_NODE) || (bdd == BDD_zero && nodetype == TN_NODE)) {
        if (avt_islog(3,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            avt_log(LOGYAG, 3,  "\tTransistor blocked\n");
        }
        result = YAG_NO;
        transblocked = TRUE;
    }
    else result = get_opinion();

    if (avt_islog(3,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        if (result == YAG_NO) avt_log(LOGYAG, 3,  "\tNO\n");
        else avt_log(LOGYAG, 3,  "\tYES\n");
    }
    if (!transblocked && result == YAG_NO && V_INT_TAB[__YAGLE_KEEP_GLITCHERS].VALUE && yagDetectGlitcher(ptbranch, pttrans, TRUE)) {
        yagWarning(WAR_GLITCHER, YAG_CONTEXT->YAG_CURCIRCUIT->name, NULL, NULL, 0);
        return YAG_YES;
    }

    return result;
}

int
yagRemoveFalseBranches(cone_list *ptcone)
{
    branch_list *brlist[4];
    branch_list *ptbranch, *ptnextbranch;
    long         savetype, typemask;
    int          changed, redo, conflictual, destroy;
    int          i;
    short        save_depth;

    if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) return FALSE;
    if (yagCheckBranches(ptcone) == FALSE) return FALSE;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    avt_log(LOGYAG, 1,  "Attempting to clean cone '%s'\n", ptcone->NAME);

    /* To handle a return from BDD explosion */
    save_depth = YAG_CONTEXT->YAG_DEPTH;
    setBddCeiling(YAG_CONTEXT->YAG_BDDCEILING);
    if (setjmp(abandon_analyse) != 0) {
        yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_DEPTH--;
        if (YAG_CONTEXT->YAG_DEPTH == 0) unsetBddCeiling();
    }

    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);

    typemask = CNS_CONFLICT|CNS_TRI|CNS_MEMSYM;
    savetype = ptcone->TYPE & typemask;
    conflictual = ((ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT);
    changed = FALSE;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptnextbranch) {
            ptnextbranch = ptbranch->NEXT;
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if (i==2 && ptbranch->LINK->NEXT == NULL) continue;
            destroy = zeroBdd(yagCalcBranchConductance(ptbranch));
            if (destroy && V_INT_TAB[__YAGLE_KEEP_GLITCHERS].VALUE && yagDetectGlitcher(ptbranch, NULL, TRUE)) {
                yagWarning(WAR_GLITCHER, YAG_CONTEXT->YAG_CURCIRCUIT->name, NULL, NULL, 0);
                destroy = FALSE;
            }
            if (destroy) {
                yagDestroyBranch(ptcone, ptbranch);
                changed = TRUE;
            }
        }
    }

    unsetBddCeiling();

    if ((ptcone->TYPE & YAG_AUTOLATCH) != 0) {
/*        yagUnmarkLatch(ptcone, FALSE, FALSE);*/
        if (YAG_CONTEXT->YAG_CONE_GRAPH) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
        redo = TRUE;
    }
    else redo = FALSE;

    /* re-verify the cone */
    if (conflictual || changed || redo) {
        yagChainCone(ptcone);
        yagAnalyseCone(ptcone);
    }
    if ((ptcone->TYPE & typemask) != savetype) {
        changed = TRUE;
    }

    if (YAG_CONTEXT->YAG_CONE_GRAPH) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    YAG_CONTEXT->YAG_CONE_GRAPH = NULL;

    YAG_CONTEXT->YAG_DEPTH = save_depth;

    return changed;
}

int
yagDetectDualGlitcherBranches(cone_list *ptcone)
{
    branch_list *brlist[2];
    branch_list *ptbranch, *ptnextbranch;
    long         savetype, typemask;
    int          changed, redo, conflictual, destroy;
    int          i;
    short        save_depth;
    int          setup = FALSE, ret=FALSE;

    if ((ptcone->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS) return FALSE;
    if (yagCheckBranches(ptcone) == FALSE) return FALSE;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;

    avt_log(LOGYAG, 1,  "Detecting glitchers on dual cone '%s'\n", ptcone->NAME);

    /* To handle a return from BDD explosion */
    save_depth = YAG_CONTEXT->YAG_DEPTH;
    setBddCeiling(YAG_CONTEXT->YAG_BDDCEILING);
    if (setjmp(abandon_analyse) != 0) {
        yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_DEPTH--;
        if (YAG_CONTEXT->YAG_DEPTH == 0) unsetBddCeiling();
        setup = FALSE;
    }

    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);

    for (i=0; i<2; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptnextbranch) {
            ptnextbranch = ptbranch->NEXT;
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            destroy = zeroBdd(yagCalcBranchConductance(ptbranch));
            if (yagDetectGlitcher(ptbranch, NULL, FALSE)) {
                if (!setup) {
                    yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
                    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
                    setup = TRUE;
                }
                if (zeroBdd(yagCalcBranchConductance(ptbranch)) && yagDetectGlitcher(ptbranch, NULL, TRUE)) {
                    yagWarning(WAR_GLITCHER, YAG_CONTEXT->YAG_CURCIRCUIT->name, NULL, NULL, 0);
                    ret=TRUE;
                }
            }
        }
    }

    unsetBddCeiling();

    if (YAG_CONTEXT->YAG_CONE_GRAPH) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    YAG_CONTEXT->YAG_CONE_GRAPH = NULL;

    YAG_CONTEXT->YAG_DEPTH = save_depth;
    return ret;
}

pNode
yagCalcBranchConductance(branch_list *ptbranch)
{
    cone_list   *ptcone;
    gnode_list  *node;
    ptype_list  *root;
    chain_list  *bddlist = NULL;
    pNode       bdd, conductance;
    char        *name;
    int         negate;

    /* recreate support graph and circuit */

    YAG_CONTEXT->YAG_SUPPORT_GRAPH = yagNewGraph();
    YAG_CONTEXT->YAG_SUPPORT_GRAPH->COMPLETE = TRUE;
    build_circuit(ptbranch, abandon_analyse);

    for (root = YAG_CONTEXT->YAG_SUPPORT_GRAPH->ROOTNODES; root; root = root->NEXT) {

        node = (gnode_list *)root->DATA;
        ptcone = node->OBJECT.CONE;
        negate = (ptcone->TYPE & CNS_TRI) != 0 && (root->TYPE == TP_NODE);
        if (negate) {
            name = yagDownName(ptcone->NAME);
        }
        else name = ptcone->NAME;

        if ((bdd = searchOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name)) == NULL) {
            yagBug(DBG_NO_BDD, "yagCalcBranchConductance", YAG_CONTEXT->YAG_CURCIRCUIT->name, name, 0);
            continue;
        }
        /* invert bdd for P type transistor */
        if (root->TYPE == TP_NODE) bdd = yagNotBdd(bdd, abandon_analyse);

        bddlist = addchain(bddlist, (void *)bdd);
    }
    if (bddlist == NULL) conductance = BDD_one;
    else if (bddlist->NEXT == NULL) conductance = (pNode)bddlist->DATA;
    else conductance = yagApplyBdd(AND, bddlist, abandon_analyse);
    if (zeroBdd(conductance)) {
        yagFreeGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
        YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;
        freechain(bddlist);
        return conductance;
    }

    /* apply the contraints */
    conductance = yagApplyConstraints(conductance, yagAbandonAnalyse());
        
    freechain(bddlist);
    yagFreeGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
    YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;

    return conductance;
}

int
yagCheckAddExtLink(cone_list *ptcone, locon_list *ptcon, cone_list *ptrootcone)
{
    int          result;

    if (YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        avt_log(LOGYAG, 1,  "Try to add connector link '%s'\n", ptcon->NAME);
    }

/*    if ((ptrootcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) {
        result = YAG_NO;
    }
    else*/ if (ptcone == NULL) result = YAG_YES;
    else {
        if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) {
            result = YAG_NO;
        }
        else result = YAG_YES;
    }

    if (YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        if (result == YAG_NO) avt_log(LOGYAG, 3,  "\tNO\n");
        else avt_log(LOGYAG, 3,  "\tYES\n");
    }

    return result;
}

static int
get_opinion()
{
    gnode_list  *ptnewnode, *ptfather, *ptson;
    chain_list  *ptchain1, *ptchain2;
    ptype_list  *ptlist;
    chain_list  *bddlist = NULL;
    pNode       conductance;
    static int called = 0;
    
    called++;

    for (ptlist = YAG_CONTEXT->YAG_SUPPORT_GRAPH->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        ((gnode_list *)ptlist->DATA)->VISITED = FALSE;
    }

    /* build list of BDDs for correlated outputs */

    ptnewnode = (gnode_list *)YAG_CONTEXT->YAG_SUPPORT_GRAPH->ROOTNODES->DATA;
    bddlist = add_node_bdd(bddlist, ptnewnode);
    ptnewnode->VISITED = TRUE;

    for (ptchain1 = ptnewnode->FATHERS; ptchain1; ptchain1 = ptchain1->NEXT) {
        ptfather = (gnode_list *)ptchain1->DATA;
        for (ptchain2 = ptfather->SONS; ptchain2; ptchain2 = ptchain2->NEXT) {
            ptson = (gnode_list *)ptchain2->DATA;
            if (!ptson->VISITED) {
                bddlist = add_node_bdd(bddlist, ptson);
                ptson->VISITED = TRUE;
            }
        }
    }

    /* verify the conduction function of the branch */

    if (bddlist->NEXT == NULL) conductance = (pNode)bddlist->DATA;
    else conductance = yagApplyBdd(AND, bddlist, *yagAbandonGrow());
    freechain(bddlist);
    if (zeroBdd(conductance)) return YAG_NO;

    /* apply the contraints */
    conductance = yagApplyConstraints(conductance, yagAbandonGrow());
        
    if (!zeroBdd(conductance)) return YAG_YES;
    else return YAG_NO;
}

static chain_list *
add_node_bdd(chain_list *bddlist, gnode_list *ptnode)
{
    cone_list   *ptcone;
    char        *name;
    pNode       bdd;
    int         negate;

    ptcone = ptnode->OBJECT.CONE;
    negate = FALSE;
    name = ptcone->NAME;
    if ((ptnode->TYPE & TP_NODE) != 0) {
        if ((ptcone->TYPE & CNS_TRI) != 0) name = yagDownName(ptcone->NAME);
        else negate = TRUE;
    }

    bdd = searchOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name);
    if (negate) bdd = yagNotBdd(bdd,  *yagAbandonGrow());
    return(addchain(bddlist, bdd));
}

graph *
yagMakeConeGraph(cone_list *ptcone)
{
    graph      *ptgraph;
    edge_list  *inedgelist;
    chain_list *ptchain;
    list_list  *varlist;
    gnode_list *ptnode;
    ptype_list *ptuser;
    char       *name;
    int         numinputs, numoutputs;
    
    if ((ptcone->TYPE & YAG_PARTIAL) != 0 && (ptuser = getptype(ptcone->USER, YAG_INPUTS_PTYPE))) {
        inedgelist = (edge_list *)ptuser->DATA;
    }
    else inedgelist = ptcone->INCONE;
    ptgraph = yagBuildGraph(inedgelist, ptcone, FALSE);

    yagTraverseGraph(ptgraph);
    
    if (YAG_CONTEXT->YAG_CONSTRAINT_LIST) {
        for (ptchain = yagGetConstraint(YAG_CONTEXT->YAG_CONSTRAINT_LIST->NEXT); ptchain; ptchain = yagGetConstraint(ptchain)) {
            varlist = (list_list *)ptchain->DATA;
            ptgraph->CONSTRAINTS = addptype(ptgraph->CONSTRAINTS, (long)varlist->SUPDATA, (void *)varlist->USER);
            ptchain = ptchain->NEXT;
        }
    }
    
    yagAddExtraConstraints(ptgraph);

    /* initialise circuit for analysis */
    numinputs = yagCountChains(ptgraph->PRIMVARS);
    numoutputs = ptgraph->WIDTH;
    YAG_CONTEXT->YAG_CURCIRCUIT = initializeCct(ptcone->NAME, numinputs, numoutputs);

    /* add primary variables to circuit input list */
    for (ptchain = ptgraph->PRIMVARS; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *)ptchain->DATA;
        if ((ptnode->TYPE & CONE_TYPE) != 0) {
            name = ptnode->OBJECT.CONE->NAME;
        }
        else {
            /* node is an external connector */
            name = ptnode->OBJECT.LOCON->NAME;
        }
        addInputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name);
    }
    
    return ptgraph;
}

void
yagDeleteConeGraph(graph *ptgraph)
{
    yagFreeGraph(ptgraph);
    
    /* destroy the circuit */
    destroyCct(YAG_CONTEXT->YAG_CURCIRCUIT);
    YAG_CONTEXT->YAG_CURCIRCUIT = NULL;
}

static void
build_circuit(branch_list *ptbranch, jmp_buf abandon_env)
{
    link_list   *ptlink;
    lotrs_list  *pttrans;
    cone_list   *ptcone;
    pNode       bdd;
    long        nodetype;

    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & (CNS_IN|CNS_INOUT)) != 0) break;
        pttrans = ptlink->ULINK.LOTRS;
        if ((pttrans->TYPE & DIODE) != 0) continue;
        ptcone = (cone_list *)pttrans->GRID;
        nodetype = ((pttrans->TYPE & CNS_TN) == CNS_TN ? TN_NODE : TP_NODE);
        bdd = extend_circuit(ptcone, nodetype, abandon_env);

        /* detect constants to mark resistive links */
        if ((bdd == BDD_one && nodetype == TN_NODE) || (bdd == BDD_zero && nodetype == TP_NODE)) {
            pttrans->TYPE |= RESIST;
            ptlink->TYPE |= CNS_RESIST;
        }
    }
}

static pNode
extend_circuit(cone_list *ptcone, long type, jmp_buf abandon_env)
{
    chain_list  *expr, *tempexpr;
    chain_list  *support;
    gnode_list  *ptsupportnode, *ptnode;
    char        *name;
    pNode       bdd;
    int         negate;
    int         complete, existbdd;

    negate = (ptcone->TYPE & CNS_TRI) != 0 && type == TP_NODE;
    if (negate) {
        name = yagDownName(ptcone->NAME);
    }
    else name = ptcone->NAME;

    complete = yagExtendSupportRoot(YAG_CONTEXT->YAG_SUPPORT_GRAPH, ptcone, type);
    ptsupportnode = (gnode_list *)YAG_CONTEXT->YAG_SUPPORT_GRAPH->ROOTNODES->DATA;
    if (complete) {
        if ((ptsupportnode->TYPE & (TN_NODE|TP_NODE)) == (TN_NODE|TP_NODE)) {
            return NULL;
        }
    }

    ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptcone);
    if (ptnode == (gnode_list *)EMPTYHT) {
        yagBug(DBG_NO_GRAPHNODE, "extend_circuit", ptcone->NAME, NULL, 0);
        return NULL;
    }

    bdd = searchOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name);
    existbdd = (bdd != NULL);

    if (existbdd) {
        support = yagGetPrimVars(YAG_CONTEXT->YAG_CONE_GRAPH, ptnode);
    }
    else {
        tempexpr = createAtom_no_NA(name);
        expr = yagExpandExpr(ptnode, tempexpr, negate);
        freeExpr(tempexpr);
        bdd = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, expr, abandon_env);
        addOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name, bdd);
        freeExpr(expr);
        support = yagGetPrimVars(YAG_CONTEXT->YAG_CONE_GRAPH, ptnode);
    }
    yagExtendSupportGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH, ptsupportnode, support);

    if (support != NULL) freechain(support);

    return bdd;
}

chain_list *
yagGuessMutex(cnsfig_list *ptcnsfig)
{
    cone_list *ptcone, *ptincone;
    locon_list *ptlocon;
    branch_list *brlist[4];
    branch_list *ptbranch;
    link_list *ptlink;
    lotrs_list *pttrans;
    gnode_list *ptnode, *ptsupportnode;
    chain_list *support, *ptchain;
    char *candidate[3];
    long nodetype;
    int linkcount, i;
    chain_list *guess_mutex = NULL;

    /* Mark all latches and connectors constrained */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_EXT)) != 0) ptcone->TYPE |= YAG_CONSTRAINT;
    }
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (getptype(ptlocon->USER, YAG_CONSTRAINT_PTYPE) == NULL) {
            ptlocon->USER = addptype(ptlocon->USER, YAG_CONSTRAINT_PTYPE, NULL);
        }
    }
    
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TECTYPE & YAG_BADCONE) == 0) continue;
        brlist[0] = ptcone->BRVDD;
        brlist[1] = ptcone->BRVSS;
        brlist[2] = ptcone->BREXT;
        brlist[3] = ptcone->BRGND;

        avt_log(LOGYAG, 1,  "MUTEX guessing on cone '%s'\n", ptcone->NAME);
        YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
       
        /* Try to find MUTEX candidate pair in 2nd and 3rd link of bad branch (on fail try 1st and 2nd if not latch) */
        for (i=0; i<4; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                if (yagCountLinks(ptbranch->LINK) < YAG_CONTEXT->YAG_MAX_LINKS) continue;
                YAG_CONTEXT->YAG_SUPPORT_GRAPH = yagNewGraph();
                YAG_CONTEXT->YAG_SUPPORT_GRAPH->COMPLETE = TRUE;
                linkcount = 0;
                candidate[0] = NULL; candidate[1] = NULL; candidate[2] = NULL; 
                for (ptlink = ptbranch->LINK; ptlink && linkcount < 3; ptlink = ptlink->NEXT) {
                    if ((ptlink->TYPE & (CNS_IN|CNS_INOUT)) != 0) break;
                    pttrans = ptlink->ULINK.LOTRS;
                    if ((pttrans->TYPE & DIODE) != 0) continue;
                    ptincone = (cone_list *)pttrans->GRID;
                    nodetype = ((pttrans->TYPE & CNS_TN) == CNS_TN ? TN_NODE : TP_NODE);
                    yagExtendSupportRoot(YAG_CONTEXT->YAG_SUPPORT_GRAPH, ptincone, nodetype);
                    ptsupportnode = (gnode_list *)YAG_CONTEXT->YAG_SUPPORT_GRAPH->ROOTNODES->DATA;
                    ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptincone);
                    support = yagGetJustPrimVars(YAG_CONTEXT->YAG_CONE_GRAPH, ptnode);
                    yagExtendSupportGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH, ptsupportnode, support);
                    if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) != 0 && linkcount == 0) continue;
                    for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
                        ptnode = (gnode_list *)ptchain->DATA;
                        if ((ptnode->TYPE & CONSTRAINT_NODE) != 0) {
                            if (!candidate[linkcount]) {
                                if ((ptnode->TYPE & EXT) != 0) {
                                    candidate[linkcount] = ptnode->OBJECT.LOCON->NAME;
                                }
                                else candidate[linkcount] = ptnode->OBJECT.CONE->NAME;
                            }
                            else {
                                candidate[linkcount] = NULL;
                                break;
                            }
                        }
                    }
                    linkcount++;
                }
                if (candidate[2] && candidate[1]) {
                    add_mutex_pair(&guess_mutex, candidate[2], candidate[1]);
                }
                else if (candidate[1] && candidate[0]) {
                    add_mutex_pair(&guess_mutex, candidate[1], candidate[0]);
                }
                yagFreeGraph(YAG_CONTEXT->YAG_SUPPORT_GRAPH);
                YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;
            }
        }
        if (YAG_CONTEXT->YAG_CONE_GRAPH) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
    }
    return guess_mutex;
}

static void
add_mutex_pair(chain_list **ptmutex, char *name1, char *name2)
{
    chain_list *list1 = NULL, *list2 = NULL;
    chain_list *ptmutexchain, *ptchain;

    for (ptmutexchain = *ptmutex; ptmutexchain; ptmutexchain = ptmutexchain->NEXT) {
        ptchain = (chain_list *)ptmutexchain->DATA;
        if (yagGetChain(ptchain, name1) != NULL) list1 = ptmutexchain;
        if (yagGetChain(ptchain, name2) != NULL) list2 = ptmutexchain;
        if (list1 && list2) break;
    }
    if (!list1 && !list2) {
        ptchain = addchain(NULL, name1);
        ptchain = addchain(ptchain, name2);
        *ptmutex = addchain(*ptmutex, ptchain);
    }
    else if (!list1 && list2) {
        list2->DATA = addchain(list2->DATA, name1);
    }
    else if (list1 && !list2) {
        list1->DATA = addchain(list1->DATA, name2);
    }
    else if (list1 != list2) {
        list1->DATA = append(list1->DATA, list2->DATA);
        list2->DATA = NULL;
        *ptmutex = delchain(*ptmutex, list2);
    }
}

void
yagAnalyseCone(cone_list *ptcone)
{
    ptype_list      *ptuser;
    int              isLocalDual;

    avt_log(LOGYAG, 1,  "\tAnalysing cone '%s'\n", ptcone->NAME);

    if (yagDetectFalseConf(ptcone)) yagChainCone(ptcone);
    yagDetectLoopConf(ptcone);

    if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) {
        ptcone->TYPE &= ~(YAG_FALSECONF|YAG_LOOPCONF);
        return;
    }

    yagDetectBadBranches(ptcone);
    yagDetectParallelBranches(ptcone);
    yagDetectParaTrans(ptcone);
    yagDetectSwitch(ptcone);
    yagDetectPullup(ptcone);
    yagDetectPulldown(ptcone);
    if (yagDetectTransfer(ptcone)) yagChainCone(ptcone);
    yagDetectDegraded(ptcone);

    if ((ptuser = getptype(ptcone->USER, YAG_BUSBEFIG_PTYPE)) != NULL) {
        beh_delbefig((befig_list *)ptuser->DATA, (befig_list *)ptuser->DATA, 'Y');
        ptcone->USER = delptype(ptcone->USER, YAG_BUSBEFIG_PTYPE);
    }
    if ((ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE)) != NULL) {
        beh_delbefig((befig_list *)ptuser->DATA, (befig_list *)ptuser->DATA, 'Y');
        ptcone->USER = delptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
    }

    if ((ptcone->TYPE & YAG_AUTOLATCH) != 0) {
        yagUnmarkLatch(ptcone, FALSE, FALSE);
    }

    if (yagCheckExtOut(ptcone) == FALSE) return;
    if (yagCheckBranches(ptcone) == FALSE) return;
    
    if ((ptcone->TYPE & (YAG_GLUECONE|YAG_TEMPCONE)) == 0) yagBuildOutputs(ptcone);

    cnsConeFunction(ptcone, FALSE);
#ifndef WITHOUT_TAS
    if (YAG_CONTEXT->YAG_USESTMSOLVER) {
        yagPrepStmSolver(ptcone);
    }
#endif
    
    if ((ptcone->TYPE & YAG_FALSECONF) != 0) {
        ptcone->TYPE |= CNS_CONFLICT;
    }
    else {
        isLocalDual = FALSE;
        if ((ptcone->TECTYPE & (CNS_VSS_DEGRADED|CNS_VDD_DEGRADED)) == 0
        && getptype(ptcone->USER, CNS_SWITCH) == NULL) {
            isLocalDual = yagCheckLocalDuality(ptcone);
            if (isLocalDual) {
                ptcone->TECTYPE |= CNS_DUAL_CMOS;
                if (YAG_CONTEXT->YAG_LOOP_ANALYSIS) {
                    yagAnalyseLoop(ptcone, FALSE);
                }
            }
        }
        if (!isLocalDual) {
            if (YAG_CONTEXT->YAG_USE_FCF) check_cone(ptcone);
            else ptcone->TYPE |= CNS_CONFLICT;
        }
    }


    if (!YAG_CONTEXT->YAG_KEEP_REDUNDANT) {
        yagRemoveRedundantBranches(ptcone);
    }

    avt_log(LOGYAG, 1,  "\tBDD system used %d/%d reduced nodes\n", numberNodeAllBdd() - YAG_CONTEXT->YAG_LASTNODECOUNT, numberNodeAllBdd());
    if (numberNodeAllBdd() - YAG_CONTEXT->YAG_LASTNODECOUNT > YAG_CONTEXT->YAG_MAXNODES) {
        YAG_CONTEXT->YAG_MAXNODES = numberNodeAllBdd() - YAG_CONTEXT->YAG_LASTNODECOUNT;
    }
    yagControlBdd(0);

    if (avt_islog(1,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG, 1,  "\nDisplaying Cone Structure :-\n\n");
        displayconelog(LOGYAG, 1, ptcone);
    }
}

/****************************************************************************
 *                         function check_cone();                           *
 ****************************************************************************/
 /*----------------------------------------------------*
 | Check if completed cone is conflictual or HZ        |
 *----------------------------------------------------*/

static void
check_cone(cone_list *ptcone)
{
    ptype_list  *ptuser;
    chain_list  *sup_abl, *sdn_abl;
    gnode_list  *rootnode;
    branch_list *ptbranch;
    int         resist_up, resist_dn;
    int         destroy;
    int         isdlatch, forcedlatch;
    int         isprecharge = FALSE;
    short       save_depth;
    
    /* To handle a return from BDD explosion */
    save_depth = YAG_CONTEXT->YAG_DEPTH;
    setBddCeiling(YAG_CONTEXT->YAG_BDDCEILING);
    if (setjmp(abandon_analyse) != 0) {
        yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
        if (YAG_CONTEXT->YAG_DEPTH == 0) {
            ptcone->TYPE |= CNS_CONFLICT;
            fprintf(stderr,"\n*** YAG DANGER : unable to analyse cone '%s' ***\n", ptcone->NAME);
            fprintf(stderr,"                 increase BDD Ceiling or identify constraints\n");
            fflush(stderr);
            unsetBddCeiling();
            YAG_CONTEXT->YAG_DEPTH = save_depth;
            return;
        }
        YAG_CONTEXT->YAG_DEPTH--;
        cnsConeFunction(ptcone, FALSE);
    }
    
    /* build the clean global graph */
    if (YAG_CONTEXT->YAG_CONE_GRAPH == NULL) {
        YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
        destroy = TRUE;
    }
    else destroy = FALSE;

    rootnode = yagRootGraph(YAG_CONTEXT->YAG_CONE_GRAPH, ptcone);

    if (avt_islog(1,LOGYAG) && YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG, 1, "\nAfter Construction :-\n");
        yagDisplayUsedGraphLog(LOGYAG, 1, YAG_CONTEXT->YAG_CONE_GRAPH, 1);
    }

    resist_up = FALSE;
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_RESBRANCH) != 0) resist_up = TRUE;
    }
    resist_dn = FALSE;
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_RESBRANCH) != 0) resist_dn = TRUE;
    }

    if (yagCountBranches(ptcone) >= A_LOT_OF_BRANCHES) {
        check_big_cone(ptcone,rootnode);
    }
    else {
        yagCheckGlobalDuality(ptcone);
    }
    
    if ((ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) == 0) {
    
        /* use YAG_DNEXPR_PTYPE to express functionality for pseudo CMOS or cones with no UP */
        ptuser = getptype(ptcone->USER, CNS_UPEXPR);
        sup_abl = (chain_list *)ptuser->DATA;
        sdn_abl = (chain_list *)getptype(ptcone->USER, CNS_DNEXPR)->DATA;
        if (sdn_abl != NULL) {
            if ((sup_abl == NULL) || (resist_up && !resist_dn)) {
                ptuser->DATA = notExpr(copyExpr(sdn_abl));
                if (sup_abl != NULL) freeExpr(sup_abl);
            }
            freeExpr(sdn_abl);
        }
        ptcone->USER = delptype(ptcone->USER, CNS_DNEXPR);
    }
    else {
        if (avt_islog(1,LOGYAG)) {
            avt_log(LOGYAG, 1,  "\t\t");
            if ((ptcone->TYPE & CNS_CONFLICT) != 0) avt_log(LOGYAG, 1,  "CONFLICT ");
            if ((ptcone->TYPE & CNS_TRI) != 0) avt_log(LOGYAG, 1,  "HZ ");
            avt_log(LOGYAG, 1,  "\n");
        }
    }
    unsetBddCeiling();

    /* Precharge detection */
    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & YAG_INFOPRECHARGE) != 0) isprecharge = TRUE;;
    }
    if (isprecharge || (YAG_CONTEXT->YAG_DETECT_PRECHARGE && (ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) != 0)) {
        yagDetectPrecharge(ptcone, TRUE);
    }
    /* Simple latch detection */
    if (YAG_CONTEXT->YAG_SIMPLE_LATCH && (ptcone->TYPE & CNS_PRECHARGE) == 0) {
        yagMatchSimpleLatch(ptcone);
    }
    /* Automatic latch detection */
    if (YAG_CONTEXT->YAG_LOOP_ANALYSIS && (ptcone->TYPE & CNS_PRECHARGE) == 0 && (ptcone->TYPE & YAG_FALSECONF) == 0) {
        yagAnalyseLoop(ptcone, FALSE);
    }
    /* Automatic dynamic latch and bus handling detection */
    if (((ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) != 0 || (ptcone->TECTYPE & YAG_LEVELHOLD) != 0) && (ptcone->TYPE & (CNS_PRECHARGE|CNS_LATCH)) == 0) {
        isdlatch = FALSE;
        forcedlatch = FALSE;
        if ((ptcone->TECTYPE & YAG_LEVELHOLD) == 0) {
            if (YAG_CONTEXT->YAG_MARK_TRISTATE_MEMORY) {
                isdlatch = TRUE;
                if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & YAG_INFONOTDLATCH) != 0) isdlatch = FALSE;
                }
                if ((ptcone->TECTYPE & YAG_NOTLATCH) == YAG_NOTLATCH) isdlatch = FALSE;
            }
            if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & YAG_INFODLATCH) != 0) forcedlatch = TRUE;
            }
        }
        if ((YAG_CONTEXT->YAG_BUS_ANALYSIS || isdlatch || forcedlatch || ((ptcone->TYPE & CNS_EXT) == CNS_EXT && !strcmp(YAG_CONTEXT->YAGLE_TOOLNAME,"hitas"))) 
        && (ptcone->TYPE & CNS_TRI) != 0 && (ptcone->TYPE & CNS_PRECHARGE) == 0) {
            if ((isdlatch && ((ptcone->TYPE & CNS_EXT) != CNS_EXT)) || forcedlatch) {
                ptcone->TYPE |= CNS_LATCH;
                ptcone->TYPE &= ~CNS_TRI;
            }
            yagAnalyseBus(ptcone, TRUE);
        }
        else if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0 && V_BOOL_TAB[__YAGLE_LEVELHOLD_ANALYSIS].VALUE) {
            yagAnalyseBus(ptcone, TRUE);
        }
    }

    /* Mark non-conflictual feedback branches in latch cones */
    if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_MASTER|CNS_SLAVE|CNS_RS)) != 0) {
        yagDetectNonConflictualFeedback(ptcone);
    }

    /* delete graph */
    if (destroy) {
        if (YAG_CONTEXT->YAG_CONE_GRAPH) yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
        YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
    }

    YAG_CONTEXT->YAG_DEPTH = save_depth;
    
    if ((ptcone->TYPE & CNS_LATCH) != 0) {
        if ((ptcone->TYPE & YAG_AUTOLATCH) == 0) {
            cnsConeFunction(ptcone, FALSE);
            yagDetectStuck(ptcone);
        }
        if ((ptcone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0) yagPropagateStuck(ptcone);
    }
}

static void
check_big_cone(cone_list *ptcone, gnode_list *rootnode)
{
    branch_list *ptbranch0, *ptbranch1;
    pNode        bdd0, bdd1;
    pNode        sup, sdn;
    pNode        sum;
    chain_list  *sup_abl, *sdn_abl;
    chain_list  *name_expr;

    ptcone->TYPE &= ~(CNS_CONFLICT|CNS_TRI);

    for (ptbranch0 = ptcone->BRVDD; ptbranch0; ptbranch0 = ptbranch0->NEXT) {
        if ((ptbranch0->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
        for (ptbranch1 = ptcone->BRVSS; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
            if ((ptbranch1->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            bdd0 = yagCalcBranchConductance(ptbranch0);
            bdd1 = yagCalcBranchConductance(ptbranch1);
            if (!zeroBdd(yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse))) break;
        }
        if (ptbranch1 != NULL) break;
        for (ptbranch1 = ptcone->BREXT; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
            if ((ptbranch1->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if (ptbranch1->LINK->NEXT == NULL) continue;
            bdd0 = yagCalcBranchConductance(ptbranch0);
            bdd1 = yagCalcBranchConductance(ptbranch1);
            if (!zeroBdd(yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse))) break;
        }
        if (ptbranch1 != NULL) break;
    }
    if (ptbranch0 != NULL) {
        ptcone->TYPE |= (CNS_CONFLICT|CNS_TRI);
        return;
    }
    else {
        for (ptbranch0 = ptcone->BRVSS; ptbranch0; ptbranch0 = ptbranch0->NEXT) {
            if ((ptbranch0->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            for (ptbranch1 = ptcone->BREXT; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
                if ((ptbranch1->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                if (ptbranch1->LINK->NEXT == NULL) continue;
                bdd0 = yagCalcBranchConductance(ptbranch0);
                bdd1 = yagCalcBranchConductance(ptbranch1);
                if (!zeroBdd(yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse))) break;
            }
            if (ptbranch1 != NULL) break;
        }
        if (ptbranch0 != NULL) {
            ptcone->TYPE |= (CNS_CONFLICT|CNS_TRI);
            return;
        }
    }

    /* Set to HZ to generate down function */
    ptcone->TYPE |= CNS_TRI;
    
    name_expr = createAtom_no_NA(ptcone->NAME);
    
    sup_abl = yagExpandExpr(rootnode, name_expr, FALSE);
    sdn_abl = yagExpandExpr(rootnode, name_expr, TRUE);

    freeExpr(name_expr);

    sup = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sup_abl, abandon_analyse);
    sdn = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sdn_abl, abandon_analyse);
    freeExpr(sup_abl);
    freeExpr(sdn_abl);

    /* apply the contraints */
    sup = yagApplyConstraints(sup, yagAbandonAnalyse());
    sdn = yagApplyConstraints(sdn, yagAbandonAnalyse());
    
    /* test for high impedance */

    sum = yagApplyBinBdd(OR, sup, sdn, abandon_analyse);
    if (oneBdd(sum) || (ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_MASTER|CNS_SLAVE)) != 0) {
        ptcone->TYPE ^= CNS_TRI;
    }
}

int
yagCheckLocalDuality(cone_list *ptcone)
{
    ht          *ptorder;
    table       *ptnames;
    branch_list *ptbranch;
    link_list   *ptlink;
    pNode       sup = BDD_zero;
    pNode       sdown = BDD_zero;
    pNode       sum, product;
    pNode       term, symbol;
    long        index;

/* not dual if external branch containing more than just connector link */
    if (ptcone->BREXT != NULL) {
        if (ptcone->BREXT->LINK->NEXT != NULL) return FALSE;
    }
    
    ptnames = newtable();
    ptorder = order(ptcone->BRVSS, ptnames);
    
/* To calculate Sup */

    for (ptbranch = ptcone->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        term = BDD_one;
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((index = gethtitem(ptorder, ptlink->ULINK.LOTRS->GRID)) == EMPTYHT) {
                delht(ptorder);
                deltable(ptnames);
                return FALSE;
            }
            symbol = notBdd(createNodeTermBdd(index));
            term = applyBinBdd(AND, term, symbol);
        }
        sup = applyBinBdd(OR, sup, term);
    }

/* To calculate Sdown */

    for (ptbranch = ptcone->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        term = BDD_one;
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            index = gethtitem(ptorder, ptlink->ULINK.LOTRS->GRID);
            symbol = createNodeTermBdd(index);
            term = applyBinBdd(AND, term, symbol);
        }
        sdown = applyBinBdd(OR, sdown, term);
    }

    delht(ptorder);
    ptcone->TYPE &= ~(CNS_CONFLICT|CNS_TRI);
    product = applyBinBdd(AND, sup, sdown);
    if (!zeroBdd(product)) ptcone->TYPE |= CNS_CONFLICT;
    sum = applyBinBdd(OR, sup, sdown);
    if (!oneBdd(sum)) ptcone->TYPE |= CNS_TRI;
    deltable(ptnames);
    if ((ptcone->TYPE & (CNS_CONFLICT|CNS_TRI)) != 0) return FALSE;
    else return TRUE;
}

static void
yagCheckGlobalDuality(cone_list *ptcone)
{
    chain_list  *sup_abl, *sdn_abl;
    chain_list  *name_expr, *tmpabl;
    branch_list *ptbranch;
    pNode       sup, sdn;
    pNode       sum, product;
    gnode_list  *rootnode;
    abl_pair    conf_abl, hz_abl;
    int         resist_up, resist_dn;
    int         loglvl2=0;
    int         hasdual = FALSE;
    
    if ((ptcone->TYPE & YAG_HASDUAL) != 0) hasdual = TRUE;
    ptcone->TYPE &= ~YAG_HASDUAL;
    loglvl2 = avt_islog(2,LOGYAG);
    resist_up = FALSE;
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_RESBRANCH) != 0) resist_up = TRUE;
    }
    resist_dn = FALSE;
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_RESBRANCH) != 0) resist_dn = TRUE;
    }

    /* Set to HZ to generate down function */
    ptcone->TYPE |= CNS_TRI;
    ptcone->TYPE &= ~CNS_CONFLICT;
    
    name_expr = createAtom_no_NA(ptcone->NAME);
    rootnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptcone);

    sup_abl = yagExpandExpr(rootnode, name_expr, FALSE);
    sdn_abl = yagExpandExpr(rootnode, name_expr, TRUE);

    freeExpr(name_expr);

    if ( loglvl2 && YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG, 2, "Sup expr: ");
        displayInfExprLog(LOGYAG, 2, sup_abl);
        avt_log(LOGYAG, 2, "\n");
        avt_log(LOGYAG, 2, "Sdn expr: ");
        displayInfExprLog(LOGYAG, 2, sdn_abl);
        avt_log(LOGYAG, 2, "\n");
    }

    sup = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sup_abl, abandon_analyse);
    sdn = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sdn_abl, abandon_analyse);
    freeExpr(sup_abl);
    freeExpr(sdn_abl);

    if ((ptcone->TYPE & CNS_EXT) != 0 && ptcone->BREXT != NULL) {
        if (resist_up) {
            cnsMakeConeExpr(ptcone, &hz_abl, CNS_RESBRANCH|CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE|(CNS_VSS&~CNS_POWER), 0, FALSE);
            if (hz_abl.UP != NULL) {
                sup_abl = yagExpandTerm(rootnode, hz_abl.UP);
                sup = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sup_abl, abandon_analyse);
                freeExpr(sup_abl);
            }
            else sup = BDD_zero;
        }
        if (resist_dn) {
            cnsMakeConeExpr(ptcone, &hz_abl, CNS_RESBRANCH|CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE|(CNS_VDD&~CNS_POWER), 0, FALSE);
            if (hz_abl.DN != NULL) {
                sdn_abl = yagExpandTerm(rootnode, hz_abl.DN);
                sdn = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sdn_abl, abandon_analyse);
                freeExpr(sdn_abl);
            }
            else sdn = BDD_zero;
        }
    }

    if ( loglvl2 || YAG_CONTEXT->YAG_DEBUG_CONE == ptcone->NAME) {
        avt_log(LOGYAG,1,"Sup reduced: ");
        displayInfExprLog(LOGYAG, 1, tmpabl=bddToAblCct(YAG_CONTEXT->YAG_CURCIRCUIT, sup));
        freeExpr(tmpabl);
        avt_log(LOGYAG,1,"\n");
        avt_log(LOGYAG,1,"Sdn reduced: ");
        displayInfExprLog(LOGYAG, 1, tmpabl=bddToAblCct(YAG_CONTEXT->YAG_CURCIRCUIT, sdn));
        freeExpr(tmpabl);
        avt_log(LOGYAG,1,"\n");
    }

    /* apply the contraints */
    sup = yagApplyConstraints(sup, yagAbandonAnalyse());
    sdn = yagApplyConstraints(sdn, yagAbandonAnalyse());
    
    /* test for constants */
    if (sup == BDD_one && sdn == BDD_zero) {
        ptcone->TECTYPE |= CNS_ONE;
        ptcone->TYPE |= YAG_FORCEPRIM;
        yagPropagateStuck(ptcone);
    }
    if (sup == BDD_zero && sdn == BDD_one) {
        ptcone->TECTYPE |= CNS_ZERO;
        ptcone->TYPE |= YAG_FORCEPRIM;
        yagPropagateStuck(ptcone);
    }

    /* test for high impedance */

    sum = yagApplyBinBdd(OR, sup, sdn, abandon_analyse);
    if (oneBdd(sum) || (ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_MASTER|CNS_SLAVE)) != 0) ptcone->TYPE ^= CNS_TRI;
    
    /* test for conflicts but must ignore resistance branches */

    if (resist_up) {
        cnsMakeConeExpr(ptcone, &conf_abl, CNS_RESBRANCH|CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE|(CNS_VSS&~CNS_POWER), 0, FALSE);
        if (conf_abl.UP != NULL) {
            sup_abl = yagExpandTerm(rootnode, conf_abl.UP);
            sup = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sup_abl, abandon_analyse);
            freeExpr(sup_abl);
        }
        else sup = BDD_zero;
    }
    if (resist_dn) {
        cnsMakeConeExpr(ptcone, &conf_abl, CNS_RESBRANCH|CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE|(CNS_VDD&~CNS_POWER), 0, FALSE);
        if (conf_abl.DN != NULL) {
            sdn_abl = yagExpandTerm(rootnode, conf_abl.DN);
            sdn = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, sdn_abl, abandon_analyse);
            freeExpr(sdn_abl);
        }
        else sdn = BDD_zero;
    }

    product = yagApplyBinBdd(AND, sup, sdn, abandon_analyse);
    if (!zeroBdd(product)) ptcone->TYPE |= CNS_CONFLICT;
    if (hasdual) ptcone->TYPE |= YAG_HASDUAL;
}

static
ht *order(branch_list *ptbranchlist, table *ptnames)
{
    ht          *ptorder;
    branch_list *ptbranch;
    link_list   *ptlink;
    cone_list   *ptcone;
    long        index = 2;
    
    ptorder = addht(20);
    
    for (ptbranch = ptbranchlist; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            ptcone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            if (gethtitem(ptorder, ptcone) == EMPTYHT) {
                addhtitem(ptorder, ptcone, index++);
                addtableitem(ptnames, ptcone->NAME);
            }
        }
    }
    return ptorder;
}

void
yagDetectNonConflictualFeedback(cone_list *ptcone)
{
    branch_list *ptbranch0, *ptbranch1;
    pNode        bdd0, bdd1;
    pNode        resbdd;
    int          conflict;

    for (ptbranch0 = ptcone->BRVDD; ptbranch0; ptbranch0 = ptbranch0->NEXT) {
        if ((ptbranch0->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK) continue;
        conflict = FALSE;
        for (ptbranch1 = ptcone->BRVSS; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
            if ((ptbranch1->TYPE & (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) != 0) continue;
            bdd0 = yagCalcBranchConductance(ptbranch0);
            bdd1 = yagCalcBranchConductance(ptbranch1);
            resbdd = yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse);
            resbdd = yagApplyConstraints(resbdd, yagAbandonAnalyse());
            if (!zeroBdd(resbdd)) conflict = TRUE;
        }
        for (ptbranch1 = ptcone->BREXT; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
            if ((ptbranch1->TYPE & (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) != 0) continue;
            if (ptbranch1->LINK->NEXT == NULL) continue;
            bdd0 = yagCalcBranchConductance(ptbranch0);
            bdd1 = yagCalcBranchConductance(ptbranch1);
            resbdd = yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse);
            resbdd = yagApplyConstraints(resbdd, yagAbandonAnalyse());
            if (!zeroBdd(resbdd)) conflict = TRUE;
        }
        if (!conflict) ptbranch0->TYPE |= CNS_NOTCONFLICTUAL;
    }
    for (ptbranch0 = ptcone->BRVSS; ptbranch0; ptbranch0 = ptbranch0->NEXT) {
        if ((ptbranch0->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK) continue;
        conflict = FALSE;
        for (ptbranch1 = ptcone->BRVDD; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
            if ((ptbranch1->TYPE & (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) != 0) continue;
            bdd0 = yagCalcBranchConductance(ptbranch0);
            bdd1 = yagCalcBranchConductance(ptbranch1);
            resbdd = yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse);
            resbdd = yagApplyConstraints(resbdd, yagAbandonAnalyse());
            if (!zeroBdd(resbdd)) conflict = TRUE;
        }
        for (ptbranch1 = ptcone->BREXT; ptbranch1; ptbranch1 = ptbranch1->NEXT) {
            if ((ptbranch1->TYPE & (CNS_FEEDBACK|CNS_NOT_FUNCTIONAL)) != 0) continue;
            if (ptbranch1->LINK->NEXT == NULL) continue;
            bdd0 = yagCalcBranchConductance(ptbranch0);
            bdd1 = yagCalcBranchConductance(ptbranch1);
            resbdd = yagApplyBinBdd(AND, bdd0, bdd1, abandon_analyse);
            resbdd = yagApplyConstraints(resbdd, yagAbandonAnalyse());
            if (!zeroBdd(resbdd)) conflict = TRUE;
        }
        if (!conflict) ptbranch0->TYPE |= CNS_NOTCONFLICTUAL;
    }
}

pNode
yagApplyConstraints(pNode expr, jmp_buf *ptabandon_env)
{
    chain_list *support;
    chain_list *constraint_support;
    chain_list *ptabl;
    chain_list *constraint_abl;
    ptype_list *ptptype;
    pNode       constraint;
    ht *FASTSEARCH=NULL;

    if (YAG_CONTEXT->YAG_CONE_GRAPH->CONSTRAINTS != NULL) {
        support=supportChain_listBddExpr(YAG_CONTEXT->YAG_CURCIRCUIT, expr);
        for (ptptype = YAG_CONTEXT->YAG_CONE_GRAPH->CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
            constraint_support = (chain_list *)ptptype->TYPE;
            if ((constraint_abl = yagCheckConstraint(support, constraint_support, (chain_list *)ptptype->DATA, &FASTSEARCH)) != NULL) {
                if (ptabandon_env != NULL) {
                    constraint = yagAblToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, constraint_abl, *ptabandon_env);
                    expr = yagConstraintBdd(expr, constraint, *ptabandon_env);
                }
                else {
                    constraint = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, constraint_abl);
                    expr = constraintBdd(expr, constraint);
                }
                if (constraint_abl != (chain_list *)ptptype->DATA) freeExpr(constraint_abl);
            }
            if (zeroBdd(expr)) break;
        }
        freechain(support);
    }
    if (FASTSEARCH!=NULL) delht(FASTSEARCH);
    return expr;
}

/****************************************************************************
 *                         function yagDetectPrecharge()                    *
 ****************************************************************************/

/* detect precharge */

void
yagDetectPrecharge(cone_list *ptcone, int fIgnoreFalse)
{
    edge_list      *save_inputs, *ptedge;
    cone_list      *ptloopcone;
    branch_list    *ptbranch;
    pCircuit        directCct;
    long            branchmask;
    abl_pair        local_abl;
    pNode           local_sup, local_sdn;
    chain_list     *ptsupport, *ptloopchain;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    float           ratio;
    char           *name;
    int             invloop = FALSE;
    int             found = FALSE;
    long            savetype;

    ptcone->TYPE &= ~CNS_PRECHARGE;
    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & YAG_INFONOTPRECHARGE) != 0) return;
    }
    
    savetype = ptcone->TYPE;
    
    /* Check for correctly dimensioned 2 cone loop */
    if ((ptloopchain = yagCheckLoop(ptcone, TRUE)) != NULL) {
        for (ptchain = ptloopchain; ptchain; ptchain = ptchain->NEXT) {
            ptloopcone = (cone_list *)ptchain->DATA;
            if (yagMatchNOT(ptcone, ptloopcone) == TRUE && yagMatchNOT(ptloopcone, ptcone) == TRUE) {
                ratio = yagInverterStrength(ptcone, ptloopcone)/yagInverterStrength(ptloopcone, ptcone);
                if (ratio > 1.2) invloop = TRUE;
                break;
            }
        }
        freechain(ptloopchain);
    }
    
    /* Mark inverter loop as bleeder and re-analyse*/
    if (invloop) {
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if (ptbranch->LINK->NEXT == NULL && (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID == ptloopcone) {
                ptbranch->TYPE |= CNS_BLEEDER|CNS_NOT_FUNCTIONAL;
            }
        }
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if (ptbranch->LINK->NEXT == NULL && (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID == ptloopcone) {
                ptbranch->TYPE |= CNS_BLEEDER|CNS_NOT_FUNCTIONAL;
            }
        }
        ptedge = yagGetEdge(ptcone->INCONE, ptloopcone);
        ptedge->TYPE |= CNS_BLEEDER;
        
        cnsConeFunction(ptcone, FALSE);
        yagCheckGlobalDuality(ptcone);
    }

    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & YAG_INFOPRECHARGE) != 0) {
            ptcone->TYPE |= CNS_PRECHARGE;
            return;
        }
    }
    
    /* A precharge is globally tristate and globally non-conflictual */
    if ((ptcone->TYPE & CNS_TRI) != CNS_TRI) {
        if (invloop) clean_precharge_marks(ptcone, ptloopcone);
        ptcone->TYPE = savetype;
        return;
    }

    /* Ignore simple external connector branch and input */
    if (ptcone->BREXT != NULL) {
        ptbranch = ptcone->BREXT;
        if (ptbranch->NEXT != NULL || ptbranch->LINK->NEXT != NULL) {
            if (invloop) clean_precharge_marks(ptcone, ptloopcone);
            ptcone->TYPE = savetype;
            return;
        }
        ptbranch->TYPE |= CNS_BLEEDER|CNS_NOT_FUNCTIONAL;
        ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
        ptedge->TYPE |= CNS_BLEEDER;
    }

    avt_log(LOGYAG,1, "Analysing Precharge at '%s'\n", ptcone->NAME);

    if (fIgnoreFalse) {
        save_inputs = ptcone->INCONE;
        ptcone->INCONE = NULL;
        for (ptedge = save_inputs; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & YAG_FALSECONF) == 0) {
                addincone(ptcone, ptedge->TYPE, ptedge->UEDGE.PTR);
            }
        }
    }

    directCct = yagBuildDirectCct(ptcone);

    /* Generate expression for cone as a function of its direct inputs */

    branchmask = CNS_NOT_FUNCTIONAL|CNS_IGNORE|CNS_MASK_PARA;
    if (fIgnoreFalse) branchmask |= YAG_FALSECONF;
    cnsMakeConeExpr(ptcone, &local_abl, branchmask, 0, FALSE);
    if (local_abl.UP == NULL) local_abl.UP = createAtom("'0'");
    if (local_abl.DN == NULL) local_abl.DN = createAtom("'0'");

    /* Generate up and down expressions for cone in terms of direct inputs */

    local_sup = ablToBddCct(directCct, local_abl.UP);
    local_sdn = ablToBddCct(directCct, local_abl.DN);
    ptsupport = supportChain_listExpr(local_abl.UP);
    freeExpr(local_abl.UP);
    freeExpr(local_abl.DN);

    if (fIgnoreFalse) {
        yagFreeEdgeList(ptcone->INCONE);
        ptcone->INCONE = save_inputs;
    }

    /* A precharge is locally tristate and and possibly locally conflictual */
    /* and has a single variable in the local up expression                 */
    if (!zeroBdd(local_sup) && !zeroBdd(local_sdn) && !oneBdd(applyBinBdd(OR, local_sup, local_sdn)) 
    && yagCountChains(ptsupport) == 1) {
        ptcone->TYPE |= CNS_PRECHARGE;
        for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) name = ptedge->UEDGE.LOCON->NAME;
            else name = ptedge->UEDGE.CONE->NAME;
            if (name == (char *)ptsupport->DATA) break;
        }
        if (ptedge != NULL) ptedge->TYPE |= YAG_PRECHCOM;
        avt_log(LOGYAG,1, "\tFound\n");
        found = TRUE;
    }
    else {
        avt_log(LOGYAG,1, "\tNot Found\n");
    }
    freechain(ptsupport);
    
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) {
            ptbranch->TYPE &= ~(CNS_BLEEDER|CNS_NOT_FUNCTIONAL);
            ptedge = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON); {
                ptedge->TYPE &= ~CNS_BLEEDER;
            }
        }
    }
    
    /* Remove bleeder loop markings if not precharge */
    if (!found && invloop) {
        clean_precharge_marks(ptcone, ptloopcone);
        ptcone->TYPE = savetype;
    }

    destroyCct(directCct);
}

static void
clean_precharge_marks(cone_list *ptcone, cone_list *ptloopcone)
{
    branch_list *ptbranch;
    edge_list   *ptedge;

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL && (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID == ptloopcone) {
            ptbranch->TYPE &= ~(CNS_BLEEDER|CNS_NOT_FUNCTIONAL);
        }
    }
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL && (cone_list *)ptbranch->LINK->ULINK.LOTRS->GRID == ptloopcone) {
            ptbranch->TYPE &= ~(CNS_BLEEDER|CNS_NOT_FUNCTIONAL);
        }
    }
    ptedge = yagGetEdge(ptcone->INCONE, ptloopcone);
    ptedge->TYPE &= ~CNS_BLEEDER;
    cnsConeFunction(ptcone, FALSE);
}

/****************************************************************************
 *                         function yagDetectRedundant()                    *
 ****************************************************************************/

/* detect electrically poor branches for which a functional alternative exists */

static int
yagTestSatisfy(pNode pBddref, pNode pBddtest)
{
    if (applyBinBdd(AND, pBddref, pBddtest) == pBddref) return TRUE;
    return FALSE;
}

void
yagDetectRedundant(cone_list *ptcone)
{
    graph          *savegraph;
    pCircuit        savecircuit;
    branch_list    *ptbranch, *ptotherbranch;
    link_list      *ptlink;
    chain_list     *locon_abl;
    pNode           badbranch_bdd, otherbranch_bdd;
    short           save_prop_hz, save_depth;
    int             bad_vdd;
    int             bad_vss;
    
    avt_log(LOGYAG,1, "Analysing Redundancy at '%s'\n", ptcone->NAME);

    /* save context and build local cone graph */
    save_prop_hz = YAG_CONTEXT->YAG_PROP_HZ;
    savegraph = YAG_CONTEXT->YAG_CONE_GRAPH;
    savecircuit = YAG_CONTEXT->YAG_CURCIRCUIT;
    save_depth = YAG_CONTEXT->YAG_DEPTH;
    YAG_CONTEXT->YAG_DEPTH = 1;
    YAG_CONTEXT->YAG_PROP_HZ = TRUE;
    YAG_CONTEXT->YAG_CONE_GRAPH = yagMakeConeGraph(ptcone);
    YAG_CONTEXT->YAG_PROP_HZ = save_prop_hz;
    
    /* check for bad Vdd branches */
    if ((ptcone->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED) bad_vdd = TRUE;
    else {
        bad_vdd = FALSE;
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_VDD_DEGRADED) != 0 || yagCountLinks(ptbranch->LINK) > 3) {
                bad_vdd = TRUE;
                break;
            }
        }
    }

    /* check for bad Vss branches */
    if ((ptcone->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED) bad_vss = TRUE;
    else {
        bad_vss = FALSE;
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_VSS_DEGRADED) != 0 || yagCountLinks(ptbranch->LINK) > 3) {
                bad_vss = TRUE;
                break;
            }
        }
    }

    /* Prepare branch conductance functions */
    if (bad_vdd) {
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            ptbranch->USER = addptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE, yagCalcBranchConductance(ptbranch));
        }
    }
    if (bad_vss) {
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            ptbranch->USER = addptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE, yagCalcBranchConductance(ptbranch));
        }
    }
    if (bad_vdd || bad_vss) {
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            ptbranch->USER = addptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE, yagCalcBranchConductance(ptbranch));
        }
    }

    /* check whether replacement for bad Vdd branch exists */
    if (bad_vdd) {
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if ((ptbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED || yagCountLinks(ptbranch->LINK) > 3) {
                badbranch_bdd = (pNode)getptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                for (ptotherbranch = ptcone->BRVDD; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 3) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                }
                for (ptotherbranch = ptcone->BREXT; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 4) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    for (ptlink = ptotherbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
                    locon_abl = createAtom(ptlink->ULINK.LOCON->NAME);
                    otherbranch_bdd = applyBinBdd(AND, otherbranch_bdd, ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, locon_abl));
                    freeExpr(locon_abl);
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                }
            }
        }
    }
    
    /* check whether replacement for bad Vss branch exists */
    if (bad_vss) {
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if ((ptbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED || yagCountLinks(ptbranch->LINK) > 3) {
                badbranch_bdd = (pNode)getptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                for (ptotherbranch = ptcone->BRVSS; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 3) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                }
                for (ptotherbranch = ptcone->BREXT; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 4) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    for (ptlink = ptotherbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
                    locon_abl = createAtom(ptlink->ULINK.LOCON->NAME);
                    otherbranch_bdd = applyBinBdd(AND, otherbranch_bdd, notBdd(ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, locon_abl)));
                    freeExpr(locon_abl);
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                }
            }
        }
    }
    
    /* check whether replacement for bad Ext branch exists */
    if (bad_vdd) {
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
            if ((ptbranch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED) {
                badbranch_bdd = (pNode)getptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                for (ptlink = ptbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
                locon_abl = createAtom(ptlink->ULINK.LOCON->NAME);
                badbranch_bdd = applyBinBdd(AND, badbranch_bdd, ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, locon_abl));
                freeExpr(locon_abl);
                for (ptotherbranch = ptcone->BRVDD; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 3) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_UP;
                }
                for (ptotherbranch = ptcone->BREXT; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 4) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    for (ptlink = ptotherbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
                    locon_abl = createAtom(ptlink->ULINK.LOCON->NAME);
                    otherbranch_bdd = applyBinBdd(AND, otherbranch_bdd, ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, locon_abl));
                    freeExpr(locon_abl);
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_UP;
                }
            }
            if ((ptbranch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED) {
                badbranch_bdd = (pNode)getptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                for (ptlink = ptbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
                locon_abl = createAtom(ptlink->ULINK.LOCON->NAME);
                badbranch_bdd = applyBinBdd(AND, badbranch_bdd, notBdd(ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, locon_abl)));
                freeExpr(locon_abl);
                for (ptotherbranch = ptcone->BRVSS; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_DEGRADED) == CNS_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 3) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_DOWN;
                }
                for (ptotherbranch = ptcone->BREXT; ptotherbranch; ptotherbranch = ptotherbranch->NEXT) {
                    if ((ptotherbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) continue;
                    if ((ptotherbranch->TYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED || yagCountLinks(ptotherbranch->LINK) > 4) continue;
                    otherbranch_bdd = (pNode)getptype(ptotherbranch->USER, YAG_BRANCHBDD_PTYPE)->DATA;
                    for (ptlink = ptotherbranch->LINK; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
                    locon_abl = createAtom(ptlink->ULINK.LOCON->NAME);
                    otherbranch_bdd = applyBinBdd(AND, otherbranch_bdd, notBdd(ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, locon_abl)));
                    freeExpr(locon_abl);
                    if (yagTestSatisfy(badbranch_bdd, otherbranch_bdd)) ptbranch->TYPE |= CNS_NOT_DOWN;
                }
            }
        }
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_VDD_DEGRADED|CNS_VSS_DEGRADED|CNS_NOT_UP|CNS_NOT_DOWN)) == (CNS_VDD_DEGRADED|CNS_VSS_DEGRADED|CNS_NOT_UP|CNS_NOT_DOWN)) {
                ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                ptbranch->TYPE &= ~(CNS_NOT_UP|CNS_NOT_DOWN);
            }
        }
    }
    
    /* Clean branch conductance functions */
    if (bad_vdd) {
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            ptbranch->USER = testanddelptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE);
        }
    }
    if (bad_vss) {
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            ptbranch->USER = testanddelptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE);
        }
    }
    if (bad_vdd || bad_vss) {
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            ptbranch->USER = testanddelptype(ptbranch->USER, YAG_BRANCHBDD_PTYPE);
        }
    }

    yagDeleteConeGraph(YAG_CONTEXT->YAG_CONE_GRAPH);
    YAG_CONTEXT->YAG_CONE_GRAPH = savegraph;
    YAG_CONTEXT->YAG_CURCIRCUIT = savecircuit;
    YAG_CONTEXT->YAG_DEPTH = save_depth;
}

