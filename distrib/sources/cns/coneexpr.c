/* 10/26/95 Cone Netlist Structure functions: coneexpr.c                          */

#include"cnsall.h"

static chain_list *getSwitchExpr(link_list *ptlink)
{
    lotrs_list *ptlotrs;
    ptype_list *ptuser;

    if ((ptlink->TYPE & CNS_TNLINK) == CNS_TNLINK) {
        ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
        if (ptuser == NULL) return NULL;
        if (ptuser->DATA == NULL) return NULL;
        return (createAtom(((cone_list *)ptuser->DATA)->NAME));
    }
    else {
        ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_SWITCH);
        if (ptuser == NULL) return NULL;
        ptlotrs = (lotrs_list *)ptuser->DATA;
        ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
        if (ptuser == NULL) return NULL;
        if (ptuser->DATA == NULL) return NULL;
        return (createAtom(((cone_list *)ptuser->DATA)->NAME));
    }
}

static int isSwitchOn(link_list *ptlink)
{
    ptype_list *ptuser;
    lotrs_list *ptlotrs;
    cone_list  *ptcone;

    ptlotrs = ptlink->ULINK.LOTRS;
    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
    if (ptuser != NULL && ptuser->DATA != NULL) {
        ptcone = (cone_list *)ptuser->DATA;
        if ((ptlotrs->TYPE & CNS_TN) == CNS_TN && (ptcone->TYPE & CNS_VDD) == CNS_VDD) {
            return 1;
        }
        if ((ptlotrs->TYPE & CNS_TP) == CNS_TP && (ptcone->TYPE & CNS_VSS) == CNS_VSS) {
            return 1;
        }
    }
    ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_SWITCH);
    if (ptuser != NULL) {
        ptlotrs = (lotrs_list *)ptuser->DATA;
        ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
        if (ptuser != NULL && ptuser->DATA != NULL) {
            ptcone = (cone_list *)ptuser->DATA;
            if ((ptlotrs->TYPE & CNS_TN) == CNS_TN && (ptcone->TYPE & CNS_VDD) == CNS_VDD) {
                return 1;
            }
            if ((ptlotrs->TYPE & CNS_TP) == CNS_TP && (ptcone->TYPE & CNS_VSS) == CNS_VSS) {
                return 1;
            }
        }
    }      
    return 0;
}

static int isTransOn(link_list *ptlink)
{
    lotrs_list *ptlotrs; 
    cone_list  *ptcone;
    ptype_list *ptuser;

    ptlotrs = ptlink->ULINK.LOTRS;
    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
    if (ptuser != NULL && ptuser->DATA != NULL) {
        ptcone = (cone_list *)ptuser->DATA;
        if ((ptlotrs->TYPE & CNS_TN) == CNS_TN && (ptcone->TYPE & CNS_VDD) == CNS_VDD) {
            return 1;
        }
        if ((ptlotrs->TYPE & CNS_TP) == CNS_TP && (ptcone->TYPE & CNS_VSS) == CNS_VSS) {
            return 1;
        }
    }
    return 0;
}

static int isTransOff(link_list *ptlink)
{
    lotrs_list *ptlotrs;
    cone_list  *ptcone;
    ptype_list *ptuser;

    ptlotrs = ptlink->ULINK.LOTRS;
    ptuser = getptype(ptlotrs->USER, CNS_DRIVINGCONE);
    if (ptuser != NULL && ptuser->DATA != NULL) {
        ptcone = (cone_list *)ptuser->DATA;
        if ((ptlotrs->TYPE & CNS_TN) == CNS_TN && (ptcone->TYPE & CNS_VSS) == CNS_VSS) {
            return 1;
        }
        if ((ptlotrs->TYPE & CNS_TP) == CNS_TP && (ptcone->TYPE & CNS_VDD) == CNS_VDD) {
            return 1;
        }
    }
    return 0;
}

/****************************************************************************
 *                         function cnsAddConeInversion();                           *
 ****************************************************************************/
 /*----------------------------------------------------*
 | Mechanism to substitute inversion function for a     |
 | cone (used for switches in YAGLE)                   |
 *----------------------------------------------------*/

