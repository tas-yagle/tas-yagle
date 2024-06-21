/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_duals.c                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 18/03/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static chain_list *VISIT_LIST;

static int  UNRAVEL;

static branch_list *dual_branches __P((losig_list *ptsig, losig_list *ptlastsig, long *ptbranchtype));
static chain_list *verif_dual __P((branch_group *ptbranches));
static ht *order __P((branch_list *ptbranchlist, table *ptnames));

cone_list *yagMakeConeList(ifl, pt_mbkfig, pt_cnsfig)
    inffig_list *ifl;
    lofig_list  *pt_mbkfig;
    cnsfig_list *pt_cnsfig;
{
    cone_list       *ptcone = NULL;
    cone_list       *tempcone = NULL;
    cone_list       *gluecone = NULL;
    losig_list      *ptsig = NULL;
    locon_list      *ptcon = NULL;
    chain_list      *ptchain = NULL;
    chain_list      *expr = NULL;
    chain_list      *ptchain1, *ptconlist;
    lotrs_list      *pttrans;
    ptype_list      *ptuser;
    branch_group    theBranches;
    int             numCon, numGates;
    int             isPartial;
    int             isTemp, isGlue;
    int             isDual;
    long            coneType;
    char            nodeType;

/* signal list traversal */
    for (ptsig = pt_mbkfig->LOSIG; ptsig != NULL; ptsig = ptsig->NEXT) {
        nodeType = 0;
        coneType = 0;
        theBranches.BRVDD = NULL;
        theBranches.BRVSS = NULL;
        theBranches.BRGND = NULL;
        theBranches.BREXT = NULL;

        /* transparences */
        if (ptsig->TYPE == 'T') continue;

        /* FCL_NEVER directive */
        if ((ptuser = getptype(ptsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
        }

        /* check for gates and N-P intersection */
        
        numCon = 0;
        numGates = 0;
        for (ptchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
                ptchain != NULL;
                ptchain = ptchain->NEXT) {

            ptcon = ptchain->DATA;
            numCon++;
            if (ptcon->TYPE != 'T') continue;
            pttrans = (lotrs_list *)ptcon->ROOT;

            /* check for NEVER directive on transistor */
            if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                if (((long)ptuser->DATA & FCL_NEVER) != 0) {
                    numCon--;
                    continue;
                }
            }
            
            /* Don't count if transistor is a capacitance */
            if (!(V_BOOL_TAB[__YAGLE_CAPACITANCE_CONES].VALUE)) {
                if ((pttrans->TYPE & CAPACITE) == CAPACITE) {
                    numCon--;
                    continue;
                }
            }

            if (ptcon->NAME == CNS_GRIDNAME) numGates++;

            if (ptcon->NAME != CNS_GRIDNAME && ptcon->NAME != CNS_BULKNAME) nodeType |= pttrans->TYPE;
        }
    
        /* Handle Inputs from INF */
        if ((YAG_CONTEXT->YAG_FLAGS & YAG_HAS_INF_INPUTS)!=0 && ptsig->TYPE == CNS_SIGEXT) {
            ptconlist = yagGetExtLoconList(ptsig);
            for (ptchain1 = ptconlist; ptchain1; ptchain1 = ptchain1->NEXT) {
                ptcon = (locon_list *)ptchain1->DATA;
                if (getptype(ptcon->USER, YAG_INPUT_PTYPE) != NULL) {
                    nodeType = 0; /* Force input cone if gate present */
                }
            }
            freechain(ptconlist);
        }

        if ((ptsig->TYPE != CNS_SIGVDD) && (ptsig->TYPE != CNS_SIGVSS)) {

            /* extract a CMOS DUAL cone */
            isDual = FALSE;
            isTemp = FALSE;
            isGlue = FALSE;
            if ((nodeType & (CNS_TN|CNS_TP)) == (CNS_TN|CNS_TP) && numCon > 2) {
                isPartial = FALSE;
                if ((expr = yagTryDual(ptsig, &theBranches, &isPartial)) != NULL) {
                    isDual = TRUE;
                    coneType |= YAG_HASDUAL;
                    if (isPartial) coneType |= YAG_PARTIAL;
                    if (ptsig->TYPE == CNS_SIGEXT) {
                        if (numGates == 0) {
                            ptcone = yagMakeOutCone(ptcone, ptsig, &theBranches, coneType, CNS_DUAL_CMOS);
                        }
                        else {
                            ptcone = yagMakeInoutCone(ptcone, ptsig, &theBranches, coneType, CNS_DUAL_CMOS);
                        }
                        ptcone->USER = addptype(ptcone->USER, CNS_UPEXPR, expr);
                        ptcone->USER = addptype(ptcone->USER, YAG_DUALEXPR_PTYPE, copyExpr(expr));
                    }
                    else {
                        if (numGates == 0) {
                        /* not a real cone */
                            if (getptype(ptsig->USER, FCL_FORCECONE_PTYPE) != NULL) {
                                isGlue = TRUE;
                                coneType |= YAG_GLUECONE;
                                gluecone = yagMakeCone(gluecone, ptsig, NULL, coneType, 0);
                            }
                            else {
                                isTemp = TRUE;
                                coneType |= YAG_TEMPCONE;
                                tempcone = yagMakeCone(tempcone, ptsig, &theBranches, coneType, CNS_DUAL_CMOS);
                            }
                            freeExpr(expr);
                        }
                        else {
                            ptcone = yagMakeCone(ptcone, ptsig, &theBranches, coneType, CNS_DUAL_CMOS);
                            ptcone->USER = addptype(ptcone->USER, CNS_UPEXPR, expr);
                            ptcone->USER = addptype(ptcone->USER, YAG_DUALEXPR_PTYPE, copyExpr(expr));
                        }
                    }
                    if (isGlue) yagMarkUsedTrans(gluecone);
                    else if (isTemp) yagMarkUsedTrans(tempcone);
                    else yagMarkUsedTrans(ptcone);
                    if (YAG_CONTEXT->YAG_ORIENT) yagOrientTransistors(ptsig);
                }
                else {
                    yagDelBranchList(theBranches.BRVDD);
                    yagDelBranchList(theBranches.BRVSS);
                }
            }

            /* add empty cones if no DUAL but signal attacks gate or connector or marked FORCECONE */
            if (isDual == FALSE) {
                if (ptsig->TYPE == CNS_SIGEXT) {
                    /* add connector cones */
                    if (nodeType == 0 && numGates > 0) {
                        ptcone = yagMakeInCone(ptcone, ptsig, 0, 0);
                    }
                    else if (nodeType != 0 && numGates == 0) {
                        ptcone = yagMakeOutCone(ptcone, ptsig, NULL, YAG_PARTIAL, 0);
                    }
                    else if (nodeType != 0 && numGates > 0) {
                        ptcone = yagMakeInoutCone(ptcone, ptsig, NULL, YAG_PARTIAL, 0);
                    }
                }
                else if (numGates > 0) { /* normal empty cone */
                        ptcone = yagMakeCone(ptcone, ptsig, NULL, YAG_PARTIAL, 0);
                }
                else if (getptype(ptsig->USER, FCL_FORCECONE_PTYPE) != NULL) {
                    gluecone = yagMakeCone(gluecone, ptsig, NULL, YAG_PARTIAL|YAG_GLUECONE, 0);
                }
            }
        }
        else if (numGates > 0) {
            /* power supply cones */
            ptcone = yagMakeSupplyCone(ptcone, ptsig);
        }
    }
    
    if (tempcone != NULL) {
        pt_cnsfig->USER = addptype(pt_cnsfig->USER, YAG_TEMPCONE_PTYPE, tempcone);
    }
    if (gluecone != NULL) {
        pt_cnsfig->USER = addptype(pt_cnsfig->USER, YAG_GLUECONE_PTYPE, gluecone);
    }
    return ptcone;
}

chain_list *yagTryDual(ptsig, ptbranches, ptpartial)
    losig_list      *ptsig;
    branch_group    *ptbranches;
    int             *ptpartial;
{
    branch_list *newbranches;
    branch_list *ptbranch;
    chain_list  *ptchain;
    locon_list  *ptcon;
    lotrs_list  *pttrans;
    losig_list  *ptnextsig;
    long        branch_type;
    
    VISIT_LIST = addchain(NULL, ptsig);

    for (ptchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA; ptchain != NULL; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T') {
            pttrans = (lotrs_list *)ptcon->ROOT;

            /* Skip parallel transistor instances */
            if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;

            UNRAVEL = FALSE;
            if (ptcon == pttrans->SOURCE) {
                ptnextsig = pttrans->DRAIN->SIG;
            }
            else if (ptcon == pttrans->DRAIN) {
                ptnextsig = pttrans->SOURCE->SIG;
            }
            else if (ptcon == pttrans->GRID) continue;
            else if (ptcon == pttrans->BULK) continue;

            if (ptnextsig == ptsig) continue;

            branch_type = (pttrans->TYPE & CNS_TN) == CNS_TN ? CNS_VSS : CNS_VDD;
            
            if (ptnextsig->TYPE == CNS_SIGVDD) {
                newbranches = addbranch(NULL, CNS_VDD|YAG_DUALBRANCH, NULL);
                branch_type |= CNS_VDD;
            }
            else if (ptnextsig->TYPE == CNS_SIGVSS) {
                newbranches = addbranch(NULL, CNS_VSS|YAG_DUALBRANCH, NULL);
                branch_type |= CNS_VSS;
            }
            else newbranches = dual_branches(ptnextsig, ptsig, &branch_type);

            if (UNRAVEL) {
                yagDelBranchList(newbranches);
                *ptpartial = TRUE;
            }
            else {
                if (branch_type == CNS_VDD) {
                    for (ptbranch = newbranches; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
                        ptbranch->LINK = yagAddTransLink(ptbranch->LINK, pttrans, ptsig);
                    }
                    ptbranches->BRVDD = yagAppendBranch(ptbranches->BRVDD, newbranches);
                }
                else if (branch_type == CNS_VSS) {
                    for (ptbranch = newbranches; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
                        ptbranch->LINK = yagAddTransLink(ptbranch->LINK, pttrans, ptsig);
                    }
                    ptbranches->BRVSS = yagAppendBranch(ptbranches->BRVSS, newbranches);
                }
                else {
                    yagDelBranchList(newbranches);
                    *ptpartial = TRUE;
                }
            }
        }
    }
    freechain(VISIT_LIST);
    return verif_dual(ptbranches);
}

