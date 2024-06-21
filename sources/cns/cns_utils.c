#ifdef Linux
#include <values.h>
#else
#include <float.h>
#endif
#include "cnsall.h"

static int cns_updatesupplyfromswing(inffig_list *ifl, char *name, alim_list *ptsupply);
static alim_list *cns_addsupply(cnsfig_list *ptcnsfig, alim_list supply);
static void cns_addlotrsalim(lotrs_list *lotrs, alim_list *alim);
static void cns_dellotrsalim(lotrs_list * lotrs);
static int cns_updatesupplyfromneighbours(losig_list *ptlosig, lotrs_list *ptprevlotrs, alim_list *pttempsupply, float defaultvdd);

/*******************************************************************************
 * function cns_get_multivoltage                                               *
 *******************************************************************************/
alim_list *
cns_get_multivoltage(cone_list *ptcone)
{
    ptype_list     *ptuser;

    if ((ptuser = getptype(ptcone->USER, CNS_SUPPLY)) != NULL) {
        return (alim_list *)ptuser->DATA;
    }
    return NULL;
}

/*******************************************************************************
 * function cns_get_signal_multivoltage                                        *
 *******************************************************************************/
alim_list *
cns_get_signal_multivoltage(losig_list *ptlosig)
{
    ptype_list     *ptuser;

    if ((ptuser = getptype(ptlosig->USER, CNS_SUPPLY)) != NULL) {
        return (alim_list *)ptuser->DATA;
    }
    return NULL;
}

/*******************************************************************************
 * function cns_get_lotrs_multivoltage                                         *
 *******************************************************************************/
alim_list *
cns_get_lotrs_multivoltage(lotrs_list *ptlotrs)
{
    ptype_list     *ptuser;

    if ((ptuser = getptype(ptlotrs->USER, CNS_SUPPLY)) != NULL) {
        return (alim_list *) ptuser->DATA;
    }
    else return NULL;
}

/*******************************************************************************
 * function cns_updatesupplyfromswing                                          *
 *******************************************************************************/
static int
cns_updatesupplyfromswing(inffig_list *ifl, char *name, alim_list *ptsupply)
{
    chain_list     *lst, *ptchain;
    double          hvolt = -DBL_MAX, lvolt = -DBL_MAX;
    char           *infname;

    lst = inf_GetEntriesByType(ifl, INF_PIN_HIGH_VOLTAGE, INF_ANY_VALUES);
    for (ptchain = lst; ptchain; ptchain = ptchain->NEXT) {
        infname = (char *)ptchain->DATA;
        if (mbk_TestREGEX(name, infname)) {
            name = infname;
            break;
        }
    }
    freechain(lst);

    if (inf_GetDouble(ifl, name, INF_PIN_HIGH_VOLTAGE, &hvolt)) {
        if (!inf_GetDouble(ifl, name, INF_PIN_LOW_VOLTAGE, &lvolt)) lvolt = 0.0;
    }
    else if (inf_GetDouble(ifl, "default", INF_PIN_HIGH_VOLTAGE, &hvolt)) {
        if (!inf_GetDouble(ifl, "default", INF_PIN_LOW_VOLTAGE, &lvolt)) lvolt = 0.0;
    }

    if (hvolt != -DBL_MAX) {
        if (hvolt > ptsupply->VDDMAX) ptsupply->VDDMAX = hvolt;
        if (hvolt < ptsupply->VDDMIN) ptsupply->VDDMIN = hvolt;
        if (lvolt > ptsupply->VSSMAX) ptsupply->VSSMAX = lvolt;
        if (lvolt < ptsupply->VSSMIN) ptsupply->VSSMIN = lvolt;
        return 1;
    }
    return 0;
}

/*******************************************************************************
 * function cns_addsupply                                                      *
 *******************************************************************************/