void cnsAddConeInversion(cone_list *ptexprcone, cone_list *ptcone)
{
    ptype_list *ptuser;
    chain_list *ptchain;

    ptuser = getptype(ptexprcone->USER, CNS_INVCONE);
    if (ptuser == NULL || ptuser->DATA != ptcone) {
        if ((ptuser = getptype(ptcone->USER, CNS_INVCONE)) != NULL) {
            ptchain = (chain_list *)ptuser->DATA;
            if (getchain(ptchain, ptexprcone) == NULL) {
                ptuser->DATA = addchain(ptchain, ptexprcone);
            }
        }
        else ptcone->USER = addptype(ptcone->USER, CNS_INVCONE, addchain(NULL, ptexprcone));
    }
}


/****************************************************************************
 *                         function cnsMakeBranchExpr();                    *
 ****************************************************************************/
 /*----------------------------------------------------*
 | Returns the ABL of a branch                         |
 | N.B. Does not verify the functionality              |
 *----------------------------------------------------*/

chain_list *cnsMakeBranchExpr(branch_list *ptbranch, long linkmask, int simplify)
{
    chain_list  *abl;
    link_list   *ptlink; 
    link_list   *ptfirstlink; 
    chain_list  *atom;
    cone_list   *ptcone;
    short       n_link = 0;
    short       n_resist = 0;
    ptype_list *ptuser ;

    abl = NULL;
    
    ptfirstlink = ptbranch->LINK;
    while (ptfirstlink && (ptfirstlink->TYPE & linkmask) != 0)
        ptfirstlink = ptfirstlink->NEXT;
        
    if (ptfirstlink == NULL) return NULL;

    /*--------------------------------*
    | Single link case                |
    *--------------------------------*/
    if(ptfirstlink->NEXT == NULL ) {
        /*-------------------*
        | Resistance         |
        *-------------------*/

        if ((ptfirstlink->TYPE & CNS_RESIST) == CNS_RESIST) {
            ptbranch->TYPE |= CNS_RESBRANCH;
            if (simplify && isTransOn(ptfirstlink)) {
                abl = createAtom("'1'");
                return(abl);
            }
        }

        /*-------------------*
        | Switch             |
        *-------------------*/

        if (simplify && (ptfirstlink->TYPE & CNS_SWITCH) == CNS_SWITCH) {
            if (isSwitchOn(ptfirstlink)) {
                abl = createAtom("'1'");
                return(abl);
            }
            else return getSwitchExpr(ptfirstlink);
        }

        /*-------------------*
        | Diode              |
        *-------------------*/

        if ((ptfirstlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP 
        || (ptfirstlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN) {
            ptbranch->TYPE |= CNS_RESBRANCH;
            abl = createAtom("'1'");
            return(abl);
        }

        if ((ptfirstlink->TYPE & CNS_SHORT) == CNS_SHORT) {
            if (simplify) {
                abl = createAtom("'1'");
                return(abl);
            }
        }

        if (simplify) {
            if (isTransOn(ptfirstlink)) {
                abl = createAtom("'1'");
                return(abl);
            }
            if (isTransOff(ptfirstlink)) {
                abl = createAtom("'0'");
                return(abl);
            }
        }
        if ((ptfirstlink->TYPE & CNS_TNLINK) == CNS_TNLINK) {
            ptuser = getptype(ptfirstlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
            if (ptuser == NULL || ptuser->DATA == NULL) {
                freeExpr(abl);
                return NULL;
            }
            else ptcone = (cone_list*)ptuser->DATA ;
            abl = createAtom(ptcone->NAME);
            return(abl);
        }
        else if ((ptfirstlink->TYPE & CNS_TPLINK) == CNS_TPLINK) {
            ptuser = getptype(ptfirstlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
            if (ptuser == NULL || ptuser->DATA == NULL) {
                freeExpr(abl);
                return NULL;
            }
            else ptcone = (cone_list*)ptuser->DATA ;
            abl = createAtom(ptcone->NAME);
            return(notExpr(abl));
        }
    }

    /*----------------------------*
    | Multiple link branch        |
    *----------------------------*/
    else {
        abl = createExpr(AND);

        for (ptlink = ptfirstlink; ptlink; ptlink = ptlink->NEXT) {
            n_link++;
            if ((ptlink->TYPE & CNS_RESIST) == CNS_RESIST) {
                n_resist++;
                if (!simplify) {
                    atom = createAtom("'1'");
                    addQExpr(abl,atom);
                    continue;
                }
            }
            if (simplify && (ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) {
                if (isSwitchOn(ptlink)) {
                    atom = createAtom("'1'");
                    addQExpr(abl,atom);
                }
                else {
                    atom = getSwitchExpr(ptlink);
                    if (atom == NULL) {
                        freeExpr(abl);
                        return NULL;
                    }
                    addQExpr(abl, atom);
                }
                continue;
            }

            if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT) {
                atom = createAtom("'1'");
                addQExpr(abl,atom);
                continue;
            }
            
            if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP 
            || (ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN) {
                n_resist++;
                atom = createAtom("'1'");
                addQExpr(abl,atom);
                continue;
            }
            
            ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
            if (ptuser == NULL || ptuser->DATA == NULL) {
                freeExpr(abl);
                return NULL;
            }
            else ptcone = (cone_list*)ptuser->DATA ;
    
            if (simplify) {
                if (isTransOn(ptlink)) {
                    atom = createAtom("'1'");
                    addQExpr(abl,atom);
                    continue;
                }
                if (isTransOff(ptlink)) {
                    atom = createAtom("'0'");
                    addQExpr(abl,atom);
                    continue;
                }
            }
            if ((ptlink->TYPE & CNS_TNLINK) == CNS_TNLINK) {
                atom = createAtom(ptcone->NAME);
                addQExpr(abl,atom);
            }
            else if ((ptlink->TYPE & CNS_TPLINK) == CNS_TPLINK) {
                atom = createAtom(ptcone->NAME);
                addQExpr(abl,notExpr(atom));
            }
        }

        if (n_resist == n_link) ptbranch->TYPE |= CNS_RESBRANCH;

        return(abl);
    }
    return NULL;
}

/****************************************************************************
 *                         function cnsMakeExtBranchExpr();                       *
 ****************************************************************************/
 /*----------------------------------------------------*
 | Returns the ABLs of an external branch              |
 | N.B. Does not verify the functionality              |
 |      Two values given to an external connector      |
 *----------------------------------------------------*/

void cnsMakeExtBranchExpr(branch_list *ptbranch, abl_pair *branch_abl, long linkmask, int simplify)
{
    link_list   *ptfirstlink, *ptlink; 
    chain_list  *atom;
    cone_list   *ptcone;
    chain_list  *ablUp;
    chain_list  *ablDn;
    short       n_link = 0;
    short       n_resist = 0;
    ptype_list *ptuser;

    branch_abl->UP = NULL;
    
    ptlink = ptbranch->LINK;

    ptfirstlink = ptbranch->LINK;
    while ((ptfirstlink->TYPE & linkmask) != 0)
        ptfirstlink = ptfirstlink->NEXT;

    /*--------------------------------*
    | Single link case                |
    *--------------------------------*/
    if(ptfirstlink->NEXT == NULL ) {
            branch_abl->UP = createAtom(ptfirstlink->ULINK.LOCON->NAME);
            branch_abl->DN = notExpr(copyExpr(branch_abl->UP));
    }

    /*----------------------------------------*
    | EXT branch with multiple links          |
    *----------------------------------------*/
    else {

        ablUp = createExpr(AND);

        for (ptlink = ptfirstlink; ptlink->NEXT != NULL; ptlink = ptlink->NEXT) {
            n_link++;
            if ((ptlink->TYPE & CNS_RESIST) == CNS_RESIST) {
                n_resist++;
                if (!simplify) {
                    atom = createAtom("'1'");
                    addQExpr(ablUp,atom);
                    continue;
                }
            }
            if (simplify && (ptlink->TYPE & CNS_SWITCH) == CNS_SWITCH) {
                if (isSwitchOn(ptlink)) {
                    atom = createAtom("'1'");
                    addQExpr(ablUp,atom);
                }
                else {
                    atom = getSwitchExpr(ptlink);
                    if (atom == NULL) {
                        freeExpr(ablUp);
                        return;
                    }
                    addQExpr(ablUp, atom);
                }
                continue;
            }

            if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT) {
                atom = createAtom("'1'");
                addQExpr(ablUp,atom);
                continue;
            }
            
            if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP 
            || (ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN) {
                n_resist++;
                atom = createAtom("'1'");
                addQExpr(ablUp,atom);
                continue;
            }
            
            ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE);
            if (ptuser == NULL || ptuser->DATA == NULL) {
                freeExpr(ablUp);
                return;
            }
            else ptcone = (cone_list*)ptuser->DATA ;

            if (simplify) {
                if (isTransOn(ptlink)) {
                    atom = createAtom("'1'");
                    addQExpr(ablUp,atom);
                    continue;
                }
                if (isTransOff(ptlink)) {
                    atom = createAtom("'0'");
                    addQExpr(ablUp,atom);
                    continue;
                }
            }
            if ((ptlink->TYPE & CNS_TNLINK) == CNS_TNLINK) {
                atom = createAtom(ptcone->NAME);
                addQExpr(ablUp,atom);
            }
            else if ((ptlink->TYPE & CNS_TPLINK) == CNS_TPLINK) {
                atom = createAtom(ptcone->NAME);
                addQExpr(ablUp,notExpr(atom));
            }
        }

        /* Treatment of connector link */
        if (n_link == n_resist) {
            freeExpr(ablUp);
            branch_abl->UP = createAtom(ptlink->ULINK.LOCON->NAME);
            branch_abl->DN = notExpr(createAtom(ptlink->ULINK.LOCON->NAME));
        }
        else {
            ablDn = copyExpr(ablUp);
            atom = createAtom(ptlink->ULINK.LOCON->NAME);
            addQExpr(ablUp,atom);
            atom = createAtom(ptlink->ULINK.LOCON->NAME);
            addQExpr(ablDn,notExpr(atom));
            branch_abl->UP = ablUp;
            branch_abl->DN = ablDn;
        }
    }
}

/****************************************************************************
*                         function cnsMakeConeExpr();                            *
****************************************************************************/

void cnsMakeConeExpr(cone_list *ptcone, abl_pair *cone_abl, long branchmask, long linkmask, int simplify)
{
    branch_list *ptbranch;
    abl_pair    ablXt;
    int         n_branch = 0;
    int         n_dn = 0;
    int         n_up = 0;

    cone_abl->UP = NULL;
    cone_abl->DN = NULL;

    /*-------------------------*
    | Count the branches       |
    *-------------------------*/
    for (ptbranch = ptcone->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & branchmask) == 0) n_dn++;
    }
    for (ptbranch = ptcone->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & branchmask) == 0) n_up++;
    }
    for (ptbranch = ptcone->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        /* ignore external connector branch */
        if (ptbranch->LINK->NEXT == NULL) continue;
        if ((ptbranch->TYPE & branchmask) == 0) {
            if ((ptbranch->TYPE & CNS_NOT_UP) == 0) n_up++;
            if ((ptbranch->TYPE & CNS_NOT_DOWN) == 0)n_dn++;
        }
    }
    n_branch = n_up + n_dn;
    
    /* external input only cone */
    if ((ptcone->TYPE & CNS_EXT) != 0 && n_branch == 0) return;

    /*--------------------------*
    | Single branch case        |
    *--------------------------*/
    if (n_branch == 1) {
        for (ptbranch = ptcone->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & branchmask) == 0) {
                cone_abl->DN = cnsMakeBranchExpr(ptbranch, linkmask, simplify);
                return;
            }
        }
        for (ptbranch = ptcone->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & branchmask) == 0) {
                cone_abl->UP = cnsMakeBranchExpr(ptbranch, linkmask, simplify);
                return;
            }
        }
    }

    /*--------------------------*
    | Multiple branches         |
    *--------------------------*/
    else if (n_branch >=2) {
    
        if (n_up >= 2) cone_abl->UP = createExpr(OR);
        if (n_dn >= 2) cone_abl->DN = createExpr(OR);
        
        /*-------------------------------------------*
        | Create intermediary ABLs                   |
        *-------------------------------------------*/
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if((ptbranch->TYPE & branchmask) != 0) continue;
            if (n_dn >= 2) addQExpr(cone_abl->DN, cnsMakeBranchExpr(ptbranch, linkmask, simplify));
            else cone_abl->DN = cnsMakeBranchExpr(ptbranch, 0, simplify);
        }
        
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if((ptbranch->TYPE & branchmask) != 0) continue;
            if (n_up >= 2) addQExpr(cone_abl->UP, cnsMakeBranchExpr(ptbranch, linkmask, simplify));
            else cone_abl->UP = cnsMakeBranchExpr(ptbranch, linkmask, simplify);
        }

        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            /* ignore external connector branch */
            if (ptbranch->LINK->NEXT == NULL) continue;
            if((ptbranch->TYPE & branchmask) != 0) continue;
            cnsMakeExtBranchExpr(ptbranch, &ablXt, linkmask, simplify);
            if ((ptbranch->TYPE & CNS_NOT_UP) == 0) {
                if (n_up >= 2) addQExpr(cone_abl->UP, ablXt.UP);
                else cone_abl->UP = ablXt.UP;
            }
            else freeExpr(ablXt.UP);
            if ((ptbranch->TYPE & CNS_NOT_DOWN) == 0) {
                if (n_dn >= 2) addQExpr(cone_abl->DN, ablXt.DN);
                else cone_abl->DN = ablXt.DN;
            }
            else freeExpr(ablXt.DN);
        }
    }
}

