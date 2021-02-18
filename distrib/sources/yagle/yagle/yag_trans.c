/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_trans.c                                                 */
/*                                                                          */
/*    (c) copyright 1991 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc LAURENTIN                        le : 06/09/1991     */
/*                                                                          */
/*    Modifie par : Anthony LESTER                      le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static int test_connection __P((lotrs_list *pttrans));
static int test_resistance __P((lotrs_list *pttrans));
static int test_diode __P((lotrs_list *pttrans));
static int test_blocked __P((lotrs_list *pttrans));
static int test_capacitance __P((lotrs_list *pttrans));

/****************************************************************************
 *                         function test_connection()                       *
 ****************************************************************************/
        /* test the connectivity of a transistor */

static int
test_connection(pttrans)
    lotrs_list     *pttrans;

{
    locon_list     *ptcon = NULL;
    chain_list     *ptchain = NULL;
    int             count = 0;

    /* traverse list of connectors on grid signal */
    for (ptchain = (chain_list *)getptype(pttrans->GRID->SIG->USER, LOFIGCHAIN)->DATA;
        ptchain != NULL;
        ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon != pttrans->GRID) {
            break;
        }
    }
    if ((ptchain == NULL) && (ptcon == pttrans->GRID)) {
        /* grid unconnected */
        count = 1;
        yagWarning(WAR_UNCONNECTED, "test_connection", "Gate", (char *)pttrans, 0);
    }

    /* traverse list of connectors on source signal */
    if ((pttrans->SOURCE->SIG->TYPE != CNS_SIGVDD)
    && (pttrans->SOURCE->SIG->TYPE != CNS_SIGVSS)) {
        for (ptchain = (chain_list *)getptype(pttrans->SOURCE->SIG->USER, LOFIGCHAIN)->DATA;
            ptchain != NULL;
            ptchain = ptchain->NEXT) {
            ptcon = (locon_list *)ptchain->DATA;
            if (ptcon != pttrans->SOURCE) {
                break;
            }
        }
        if (ptchain == NULL) {
            /* source unconnected */
            count = 1;
            yagWarning(WAR_UNCONNECTED, "test_connection", "Source", (char *)pttrans, 0);
        }
    }

    /* traverse list of connectors on drain signal */
    if ((pttrans->DRAIN->SIG->TYPE != CNS_SIGVDD)
    && (pttrans->DRAIN->SIG->TYPE != CNS_SIGVSS)) {
        for (ptchain = (chain_list *)getptype(pttrans->DRAIN->SIG->USER, LOFIGCHAIN)->DATA;
            ptchain != NULL;
            ptchain = ptchain->NEXT) {
            ptcon = (locon_list *)ptchain->DATA;
            if (ptcon != pttrans->DRAIN) {
                break;
            }
        }
        if ((ptchain == NULL) && (ptcon == pttrans->DRAIN)) {
            /* drain unconnected */
            count = 1;
            yagWarning(WAR_UNCONNECTED, "test_connection", "Drain", (char *)pttrans, 0);
        }
    }

    return count;
}

/****************************************************************************
 *                         function test_resistance()                       *
 ****************************************************************************/
        /* detection of transistors connected as resistances */

static int 
test_resistance(pttrans)
    lotrs_list     *pttrans;
{
    int             count = 0;

/*-------------------------------------------+
|  test whether a N-type is a resistance     |
+-------------------------------------------*/
    if ((pttrans->TYPE == CNS_TN) && (pttrans->GRID->SIG->TYPE == CNS_SIGVDD)) {
        yagWarning(WAR_RESISTANCE, "test_resistance", "N", (char *)pttrans, 0);
        pttrans->TYPE |= RESIST;
        count++;
    }

/*-------------------------------------------+
|  test whether a P-type is a resistance     |
+-------------------------------------------*/
    else if ((pttrans->TYPE == CNS_TP) && (pttrans->GRID->SIG->TYPE == CNS_SIGVSS)) {
        yagWarning(WAR_RESISTANCE, "test_resistance", "P", (char *)pttrans, 0);
        pttrans->TYPE |= RESIST;
        count++;
    }

    return count;
}