static alim_list *
cns_addsupply(cnsfig_list *ptcnsfig, alim_list supply)
{
    ptype_list *ptuser;
    alim_list  *head_supply = NULL;
    alim_list  *ptsupply;
    
    if ((ptuser = getptype(ptcnsfig->USER, CNS_POWER_SUPPLIES)) == NULL) {
        ptcnsfig->USER = addptype(ptcnsfig->USER, CNS_POWER_SUPPLIES, head_supply);
        ptuser = ptcnsfig->USER;
    }
    else head_supply = (alim_list *)ptuser->DATA;
    
    for (ptsupply = head_supply; ptsupply; ptsupply = ptsupply->NEXT) {
        if (ptsupply->VDDMAX == supply.VDDMAX && ptsupply->VDDMIN == supply.VDDMIN
        && ptsupply->VSSMAX == supply.VSSMAX && ptsupply->VSSMIN == supply.VSSMIN) {
            return ptsupply;
        }
    }

    ptsupply = (alim_list *)mbkalloc(sizeof(alim_list));
    ptsupply->NEXT = head_supply;
    ptsupply->VDDMAX = supply.VDDMAX;
    ptsupply->VDDMIN = supply.VDDMIN;
    ptsupply->VSSMAX = supply.VSSMAX;
    ptsupply->VSSMIN = supply.VSSMIN;
    
    ptuser->DATA = (void *)ptsupply;
    
    return ptsupply;
}

/*******************************************************************************
 * function cns_addlotrsalim                                                   *
 *******************************************************************************/
static void 
cns_addlotrsalim(lotrs_list *lotrs, alim_list *alim)
{
    ptype_list     *ptuser, *para;
    chain_list     *cl, *ch;

    if ((para = getptype(lotrs->USER, MBK_TRANS_PARALLEL)) != NULL) {
        cl = (chain_list *) para->DATA;
    }
    else cl = NULL;

    cl = addchain(cl, lotrs);
    for (ch = cl; ch != NULL; ch = ch->NEXT) {
        lotrs = (lotrs_list *) ch->DATA;
        if ((ptuser = getptype(lotrs->USER, CNS_SUPPLY)) == NULL) {
            lotrs->USER = addptype(lotrs->USER, CNS_SUPPLY, alim);
        }
        else ptuser->DATA = alim;
    }
    cl->NEXT = NULL;
    freechain(cl);
}

/*******************************************************************************
 * function cns_dellotrsalim                                                   *
 *******************************************************************************/
static void 
cns_dellotrsalim(lotrs_list * lotrs)
{
    ptype_list     *para;
    chain_list     *cl, *ch;
    int             done = 0;

    if ((para = getptype(lotrs->USER, MBK_TRANS_PARALLEL)) != NULL) {
        cl = (chain_list *) para->DATA;
    }
    else cl = NULL;

    cl = addchain(cl, lotrs);
    for (ch = cl; ch != NULL; ch = ch->NEXT) {
        lotrs = (lotrs_list *) ch->DATA;
        if (getptype(lotrs->USER, CNS_SUPPLY) != NULL) {
            lotrs->USER = delptype(lotrs->USER, CNS_SUPPLY);
            done = 1;
        }
    }
    cl->NEXT = NULL;
    freechain(cl);
}

/*******************************************************************************
 * function cns_getlotrsalim                                                   *
 *******************************************************************************/
int 
cns_getlotrsalim(lotrs_list * lotrs, char type, float *alim)
{
    ptype_list     *ptuser;
    alim_list      *power;

    if ((ptuser = getptype(lotrs->USER, CNS_SUPPLY)) != NULL) {
        power = ptuser->DATA;
        if (type == 'M') *alim = power->VDDMIN - power->VSSMAX;
        else *alim = power->VDDMAX - power->VSSMIN;
        return 1;
    }
    return 0;
}

/*******************************************************************************
 * function cns_delmultivoltage_cone                                           *
 *******************************************************************************/
