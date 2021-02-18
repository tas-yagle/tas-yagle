/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE v3.50                                                 */
/*    Fichier : yag_useabl.c                                                */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 06/05/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static biabl_list *latchexpr __P((cone_list *ptcone));
static chain_list *calcdriver __P((cone_list *ptcone, chain_list *ptcomtranslist));
static biabl_list *difflatchexpr __P((cone_list *ptcone));

/****************************************************************************
*                         function yagExpandExpr();                            *
****************************************************************************/
/*--------------------------------------------+
| Calculates the global cone function up to   |
| the current set of primary variables or     |
| the output of the current circuit           |
+--------------------------------------------*/

chain_list *yagExpandExpr(ptnode, rootexpr, negate)
    gnode_list  *ptnode;
    chain_list  *rootexpr;
    int         negate;
{
    cone_list   *ptcone;
    chain_list  *headexpr;
    chain_list  *expr;
    ptype_list  *ptuser = NULL;
    int         usedown;
    char        *name;
    
    /* external connector node */
    
    if ((ptnode->TYPE & EXT) != 0) {
        if (!negate) return copyExpr(rootexpr);
        else return notExpr(copyExpr(rootexpr));
    }

    ptcone = ptnode->OBJECT.CONE;
    
    /* constant value cones */
    if ((ptcone->TYPE & CNS_POWER) != 0) {
        if ((ptcone->TYPE & CNS_VDD) == CNS_VDD) {
            expr = createAtom("'1'");
        }
        else expr = createAtom("'0'");
        if (negate) return notExpr(expr);
        else return expr;
    }
    if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
        expr = createAtom("'1'");
        if (negate) return notExpr(expr);
        else return expr;
    }
    if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
        expr = createAtom("'0'");
        if (negate) return notExpr(expr);
        else return expr;
    }
    
    /* cone is a primary variable */
    if ((ptnode->TYPE & PRIMNODE) != 0) {
        if (!negate) return copyExpr(rootexpr);
        else return notExpr(copyExpr(rootexpr));
    }
    
    if (YAG_CONTEXT->YAG_CURCIRCUIT == NULL) {
        yagBug(DBG_NULL_PTR, "yagExpandExpr", ptcone->NAME, NULL, 0);
    }
    
    usedown = (negate && (ptcone->TYPE & CNS_TRI) == CNS_TRI);
    
    /* return atomic expression if BDD already exists */
    
    if (usedown) name = yagDownName(ptcone->NAME);
    else name = ptcone->NAME;
    if (searchOutputCct_no_NA(YAG_CONTEXT->YAG_CURCIRCUIT, name) != NULL) {
        expr = createAtom(name);
        if (negate && !usedown) {
            return notExpr(expr);
        }
        else {
            return expr;
        }
    }

    if (usedown) {
        ptuser = getptype(ptcone->USER, CNS_DNEXPR);
        if (ptuser == NULL) {
            yagBug(DBG_NO_ABL, "yagExpandExpr", ptcone->NAME, NULL, 0);
        }
    }
    else {
        //if ((ptcone->TYPE & CNS_CONFLICT) != 0 && (ptcone->TYPE & YAG_HASDUAL) != 0) {
        if ((ptcone->TYPE & YAG_HASDUAL) != 0) {
           ptuser = getptype(ptcone->USER, YAG_DUALEXPR_PTYPE);
        }
        else ptuser = getptype(ptcone->USER, CNS_UPEXPR);
        if (ptuser == NULL) {
            yagBug(DBG_NO_ABL, "yagExpandExpr", ptcone->NAME, NULL, 0);
        }
    }
    if (ptuser->DATA == NULL) return(createAtom("'0'"));
    else headexpr = copyExpr((chain_list *)ptuser->DATA);
    
    headexpr = yagExpandTerm(ptnode, headexpr);

    if (negate && !usedown) {
        return (notExpr(headexpr));
    }
    else return headexpr;
}

