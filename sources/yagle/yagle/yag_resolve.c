/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.0                                                 */
/*    Fichier : yag_resolve.c                                               */
/*                                                                          */
/*    (c) copyright 1997 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 04/09/1997     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static pNode mintermResolveBdd __P((pNode bdd));
static pNode mintermTraverse __P((pNode bdd, pNode resbdd));
static pNode enumResolveBdd __P((pNode bdd, pNode resbdd));
static pNode enumerateTraverse __P((chain_list *varlist, pNode enumbdd, pNode resbdd));
static pNode electricResolve __P((pNode inbdd, pNode sumbdd));
static int calcLevel __P((pNode inbdd));

//static pNode unexpand __P((pNode ptbdd, int curindex));

static pCircuit PRIMARY_CCT;
static pCircuit DIRECT_CCT;

static cone_list *RESCONE;

static pNode CONFLICT;
static int TEST;

/*************************************************************************
 *                    function yagResolveConflicts()                     *
 *************************************************************************/

/* takes a bdd of the conditions for electrical conflict  */
/* for a cone and returns bdd representing the conditions */
/* for which the conflict resolves to one                 */

pNode
yagResolveConflicts(directCct, primaryCct, pConflictBdd, ptcone)
    pCircuit    directCct;
    pCircuit    primaryCct;
    pNode      *pConflictBdd;
    cone_list  *ptcone;
{
    pNode       res;

    TEST = 0;
    PRIMARY_CCT = primaryCct;
    DIRECT_CCT = directCct;

    RESCONE = ptcone;

    CONFLICT = BDD_zero;

    res = mintermResolveBdd(*pConflictBdd);

    *pConflictBdd = CONFLICT;
    return res;
}

static chain_list *ABL;

/*************************************************************************
 *                    function mintermResolveBdd()                       *
 *************************************************************************/

/* minterm traversal of a given Bdd for conflict resolution */

static pNode
mintermResolveBdd(bdd)
    pNode   bdd;
{
    pNode   resbdd;

    resbdd = BDD_zero;
    ABL = NULL;
    if (bdd == BDD_one) resbdd = enumResolveBdd(BDD_one, resbdd);
    else if (bdd != BDD_zero) {
        ABL = createExpr(AND);
        addQExpr(ABL, createAtom("'1'"));
        resbdd = mintermTraverse(bdd, resbdd);
        if (!ABL->NEXT) { freechain(ABL->DATA); freechain(ABL); }
        else { freeExpr(ABL); }
    }
    return resbdd;
}

static pNode
mintermTraverse(bdd, resbdd)
    pNode   bdd;
    pNode   resbdd;
{
    chain_list *toDel;

    if (bdd == BDD_one) resbdd = enumResolveBdd(ablToBddCct(DIRECT_CCT, ABL), resbdd);
    else if (bdd != BDD_zero) {
        addHExpr(ABL, notExpr(createAtom(searchIndexCct(DIRECT_CCT, bdd->index))));
        resbdd = mintermTraverse(bdd->low, resbdd);
        addHExpr(ABL, createAtom(searchIndexCct(DIRECT_CCT, bdd->index)));
        resbdd = mintermTraverse(bdd->high, resbdd);
    }
    toDel = ABL->NEXT;
    ABL->NEXT = ABL->NEXT->NEXT;
    toDel->NEXT = NULL;
    freeExpr(toDel->DATA);
    freechain(toDel);

    return resbdd;
}

/*************************************************************************
 *                    function enumResolveBdd()                          *
 *************************************************************************/

/* Dont Care enumeration of a given BDD for conflict resolution */

static pNode
enumResolveBdd(bdd, resbdd)
    pNode   bdd;
    pNode   resbdd;
{
    chain_list *support;
    chain_list *dontcare = NULL;
    ptype_list *ptroot;
    gnode_list *ptnode;
    short       index;
    char       *name;

    support = supportChain_listBdd(bdd);
    for (ptroot = YAG_CONTEXT->YAG_CONE_GRAPH->ROOTNODES; ptroot; ptroot = ptroot->NEXT) {
        ptnode = (gnode_list *)ptroot->DATA;
        if ((ptnode->TYPE & CONE_TYPE) != 0) {
            name = ptnode->OBJECT.CONE->NAME;
        }
        else {
            /* node is an external connector */
            name = ptnode->OBJECT.LOCON->NAME;
        }
        index = searchInputCct_no_NA(DIRECT_CCT, name);
        if (index <= 1) continue;
        if (yagSearchBddList(support, index) == NULL) {
            dontcare = addchain(dontcare, createNodeTermBdd(index));
        }
    } 

    if (dontcare != NULL) {
        resbdd = enumerateTraverse(dontcare, bdd, resbdd);
        freechain(dontcare);
    }
    else {
        resbdd = electricResolve(bdd, resbdd);
    }

    freechain(support);
    return resbdd;
}

pNode
yagSearchBddList(ptlist, index)
    chain_list *ptlist;
    short       index;
{
    chain_list *ptchain;
    pNode       ptbdd;

    for (ptchain = ptlist; ptchain; ptchain = ptchain->NEXT) {
        ptbdd = (pNode)ptchain->DATA;
        if (ptbdd->index == index) break;
    }
    if (ptchain != NULL) return ptbdd;
    else return NULL;
}