void 
cns_delmultivoltage_cone(cone_list *ptcone)
{
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    losig_list     *ptconesig;
    link_list      *ptlink;
    int             i;
    ptype_list     *ptuser;

    ptuser = getptype(ptcone->USER, CNS_SUPPLY);
    if (ptuser) {
        brlist[0] = ptcone->BREXT;
        brlist[1] = ptcone->BRVDD;
        brlist[2] = ptcone->BRVSS;
        for (i = 0; i < 3; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    if ((ptlink->TYPE & CNS_EXT) == 0) {
                        cns_dellotrsalim(ptlink->ULINK.LOTRS);
                    }
                }
            }
        }
        ptcone->USER = delptype(ptcone->USER, CNS_SUPPLY);
        ptconesig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
        ptconesig->USER = testanddelptype(ptconesig->USER, CNS_SUPPLY);
    }
}

/*******************************************************************************
 * function cns_addmultivoltage_cone                                           *
 *******************************************************************************/
void 
cns_addmultivoltage_cone(cnsfig_list *ptcnsfig, inffig_list *ifl, cone_list *ptcone)
{
    branch_list    *brlist[3];
    branch_list    *ptbranch;
    link_list      *ptlink;
    alim_list      *ptsupply, *ptlotrs_supply;
    alim_list       supply = {NULL, FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX};
    alim_list       lotrs_supply;
    chain_list     *ptloconchain;
    lotrs_list     *ptlotrs;
    locon_list     *ptlocon;
    losig_list     *ptconesig;
    ptype_list     *ptuser;
    float           alim, vbulk;
    float           foundvdd = -1000;
    int             changed, i, has_supply = 0;

    if (getptype(ptcone->USER, CNS_SUPPLY)) {
        cns_delmultivoltage_cone(ptcone);
    }

    ptconesig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;

    /* power supply cones and breakpoints */
    if (!brlist[0] && !brlist[1] && !brlist[2]) {
        if (getlosigalim(ptconesig, &alim)) {
            supply.VDDMIN = alim;
            supply.VDDMAX = alim;
            supply.VSSMIN = 0.0;
            supply.VSSMAX = 0.0;
            has_supply = 1;
        }
        else has_supply = cns_updatesupplyfromswing(ifl, ptcone->NAME, &supply);
        if (has_supply) {
            ptsupply = cns_addsupply(ptcnsfig, supply);
            ptcone->USER = addptype(ptcone->USER, CNS_SUPPLY, ptsupply);
            ptconesig->USER = addptype(ptconesig->USER, CNS_SUPPLY, ptsupply);
        }
        return;
    }

    /* create the cone supply structure by traversing all transistor links in cone */
    for (i = 0; i < 3; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink->NEXT; ptlink = ptlink->NEXT);
            if (i < 2) {
                if (getlosigalim(ptlink->ULINK.LOTRS->SOURCE->SIG, &alim) || getlosigalim(ptlink->ULINK.LOTRS->DRAIN->SIG, &alim)) {
                    if (i == 0 && (alim > supply.VDDMAX)) supply.VDDMAX = alim;
                    if (i == 0 && (alim < supply.VDDMIN)) supply.VDDMIN = alim;
                    if (i == 1 && (alim > supply.VSSMAX)) supply.VSSMAX = alim;
                    if (i == 1 && (alim < supply.VSSMIN)) supply.VSSMIN = alim;
                    if (i == 1 && !ptcone->BREXT && !ptcone->BRVDD) {
                        for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                            if (getlosigalim(ptlink->ULINK.LOTRS->BULK->SIG, &vbulk) &&
                                MLO_IS_TRANSP(ptlink->ULINK.LOTRS->TYPE)) {
                                if (vbulk < supply.VDDMIN) supply.VDDMIN = vbulk;
                                if (vbulk > supply.VDDMAX) supply.VDDMAX = vbulk;
                            }
                        }
                    }
                    has_supply = TRUE;
                }
            }
            else {
                if (!has_supply) {
                    if (ptcone->BRVDD) {
                        supply.VDDMAX = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
                        supply.VDDMIN = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
                    }
                    if (ptcone->BRVSS) {
                        supply.VSSMAX = 0;
                        supply.VSSMAX = 0;
                    }
                }
                has_supply = cns_updatesupplyfromswing(ifl, ptlink->ULINK.LOCON->NAME, &supply);
            }
        }
    }

    if (has_supply) {

        /* try to find a P transistor in case of stuck vss cone */
        foundvdd = -1000;
        if (supply.VDDMIN == FLT_MAX) {
            if ((ptuser = getptype(ptconesig->USER, LOFIGCHAIN)) != NULL) {
                ptloconchain = (chain_list *)ptuser->DATA;
            }
            else ptloconchain = NULL;

            for (; ptloconchain; ptloconchain = ptloconchain->NEXT) {
                ptlocon = (locon_list *)ptloconchain->DATA;
                if (ptlocon->NAME != CNS_DRAINNAME && ptlocon->NAME != CNS_SOURCENAME) continue;
                ptlotrs = ptlocon->ROOT;
                if ((ptlotrs->TYPE & CNS_TP) == CNS_TP && ptlotrs->BULK && ptlotrs->BULK->SIG && getlosigalim(ptlotrs->BULK->SIG, &vbulk)) {
                    if (vbulk > foundvdd) foundvdd = vbulk;
                }
            }
        }
        if (foundvdd == -1000) foundvdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;

        /* defaults */
        if (supply.VDDMIN == FLT_MAX) supply.VDDMIN = foundvdd;
        if (supply.VDDMAX == -FLT_MAX) supply.VDDMAX = foundvdd;
        if (supply.VSSMIN == FLT_MAX) supply.VSSMIN = 0.0;
        if (supply.VSSMAX == -FLT_MAX) supply.VSSMAX = 0.0;
        
        /* add the cone supply */
        ptsupply = cns_addsupply(ptcnsfig, supply);
        ptcone->USER = addptype(ptcone->USER, CNS_SUPPLY, ptsupply);
        ptconesig->USER = addptype(ptconesig->USER, CNS_SUPPLY, ptsupply);

        /* mark all transistors in the cone */
        for (i = 0; i < 3; i++) {
            for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
                for (ptlink = ptbranch->LINK; ptlink->NEXT; ptlink = ptlink->NEXT);
                lotrs_supply = supply;
                changed = 0;
                if (i < 2) {
                    if (getlosigalim(ptlink->ULINK.LOTRS->SOURCE->SIG, &alim) || getlosigalim(ptlink->ULINK.LOTRS->DRAIN->SIG, &alim)) {
                        if (i == 0) {
                            lotrs_supply.VDDMAX = alim;
                            lotrs_supply.VDDMIN = alim;
                            if (supply.VDDMAX != alim || supply.VDDMIN != alim) changed = 1;
                        }
                        if (i == 1) {
                            lotrs_supply.VSSMAX = alim;
                            lotrs_supply.VSSMIN = alim;
                            if (supply.VSSMAX != alim || supply.VSSMIN != alim) changed = 1;
                        }
                    }
                }
                else {
                    changed = cns_updatesupplyfromswing(ifl, ptlink->ULINK.LOCON->NAME, &lotrs_supply);
                }
                for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
                    if ((ptlink->TYPE & CNS_EXT) == 0) {
                        if (changed || (ptuser = getptype(ptlink->ULINK.LOTRS->USER, CNS_SUPPLY)) != NULL) {
                            if (ptuser) {
                                changed = 0;
                                ptlotrs_supply = (alim_list *)ptuser->DATA;
                                if (ptlotrs_supply->VDDMAX > lotrs_supply.VDDMAX) {
                                    lotrs_supply.VDDMAX = ptlotrs_supply->VDDMAX;
                                    changed = 1;
                                }
                                if (ptlotrs_supply->VDDMIN < lotrs_supply.VDDMIN) {
                                    lotrs_supply.VDDMIN = ptlotrs_supply->VDDMIN;
                                    changed = 1;
                                }
                                if (ptlotrs_supply->VSSMAX > lotrs_supply.VSSMAX) {
                                    lotrs_supply.VSSMAX = ptlotrs_supply->VSSMAX;
                                    changed = 1;
                                }
                                if (ptlotrs_supply->VSSMIN < lotrs_supply.VSSMIN) {
                                    lotrs_supply.VSSMIN = ptlotrs_supply->VSSMIN;
                                    changed = 1;
                                }
                            }
                            if (changed ) {
                                ptlotrs_supply = cns_addsupply(ptcnsfig, lotrs_supply);
                                cns_dellotrsalim(ptlink->ULINK.LOTRS);
                                cns_addlotrsalim(ptlink->ULINK.LOTRS, ptlotrs_supply);
                            }
                        }
                        else cns_addlotrsalim(ptlink->ULINK.LOTRS, ptsupply);
                    }
                }
            }
        }
    }
}