chain_list *yagExpandTerm(ptnode, headarg)
    gnode_list  *ptnode;
    chain_list  *headarg;
{
    chain_list  *arg, *argexpr;
    chain_list  *notarg, *notargexpr;
    chain_list  *delexpr;
    gnode_list  *ptfather;
    char        *name;
    int         operator;
    
    name = ptnode->OBJECT.CONE->NAME;

    arg = headarg;
    delexpr = arg;
    if (ATOM(arg)) {
        if (!strcmp(VALUE_ATOM(arg), "'0'")
        || !strcmp(VALUE_ATOM(arg), "'1'")) 
            return arg;
        ptfather = yagGetNodeFather(ptnode, VALUE_ATOM(arg));
        if (ptfather) arg = yagExpandExpr(ptfather, arg, FALSE);
        else arg = copyExpr(arg);
        freeExpr(delexpr);
        return arg;
    }
    else if (OPER(arg) == NOT) {
        notarg = CDR(arg);
        notargexpr = CAR(notarg);
        if (ATOM(notargexpr)) {
            if (!strcmp(VALUE_ATOM(notargexpr), "'0'")
            || !strcmp(VALUE_ATOM(notargexpr), "'1'")) 
                return arg;
            ptfather = yagGetNodeFather(ptnode, VALUE_ATOM(notargexpr));
            if (ptfather) notargexpr = yagExpandExpr(ptfather, notargexpr, TRUE);
            else notargexpr = copyExpr(notargexpr);
            freeExpr(delexpr);
            return notargexpr;
        }
        else notarg->DATA = yagExpandTerm(ptnode, notargexpr);
    }
    else {
        while ((arg = CDR(arg))) {
            argexpr = CAR(arg);
            delexpr = argexpr;
            if (ATOM(argexpr)) {
                if (strcmp(VALUE_ATOM(argexpr), "'0'") != 0
                && strcmp(VALUE_ATOM(argexpr), "'1'") != 0) {
                    ptfather = yagGetNodeFather(ptnode, VALUE_ATOM(argexpr));
                    if (ptfather) argexpr = yagExpandExpr(ptfather, argexpr, FALSE);
                    else argexpr = copyExpr(argexpr);
                    freeExpr(delexpr);
                    arg->DATA = argexpr;
                }
            }
            else {
                operator = OPER(argexpr);
                if (operator == NOT) {
                    notarg = CDR(argexpr);
                    notargexpr = CAR(notarg);
                    if (ATOM(notargexpr)) {
                        if (strcmp(VALUE_ATOM(notargexpr), "'0'") != 0
                        && strcmp(VALUE_ATOM(notargexpr), "'1'") != 0) {
                            ptfather = yagGetNodeFather(ptnode, VALUE_ATOM(notargexpr));
                            if (ptfather) notargexpr = yagExpandExpr(ptfather, notargexpr, TRUE);
                            else notargexpr = copyExpr(notargexpr);
                            freeExpr(delexpr);
                            arg->DATA = notargexpr;
                        }
                    }
                    else {
                        notarg->DATA = yagExpandTerm(ptnode, notargexpr);
                    }
                }
                else if (operator == AND || operator == OR) {
                    arg->DATA = yagExpandTerm(ptnode, argexpr);
                }
                else {
                    yagBug(DBG_BAD_ABL, "yagExpandTerm", name, NULL, 0);
                }
            }
        }
    }
    return headarg; 
}

/****************************************************************************
*                         function yagMakeBiabl();                            *
****************************************************************************/
/*------------------------------------------------+
| Build a biabl_list structure from an abl_pair   |
+------------------------------------------------*/

biabl_list *yagMakeBiabl(constexpr)
    abl_pair    *constexpr;
{
    biabl_list  *ptbiabl = NULL;
    chain_list  *condition, *value;
    char        label[16];

    if (constexpr->UP != NULL) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX);
        value = createAtom("'1'");
        condition = copyExpr(constexpr->UP);
        ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
        YAG_CONTEXT->YAG_BIABL_INDEX++;
    }
    if (constexpr->DN != NULL) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX);
        value = createAtom("'0'");
        condition = copyExpr(constexpr->DN);
        ptbiabl = beh_addbiabl(ptbiabl, label, condition, value);
        YAG_CONTEXT->YAG_BIABL_INDEX++;
    }
    
    return ptbiabl;
}

/****************************************************************************
*                         function yagMakeLatchExpr();                        *
****************************************************************************/

biabl_list *yagMakeLatchExpr(ptcone)
    cone_list  *ptcone;
{
    ptype_list *ptuser;
    long        info = 0;

    ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
    if (ptuser != NULL) info = (long)ptuser->DATA;
    if ((info & DIFF) != 0) return difflatchexpr(ptcone);
    else return latchexpr(ptcone);
}
    
/****************************************************************************
*                         function latchexpr();                             *
****************************************************************************/
/*---------------------------------------------------------+
| Build the biabl expression for a normal asymmetric latch |
+---------------------------------------------------------*/

