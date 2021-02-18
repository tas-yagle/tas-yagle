/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_matrix.c                                                */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

static int      identique_sig(losig_list *ptsig, losig_list *ptmodelsig, fcl_label model_label);
static int      identique_trs(lotrs_list *pttrans, lotrs_list *ptmodeltrans);
static int      identique_ins(loins_list *ptloins, loins_list *ptmodelloins);
static int      check_loop(matrice_list *ptm, void *ptdata, int looplevel);
static int      check_not_loopsig(matrice_list *ptm, losig_list *ptmodelsig, losig_list *ptsig, int looplevel);
static int      check_not_loop(matrice_list *ptm, void *ptdata, int looplevel, ht *matchht);
static int      same_supply(losig_list *ptsig, losig_list *ptmodelsig);

static int
check_loop(matrice_list *ptm, void *ptdata, int looplevel)
{
    if (looplevel == -1) return TRUE;
    else if (ptm->tab[looplevel] == ptdata) return TRUE;

    return FALSE;
}

static int
check_not_loop(matrice_list *ptm, void *ptdata, int looplevel, ht *matchht)
{
    int i;

    if (looplevel != -1) return TRUE;
    if (matchht) {
        if (gethtitem(matchht, ptdata) != EMPTYHT) return FALSE;
        else return TRUE;
    }
    for (i = 0; i < niveau_last; i++) {
        if (ptm->tab[i] == ptdata) return FALSE;
    }
    return TRUE;
}

static int
check_not_loopsig(matrice_list *ptm, losig_list *ptmodelsig, losig_list *ptsig, int looplevel)
{
    int i;

    if (looplevel != -1) return TRUE;
    if (ptmodelsig->TYPE != INTERNAL) return TRUE;
    for (i = 0; i < niveau_last; i++) {
        if (ptm->tab[i] == ptsig) return FALSE;
    }
    return TRUE;
}