/****************************************************************************
*                         function cnsConeFunction();                             *
****************************************************************************/
/*--------------------------------------------+
| Calculates the local cone function and puts |
| the required abl pointers in the USER field |
+--------------------------------------------*/

void cnsConeFunction(cone_list *ptcone, int simplify)
{
    abl_pair    cone_abl;
    ptype_list  *user;
    chain_list  *sw_chain, *pairlist;
    chain_list  *ptchain;
    lotrs_list  *ptlotrs1, *ptlotrs2;
    cone_list   *ptcone1, *ptcone2;
    edge_list   *ptinput;
    ptype_list  *ptuser;

    ptcone->USER=testanddelptype(ptcone->USER, CNS_CONEFUNCTION_SIMPLIFIED);
  
    user = getptype(ptcone->USER, CNS_UPEXPR);
    if (user != NULL) {
        freeExpr((chain_list *)user->DATA);
        ptcone->USER = delptype(ptcone->USER, CNS_UPEXPR);
    }
    user = getptype(ptcone->USER, CNS_DNEXPR);
    if (user != NULL) {
        freeExpr((chain_list *)user->DATA);
        ptcone->USER = delptype(ptcone->USER, CNS_DNEXPR);
    }
    
    cnsMakeConeExpr(ptcone, &cone_abl, CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE, 0, simplify);

    ptcone->USER = addptype(ptcone->USER, CNS_UPEXPR, (void *)cone_abl.UP);
    ptcone->USER = addptype(ptcone->USER, CNS_DNEXPR, (void *)cone_abl.DN);

    if (simplify && (ptuser = getptype(ptcone->USER, CNS_SWITCH)) != NULL) {
        sw_chain = (chain_list *)ptuser->DATA;
        for (ptchain = sw_chain; ptchain; ptchain = ptchain->NEXT) {
            pairlist = (chain_list *)ptchain->DATA;
            ptlotrs1 = (lotrs_list *)pairlist->DATA;
            ptcone1 = (cone_list *)getptype(ptlotrs1->USER, CNS_DRIVINGCONE)->DATA;
            ptlotrs2 = (lotrs_list *)pairlist->NEXT->DATA;
            ptcone2 = (cone_list *)getptype(ptlotrs2->USER, CNS_DRIVINGCONE)->DATA;
            for (ptinput = ptcone->INCONE; ptinput; ptinput = ptinput->NEXT) {
                if (ptinput->UEDGE.CONE == ptcone1 && getptype(ptinput->USER, CNS_SWITCHPAIR) == NULL) {
                    ptinput->USER = addptype(ptinput->USER, CNS_SWITCHPAIR, ptcone2);
                }
                else if (ptinput->UEDGE.CONE == ptcone2 && getptype(ptinput->USER, CNS_SWITCHPAIR) == NULL) {
                    ptinput->USER = addptype(ptinput->USER, CNS_SWITCHPAIR, ptcone1);
                }
            }
        }
    }
}