static
biabl_list *latchexpr(ptcone)
    cone_list   *ptcone;
{
    chain_list  *pttranslist;
    lotrs_list  *pttrans;
    branch_list *ptbranch;
    branch_list *brlist[3];
    link_list   *ptlink;
    cone_list   *ptcomcone, *ptloopcone;
    edge_list   *ptedge;
    chain_list  *ptdriver, *ptcondition;
    biabl_list  *ptbiabl = NULL;
    chain_list  *ptchain;
    ptype_list  *ptuser;
    char        label[16];
    int         i;
    
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & YAG_INFOMODELLOOP) != 0) {
            sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
            for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & (CNS_FEEDBACK|CNS_LOOP)) == (CNS_FEEDBACK|CNS_LOOP)) break;
            }
            if (ptedge) {
                sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
                ptloopcone = ptedge->UEDGE.CONE;
                ptbiabl = beh_addbiabl(ptbiabl, label, createAtom("'1'"), notExpr(createAtom(ptloopcone->NAME)));
                ptbiabl->FLAG |= BEH_CND_WEAK;
            }
        }
    }
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
                ptdriver = calcdriver(ptcone, pttranslist);
                if (ptdriver != NULL) {
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
                    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
                    ptbiabl = beh_addbiabl(ptbiabl, label, ptcondition, ptdriver);
                }
                freechain(pttranslist);
            }
        }
    }

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            ptbranch->TYPE &= ~YAG_TREATED;
        }
    }

    return ptbiabl;
}

static chain_list *
calcdriver(ptcone, ptcomtranslist)
    cone_list   *ptcone;
    chain_list  *ptcomtranslist;
{
    branch_list *ptbranch;
    branch_list *brlist[3];
    link_list   *ptlink;
    chain_list  *ptdriver = NULL;
    chain_list  *branch_expr, *tempexpr;
    abl_pair    ablXt;
    int         i, counter = 0;
    
    brlist[0] = ptcone->BREXT;
    brlist[1] = ptcone->BRVDD;
    brlist[2] = ptcone->BRVSS;

    for (i=0; i<3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
            if ((ptbranch->LINK->TYPE & CNS_COMMAND) == 0) continue;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & CNS_COMMAND) == 0) break;
                if (yagGetChain(ptcomtranslist, ptlink->ULINK.LOTRS) == NULL) break;
            }
            if (ptlink != NULL) {
                if ((ptlink->TYPE & CNS_COMMAND) != 0) continue;
            }
            ptbranch->TYPE |= YAG_TREATED;
            if (counter != 0 && i == 2) continue;

            if ((ptbranch->TYPE & CNS_EXT) != 0) {
                cnsMakeExtBranchExpr(ptbranch, &ablXt, CNS_COMMAND, FALSE);
                branch_expr = ablXt.UP;
                freeExpr(ablXt.DN);
            }
            else if (ptlink == NULL) {
                if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD) {
                    branch_expr = createAtom("'1'");
                }
                else branch_expr = createAtom("'0'");
            }
            else {
                if (i == 1) branch_expr = cnsMakeBranchExpr(ptbranch, CNS_COMMAND, FALSE);
                else branch_expr = notExpr(cnsMakeBranchExpr(ptbranch, CNS_COMMAND, FALSE));
            }
            if (counter == 0) {
                ptdriver = branch_expr;
            }
            else if (counter == 1) {
                tempexpr = ptdriver;
                ptdriver = createExpr(OR);
                addQExpr(ptdriver, tempexpr);
                addQExpr(ptdriver, branch_expr);
            }
            else addQExpr(ptdriver, branch_expr);
            counter++;
        }
    }
    return ptdriver;
}
                
/****************************************************************************
*                         function difflatchexpr();                         *
****************************************************************************/
/*----------------------------------------------------+
| Build the biabl expression for a differential latch |
+----------------------------------------------------*/

static
biabl_list *difflatchexpr(ptcone)
    cone_list  *ptcone;
{
    edge_list   *ptedge;
    branch_list *ptbranch;
    cone_list   *ptcomcone, *ptdrivecone;
    link_list   *ptdrivelink;
    chain_list  *ptdriver, *ptcondition;
    chain_list  *comexpr;
    biabl_list  *ptbiabl = NULL;
    long        commandtype;
    char        label[16];

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_COMMAND) == CNS_COMMAND) break;
    }
    ptcomcone = ptedge->UEDGE.CONE;
    ptdriver = NULL;
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
        if (ptbranch->LINK->NEXT == NULL) continue;
        if ((ptbranch->LINK->NEXT->TYPE & CNS_COMMAND) != 0) {
            commandtype = ptbranch->LINK->NEXT->TYPE & (CNS_TNLINK|CNS_TPLINK);
            if (ptcomcone == (cone_list *)ptbranch->LINK->NEXT->ULINK.LOTRS->GRID) {
                ptdrivelink = ptbranch->LINK;
                ptdrivecone = (cone_list *)ptdrivelink->ULINK.LOTRS->GRID;
                ptdriver = notExpr(createAtom(ptdrivecone->NAME));
            }
        }
    }
    if (ptdriver != NULL) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX);
        comexpr = createAtom(ptcomcone->NAME);
        if (commandtype == CNS_TNLINK) ptcondition = comexpr;
        else ptcondition = notExpr(comexpr);
        ptbiabl = beh_addbiabl(ptbiabl, label, ptcondition, ptdriver);
        YAG_CONTEXT->YAG_BIABL_INDEX++;
    }
    return ptbiabl;
}