/*******************************************************************************
 * function cns_addmultivoltage                                                *
 *******************************************************************************/
void 
cns_addmultivoltage(inffig_list *ifl, cnsfig_list *ptcnsfig)
{
    cone_list      *ptcone;
    lofig_list     *ptlofig;
    lotrs_list     *ptlotrs;
    losig_list     *ptlosig;
    ptype_list     *ptuser;
    alim_list      *ptdefaultsupply, *ptlotrs_supply, *pttempsupply, *ptsupply;
    alim_list       lotrs_supply;
    float           alim, defaultvdd;
    float           myvdd, myvss;
    int             numtemp, changed, iterations;

    defaultvdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    ptlofig = ptcnsfig->LOFIG;
    if (ptlofig) {
        if (!V_FLOAT_TAB[__SIM_POWER_SUPPLY].SET) {
            V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = -FLT_MAX;
            for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
                if (mbk_LosigIsVDD(ptlosig)) {
                    if (getlosigalim(ptlosig, &alim)) {
                        if (alim > V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE) V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = alim;
                    }
                }
            }
            if (defaultvdd > V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE) V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = defaultvdd;
        }
    }

    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        cns_addmultivoltage_cone(ptcnsfig, ifl, ptcone);
    }
    
    lotrs_supply.VDDMAX = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    lotrs_supply.VDDMIN = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    lotrs_supply.VSSMAX = 0;
    lotrs_supply.VSSMIN = 0;
    ptdefaultsupply = cns_addsupply(ptcnsfig, lotrs_supply);

    if (V_BOOL_TAB[__AVT_ALL_TRANS_SUPPLIES].VALUE) {
        numtemp = 0;
        for (ptlotrs = ptcnsfig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
            if (getptype(ptlotrs->USER, CNS_UNUSED) == NULL) continue;
            if ((ptuser = getptype(ptlotrs->USER, CNS_SUPPLY)) == NULL) {
                pttempsupply = (alim_list *)mbkalloc(sizeof(alim_list));
                pttempsupply->NEXT = NULL;
                pttempsupply->VDDMAX = -FLT_MAX;
                pttempsupply->VDDMIN = FLT_MAX;
                pttempsupply->VSSMAX = -FLT_MAX;
                pttempsupply->VSSMIN = FLT_MAX;
                ptlotrs->USER = addptype(ptlotrs->USER, CNS_TEMP_SUPPLY, pttempsupply);
                numtemp++;
            }
        }
        changed = 1;
        iterations = 0;
        while (numtemp > 0 && changed && iterations < 10) {
            changed = 0;
            for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
                if ((ptuser = getptype(ptlotrs->USER, CNS_TEMP_SUPPLY)) == NULL) continue;
                pttempsupply = (alim_list *)ptuser->DATA;
                if (cns_updatesupplyfromneighbours(ptlotrs->DRAIN->SIG, ptlotrs, pttempsupply, defaultvdd)) changed = 1;
                if (cns_updatesupplyfromneighbours(ptlotrs->SOURCE->SIG, ptlotrs, pttempsupply, defaultvdd)) changed = 1;
            }
            iterations++;
        }
    }

    /* final treatment of unused transistors without supply values */
    for (ptlotrs = ptcnsfig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        if (getptype(ptlotrs->USER, CNS_UNUSED) == NULL) continue;
        if (getptype(ptlotrs->USER, CNS_SUPPLY) == NULL) {
            if ((ptuser = getptype(ptlotrs->USER, CNS_TEMP_SUPPLY)) != NULL) {
                lotrs_supply = *(alim_list *)ptuser->DATA;
                ptlotrs->USER = delptype(ptlotrs->USER, CNS_TEMP_SUPPLY);
            }
            else {
                lotrs_supply.VDDMAX = -FLT_MAX;
                lotrs_supply.VDDMIN = FLT_MAX;
                lotrs_supply.VSSMAX = -FLT_MAX;
                lotrs_supply.VSSMIN = FLT_MAX;
            }
            myvdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
            myvss = 0;
            if (MLO_IS_TRANSP(ptlotrs->TYPE) && ptlotrs->BULK && ptlotrs->BULK->SIG) {
                if (getlosigalim(ptlotrs->BULK->SIG, &alim)) myvdd = alim;
            }
            if (MLO_IS_TRANSN(ptlotrs->TYPE) && ptlotrs->BULK && ptlotrs->BULK->SIG) {
                if (getlosigalim(ptlotrs->BULK->SIG, &alim)) myvss = alim;
            }

            if (myvdd > lotrs_supply.VDDMAX) lotrs_supply.VDDMAX = myvdd;
            if (myvdd < lotrs_supply.VDDMIN) lotrs_supply.VDDMIN = myvdd;
            if (myvss > lotrs_supply.VSSMAX) lotrs_supply.VSSMAX = myvss;
            if (myvss < lotrs_supply.VSSMIN) lotrs_supply.VSSMIN = myvss;
            if (lotrs_supply.VDDMAX != defaultvdd || lotrs_supply.VDDMIN != defaultvdd || lotrs_supply.VSSMAX != 0 || lotrs_supply.VSSMIN != 0) {
                ptsupply = cns_addsupply(ptcnsfig, lotrs_supply);
            }
            else ptsupply = ptdefaultsupply;
            cns_addlotrsalim(ptlotrs, ptsupply);
        }
    }
}