void cnsConeFunction_once(cone_list *ptcone, int simplify)
{
  ptype_list *pt;
  if ((pt=getptype(ptcone->USER, CNS_CONEFUNCTION_SIMPLIFIED))!=NULL && (int)(long)pt->DATA==simplify) return;
  cnsConeFunction(ptcone, simplify);
  ptcone->USER=addptype(ptcone->USER, CNS_CONEFUNCTION_SIMPLIFIED, (void *)(long)simplify);
}

/****************************************************************************
*                         function cnsCalcConeState();                      *
****************************************************************************/
/*----------------------------------------------+
| Calculates the local cone state as a function |
| of the input states                           |
+----------------------------------------------*/

static pCircuit cnsBuildDirectCct(cone_list *ptcone)
{
    pCircuit    circuit;
    edge_list  *ptedge;
    char       *name;
    int         numinputs;

    numinputs = countchain((chain_list *)ptcone->INCONE);
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

    return circuit;
}

int
cnsCalcConeState(cone_list *ptcone, long defaultstate)
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
    long        oldstate, newstate;
    char        freeup = FALSE, freedown = FALSE;

    /* stuck cones have state set accordingly and register a change on first visit */ 
    if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
        if ((ptcone->TECTYPE & CNS_STATE_ONE) != CNS_STATE_ONE) { 
            ptcone->TECTYPE |= CNS_STATE_ONE;
            return 1;
        }
        else return 0;
    }
    if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
        if ((ptcone->TECTYPE & CNS_STATE_ZERO) != CNS_STATE_ZERO) { 
            ptcone->TECTYPE |= CNS_STATE_ZERO;
            return 1;
        }
        else return 0;
    }

    oldstate = ptcone->TECTYPE & (CNS_STATE_ONE|CNS_STATE_ZERO);
    ptcone->TECTYPE &= ~(CNS_STATE_UNKNOWN|CNS_STATE_ONE|CNS_STATE_ZERO);
    if (defaultstate == CNS_STATE_UNKNOWN && oldstate != 0) defaultstate = oldstate;

    cnsConeFunction_once(ptcone, 1);
    if ((ptuser = getptype(ptcone->USER, CNS_UPEXPR)) != NULL) {
        ptupexpr = (chain_list *)ptuser->DATA;
        if (!ptupexpr) {
            ptupexpr = createAtom("'0'");
            freeup = TRUE;
        }
    }
    else return 0;
    if ((ptuser = getptype(ptcone->USER, CNS_DNEXPR)) != NULL) {
        ptdnexpr = (chain_list *)ptuser->DATA;
        if (!ptdnexpr) {
            ptdnexpr = createAtom("'0'");
            freedown=TRUE;
        }
    }
    else return 0;

    circuit = cnsBuildDirectCct(ptcone);
    upbdd = ablToBddCct(circuit, ptupexpr);
    dnbdd = ablToBddCct(circuit, ptdnexpr);
    
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        ptconstraint = NULL;
        if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptinlocon = ptedge->UEDGE.LOCON;
            ptuser = getptype(ptinlocon->USER, CNS_TYPELOCON);
            if (ptuser == NULL) continue;
            if (((long)ptuser->DATA & (CNS_ZERO|CNS_STATE_ZERO)) != 0) {
                ptconstraint = createAtom(ptinlocon->NAME);
                ptconstraintbdd = notBdd(ablToBddCct(circuit, ptconstraint));
            }
            else if (((long)ptuser->DATA & (CNS_ONE|CNS_STATE_ONE)) != 0) {
                ptconstraint = createAtom(ptinlocon->NAME);
                ptconstraintbdd = ablToBddCct(circuit, ptconstraint);
            }
        }
        else {
            ptincone = ptedge->UEDGE.CONE;
            if ((ptincone->TECTYPE & (CNS_ZERO|CNS_STATE_ZERO)) != 0) {
                ptconstraint = createAtom(ptincone->NAME);
                ptconstraintbdd = notBdd(ablToBddCct(circuit, ptconstraint));
            }
            else if ((ptincone->TECTYPE & (CNS_ONE|CNS_STATE_ONE)) != 0) {
                ptconstraint = createAtom(ptincone->NAME);
                ptconstraintbdd = ablToBddCct(circuit, ptconstraint);
            }
        }
        if (ptconstraint != NULL) {
            upbdd = constraintBdd(upbdd, ptconstraintbdd);
            if (ptdnexpr) dnbdd = constraintBdd(dnbdd, ptconstraintbdd);
            freeExpr(ptconstraint);
        }
    }
    
    if (upbdd == BDD_one && (dnbdd == BDD_zero || dnbdd == NULL)) {
        ptcone->TECTYPE |= CNS_STATE_ONE;
    }
    else if (upbdd == BDD_zero && (dnbdd == BDD_one || dnbdd == NULL)) {
        ptcone->TECTYPE |= CNS_STATE_ZERO;
    }
    else ptcone->TECTYPE |= defaultstate;
    
    newstate = ptcone->TECTYPE & (CNS_STATE_ONE|CNS_STATE_ZERO);

    destroyCct(circuit);
    if (freeup) freeExpr(ptupexpr);
    if (freedown) freeExpr(ptdnexpr);
    if (newstate != oldstate) return 1;
    return 0;
}