/****************************************************************************
 *                         function test_diode()                            *
 ****************************************************************************/
        /* detection of transistors connected as diodes */

static int 
test_diode(pttrans)
    lotrs_list     *pttrans;
{
    int             count = 0;


    if ((pttrans->GRID->SIG == pttrans->DRAIN->SIG)
    && (pttrans->GRID->SIG->TYPE != CNS_SIGVDD)
    && (pttrans->GRID->SIG->TYPE != CNS_SIGVSS)) {
        yagWarning(WAR_DIODE, "test_diode", NULL, (char *)pttrans, 0);
        pttrans->TYPE |= DIODE;
        count++;
    }


    if ((pttrans->GRID->SIG == pttrans->SOURCE->SIG)
    && (pttrans->GRID->SIG->TYPE != CNS_SIGVDD)
    && (pttrans->GRID->SIG->TYPE != CNS_SIGVSS)) {
        yagWarning(WAR_DIODE, "test_diode", NULL, (char *)pttrans, 0);
        pttrans->TYPE |= DIODE;
        count++;
    }

    if (count > 0) {
        pttrans->USER = addptype(pttrans->USER, YAG_GRIDCON_PTYPE, pttrans->GRID);
    }
    return count;
}


/****************************************************************************
 *                         function test_blocked()                          *
 ****************************************************************************/
        /* detection of blocked transistors */

static int 
test_blocked(pttrans)
    lotrs_list     *pttrans;
{
    int             count = 0;


    /* test for blocked P transistor */
    if ((pttrans->TYPE == CNS_TP) && (pttrans->GRID->SIG->TYPE == CNS_SIGVDD)) {
        yagWarning(WAR_ALWAYSOFF, "test_blocked", "P", (char *)pttrans, 0);
        count++;
    }

    /* test for blocked N transistor */
    else if ((pttrans->TYPE == CNS_TN) && (pttrans->GRID->SIG->TYPE == CNS_SIGVSS)) {
        yagWarning(WAR_ALWAYSOFF, "test_blocked", "N", (char *)pttrans, 0);
        count++;
    }

    return count;
}


/****************************************************************************
 *                         function test_capacitance()                      *
 ****************************************************************************/
       /* detection of transistors connected as capacitances */

static int 
test_capacitance(pttrans)
    lotrs_list     *pttrans;

{
    int             count = 0;

    if (pttrans->DRAIN->SIG == pttrans->SOURCE->SIG) {
        yagWarning(WAR_CAPACITANCE, "test_capacitance", NULL, (char *)pttrans, 0);
        pttrans->TYPE |= CAPACITE;
        count++;
    }
    return count;
}

/****************************************************************************
 *                         function test_switchtrans()                      *
 ****************************************************************************/
       /* detection of transistors connected as switches */

static int 
test_switchtrans(pttrans)
    lotrs_list     *pttrans;

{
    int             count = 0;
    chain_list     *ptloconchain, *ptchain;
    locon_list     *ptcon;
    lotrs_list     *pttesttrans;
    losig_list     *ptsig, *pttestsig, *origtest;    
    
    if (pttrans->DRAIN->SIG == pttrans->SOURCE->SIG) return 0;
    if (mbk_LosigIsVSS(pttrans->GRID->SIG) && (mbk_LosigIsVSS(pttrans->DRAIN->SIG) || mbk_LosigIsVDD(pttrans->DRAIN->SIG) || mbk_LosigIsVSS(pttrans->SOURCE->SIG) || mbk_LosigIsVDD(pttrans->SOURCE->SIG))) return 0;
    
    if (!mbk_LosigIsVDD(pttrans->DRAIN->SIG) && !mbk_LosigIsVSS(pttrans->DRAIN->SIG))
      ptsig = pttrans->DRAIN->SIG, origtest=pttrans->SOURCE->SIG;
    else
      ptsig = pttrans->SOURCE->SIG, origtest=pttrans->DRAIN->SIG;

    ptloconchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = ptloconchain; ptchain; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T') {
            if (ptcon->NAME == CNS_GRIDNAME) continue;
            if (ptcon->NAME == CNS_BULKNAME) continue;
            pttesttrans = (lotrs_list *)ptcon->ROOT;
            if (pttrans == pttesttrans) continue;
            if ((pttesttrans->TYPE & CNS_TN) == CNS_TN) continue;
            if (mbk_LosigIsVDD(pttrans->GRID->SIG) && (mbk_LosigIsVSS(pttrans->DRAIN->SIG) || mbk_LosigIsVDD(pttrans->DRAIN->SIG) || mbk_LosigIsVSS(pttrans->SOURCE->SIG) || mbk_LosigIsVDD(pttrans->SOURCE->SIG))) continue;
            if (pttesttrans->SOURCE->SIG == ptsig) pttestsig = pttesttrans->DRAIN->SIG;
            else pttestsig = pttesttrans->SOURCE->SIG;
            if (pttestsig == origtest && getptype(pttesttrans->USER, CNS_SWITCH) == NULL) {
                count++;
                pttrans->USER = addptype(pttrans->USER, CNS_SWITCH, pttesttrans);
                pttesttrans->USER = addptype(pttesttrans->USER, CNS_SWITCH, pttrans);
                break;
            }
        }
    }
    return count;
}