static int
cns_updatesupplyfromneighbours(losig_list *ptlosig, lotrs_list *ptprevlotrs, alim_list *pttempsupply, float defaultvdd)
{
    locon_list *ptlocon;
    lotrs_list *ptlotrs;
    chain_list *ptloconchain;
    alim_list  *ptlotrs_supply;
    ptype_list *ptuser;
    float       alim;
    int         changed = 0;

    if (mbk_LosigIsVDD(ptlosig)) {
        if (!getlosigalim(ptlosig, &alim)) alim = defaultvdd;
        if (alim > pttempsupply->VDDMAX) {
            pttempsupply->VDDMAX = alim;
            changed = 1;
        }
        if (alim < pttempsupply->VDDMIN) {
            pttempsupply->VDDMIN = alim;
            changed = 1;
        }
    }
    else if (mbk_LosigIsVSS(ptlosig)) {
        if (!getlosigalim(ptlosig, &alim)) alim = 0;
        if (alim > pttempsupply->VSSMAX) {
            pttempsupply->VSSMAX = alim;
            changed = 1;
        }
        if (alim < pttempsupply->VSSMIN) {
            pttempsupply->VSSMIN = alim;
            changed = 1;
        }
    }
    else {
        if ((ptuser = getptype(ptlosig->USER, LOFIGCHAIN)) != NULL) {
            ptloconchain = (chain_list *)ptuser->DATA;
        }
        else ptloconchain = NULL;
        for (; ptloconchain; ptloconchain = ptloconchain->NEXT) {
            ptlocon = (locon_list *)ptloconchain->DATA;
            if (ptlocon->NAME != CNS_DRAINNAME && ptlocon->NAME != CNS_SOURCENAME) continue;
            ptlotrs = ptlocon->ROOT;
            if (ptlotrs == ptprevlotrs) continue;
            ptlotrs_supply = NULL;
            if ((ptuser = getptype(ptlotrs->USER, CNS_SUPPLY)) != NULL) {
                ptlotrs_supply = (alim_list *)ptuser->DATA;
            }
            else if ((ptuser = getptype(ptlotrs->USER, CNS_TEMP_SUPPLY)) != NULL) {
                ptlotrs_supply = (alim_list *)ptuser->DATA;
            }
            if (ptlotrs_supply) {
                if (ptlotrs_supply->VDDMAX > pttempsupply->VDDMAX) {
                    pttempsupply->VDDMAX = ptlotrs_supply->VDDMAX;
                    changed = 1;
                }
                if (ptlotrs_supply->VDDMIN < pttempsupply->VDDMIN) {
                    pttempsupply->VDDMIN = ptlotrs_supply->VDDMIN;
                    changed = 1;
                }
                if (ptlotrs_supply->VSSMAX > pttempsupply->VSSMAX) {
                    pttempsupply->VSSMAX = ptlotrs_supply->VSSMAX;
                    changed = 1;
                }
                if (ptlotrs_supply->VSSMIN < pttempsupply->VSSMIN) {
                    pttempsupply->VSSMIN = ptlotrs_supply->VSSMIN;
                    changed = 1;
                }
            }
        }
    }
    return changed;
}