/****************************************************************************
*                         function cnsCalcFigState();                       *
****************************************************************************/
/*------------------------------------------------+
| Calculates the local cnsfig state as a function |
| of the input connector states                   |
+------------------------------------------------*/

static void
cnsFIFOPush(chain_list **ptpthead, void *ptdata)
{
    chain_list *ptchain;
    
    if (*ptpthead == NULL) {
        *ptpthead = addchain(NULL, ptdata);
        return;
    }
    for (ptchain = *ptpthead; ptchain->NEXT; ptchain = ptchain->NEXT) {
        if (ptchain->DATA == ptdata) return;
    }
    ptchain->NEXT = addchain(NULL, ptdata);
}

static void *
cnsFIFOPop(chain_list **ptpthead)
{
    void *ptdata;
    
    ptdata = (*ptpthead)->DATA;
    *ptpthead = delchain(*ptpthead, *ptpthead);
    return ptdata;
}

static long
cnsCalcLatchCbhState(cone_list *ptcone, cbhseq *ptcbhseq, long cbhseq_state)
{
    int         invert;

    if (ptcone->NAME != ptcbhseq->LATCHNAME && ptcone->NAME != ptcbhseq->SLAVENAME) {
        return CNS_STATE_UNKNOWN;
    }
    if (ptcbhseq->POLARITY != CBH_INVERT && ptcbhseq->POLARITY != CBH_NONINVERT) {
        return CNS_STATE_UNKNOWN;
    }
    if (ptcbhseq->POLARITY != CBH_INVERT && ptcbhseq->POLARITY != CBH_NONINVERT) {
        return CNS_STATE_UNKNOWN;
    }
    
    if (ptcbhseq->STATEPIN == ptcbhseq->NEGPIN || ptcbhseq->STATEPIN == ptcbhseq->HZNEGPIN) {
        invert = TRUE;
    }
    else invert = FALSE;
    if (invert && ptcbhseq->POLARITY == CBH_INVERT) invert = FALSE;
    else if (!invert && ptcbhseq->POLARITY == CBH_INVERT) invert = TRUE;
    
    if (ptcone->NAME == ptcbhseq->SLAVENAME) {
        if (invert && ptcbhseq->MSPOLARITY == CBH_INVERT) invert = FALSE;
        else if (!invert && ptcbhseq->MSPOLARITY == CBH_INVERT) invert = TRUE;
    }
    
    if ((cbhseq_state & (CNS_ZERO|CNS_STATE_ZERO)) != 0) {
        if (invert) return CNS_STATE_ONE;
        else return CNS_STATE_ZERO;
    }
    if ((cbhseq_state & (CNS_ONE|CNS_STATE_ONE)) != 0) {
        if (invert) return CNS_STATE_ZERO;
        else return CNS_STATE_ONE;
    }
    return CNS_STATE_UNKNOWN;
}