/****************************************************************************
 *                        function yagTestTransistors()                     *
 ****************************************************************************/
        /* test the transistor netlist */

void 
yagTestTransistors(ptmbkfig, silent)
    lofig_list     *ptmbkfig;
    int             silent;

{
    lotrs_list     *pttrans = NULL;
    ptype_list     *ptuser;
    long            numdiode = 0L;
    long            numcapa = 0L;
    long            numresist = 0L;
    long            numblocked = 0L;
    long            numunconnected = 0L;
    long            numparallel = 0L;
    long            numswitch = 0L;

/* transistor list traversal */
    for (pttrans = ptmbkfig->LOTRS; pttrans; pttrans = pttrans->NEXT) {

        if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
        }

        /* test if transistor unconnected */
        numunconnected += test_connection(pttrans);

        /* test if transistor connected as resistance */
        numresist += test_resistance(pttrans);

        /* test if transistor always off */
        numblocked += test_blocked(pttrans);

        /* test if transistor connected as capacitance */
        numcapa += test_capacitance(pttrans);

        /* test if transistor connected as diode */
        if (V_BOOL_TAB[__YAGLE_DIODE_TRANS].VALUE && (pttrans->TYPE & CAPACITE) == 0) {
            numdiode += test_diode(pttrans);
        }
    }

    /* remove transistors in parallel */
    if (YAG_CONTEXT->YAG_REMOVE_PARA) {
        numparallel = mbk_removeparallel(ptmbkfig, TRUE);
    }

    /* test if transistor connected as switch */
    for (pttrans = ptmbkfig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
        if ((pttrans->TYPE & CNS_TN) == CNS_TN && getptype(pttrans->USER, CNS_SWITCH) == NULL) {
            numswitch += test_switchtrans(pttrans);
        }
    }

   /*-----------------+
   | statistics       |
   +-----------------*/
    if (YAG_CONTEXT->YAG_STAT_MODE) {
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors as diodes          : %ld \n", numdiode);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors as capacitances    : %ld \n", numcapa);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors as resistances     : %ld \n", numresist);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors blocked            : %ld \n", numblocked);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors paired             : %ld \n", numparallel);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors unconnected        : %ld \n", numunconnected);
    }
}

/****************************************************************************
 *                         function yagMarkUsedTrans();                       *
 ****************************************************************************/
        /* mark tansistors used in the given cone */

void 
yagMarkUsedTrans(ptcone)
    cone_list      *ptcone;
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    branch_list    *brlist[4];
    lotrs_list     *pttrans, *ptparatrans;
    ptype_list     *ptuser;
    chain_list     *ptchain;
    int             i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
            for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
                if ((ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK)) != 0) {
                    pttrans = ptlink->ULINK.LOTRS;
                    pttrans->TYPE |= USED;
                    if ((ptuser = getptype(pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
                        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                            ptparatrans = (lotrs_list *)ptchain->DATA;
                            ptparatrans->TYPE |= USED;
                        }
                    }
                }
                if ((ptlink->TYPE & (CNS_SWITCH)) != 0) {
                    pttrans = ptlink->ULINK.LOTRS;
                    ptuser = getptype(pttrans->USER, CNS_SWITCH);
                    pttrans = (lotrs_list *)ptuser->DATA;
                    pttrans->TYPE |= USED;
                    if ((ptuser = getptype(pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
                        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                            ptparatrans = (lotrs_list *)ptchain->DATA;
                            ptparatrans->TYPE |= USED;
                        }
                    }
                }
            }
        }
    }
}