static
branch_list *dual_branches(ptsig, ptlastsig, ptbranchtype)
    losig_list  *ptsig;
    losig_list  *ptlastsig;
    long        *ptbranchtype;
{
    branch_list *newbranches = NULL;
    branch_list *result_branch = NULL;
    branch_list *ptbranch;
    losig_list  *ptnextsig;
    chain_list  *ptchain, *headchain;
    locon_list  *ptcon;
    lotrs_list  *pttrans;

    headchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;

    for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {
        if (((locon_list *)ptchain->DATA)->NAME == CNS_GRIDNAME
        || ((locon_list *)ptchain->DATA)->NAME == CNS_BULKNAME) {
            UNRAVEL = TRUE;
            return NULL;
        }
    }
    
    VISIT_LIST = addchain(VISIT_LIST, ptsig);
    for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;

        if (ptcon->TYPE == 'T') {
            pttrans = (lotrs_list *)ptcon->ROOT;

            /* Skip parallel transistor instances */
            if (getptype(pttrans->USER, MBK_TRANS_MARK) != NULL) continue;

            if (ptcon == pttrans->SOURCE) {
                ptnextsig = pttrans->DRAIN->SIG;
            }
            else if (ptcon == pttrans->DRAIN) {
                ptnextsig = pttrans->SOURCE->SIG;
            }
            if (ptnextsig == ptlastsig) continue;

            if (yagGetChain(VISIT_LIST, ptnextsig) != NULL) continue;
            
            *ptbranchtype |= (pttrans->TYPE & CNS_TN) == CNS_TN ? CNS_VSS : CNS_VDD;
            
            if (ptnextsig->TYPE == CNS_SIGVDD) {
                newbranches = addbranch(NULL, CNS_VDD|YAG_DUALBRANCH, NULL);
                *ptbranchtype |= CNS_VDD;
            }
            else if (ptnextsig->TYPE == CNS_SIGVSS) {
                newbranches = addbranch(NULL, CNS_VSS|YAG_DUALBRANCH, NULL);
                *ptbranchtype |= CNS_VSS;
            }
            else newbranches = dual_branches(ptnextsig, ptsig, ptbranchtype);

            if (UNRAVEL) yagDelBranchList(newbranches);
            else {
                if (*ptbranchtype == CNS_VSS || *ptbranchtype == CNS_VDD) {
                    for (ptbranch = newbranches; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
                        ptbranch->LINK = yagAddTransLink(ptbranch->LINK, pttrans, ptsig);
                    }
                    result_branch = yagAppendBranch(result_branch, newbranches);
                }
                else {
                    yagDelBranchList(newbranches);
                    UNRAVEL = TRUE;
                }
            }
        }
        else if (ptcon->TYPE == 'E' || ptcon->TYPE == 'I' || ptcon->TYPE == 'C') {
            UNRAVEL = TRUE;
        }
        else yagBug(DBG_ILL_CONTYPE,"dual_branches",NULL,NULL,0);
        if (UNRAVEL) {
            VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
            return result_branch;
        }
    }
    VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
    return result_branch;
}