biabl_list *yagMakeTristateExpr(ptcone)
    cone_list  *ptcone;
{
    abl_pair     cone_abl;
    biabl_list  *ptloopbiabl = NULL, *ptbiabl, *pttemp = NULL;
    biabl_list  *ptdriver;
    chain_list  *expr;
    branch_list *ptbranch;
    edge_list   *ptedge;
    cone_list   *ptloopcone;
    ptype_list  *ptuser;
    char         label[16];
    
    cnsMakeConeExpr(ptcone, &cone_abl, CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE, 0, FALSE);

    if ((ptcone->TECTYPE & YAG_LEVELHOLD) == YAG_LEVELHOLD) {
        if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & YAG_INFOMODELLOOP) != 0) {
                sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
                for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
                    if ((ptedge->TYPE & (CNS_BLEEDER|CNS_LOOP)) == (CNS_BLEEDER|CNS_LOOP)) break;
                }
                if (ptedge) {
                    sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
                    ptloopcone = ptedge->UEDGE.CONE;
                    ptloopbiabl = beh_addbiabl(NULL, label, createAtom("'1'"), notExpr(createAtom(ptloopcone->NAME)));
                    ptloopbiabl->FLAG |= BEH_CND_WEAK;
                }
            }
        }
    }

    ptbiabl = yagMakeBiabl(&cone_abl);
    for (pttemp = ptbiabl; pttemp && pttemp->NEXT; pttemp = pttemp->NEXT);
    if (pttemp) pttemp->NEXT = ptloopbiabl;
    
    freeExpr(cone_abl.UP);
    freeExpr(cone_abl.DN);
    if (YAG_CONTEXT->YAG_BLEEDER_PRECHARGE) {
        if ((ptuser = getptype(ptcone->USER, CNS_BLEEDER)) != NULL && (ptcone->TECTYPE & YAG_LEVELHOLD) == 0) {
            ptbranch = (branch_list *)((chain_list *)ptuser->DATA)->DATA;
            if ((ptbranch->TYPE & CNS_VDD) == CNS_VDD) expr = createAtom("'1'");
            else expr = createAtom("'0'");
            sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX);
            ptdriver = beh_addbiabl(NULL, "label",createAtom("'1'"), expr);
            ptdriver->FLAG = BEH_CND_PRECEDE;
            ptbiabl = (biabl_list *)append((chain_list *)ptbiabl, (chain_list *)ptdriver);
        }
    }
    return ptbiabl;
}

chain_list *yagMakeDualExpr(ptcone)
    cone_list   *ptcone;
{
    abl_pair     cone_abl;
    branch_list *ptbranch;
    int          resist_up, resist_dn;
    int          useVddBranches;
    
    resist_up = FALSE;
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_RESBRANCH) != 0) resist_up = TRUE;
    }
    resist_dn = FALSE;
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_RESBRANCH) != 0) resist_dn = TRUE;
    }
    useVddBranches = (!resist_up || (resist_up && resist_dn));

    cnsMakeConeExpr(ptcone, &cone_abl, CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE, 0, FALSE);

    if (cone_abl.UP != NULL && (useVddBranches || cone_abl.DN == NULL)) {
        if (cone_abl.DN != NULL) freeExpr(cone_abl.DN);
        return cone_abl.UP;
    }
    else if (cone_abl.DN != NULL) {
        return notExpr(cone_abl.DN);
    }
    else return NULL;
}

/****************************************************************************
*                         function yagOneExpr()                             *
****************************************************************************/
int
yagOneExpr(expr)
    chain_list *expr;
{
    if (ATOM(expr)) {
        if (strcmp(VALUE_ATOM(expr), "'1'") == 0) return TRUE;
        else return FALSE;
    }
    return FALSE;
}

/****************************************************************************
*                         function yagZeroExpr()                            *
****************************************************************************/
int
yagZeroExpr(expr)
    chain_list *expr;
{
    if (ATOM(expr)) {
        if (strcmp(VALUE_ATOM(expr), "'0'") == 0) return TRUE;
        else return FALSE;
    }
    return FALSE;
}