static int
same_supply(losig_list *ptsig, losig_list *ptmodelsig)
{
    if (ptsig == NULL || ptmodelsig == NULL) return FALSE;
    if (ptsig->TYPE == CNS_SIGVDD && ptmodelsig->TYPE == CNS_SIGVDD) return TRUE;
    if (ptsig->TYPE == CNS_SIGVSS && ptmodelsig->TYPE == CNS_SIGVSS) return TRUE;
    return FALSE;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                          fclFindCorrespondingLosig                     ##
  ##                                                                        ##
  ####====================================================================####
  ## Cherche des signaux ayant des poids de connecteur donnee pour chacune  ##
  ## des solutions.                                                         ##
  ## Ces signaux doivent avoir les memes caracteristiques que le signal du  ##
  ## sous-graphe : rebouclage ou pas, alimentation ou pas                   ##
  ####====================================================================####
*/
int
fclFindCorrespondingLosig(matrice_list *ptmatrice, int niveau, int connecteur, fcl_label prevcon, int precniveau, int loop, int niveau_donedrainsource)
{
    matrice_list   *ptm;
    lotrs_list     *ptlotrs;
    loins_list     *ptloins;
    losig_list     *ptlosig;
    losig_list     *ptlosig_model;
    locon_list     *ptlocon;
    losig_list     *done_drainsource;
    losig_list     *from_drainsource;
    int             first;
    fcl_label       model_label;

    ptlosig_model = (losig_list *) (ptmatrice->tab)[niveau_last];
    model_label = getlabelsig(ptlosig_model);

    if (connecteur == POID_GRID) {
        /* On cherche des signaux sur les GRID */

        for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
            /* On parcourt tous les signaux de la GRILLE du dernier */
            /* transistor de chaque solution                       */
            ptlotrs = (lotrs_list *) ((ptm->tab)[niveau]);
            ptlosig = ptlotrs->GRID->SIG;

            if (same_supply(ptlosig, ptlosig_model) 
            || (identique_sig(ptlosig, ptlosig_model, model_label) && check_loop(ptm, ptlosig, loop) && check_not_loopsig(ptm, ptlosig_model, ptlosig, loop))) {
                tab_ajoute(ptm, niveau_last, ptlosig);
            }
            else {
                /* Il n'existe pas de solution dans la ligne courrante */
                ptm = tab_del(ptmatrice->NEXT, ptm);
                if (ptmatrice->NEXT->NEXT == NULL) return 0;
            }
        }
    }
    else if (connecteur == POID_BULK) {
        /* On cherche des signaux sur les BULK */

        for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
            /* On parcourt tous les signaux de la BULK du dernier */
            /* transistor de chaque solution                       */
            ptlotrs = (lotrs_list *) ((ptm->tab)[niveau]);
            ptlosig = ptlotrs->BULK->SIG;

            if (same_supply(ptlosig, ptlosig_model) 
            || (identique_sig(ptlosig, ptlosig_model, model_label) && check_loop(ptm, ptlosig, loop) && check_not_loopsig(ptm, ptlosig_model, ptlosig, loop))) {
                tab_ajoute(ptm, niveau_last, (void *)ptlosig);
            }
            else {
                /* Il n'existe pas de solution dans la ligne courrante */
                ptm = tab_del(ptmatrice->NEXT, ptm);
                if (ptmatrice->NEXT->NEXT == NULL) return 0;
            }
        }
    }
    else if (connecteur == POID_SOURCE || connecteur == POID_DRAIN) {
        /* On cherche un signal sur le SOURCE ou sur le DRAIN      */
        /* comme ces deux connecteurs ne peuvent etre differencies */
        /* Les recherches doivent avoir lieu sur les deux          */
        /* connecteurs                                             */

        for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
            done_drainsource = NULL;
            from_drainsource = NULL;
            first = TRUE;

            ptlotrs = (lotrs_list *) ((ptm->tab)[niveau]);
            if (niveau_donedrainsource != -1) {
                done_drainsource = (losig_list *) ((ptm->tab)[niveau_donedrainsource]);
            }
            if (prevcon == POID_SOURCE || prevcon == POID_DRAIN) {
                from_drainsource = (losig_list *) ((ptm->tab)[precniveau]);
            }

            ptlosig = ptlotrs->SOURCE->SIG;
            if ((ptlosig != done_drainsource && ptlosig != from_drainsource) || ptlosig == ptlotrs->DRAIN->SIG) {
                if (same_supply(ptlosig, ptlosig_model) 
                || (identique_sig(ptlosig, ptlosig_model, model_label) && check_loop(ptm, ptlosig, loop) && check_not_loopsig(ptm, ptlosig_model, ptlosig, loop))) {
                    tab_ajoute(ptm, niveau_last, (void *)ptlosig);
                    first = FALSE;
                }
            }

            ptlosig = ptlotrs->DRAIN->SIG;
            if (ptlosig != done_drainsource && ptlosig != from_drainsource && ptlosig != ptlotrs->SOURCE->SIG) {
                if (same_supply(ptlosig, ptlosig_model) 
                || (identique_sig(ptlosig, ptlosig_model, model_label) && check_loop(ptm, ptlosig, loop) && check_not_loopsig(ptm, ptlosig_model, ptlosig, loop))) {
                    if (first == FALSE) {
                        /* Il y a deux solutions DRAIN + SOURCE        */
                        tab_ajoute(tab_copy (ptmatrice->NEXT, ptm, niveau_last), niveau_last, ptlosig);
                    }
                    else {
                        tab_ajoute(ptm, niveau_last, ptlosig);
                        first = FALSE;
                    }
                }
            }
            if (first == TRUE) {
                /* Il n'existe pas de solution dans la ligne courrante */
                ptm = tab_del(ptmatrice->NEXT, ptm);
                if (ptmatrice->NEXT->NEXT == NULL) return 0;
            }
        }
    }
    else if (connecteur >= POID_INS) {
        for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
            first = TRUE;

            ptloins = (loins_list *) ((ptm->tab)[niveau]);
            /* For each connector with the given weight */
            for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
                if (getlabellocon(ptlocon) != connecteur) continue;
                ptlosig = ptlocon->SIG;
                if (same_supply(ptlosig, ptlosig_model) 
                || (identique_sig(ptlosig, ptlosig_model, model_label) && check_loop(ptm, ptlosig, loop) && check_not_loopsig(ptm, ptlosig_model, ptlosig, loop))) {
                    if (first == FALSE) {
                        tab_ajoute(tab_copy (ptmatrice->NEXT, ptm, niveau_last), niveau_last, (void *)ptlosig);
                    }
                    else {
                        tab_ajoute(ptm, niveau_last, (void *)(ptlocon->SIG));
                        first = FALSE;
                    }
                }
            }
            if (first == TRUE) {
                /* Il n'existe pas de solution dans la ligne courrante */
                ptm = tab_del(ptmatrice->NEXT, ptm);
                if (ptmatrice->NEXT->NEXT == NULL) return 0;
            }
        }
    }

    return 1;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              IDENTIQUE_SIG                             ##
  ##                                                                        ##
  ####====================================================================####