static
chain_list *verif_dual(ptbranches)
    branch_group *ptbranches;
{
    ht          *ptorder;
    table       *ptnames;
    branch_list *ptbranch;
    losig_list  *ptsig;
    link_list   *ptlink;
    pNode       sup = BDD_zero;
    pNode       sdown = BDD_zero;
    pNode       stot = BDD_zero;
    pNode       term, symbol;
    long        index;
    chain_list  *expr;
    
    ptnames = newtable();
    ptorder = order(ptbranches->BRVSS, ptnames);
    
/* To calculate Sup */

    for (ptbranch = ptbranches->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        term = BDD_one;
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            ptsig = ((ptlink->ULINK).LOTRS)->GRID->SIG;
            if (ptsig->TYPE == CNS_SIGVDD) symbol = BDD_zero;
            else if (ptsig->TYPE == CNS_SIGVSS) symbol = BDD_one;
            else {
                if ((index = gethtitem(ptorder, ptsig)) == EMPTYHT) {
                    delht(ptorder);
                    deltable(ptnames);
                    return FALSE;
                }
                symbol = notBdd(createNodeTermBdd(index));
            }
            term = applyBinBdd(AND, term, symbol);
        }
        if (term == BDD_zero) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
        sup = applyBinBdd(OR, sup, term);
    }