static pNode
enumerateTraverse(varlist, enumbdd, resbdd)
    chain_list *varlist;
    pNode       enumbdd;
    pNode       resbdd;
{
    pNode       nextenum;

    nextenum = applyBinBdd(AND, enumbdd, notBdd((pNode)varlist->DATA));

    if (varlist->NEXT != NULL) {
        resbdd = enumerateTraverse(varlist->NEXT, nextenum, resbdd);
    }
    else {
        resbdd = electricResolve(nextenum, resbdd);
    }

    nextenum = applyBinBdd(AND, enumbdd, (pNode)varlist->DATA);

    if (varlist->NEXT != NULL) {
        resbdd = enumerateTraverse(varlist->NEXT, nextenum, resbdd);
    }
    else {
        resbdd = electricResolve(nextenum, resbdd);
    }

    return resbdd;
}

static pNode
electricResolve(inbdd, sumbdd)
    pNode   inbdd;
    pNode   sumbdd;
{
    chain_list *inabl;
    pNode       primbdd;

    /* check that condition is functionally possible */
    inabl = bddToAblCct(DIRECT_CCT, inbdd);
    primbdd = ablToBddCct(PRIMARY_CCT, inabl);
    /* apply the contraints */
    primbdd = yagApplyConstraints(primbdd, NULL);
    freeExpr(inabl);
    if (primbdd != BDD_zero) {
        if (calcLevel(inbdd) == TRUE) {
            return applyBinBdd(OR, inbdd, sumbdd);
        }
        else return sumbdd;
    }
    else return sumbdd;
}

static int
calcLevel(inbdd)
    pNode   inbdd;
{
    branch_list    *ptbranch;
    chain_list     *abl;
    chain_list     *upList = NULL;
    chain_list     *downList = NULL;
    abl_pair        extabl;
    pNode           branchbdd;
    pNode           extupbdd, extdnbdd;
    float           upresistance;
    float           downresistance;
    int             stmresult = FALSE;

    for (ptbranch = RESCONE->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_IGNORE)) != 0) continue;
        abl = cnsMakeBranchExpr(ptbranch, 0, FALSE);
        branchbdd = ablToBddCct(DIRECT_CCT, abl);
        if (applyBinBdd(AND, branchbdd, inbdd) != BDD_zero) {
            upList = addchain(upList, ptbranch);
        }
        freeExpr(abl);
    }

    for (ptbranch = RESCONE->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_IGNORE)) != 0) continue;
        abl = cnsMakeBranchExpr(ptbranch, 0, FALSE);
        branchbdd = ablToBddCct(DIRECT_CCT, abl);
        if (applyBinBdd(AND, branchbdd, inbdd) != BDD_zero) {
            downList = addchain(downList, ptbranch);
        }
        freeExpr(abl);
    }

    for (ptbranch = RESCONE->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_IGNORE)) != 0) continue;
        cnsMakeExtBranchExpr(ptbranch, &extabl, 0, FALSE);
        extupbdd = ablToBddCct(DIRECT_CCT, extabl.UP);
        extdnbdd = ablToBddCct(DIRECT_CCT, extabl.DN);
        if (applyBinBdd(AND, extupbdd, inbdd) != BDD_zero) {
            upList = addchain(upList, ptbranch);
        }
        else if (applyBinBdd(AND, extdnbdd, inbdd) != BDD_zero) {
            downList = addchain(downList, ptbranch);
        }
        freeExpr(extabl.UP);
        freeExpr(extabl.DN);
    }

#ifndef WITHOUT_TAS
    if (YAG_CONTEXT->YAG_USESTMSOLVER) {
        stmresult = yagCalcStmResPair(upList, downList, RESCONE, &upresistance, &downresistance);
    }
#endif

    if (!stmresult) {
        upresistance = yagCalcParallelResistance(upList);
        downresistance = yagCalcParallelResistance(downList);
    }
    freechain(upList);
    freechain(downList);
    
    if ( avt_islog(3,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        avt_log(LOGYAG,1,"Input '");
        abl = bddToAblCct(DIRECT_CCT, inbdd);
        displayInfExprLog(LOGYAG, 1, abl);
        freeExpr(abl);
        avt_log(LOGYAG,1,"' resolves to %.2f", downresistance/(downresistance+upresistance));
    }
    if (downresistance >= YAG_CONTEXT->YAG_THRESHOLD*upresistance) {
        if ( avt_islog(3,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            avt_log(LOGYAG,1, " (\"1\")\n");
        }
        return TRUE;
    }
    if (upresistance >= YAG_CONTEXT->YAG_THRESHOLD*downresistance) {
        if ( avt_islog(3,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            avt_log(LOGYAG,1, " (\"0\")\n");
        }
        return FALSE;
    }
    CONFLICT = applyBinBdd(OR, CONFLICT, inbdd);
    if ( avt_islog(3,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        avt_log(LOGYAG,1, "\n");
    }
    if (downresistance > upresistance) return TRUE;
    else return FALSE;
}
