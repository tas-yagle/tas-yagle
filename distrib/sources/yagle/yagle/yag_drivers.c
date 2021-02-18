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

static binode_list *mintermDriverBdd (pNode expr, chain_list *write_support);
static binode_list *enumDrivers (binode_list *drivers, pNode expr, chain_list *write_support);
static binode_list *addMintermDriver (binode_list *headdriver, chain_list *vars, pNode expr);

static pCircuit DRIVE_CCT;
static pNode CONFLICT, CONFLICT_ZERO, CONFLICT_ONE;
static pNode HZCOND, HZ_ZERO, HZ_ONE;
static pNode WRITE_CONFLICT;
static pNode GLOBAL_WRITE_COND;

/*************************************************************************
 *                     function yagExtractBusDrivers()                   *
 *************************************************************************/

biabl_list * 
yagExtractBusDrivers(pCircuit busCct, cone_list *buscone, chain_list *writesupport, pNode conflict, pNode write)
{
    pNode           busexpr;
    befig_list     *ptbefig;
    binode_list    *drivers;
    biabl_list     *ptbiabl = NULL;
    binode_list    *ptbinode;
    chain_list     *support;
    chain_list     *ptchain;
    chain_list     *ptcndabl, *ptvalabl, *tempabl;
    char           *name;
    char            label[16];
    int             index;

    DRIVE_CCT = busCct;
    CONFLICT = conflict;
    HZCOND = BDD_zero;
    CONFLICT_ZERO = NULL;
    CONFLICT_ONE = NULL;
    HZ_ZERO = NULL;
    HZ_ONE = NULL;
    WRITE_CONFLICT = BDD_zero;
    GLOBAL_WRITE_COND = write;

    busexpr = searchOutputCct_no_NA(busCct, buscone->NAME);
    if ( avt_islog(2,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == buscone->NAME) {
        avt_log(LOGYAG,1,"Bus Expr : ");
        tempabl = bddToAblCct(busCct, busexpr);
        displayInfExprLog(LOGYAG, 1, tempabl);
        freeExpr(tempabl);
        avt_log(LOGYAG,1,"\n");
    }
    drivers = mintermDriverBdd(busexpr, writesupport);

    ptbefig = beh_addbefig(NULL, buscone->NAME);
    support = supportChain_listBdd(busexpr);
    for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
        index = ((pNode)ptchain->DATA)->index;
        name = searchIndexCct(DRIVE_CCT, index);
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
    }

    if (WRITE_CONFLICT != BDD_zero && V_INT_TAB[__YAGLE_DRIVE_CONFLICT].VALUE) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        ptbiabl = beh_addbiabl(ptbiabl, label, bddToAblCct(DRIVE_CCT, WRITE_CONFLICT), createAtom("'u'"));
    }
    for (ptbinode = drivers; ptbinode; ptbinode = ptbinode->NEXT) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        ptcndabl = bddToAblCct(DRIVE_CCT, ptbinode->CNDNODE);
        ptvalabl = bddToAblCct(DRIVE_CCT, ptbinode->VALNODE);
        ptbiabl = beh_addbiabl(ptbiabl, label, ptcndabl, ptvalabl);
        if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
    }
    beh_frebinode(drivers);
 
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, buscone->NAME, 'O', 'B');

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    if ((buscone->TYPE & CNS_LATCH) == CNS_LATCH) {
        ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, buscone->NAME, ptbiabl, yagMakeBinode(ptbiabl, buscone),0);
        buscone->USER = addptype(buscone->USER, YAG_LATCHBEFIG_PTYPE, ptbefig);
    }
    else {
        if (YAG_CONTEXT->YAG_TRISTATE_MEMORY) {
            ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, buscone->NAME, ptbiabl, yagMakeBinode(ptbiabl, buscone),0);
        }
        else ptbefig->BEBUS = beh_addbebus(ptbefig->BEBUS, buscone->NAME, ptbiabl, yagMakeBinode(ptbiabl, buscone),0,0);
        buscone->USER = addptype(buscone->USER, YAG_BUSBEFIG_PTYPE, ptbefig);
    }
    freechain(support);

    return ptbiabl;
}

/*************************************************************************
 *                     function yagExtractLatchDrivers()                 *
 *************************************************************************/