/* To calculate Sdown */

    for (ptbranch = ptbranches->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        term = BDD_one;
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            ptsig = ((ptlink->ULINK).LOTRS)->GRID->SIG;
            if (ptsig->TYPE == CNS_SIGVDD) symbol = BDD_one;
            else if (ptsig->TYPE == CNS_SIGVSS) symbol = BDD_zero;
            else {
                index = gethtitem(ptorder, ptsig);
                symbol = createNodeTermBdd(index);
            }
            term = applyBinBdd(AND, term, symbol);
        }
        if (term == BDD_zero) ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
        sdown = applyBinBdd(OR, sdown, term);
    }

    delht(ptorder);
    stot = applyBinBdd(AND, sup, sdown);
    if (stot == BDD_zero) {
        stot = applyBinBdd(OR, sup, sdown);
        if (stot == BDD_one) {
            expr = bddToAbl(sup, (char **)ptnames->DATA);
            deltable(ptnames);
            yagControlBdd(0);
            return expr;
        }
    }
    deltable(ptnames);
    yagControlBdd(0);
    return NULL;
}

static
ht *order(ptbranchlist, ptnames)
    branch_list *ptbranchlist;
    table       *ptnames;
{
    ht          *ptorder;
    branch_list *ptbranch;
    link_list   *ptlink;
    losig_list  *ptsig;
    char        *name;
    long        index = 2;
    
    ptorder = addht(20);
    
    for (ptbranch = ptbranchlist; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            ptsig = ((ptlink->ULINK).LOTRS)->GRID->SIG;
            if (ptsig->TYPE == CNS_SIGVDD) continue;
            if (ptsig->TYPE == CNS_SIGVSS) continue;
            if (gethtitem(ptorder, ptsig) == EMPTYHT) {
                addhtitem(ptorder, ptsig, index++);
                switch(ptsig->TYPE) {
                    case CNS_SIGEXT: name = (yagGetExtLocon(ptsig))->NAME; break;
                    default: name = (char *)(ptsig->NAMECHAIN->DATA);
                }
                addtableitem(ptnames, name);
            }
        }
    }
    return ptorder;
}