void
cnsCalcFigState(cnsfig_list *ptcnsfig, cbhseq *ptcbhseq)
{
    locon_list *ptlocon;
    cone_list  *ptloconcone, *ptcone, *ptoutcone;
    edge_list  *ptedge;
    ptype_list *ptuser;
    chain_list *ptFIFO = NULL;
    chain_list *ptchain;
    long        state_type;
    long        cbhseq_state = 0;
    long        default_state;
    int         changed;
    

    /* Initialize cones to unknown until we can say otherwise */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TECTYPE |= CNS_STATE_UNKNOWN;
    }

    /* Initialize FIFO with cone driven by input connectors */
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptuser = getptype(ptlocon->USER, CNS_TYPELOCON);
        if (ptuser == NULL) continue;
        state_type = ((long)ptuser->DATA) & (CNS_ZERO|CNS_INIT_ZERO|CNS_ONE|CNS_INIT_ONE);
        if (state_type != 0) {
            if ((state_type & CNS_INIT_ONE) != 0) state_type |= CNS_STATE_ONE;
            if ((state_type & CNS_INIT_ZERO) != 0) state_type |= CNS_STATE_ZERO;
            state_type &= (CNS_ZERO|CNS_STATE_ZERO|CNS_ONE|CNS_STATE_ONE);
            ptuser->DATA = (void *)((long)ptuser->DATA | state_type);
            ptloconcone = NULL;
            if ((ptuser = getptype(ptlocon->USER, CNS_EXT)) != NULL) {
                ptloconcone = (cone_list *)ptuser->DATA;
                ptloconcone->TECTYPE &= ~CNS_STATE_UNKNOWN;
                ptloconcone->TECTYPE |= state_type;
                for (ptedge = ptloconcone->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
                    if ((ptedge->TYPE & CNS_EXT) == 0) {
                        cnsFIFOPush(&ptFIFO, ptedge->UEDGE.CONE);
                    }
                }
            }
            if ((ptuser = getptype(ptlocon->USER, CNS_CONE)) != NULL) {
                for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptcone = (cone_list *)ptchain->DATA;
                    if (ptcone != ptloconcone) cnsFIFOPush(&ptFIFO, ptcone);
                }
            }
            if (ptcbhseq && ptlocon->NAME == ptcbhseq->STATEPIN) cbhseq_state = state_type;
        }
    }

    /* run until the FIFO is empty */
    while (ptFIFO != NULL) {
        ptcone = (cone_list *)cnsFIFOPop(&ptFIFO);
        if (ptcbhseq && (ptcone->NAME == ptcbhseq->LATCHNAME || ptcone->NAME == ptcbhseq->SLAVENAME)) {
            default_state = cnsCalcLatchCbhState(ptcone, ptcbhseq, cbhseq_state);
        }
        else default_state = CNS_STATE_UNKNOWN;
        changed = cnsCalcConeState(ptcone, default_state);
        state_type = ptcone->TECTYPE & (CNS_ZERO|CNS_STATE_ZERO|CNS_ONE|CNS_STATE_ONE);
        if (changed && state_type != 0) {
            for (ptedge = ptcone->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
                if ((ptedge->TYPE & CNS_EXT) == 0) {
                    ptoutcone = ptedge->UEDGE.CONE;
                    cnsFIFOPush(&ptFIFO, ptedge->UEDGE.CONE);
                }
                else {
                    ptlocon = ptedge->UEDGE.LOCON;
                    if ((ptuser = getptype(ptlocon->USER, CNS_TYPELOCON)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | state_type);
                    }
                    else ptlocon->USER = addptype(ptlocon->USER, CNS_TYPELOCON, (void *)state_type);
                }
            }
        }
    }
}

/****************************************************************************
*                         function cnsCleanFigState();                      *
****************************************************************************/
/*------------------------------------------------+
| Removes thee calculated state markings leaving  |
| just those of the input connectors              |
+------------------------------------------------*/

void
cnsCleanFigState(cnsfig_list *ptcnsfig, cbhseq *ptcbhseq)
{
    locon_list *ptlocon;
    cone_list  *ptcone;
    ptype_list *ptuser;

    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (ptlocon->DIRECTION == CNS_I || ptlocon->DIRECTION == CNS_T) continue;
        if (ptcbhseq && ptlocon->NAME == ptcbhseq->STATEPIN) continue;
        if ((ptuser = getptype(ptlocon->USER, CNS_TYPELOCON)) != NULL) {
            ptuser->DATA = (void *)((long)ptuser->DATA & ~(CNS_STATE_ZERO|CNS_STATE_ONE));
            if (ptuser->DATA == NULL) {
                ptlocon->USER = delptype(ptlocon->USER, CNS_TYPELOCON);
            }
        }
    }
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        ptcone->TECTYPE &= ~(CNS_STATE_ZERO|CNS_STATE_ONE|CNS_STATE_UNKNOWN);
    }
}