*/
static int
identique_sig(losig_list *ptsig, losig_list *ptmodelsig, fcl_label model_label)
{
    char           *refname;
    chain_list     *ptchain;
    ptype_list     *ptuser;
    fcl_label       label;

    if (ptsig == NULL || ptmodelsig == NULL) return FALSE;
    if (ptsig->TYPE == CNS_SIGVDD && ptmodelsig->TYPE == CNS_SIGVDD) return TRUE;
    if (ptsig->TYPE == CNS_SIGVSS && ptmodelsig->TYPE == CNS_SIGVSS) return TRUE;
    if ((ptmodelsig->TYPE != ptsig->TYPE) && (ptmodelsig->TYPE != EXTERNAL)) return FALSE;

    label = getlabelsig(ptsig);
    if (!((model_label == label && ptmodelsig->TYPE == INTERNAL) || (model_label <= label && ptmodelsig->TYPE == EXTERNAL))) return FALSE;

    if (FCL_REAL_CORRESP_HT != NULL) {
        if ((refname = (char *)gethtitem(FCL_REAL_CORRESP_HT, ptmodelsig->NAMECHAIN->DATA)) != (char *)EMPTYHT) {
            for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
                if (refname == (char *) ptchain->DATA) return TRUE;
            }
            return FALSE;
        }
    }   
    
    if ((ptuser = getptype(ptmodelsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        if (((long) ptuser->DATA & FCL_MATCHNAME) != FCL_MATCHNAME) return TRUE;
    }
    else return TRUE;

    refname = ptmodelsig->NAMECHAIN->DATA;
    for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
        if (refname == (char *) ptchain->DATA) return TRUE;
    }
    return FALSE;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                         fclFindCorrespondingLotrs                      ##
  ##                                                                        ##
  ####====================================================================####
  ## Cherche un transistor avec un poid de connecteur donnee pour chaque    ##
  ## solution retenue                                                       ##
  ####====================================================================####
*/
int
fclFindCorrespondingLotrs(matrice_list *ptmatrice, int niveau, int connecteur, int loop)
{
    losig_list     *ptlosig;
    locon_list     *ptlocon;
    lotrs_list     *pttrans, *ptmodeltrans;
    chain_list     *ptchain;
    chain_list     *ptc;
    matrice_list   *ptm, *ptnextm;
    char            first;

    ptmodeltrans = (lotrs_list *) (ptmatrice->tab)[niveau_last];

    for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptnextm) {
        ptnextm = ptm->NEXT;
        ptlosig = (losig_list *) ((ptm->tab)[niveau]);

        ptchain = getptype(ptlosig->USER, (long) LOFIGCHAIN)->DATA;

        first = TRUE;

        for (ptc = ptchain; ptc; ptc = ptc->NEXT) {
            ptlocon = (locon_list *) (ptc->DATA);
            if (ptlocon->TYPE != 'T') continue;

            /* Chercher des connecteurs identiques */
            if (getlabellocon(ptlocon) == connecteur) {
                pttrans = (lotrs_list *) ptlocon->ROOT;
                /* Le transistor est-il identique */
                if (identique_trs(pttrans, ptmodeltrans) && check_loop(ptm, pttrans, loop) && check_not_loop(ptm, pttrans, loop, NULL)) {
                    if (first == FALSE) {
                        tab_ajoute(tab_copy(ptmatrice->NEXT, ptm, niveau_last), niveau_last, ptlocon->ROOT);
                    }
                    else tab_ajoute(ptm, niveau_last, ptlocon->ROOT);
                    first = FALSE;
                }
            }
        }

        if (first == TRUE) {
            /* Il n'existe pas de solution dans la ligne courrante */
            ptm = tab_del(ptmatrice->NEXT, ptm);
            if (ptmatrice->NEXT->NEXT == NULL) return 0;
        }
    }

    return 1;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              IDENTIQUE_TRS                             ##
  ##                                                                        ##
  ####====================================================================####
*/
static int
identique_trs(lotrs_list *pttrans, lotrs_list *ptmodeltrans)
{
    ptype_list     *ptuser;
    long            mark = 0;
    int             usegrid = FALSE;
    int             usebulk = FALSE;
    int             usesource = FALSE;
    int             usedrain = FALSE;
    int             vddmatch = 0, vssmatch = 0;
    int             source, drain;
    int             modelsource, modeldrain;

    ptuser = getptype(pttrans->USER, FCL_MARK_PTYPE);
    if (ptuser != NULL)
        mark = (long) ptuser->DATA;
    if ((mark & FCL_NOSHARE) == FCL_NOSHARE) {
        return FALSE;
    }

    if ((pttrans->TYPE & (TRANSN | TRANSP)) !=
        (ptmodeltrans->TYPE & (TRANSN | TRANSP))) return FALSE;
    if (ptmodeltrans->MODINDEX != (short) EMPTYHT
        && getnum(FCL_ANY_NMOS_IDX, (long)ptmodeltrans->MODINDEX) == NULL
        && getnum(FCL_ANY_PMOS_IDX, (long)ptmodeltrans->MODINDEX) == NULL
        && pttrans->MODINDEX != ptmodeltrans->MODINDEX) return FALSE;

    if ((ptuser = getptype(ptmodeltrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        if (((long) ptuser->DATA & FCL_MATCHSIZE) != 0) {
            if (FCL_SIZE_TOLERANCE == 0) {
                if (pttrans->WIDTH != ptmodeltrans->WIDTH
                    || pttrans->LENGTH != pttrans->LENGTH) {
                    return FALSE;
                }
            }
            else {
                if (abs(pttrans->WIDTH - ptmodeltrans->WIDTH) <
                    ((FCL_SIZE_TOLERANCE * ptmodeltrans->WIDTH) / 100)
                    || abs(pttrans->LENGTH - ptmodeltrans->LENGTH) <
                    ((FCL_SIZE_TOLERANCE * ptmodeltrans->LENGTH) / 100)) {
                    return FALSE;
                }
            }
        }
    }

    if (ptmodeltrans->GRID->SIG->TYPE == 'I')
        usegrid = TRUE;
    else {
        if (ptmodeltrans->GRID->SIG->TYPE == CNS_SIGVSS
            && pttrans->GRID->SIG->TYPE != CNS_SIGVSS)
            return FALSE;
        if (ptmodeltrans->GRID->SIG->TYPE == CNS_SIGVDD
            && pttrans->GRID->SIG->TYPE != CNS_SIGVDD)
            return FALSE;
    }

    if (ptmodeltrans->BULK && pttrans->BULK && ptmodeltrans->BULK->SIG
        && pttrans->BULK->SIG) {
        if (ptmodeltrans->BULK->SIG->TYPE == 'I')
            usebulk = TRUE;
        else {
            if (ptmodeltrans->BULK->SIG->TYPE == CNS_SIGVSS
                && pttrans->BULK->SIG->TYPE != CNS_SIGVSS)
                return FALSE;
            if (ptmodeltrans->BULK->SIG->TYPE == CNS_SIGVDD
                && pttrans->BULK->SIG->TYPE != CNS_SIGVDD)
                return FALSE;
        }
    }

    source = getlabelsig(pttrans->SOURCE->SIG);
    modelsource = getlabelsig(ptmodeltrans->SOURCE->SIG);
    if (ptmodeltrans->SOURCE->SIG->TYPE == 'I')
        usesource = TRUE;
    else if (ptmodeltrans->SOURCE->SIG->TYPE == CNS_SIGVDD)
        vddmatch++;
    else if (ptmodeltrans->SOURCE->SIG->TYPE == CNS_SIGVSS)
        vssmatch++;

    drain = getlabelsig(pttrans->DRAIN->SIG);
    modeldrain = getlabelsig(ptmodeltrans->DRAIN->SIG);
    if (ptmodeltrans->DRAIN->SIG->TYPE == 'I')
        usedrain = TRUE;
    else if (ptmodeltrans->DRAIN->SIG->TYPE == CNS_SIGVDD)
        vddmatch++;
    else if (ptmodeltrans->DRAIN->SIG->TYPE == CNS_SIGVSS)
        vssmatch++;

    if (vddmatch != 0) {
        if (vddmatch !=
            (pttrans->DRAIN->SIG->TYPE ==
             CNS_SIGVDD) + (pttrans->SOURCE->SIG->TYPE == CNS_SIGVDD))
            return FALSE;
    }
    if (vssmatch != 0) {
        if (vssmatch !=
            (pttrans->DRAIN->SIG->TYPE ==
             CNS_SIGVSS) + (pttrans->SOURCE->SIG->TYPE == CNS_SIGVSS))
            return FALSE;
    }

    if (usegrid) {
        if (getlabelsig(ptmodeltrans->GRID->SIG) !=
            getlabelsig(pttrans->GRID->SIG)) return FALSE;
    }

    if (usebulk) {
        if (getlabelsig(ptmodeltrans->BULK->SIG) !=
            getlabelsig(pttrans->BULK->SIG)) return FALSE;
    }

    if (usesource && usedrain) {
        if ((source == modelsource && drain == modeldrain)
            || (source == modeldrain && drain == modelsource))
            return TRUE;
    }
    else if (usesource) {
        if (modelsource == source || modelsource == drain)
            return TRUE;
    }
    else if (usedrain) {
        if (modeldrain == drain || modeldrain == source)
            return TRUE;
    }
    else
        return TRUE;

    return FALSE;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                          fclFindCorrespondingLoins                     ##
  ##                                                                        ##
  ####====================================================================####
  ## Cherche une instance avec un poid de connecteur donnee pour chaque     ##
  ## solution retenue                                                       ##
  ####====================================================================####
*/
int
fclFindCorrespondingLoins(matrice_list *ptmatrice, int niveau, int connecteur, int loop)
{
    losig_list     *ptlosig;
    locon_list     *ptlocon;
    loins_list     *ptloins, *ptmodelloins;
    chain_list     *ptchain;
    chain_list     *ptc;
    matrice_list   *ptm, *ptnextm;
    ht             *matchht = NULL;
    int             useht;
    int             i;
    char            first;

    ptmodelloins = (loins_list *)(ptmatrice->tab)[niveau_last];

    for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptnextm) {
        ptnextm = ptm->NEXT;
        ptlosig = (losig_list *) ((ptm->tab)[niveau]);
        ptchain = getptype(ptlosig->USER, (long) LOFIGCHAIN)->DATA;
        if (getlabelsig(ptlosig) > 10 && niveau_last > 10) useht = TRUE;
        else useht = FALSE;
        first = TRUE;

        if (useht) {
            matchht = addht(niveau_last*5);
            for (i = 0; i < niveau_last; i++) addhtitem(matchht, ptm->tab[i], 0);
        }
            
        for (ptc = ptchain; ptc; ptc = ptc->NEXT) {
            ptlocon = (locon_list *) (ptc->DATA);
            if (ptlocon->TYPE != 'I') continue;

            /* Chercher des connecteurs identiques */
            if (getlabellocon(ptlocon) == connecteur) {
                ptloins = (loins_list *) ptlocon->ROOT;
                /* Instance est-il identique */
                if (identique_ins(ptloins, ptmodelloins) && check_loop(ptm, ptloins, loop) && check_not_loop(ptm, ptloins, loop, matchht)) {
                    if (first == FALSE) {
                        tab_ajoute(tab_copy(ptmatrice->NEXT, ptm, niveau_last), niveau_last, ptlocon->ROOT);
                    }
                    else tab_ajoute(ptm, niveau_last, ptlocon->ROOT);
                    first = FALSE;
                }
            }
        }
        if (useht) {
            delht(matchht);
            matchht = NULL;
        }

        if (first == TRUE) {
            /* Il n'existe pas de solution dans la ligne courrante */
            ptm = tab_del(ptmatrice->NEXT, ptm);
            if (ptmatrice->NEXT->NEXT == NULL) return 0;
        }
    }

    return 1;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              IDENTIQUE_INS                             ##
  ##                                                                        ##
  ####====================================================================####
*/
static int
identique_ins(loins_list *ptloins, loins_list *ptmodelloins)
{
    char *refname;

    if (FCL_REAL_CORRESP_HT != NULL) {
        if ((refname = (char *)gethtitem(FCL_REAL_CORRESP_HT, ptmodelloins->INSNAME)) != (char *)EMPTYHT) {
            if (refname == ptloins->INSNAME) return TRUE;
            else return FALSE;
        }
    }   
    
    if (ptloins->FIGNAME == ptmodelloins->FIGNAME) return 1;
    return 0;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              TAB_AJOUTE                                ##
  ##                                                                        ##
  ####====================================================================####
  ## Ajoute un element dans le tableau indique                              ##
  ####====================================================================####
*/

void
tab_ajoute(ptmatrice, niveau, ptdata)
    matrice_list   *ptmatrice;
    int             niveau;
    void           *ptdata;
{
    if (niveau % BUFFER == 0) {
        /* On est en fin de tableau on realloue de la place */
        ptmatrice->tab = mbkrealloc(ptmatrice->tab, (niveau + BUFFER) * sizeof(void *));
    }

    (ptmatrice->tab)[niveau] = ptdata;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              TAB_DEL                                   ##
  ##                                                                        ##
  ####====================================================================####
  ## Supprime une ligne de la matrice                                       ##
  ####====================================================================####
*/
matrice_list   *
tab_del(ptmatrice_begin, ptmatrice)
    matrice_list   *ptmatrice_begin;
    matrice_list   *ptmatrice;
{
    matrice_list   *ptm;
#ifndef __ALL__WARNING__
    ptmatrice_begin	= NULL;
#endif
    /* never delete the first two rows */
    ptmatrice->PREV->NEXT = ptmatrice->NEXT;
    if (ptmatrice->NEXT) ptmatrice->NEXT->PREV = ptmatrice->PREV;
    ptm = ptmatrice->PREV;
    mbkfree(ptmatrice->tab);
    mbkfree(ptmatrice);

    return ptm;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              TAB_INIT                                  ##
  ##                                                                        ##
  ####====================================================================####
  ## Initialise la matrice                                                  ##
  ####====================================================================####
*/
matrice_list   *
tab_init()
{
    matrice_list   *ptmatrice;

    ptmatrice = (matrice_list *) mbkalloc(sizeof(matrice_list));
    ptmatrice->NEXT = (matrice_list *) mbkalloc(sizeof(matrice_list));
    ptmatrice->PREV = NULL;
    ptmatrice->NEXT->NEXT = (matrice_list *) mbkalloc(sizeof(matrice_list));
    ptmatrice->NEXT->PREV = ptmatrice;
    ptmatrice->NEXT->NEXT->NEXT = (matrice_list *) NULL;
    ptmatrice->NEXT->NEXT->PREV = ptmatrice->NEXT;

    ptmatrice->tab = mbkalloc(sizeof(void *) * BUFFER);
    ptmatrice->NEXT->tab = mbkalloc(BUFFER * sizeof(void *));
    ptmatrice->NEXT->NEXT->tab = mbkalloc(BUFFER * sizeof(void *));

    return ptmatrice;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              TAB_COPY                                  ##
  ##                                                                        ##
  ####====================================================================####
  ## Recopie une ligne de la matrice sur une nouvelle ligne                 ##
  ####====================================================================####
*/
matrice_list *
tab_copy(ptmatrice_begin, ptmatrice, niveau)
    matrice_list   *ptmatrice_begin;
    matrice_list   *ptmatrice;
    int             niveau;
{
    matrice_list   *ptmatrice_new;

#ifndef __ALL__WARNING__
    ptmatrice_begin	= NULL;
#endif
    ptmatrice_new = (matrice_list *) mbkalloc(sizeof(matrice_list));
    ptmatrice_new->NEXT = ptmatrice;
    ptmatrice_new->PREV = ptmatrice->PREV;
    ptmatrice_new->tab = mbkalloc(((niveau / BUFFER) + 1) * BUFFER * sizeof(void *));
    memcpy(ptmatrice_new->tab, ptmatrice->tab, ((niveau / BUFFER) + 1) * BUFFER * sizeof(void *));

    ptmatrice->PREV->NEXT = ptmatrice_new;
    ptmatrice->PREV = ptmatrice_new;

    return ptmatrice_new;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              TAB_FREE                                  ##
  ##                                                                        ##
  ####====================================================================####
  ## Libere la matrice                                                      ##
  ####====================================================================####
*/
void
tab_free(ptmatrice)
    matrice_list   *ptmatrice;
{
    matrice_list   *ptm;

    for (ptm = ptmatrice->NEXT; ptm; ptm = ptm->NEXT) {
        mbkfree(ptmatrice->tab);
        mbkfree(ptmatrice);
        ptmatrice = ptm;
    }

    mbkfree(ptmatrice->tab);
    mbkfree(ptmatrice);
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              FILS_SIG                                  ##
  ##                                                                        ##
  ####====================================================================####
  ## Cherche les transistors et instances d'un signal                       ##
  ####====================================================================####
*/
chain_list *
fils_sig(ptlosig, ptlocon)
    losig_list     *ptlosig;
    locon_list     *ptlocon;
{
    chain_list     *ptchain;
    chain_list     *ptchain_debut = NULL;
    ptype_list     *ptptype;
    lotrs_list     *ptlotrs;
    loins_list     *ptloins;
    locon_list     *ptnextlocon;

    ptptype = getptype(ptlosig->USER, LOFIGCHAIN);

    for (ptchain = ptptype->DATA; ptchain; ptchain = ptchain->NEXT) {
        ptnextlocon = (locon_list *)ptchain->DATA;
        if (ptnextlocon == ptlocon) continue;
        if (ptnextlocon->TYPE == 'T') {
            ptlotrs = (lotrs_list *)ptnextlocon->ROOT;

            /* Le transistor est il MATCH */
            if (getptype(ptlotrs->USER, FCL_MATCH_PTYPE) == NULL) {
                ptchain_debut = addchain(ptchain_debut, ptnextlocon);
            }
        }
        if (ptnextlocon->TYPE == 'I') {
            ptloins = (loins_list *)ptnextlocon->ROOT;

            /* L'instance est il MATCH */
            if (getptype(ptloins->USER, FCL_MATCH_PTYPE) == NULL) {
                ptchain_debut = addchain(ptchain_debut, ptnextlocon);
            }
        }
    }

    for (ptchain = ptptype->DATA; ptchain; ptchain = ptchain->NEXT) {
        ptnextlocon = (locon_list *)ptchain->DATA;
        if (ptnextlocon == ptlocon) continue;
        if (ptnextlocon->TYPE == 'T') {
            ptlotrs = (lotrs_list *)ptnextlocon->ROOT;

            /* Le transistor est il MATCH */
            if (getptype(ptlotrs->USER, FCL_MATCH_PTYPE) != NULL) {
                ptchain_debut = addchain(ptchain_debut, ptnextlocon);
            }
        }
        if (ptnextlocon->TYPE == 'I') {
            ptloins = (loins_list *)ptnextlocon->ROOT;

            /* L'instance est il MATCH */
            if (getptype(ptloins->USER, FCL_MATCH_PTYPE) != NULL) {
                ptchain_debut = addchain(ptchain_debut, ptnextlocon);
            }
        }
    }

    return ptchain_debut;

}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              PRINT_MATRICE                             ##
  ##                                                                        ##
  ####====================================================================####
*/
void
printmatrice(ptmatrice, niveau)
    matrice_list   *ptmatrice;
    int             niveau;
{
    int             i;
    matrice_list   *ptm;

    printf("matrice:\n");
    for (ptm = ptmatrice; ptm; ptm = ptm->NEXT) {
        for (i = 0; i <= niveau; i++)
            printf("%6x ", (int)(long)(ptm->tab)[i]);
        printf("\n");
    }

}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                              DISPLAY_MATRICE                           ##
  ##                                                                        ##
  ####====================================================================####
*/
void
displaymatrice(ptmatrice, niveau, full)
    matrice_list   *ptmatrice;
    int             niveau;
    int             full;
{
    int             i, num = 0;
    losig_list     *ptlosig;
    lotrs_list     *pttrans;
    loins_list     *ptloins;
    ptype_list     *ptuser;
    num_list       *ptnum;
    matrice_list   *ptm;

    printf("matrice:\n\n");
    printf("Model :-\n");
    for (i = 0; i <= niveau; i++) {
        if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_S) {
            ptlosig = (losig_list *) (ptmatrice->tab)[i];
            if (ptlosig->NAMECHAIN == NULL) {
                printf("\tmbk_sig%ld", ptlosig->INDEX);
            }
            else printf("\t%s", (char *) ptlosig->NAMECHAIN->DATA);
            if ((ptuser = getptype(ptlosig->USER, FCL_COUP_LIST_PTYPE)) != NULL) {
                printf("\t coupling list ( ");
                for (ptnum = (num_list *)ptuser->DATA; ptnum; ptnum = ptnum->NEXT) {
                    printf("%ld ", ptnum->DATA);
                }
                printf(")");
            }
            printf("\n");
        }
        else if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_I) {
            ptloins = (loins_list *) (ptmatrice->tab)[i];
            printf("\t%s\n", ptloins->INSNAME);
        }
        else {
            pttrans = (lotrs_list *) (ptmatrice->tab)[i];
            printf("\t%s\n", pttrans->TRNAME);
        }
    }
    printf("\n");

    if (full) {
        for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
            num++;
            printf("Solution number %d :-\n", num);
            for (i = 0; i <= niveau; i++) {
                if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_S) {
                    ptlosig = (losig_list *) (ptm->tab)[i];
                    printf("\t%s", (char *) ptlosig->NAMECHAIN->DATA);
                    if ((ptuser = getptype(ptlosig->USER, FCL_COUP_LIST_PTYPE)) != NULL) {
                        printf("\t coupling list ( ");
                        for (ptnum = (num_list *)ptuser->DATA; ptnum; ptnum = ptnum->NEXT) {
                            printf("%ld ", ptnum->DATA);
                        }
                        printf(")");
                    }
                    printf("\n");
                }
                else if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_I) {
                    ptloins = (loins_list *) (ptm->tab)[i];
                    printf("\t%s\n", (char *) ptloins->INSNAME);
                }
                else {
                    pttrans = (lotrs_list *) (ptm->tab)[i];
                    if (pttrans->TRNAME != NULL) {
                        printf("\t%s\n", pttrans->TRNAME);
                    }
                    else {
                        printf("\tD-%ld,G-%ld,S-%ld\n",
                               pttrans->DRAIN->SIG->INDEX,
                               pttrans->GRID->SIG->INDEX,
                               pttrans->SOURCE->SIG->INDEX);
                    }
                }
            }
            printf("\n");
        }
    }
}