/*******************************************************************************
 * function cns_delmultivoltage                                                *
 *******************************************************************************/
void 
cns_delmultivoltage(cnsfig_list *ptcnsfig)
{
    cone_list      *ptcone;
    lotrs_list     *ptlotrs;
    alim_list      *ptsupply, *ptnextsupply;
    ptype_list     *ptuser;

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT)
        cns_delmultivoltage_cone(ptcone);

    /* unused transistors */
    for (ptlotrs = ptcnsfig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        cns_dellotrsalim(ptlotrs);
    }
    if ((ptuser = getptype(ptcnsfig->USER, CNS_POWER_SUPPLIES)) != NULL) {
        for (ptsupply = (alim_list *)ptuser->DATA; ptsupply; ptsupply = ptnextsupply) {
            ptnextsupply = ptsupply->NEXT;
            mbkfree(ptsupply);
        }
        ptcnsfig->USER = delptype(ptcnsfig->USER, CNS_POWER_SUPPLIES);
    }
}

/*******************************************************************************
 * function cns_get_cone_external_connectors                                   *
 *******************************************************************************/
chain_list *
cns_get_cone_external_connectors(cone_list *ptcone)
{
    ptype_list     *ptuser;

    if ((ptuser = getptype(ptcone->USER, CNS_EXT)) == NULL) return NULL;
    return dupchainlst((chain_list *)ptuser->DATA);
}

/*******************************************************************************
 * function cns_get_one_cone_external_connector                                *
 *******************************************************************************/
locon_list *
cns_get_one_cone_external_connector(cone_list *ptcone)
{
    chain_list     *ptchain;
    locon_list     *ptlocon = NULL;

    ptchain = cns_get_cone_external_connectors(ptcone);
    if (ptchain) ptlocon = (locon_list *)ptchain->DATA;
    freechain(ptchain);
    return ptlocon;
}

/*******************************************************************************
 * function cns_signcns                                                        *
 *******************************************************************************/
unsigned int 
cns_signcns(cnsfig_list * cnsfig)
{
    unsigned int    n, s;

    s = 0;

    n = countchain((chain_list *) cnsfig->LOCON);
    s = n;

    n = countchain((chain_list *) cnsfig->INTCON);
    s = ROT(s, 6) ^ n;

    n = countchain((chain_list *) cnsfig->LOTRS);
    s = ROT(s, 6) ^ n;

    n = countchain((chain_list *) cnsfig->LOINS);
    s = ROT(s, 6) ^ n;

    n = countchain((chain_list *) cnsfig->CONE);
    s = ROT(s, 6) ^ n;

    return s;
}