biabl_list * 
yagExtractLatchDrivers(pCircuit latchCct, cone_list *latchcone, chain_list *writesupport, pNode conflict, pNode hz, pNode write, chain_list *looplist)
{
    pNode           latchexpr, latchvar;
    befig_list     *ptbefig;
    binode_list    *drivers;
    biabl_list     *ptbiabl = NULL;
    binode_list    *ptbinode;
    chain_list     *support;
    chain_list     *ptchain;
    chain_list     *ptcndabl, *ptvalabl, *tempabl;
    ptype_list     *ptuser;
    cone_list      *ptloopcone;
    char           *name;
    char            regout[YAGBUFSIZE];
    char            label[16];
    int             index;
    int             modelloop = FALSE;

    DRIVE_CCT = latchCct;
    CONFLICT = conflict;
    HZCOND = hz;
    WRITE_CONFLICT = BDD_zero;
    GLOBAL_WRITE_COND = write;

    index = searchInputCct_no_NA(DRIVE_CCT, latchcone->NAME);
    if (index <= 1) {
        avt_log(LOGYAG,1, "latchname: %s\n", latchcone->NAME);
        displayCctLog(LOGYAG,1,DRIVE_CCT, 0);
    }
    latchvar = createNodeTermBdd(index);
    CONFLICT_ONE = constraintBdd(CONFLICT, latchvar);
    CONFLICT_ZERO = constraintBdd(CONFLICT, notBdd(latchvar));
    if (CONFLICT_ZERO == CONFLICT && CONFLICT_ONE == CONFLICT) {
        CONFLICT_ZERO = NULL;
        CONFLICT_ONE = NULL;
    }
    HZ_ONE = constraintBdd(HZCOND, latchvar);
    HZ_ZERO = constraintBdd(HZCOND, notBdd(latchvar));
    if (HZ_ZERO == HZCOND && HZ_ONE == HZCOND) {
        HZ_ZERO = NULL;
        HZ_ONE = NULL;
    }

    latchexpr = searchOutputCct_no_NA(latchCct, latchcone->NAME);
    if ( avt_islog(2,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == latchcone->NAME) {
        avt_log(LOGYAG,1,"Latch Expr : ");
        tempabl = bddToAblCct(latchCct, latchexpr);
        displayInfExprLog(LOGYAG, 1, tempabl);
        freeExpr(tempabl);
        avt_log(LOGYAG,1,"\n");
    }
    drivers = mintermDriverBdd(latchexpr, writesupport);

    if (YAG_CONTEXT->YAG_STUCK_LATCH && drivers != NULL && drivers->NEXT == NULL) {
        if (drivers->VALNODE == BDD_zero) latchcone->TECTYPE |= CNS_ZERO;
        if (drivers->VALNODE == BDD_one) latchcone->TECTYPE |= CNS_ONE;
    }

    ptbefig = beh_addbefig(NULL, latchcone->NAME);
    support = supportChain_listBdd(latchexpr);
    for (ptchain = support; ptchain; ptchain = ptchain->NEXT) {
        index = ((pNode)ptchain->DATA)->index;
        name = searchIndexCct(DRIVE_CCT, index);
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
    }

    if ((ptuser = getptype(latchcone->USER, YAG_INFO_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & YAG_INFOMODELLOOP) != 0) modelloop = TRUE;
    }

    if (modelloop) {
        if (looplist && !looplist->NEXT) {
            sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
            ptloopcone = (cone_list *)looplist->DATA;
            if (yagIsInversion(latchCct, latchcone->NAME, ptloopcone->NAME)) {
                ptbiabl = beh_addbiabl(ptbiabl, label, createAtom("'1'"), notExpr(createAtom(ptloopcone->NAME)));
            }
            else {
                ptbiabl = beh_addbiabl(ptbiabl, label, createAtom("'1'"), createAtom(ptloopcone->NAME));
            }
            ptbiabl->FLAG |= BEH_CND_WEAK|BEH_CND_PRECEDE;
        }
    }
    if (WRITE_CONFLICT != BDD_zero && V_INT_TAB[__YAGLE_DRIVE_CONFLICT].VALUE) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        ptbiabl = beh_addbiabl(ptbiabl, label, bddToAblCct(DRIVE_CCT, WRITE_CONFLICT), createAtom("'u'"));
    }
    for (ptbinode = drivers; ptbinode; ptbinode = ptbinode->NEXT) {
        sprintf(label, "lb%d", YAG_CONTEXT->YAG_BIABL_INDEX++);
        ptcndabl = bddToAblCct(DRIVE_CCT, ptbinode->CNDNODE);
        ptvalabl = bddToAblCct(DRIVE_CCT, ptbinode->VALNODE);
        ptbiabl = beh_addbiabl(ptbiabl, label, ptcndabl, ptvalabl);
        if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
    }
    beh_frebinode(drivers);
 
    sprintf(regout, "yagaux_%s", latchcone->NAME);
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, regout, 'O', 'B');
    ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, regout, createAtom(latchcone->NAME), NULL, 0);
    ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, latchcone->NAME, ptbiabl, yagMakeBinode(ptbiabl, latchcone),0);
    ptbefig->BERIN = beh_addberin(ptbefig->BERIN, latchcone->NAME);

    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vdd", 'I', 'B');
    ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "vss", 'I', 'B');

    latchcone->USER = addptype(latchcone->USER, YAG_LATCHBEFIG_PTYPE, ptbefig);
    freechain(support);

    return ptbiabl;
}