/****************************************************************************
 *                         function yagMarkTransLinks();                    *
 ****************************************************************************/
        /* mark links with transistor type */

void 
yagMarkTransLinks(ptcnsfig)
    cnsfig_list  *ptcnsfig;
{
    branch_list    *ptbranch;
    link_list      *ptlink;
    branch_list    *brlist[4];
    cone_list      *ptcone;
    int             i;

    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        brlist[0] = ptcone->BRVDD;
        brlist[1] = ptcone->BRVSS;
        brlist[2] = ptcone->BREXT;
        brlist[3] = ptcone->BRGND;

        for (i=0; i<4; i++) {
            for (ptbranch = brlist[i]; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
                for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
                    if ((ptlink->TYPE & CNS_EXT) == 0) {
                        ptlink->TYPE |= ((ptlink->ULINK.LOTRS->TYPE & CNS_TN) == CNS_TN ? CNS_TNLINK : CNS_TPLINK);
                    }
                }
            }
        }
    }
}

/****************************************************************************
 *                       function yagOrientTransistors()                    *
 ****************************************************************************/
/*---------------------------------------+
| orient the pass-transistors connected  |
| to the output of a clean cone which is |
| not connected to any grid.             |
+---------------------------------------*/

void 
yagOrientTransistors(ptsig)
    losig_list     *ptsig;
{
    chain_list     *headchain, *ptchain;
    lotrs_list     *pttrans;
    locon_list     *ptcon;
    cone_list      *ptcone;
    ptype_list     *ptuser;
    int             numdirconfs = 0;
    int             gridcon = FALSE;

    ptuser = getptype(ptsig->USER, YAG_CONE_PTYPE);
    if (ptuser == NULL) return;
    else ptcone = (cone_list *)ptuser->DATA;

    if (ptcone != NULL && ptcone->TYPE != 0) {
        headchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;

        for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {
            if (((locon_list *)ptchain->DATA)->NAME == CNS_GRIDNAME)
                gridcon = TRUE;
        }
        if (!gridcon) {
            for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {
                ptcon = (locon_list *)ptchain->DATA;
                if (ptcon->TYPE != 'T') continue;
                pttrans = (lotrs_list *)ptcon->ROOT;
                if ((pttrans->TYPE & USED) == 0) {
                    if (ptcon->NAME == CNS_SOURCENAME) {
                        if (pttrans->SOURCE->DIRECTION == 'O') {
                            yagError(ERR_TRANS_DIRCONF, NULL, NULL, NULL, pttrans->X, pttrans->Y);
                            pttrans->TYPE &= ~ORIENTED;
                            pttrans->SOURCE->DIRECTION = 'I';
                        }
                        else {
                            pttrans->TYPE |= ORIENTED;
                            pttrans->DRAIN->DIRECTION = 'O';
                        }
                    }
                    else if (ptcon->NAME == CNS_DRAINNAME) {
                        if (pttrans->DRAIN->DIRECTION == 'O') {
                            numdirconfs++;
                            yagError(ERR_TRANS_DIRCONF, NULL, NULL, NULL, pttrans->X, pttrans->Y);
                            pttrans->TYPE &= ~ORIENTED;
                            pttrans->DRAIN->DIRECTION = 'I';
                        }
                        else {
                            pttrans->TYPE |= ORIENTED;
                            pttrans->SOURCE->DIRECTION = 'O';
                        }
                    }
                }
            }
        }
    }
    if (numdirconfs != 0 && YAG_CONTEXT->YAG_STAT_MODE) {
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Transistor orientation conflicts : %ld \n", numdirconfs);
    }
}