static chain_list *VARLIST;

/*************************************************************************
 *                    function mintermDriverBdd()                        *
 *************************************************************************/

/* enumeration of all possible write conditions to obtain the drivers */

static binode_list *
mintermDriverBdd(expr, write_support)
    pNode           expr;
    chain_list     *write_support;
{
    binode_list    *drivers, *ptdriver;
    chain_list     *cond_abl, *val_abl;

    drivers = NULL;
    VARLIST = NULL;
    drivers = enumDrivers(drivers, expr, write_support);
    if ( avt_islog(3,LOGYAG) || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        for (ptdriver = drivers; ptdriver; ptdriver = ptdriver->NEXT) {    
            cond_abl = bddToAblCct(DRIVE_CCT, ptdriver->CNDNODE);
            val_abl = bddToAblCct(DRIVE_CCT, ptdriver->VALNODE);
            avt_log(LOGYAG,1,"DRIVER: Condition '");
            displayInfExprLog(LOGYAG, 1, cond_abl);
            avt_log(LOGYAG,1,"', Value '");
            displayInfExprLog(LOGYAG, 1, val_abl);
            avt_log(LOGYAG,1,"'\n");
            freeExpr(val_abl);
            freeExpr(cond_abl);
        }
    }
    return drivers;
}

static binode_list *
enumDrivers(drivers, expr, write_support)
    binode_list    *drivers;
    pNode           expr;
    chain_list     *write_support;
{
    VARLIST = addchain(VARLIST, notBdd((pNode)write_support->DATA));

    if (write_support->NEXT != NULL) {
        drivers = enumDrivers(drivers, expr, write_support->NEXT);
    }
    else {
        drivers = addMintermDriver(drivers, VARLIST, expr);
    }
    VARLIST = delchain(VARLIST, VARLIST);

    VARLIST = addchain(VARLIST, (pNode)write_support->DATA);

    if (write_support->NEXT != NULL) {
        drivers = enumDrivers(drivers, expr, write_support->NEXT);
    }
    else {
        drivers = addMintermDriver(drivers, VARLIST, expr);
    }
    VARLIST = delchain(VARLIST, VARLIST);

    return drivers;
}

static binode_list *
addMintermDriver(headdriver, vars, expr)
    binode_list    *headdriver;
    chain_list     *vars;
    pNode           expr;
{
    chain_list     *ptchain;
    binode_list    *driver;
    chain_list     *cond_abl;
    chain_list     *val_abl;
    pNode           cond, global_cond, conflict_cond;
    pNode           val;
    int             is_conflict, is_hz;
    int             loglvl3=0;

    loglvl3 = avt_islog(3,LOGYAG);
    if (VARLIST->NEXT != NULL) cond = applyBdd(AND, VARLIST);
    else cond = (pNode)VARLIST->DATA;

    /* check that condition is functionally possible */
    cond_abl = bddToAblCct(DRIVE_CCT, cond);
    global_cond = ablToBddCct(YAG_CONTEXT->YAG_CURCIRCUIT, cond_abl);
    /* apply the contraints */
    global_cond = yagApplyConstraints(global_cond, NULL);

    freeExpr(cond_abl);
    if (global_cond == BDD_zero && (V_INT_TAB[__YAGLE_KEEP_GLITCHERS].VALUE == 0)) {
        if (loglvl3  || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            cond_abl = bddToAblCct(DRIVE_CCT, cond);
            avt_log(LOGYAG,1,"The Condition '");
            displayInfExprLog(LOGYAG, 1, cond_abl);
            avt_log(LOGYAG,1,"' is impossible\n");
            freeExpr(cond_abl);
        }
        return headdriver;
    }
    
    /* check that condition corresponds to a latch write */
    if (applyBinBdd(AND, global_cond, GLOBAL_WRITE_COND) == BDD_zero) {
        if (loglvl3  || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            cond_abl = bddToAblCct(DRIVE_CCT, cond);
            avt_log(LOGYAG,1,"The Condition '");
            displayInfExprLog(LOGYAG, 1, cond_abl);
            avt_log(LOGYAG,1,"' is not a write condition\n");
            freeExpr(cond_abl);
        }
        return headdriver;
    }

    /* check for write conflict */
    if (CONFLICT_ZERO && CONFLICT_ONE) {
        is_conflict = ((applyBinBdd(AND, CONFLICT_ZERO, cond) == cond) || (applyBinBdd(AND, CONFLICT_ONE, cond) == cond));
    }
    else is_conflict = (applyBinBdd(AND, CONFLICT, cond) == cond);
    if (is_conflict) {
        WRITE_CONFLICT = applyBinBdd(OR, WRITE_CONFLICT, cond);
        if (loglvl3  || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            cond_abl = bddToAblCct(DRIVE_CCT, cond);
            avt_log(LOGYAG,1,"The Condition '");
            displayInfExprLog(LOGYAG, 1, cond_abl);
            avt_log(LOGYAG,1,"' is a conflictual driver\n");
            freeExpr(cond_abl);
        }
        return headdriver;
    }
    else if (V_INT_TAB[__YAGLE_DRIVE_CONFLICT].VALUE == 2)  {
        if ((conflict_cond = applyBinBdd(AND, CONFLICT, cond)) != BDD_zero) {
            WRITE_CONFLICT = applyBinBdd(OR, WRITE_CONFLICT, conflict_cond);
            if (loglvl3  || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
                cond_abl = bddToAblCct(DRIVE_CCT, cond);
                avt_log(LOGYAG,1,"The Condition '");
                displayInfExprLog(LOGYAG, 1, cond_abl);
                avt_log(LOGYAG,1,"' may lead to a conflict\n");
                freeExpr(cond_abl);
            }
            cond = applyBinBdd(AND, notBdd(CONFLICT), cond);
        }
    }

    /* check for hz */
    if (HZ_ZERO && HZ_ONE) {
        is_hz = ((applyBinBdd(AND, HZ_ZERO, cond) == cond) || (applyBinBdd(AND, HZ_ONE, cond) == cond));
    }
    else is_hz = (applyBinBdd(AND, HZCOND, cond) == cond);
    if (is_hz) {
        if (loglvl3  || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
            cond_abl = bddToAblCct(DRIVE_CCT, cond);
            avt_log(LOGYAG,1,"The Condition '");
            displayInfExprLog(LOGYAG, 1, cond_abl);
            avt_log(LOGYAG,1,"' is high impedance (probably statically impossible)\n");
            freeExpr(cond_abl);
        }
        return headdriver;
    }

    /* generate driver from write condition */
    val = expr;
    for (ptchain = VARLIST; ptchain; ptchain = ptchain->NEXT) {
        val = constraintBdd(val, (pNode)ptchain->DATA);
    }

    if (loglvl3  || YAG_CONTEXT->YAG_DEBUG_CONE == YAG_CONTEXT->YAG_CURCIRCUIT->name) {
        cond_abl = bddToAblCct(DRIVE_CCT, cond);
        val_abl = bddToAblCct(DRIVE_CCT, val);
        avt_log(LOGYAG,1,"For Condition '");
        displayInfExprLog(LOGYAG, 1, cond_abl);
        avt_log(LOGYAG,1,"' the driver is '");
        displayInfExprLog(LOGYAG, 1, val_abl);
        avt_log(LOGYAG,1,"'\n");
        freeExpr(val_abl);
        freeExpr(cond_abl);
    }

    /* check if another condition has same driver */
    for (driver = headdriver; driver; driver = driver->NEXT) {
        if (driver->VALNODE == val) break;
    }
    if (driver != NULL) {
        driver->CNDNODE = applyBinBdd(OR, driver->CNDNODE, cond);
    }
    else {
        headdriver = beh_addbinode(headdriver, cond, val);
    }

    return headdriver;
}

