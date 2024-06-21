/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_phase2.c                                                */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

/*
   ####====================================================================####
   ##                                                                        ##
   ##                         EN TETE DES FONCTIONS                          ##
   ##                                                                        ##
   ####====================================================================####
 */
static void            init_phase2();       /* init                           */
static void            clean_phase2();      /* netoie                           */
static int             parcours_sig();      /* parcours les S connectes a un T  */
static int             parcours_trs_ins();  /* parcours les T connectes a un S  */
static int             test_non_match();    /* cherche les branches perdues     */
static void            clear();             /* supprime les match de la phase 2 */
//static int             rebouclage();        /* supprime les solutions bouclees  */
static int             check_couplings();   /* remove broken couplings          */
static num_list       *get_numlist_intersect();
/*
   ####====================================================================####
   ##                                                                        ##
   ##                             VARIABLES GLOBALES                         ##
   ##                                                                        ##
   ####====================================================================####
 */

matrice_list   *ptmatrice = (matrice_list *) NULL; /* matrice des solutions   */

int             niveau_last = 0;        /* numero de la derniere colonne dans */
                                        /* la matrice                         */

cell_list      *FCL_CELL_LIST = NULL;
chain_list     *FCL_INSTANCE_LIST = NULL;
locon_list     *FCL_LOCON_LIST = NULL;

int             FCL_BUILD_CELLS = FALSE;
int             FCL_BUILD_INSTANCES = FALSE;
int             FCL_BUILD_CORRESP = FALSE;

int             FCL_NEED_ZERO = FALSE;
int             FCL_NEED_ONE = FALSE;

/*
   ####====================================================================####
   ##                                                                        ##
   ##                               phase2()                                 ##
   ##                                                                        ##
   ## Tante de faire un parcours identique dans le model et dans le circuit  ##
   ##                                                                        ##
   ####====================================================================####
   ##  entree : pointeur sur la lofig du circuit                             ##
   ##         : pointeur sur la lofig du model                               ##
   ##         : pointeur sur la liste des vecteurs candidats                 ##
   ##         : pointeur sur le vecteur cle du model                         ##
   ##  sortie :                                                              ##
   ####====================================================================####
 */

void
fclPhase2(ptlofig_c, ptlofig_m, ptbefig_m, ptsolution, ptcle, findall)
    lofig_list     *ptlofig_c;
    lofig_list     *ptlofig_m;
    befig_list     *ptbefig_m;
    partition_list *ptsolution;
    void           *ptcle;
    int             findall;
{
    chain_list     *ptchain;        /* parcours de tous les vecteurs camdidats  */
    lotrs_list     *ptlotrs;        /* vecteur cle type                         */
    losig_list     *ptlosig;        /* vecteur cle type                         */
    loins_list     *ptloins;        /* vecteur cle type                         */
    matrice_list   *ptm;
    int             instance = 1;
    

    /* Effectue les netoyages pour la phase 1 et intialise les labels */
    init_phase2(ptlofig_c);
    init_phase2(ptlofig_m);

    if (findall) {
        /* le vecteur cle est marque MATCH avec le niveau 0 */
        niveau_last = 0;
        if (ptsolution->TYPE == TYPE_T) {
            ptlotrs = (lotrs_list *) ptcle;
            ptlotrs->USER = addptype(ptlotrs->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);
        }
        else if (ptsolution->TYPE == TYPE_S) {
            ptlosig = (losig_list *) (ptcle);
            ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);
        }
        else if (ptsolution->TYPE == TYPE_I) {
            ptloins = (loins_list *) (ptcle);
            ptloins->USER = addptype(ptloins->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);
        }

        /* la matrice est preparee */
        ptmatrice = tab_init();
        (ptmatrice->tab)[0] = ptcle;
        (ptmatrice->NEXT->tab)[0] = (void *) (long)ptsolution->TYPE;

        ptm = ptmatrice;
        for (ptchain = ptsolution->DATA; ptchain; ptchain = ptchain->NEXT) {
            ptm->NEXT = (matrice_list *) mbkalloc(sizeof(matrice_list)); 
            ptm = ptm->NEXT;
            ptm->tab = mbkalloc(BUFFER * sizeof(void *)) ;
            (ptm->tab)[0] = ptchain->DATA;
        }

        /* le parcours peut commencer */ 
        if (ptsolution->TYPE == TYPE_T) {
            if (parcours_sig((locon_list *) NULL, TYPE_T, (int) - 1) && test_non_match(ptlofig_m)) {
                fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, &instance);
            }
            else if (FCL_TRACE_LEVEL > 1) printf("NOT\n");
        }
        else if (ptsolution->TYPE == TYPE_S) {
            if (parcours_trs_ins((locon_list *) NULL) && test_non_match(ptlofig_m)) {
                fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, &instance);
            }
            else if (FCL_TRACE_LEVEL > 1) printf("NOT\n");
        }
        else if (ptsolution->TYPE == TYPE_I) {
            if (parcours_sig((locon_list *) NULL, TYPE_I, (int) - 1) && test_non_match(ptlofig_m)) {
                fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, &instance);
            }
            else if (FCL_TRACE_LEVEL > 1) printf("NOT\n");
        }
        tab_free(ptmatrice);
        clear(ptlofig_m);
    }
    else {
        for (ptchain = ptsolution->DATA; ptchain; ptchain = ptchain->NEXT) {
            /* le vecteur cle est marque MATCH avec le niveau 0 */
            niveau_last = 0;
            if (ptsolution->TYPE == TYPE_T) {
                ptlotrs = (lotrs_list *) ptcle;
                ptlotrs->USER = addptype(ptlotrs->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);
            }
            else if (ptsolution->TYPE == TYPE_S) {
                ptlosig = (losig_list *) (ptcle);
                ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);
            }
            else if (ptsolution->TYPE == TYPE_I) {
                ptloins = (loins_list *) (ptcle);
                ptloins->USER = addptype(ptloins->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);
            }

            /* la matrice est preparee */
            ptmatrice = tab_init();
            (ptmatrice->tab)[0] = ptcle;
            (ptmatrice->NEXT->tab)[0] = (void *) (long)ptsolution->TYPE;
            (ptmatrice->NEXT->NEXT->tab)[0] = ptchain->DATA;

            /* le parcours peut commencer */ 
            if (ptsolution->TYPE == TYPE_T) {
                if (parcours_sig((locon_list *) NULL, TYPE_T, (int) - 1) && test_non_match(ptlofig_m)) {
                    fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, &instance);
                }
                else if (FCL_TRACE_LEVEL > 1) printf("NOT\n");
            }
            else if (ptsolution->TYPE == TYPE_S) {
                if (parcours_trs_ins((locon_list *) NULL) && test_non_match(ptlofig_m)) {
                    fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, &instance);
                }
                else if (FCL_TRACE_LEVEL > 1) printf("NOT\n");
            }
            else if (ptsolution->TYPE == TYPE_I) {
                if (parcours_sig((locon_list *) NULL, TYPE_I, (int) - 1) && test_non_match(ptlofig_m)) {
                    fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, &instance);
                }
                else if (FCL_TRACE_LEVEL > 1) printf("NOT\n");
            }
            tab_free(ptmatrice);
            clear(ptlofig_m);
        }
    }
    if (FCL_FILE && instance > 1) {
        if (FCL_CELL_LIST != NULL) {
            fprintf(FCL_OUTPUT, "Instances of '%s' = %d (Model = %ld)\n\n", ptlofig_m->NAME, instance - 1, FCL_CELL_LIST->TYPE & ~CNS_UNKNOWN);
        }
        else {
            fprintf(FCL_OUTPUT, "Instances of '%s' = %d\n\n", ptlofig_m->NAME, instance - 1);
        }
    }
    clean_phase2(ptlofig_c);
    clean_phase2(ptlofig_m);
}


/*
   ####====================================================================####
   ##                                                                        ##
   ##                             PARCOURS_SIG                               ##
   ##                                                                        ##
   ## Parcours les signaux connectes a un transistor ou instance donne       ## 
   ####====================================================================####
   ## A partir d'un transistor d'un sous-graphe, parcours l'ensemble des     ##
   ## signaux qui y sont conectes. Ce parcours doit etre identique dans      ##
   ## le circuit. Si on cherche a passer par un signal qui a deja ete        ##
   ## parcouru, ce rebouclage doit egalement exister dans le circuit         ##
   ## Plusieurs Solutions peuvent exister dans le circuit. Elles sont alors  ##
   ## memoriser dans une matrice.                                            ##
   ##                                                                        ##
   ####====================================================================####
 */
static int
parcours_sig(ptlocon, sourcetype, precniveau)
    locon_list     *ptlocon;        /* Ptr sur LOCON par lequel on arrive   */
    char            sourcetype;
    int             precniveau;
{
    fcl_label       prevcon;        /* label du connecteur precedent        */
    ptype_list     *ptuser;        /* pointeur sur le ptype MATCH du TRS   */
    lotrs_list     *ptlotrs = NULL;        /* TRS du parcours precedent            */
    loins_list     *ptloins = NULL;        /* INS du parcours precedent            */
    int             niveau = niveau_last;        /* niveau du transistor      */
    int             niveau_donesourcedrain = -1;
    losig_list     *ptlosig;        /* ptr sur signal a parcourir           */
    matrice_list   *ptm;        /* pointeur sur une solution            */
    locon_list     *ptnextlocon;
    int             loop;

    if (FCL_TRACE_LEVEL > 5) {
        int             i;

        printf("parcours sig\n");
        displaymatrice(ptmatrice,niveau_last, FALSE); 
        for (i = 0, ptm = ptmatrice; ptm; i++, ptm = ptm->NEXT);
        printf("n=%d     nb=%d\n", niveau_last, i);
    }

    prevcon = getlabellocon(ptlocon);

    /* On verifie si le vecteur cle courrant n'est pas deja MATCH */
    /* a un niveau inferieur, si c'est le cas les vecteurs des    */
    /* solutions doivent etre MATCH egalement et sur le meme      */
    /* niveau                                                     */

    if (sourcetype == TYPE_T) {
        ptlotrs = (lotrs_list *) ((ptmatrice->tab)[niveau]);
    }
    else if (sourcetype == TYPE_I) {
        ptloins = (loins_list *) ((ptmatrice->tab)[niveau]);
    }

    /* Il faut tester les rebouclages du graphe             */
    /* seules les solutions sans rebouclage doivent exister */
/*    if (!rebouclage()) {
        if (FCL_TRACE_LEVEL > 5) {
            printf("phase 2 match failed: no loop to transistor in model but all solutions have loop\n");
        }
        return 0;
    }*/

    if (sourcetype == TYPE_T) {
        /* *************************** */
        /* Pour le signal sur le DRAIN */
        /* *************************** */
        ptlosig = ptlotrs->DRAIN->SIG;
        if (ptlotrs->DRAIN != ptlocon) {
            niveau_last++;

            /* Le signal est marque MATCH, s'il ne l'est pas deja */
            if ((ptuser = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) == NULL) {
                ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                loop = -1;
            }
            else loop = (int)(long)ptuser->DATA;

            /* Le signal est ajoute sur le premier tableau */
            tab_ajoute(ptmatrice, niveau_last, (void *)ptlosig);
            tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_S);

            /* On cherche les signaux identiques dans le graphe */
            if (!fclFindCorrespondingLosig(ptmatrice, niveau, POID_DRAIN, prevcon, precniveau, loop, -1)) return 0;
            /* on part sur le signal trouve */
            niveau_donesourcedrain = niveau_last;
            if (loop >= 0) niveau_last--;
            else if (!parcours_trs_ins(ptlotrs->DRAIN)) return 0;
        }

        /* **************************** */
        /* Pour le signal sur le SOURCE */
        /* **************************** */
        ptlosig = ptlotrs->SOURCE->SIG;
        if (ptlotrs->SOURCE != ptlocon) {
            niveau_last++;

            /* Le signal est marque MATCH, s'il ne l'est pas deja */
            if ((ptuser = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) == NULL) {
                ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                loop = -1;
            }
            else loop = (int)(long)ptuser->DATA;

            /* Le signal est ajoute sur le premier tableau */
            tab_ajoute(ptmatrice, niveau_last, (void *)ptlosig);
            tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_S);

            /* On cherche les signaux identiques */
            if (!fclFindCorrespondingLosig(ptmatrice, niveau, POID_SOURCE, prevcon, precniveau, loop, niveau_donesourcedrain)) return 0;
            /* on part sur le signal trouve */
            if (loop >= 0) niveau_last--;
            else if (!parcours_trs_ins(ptlotrs->SOURCE)) return 0;
        }

        /* ************************** */
        /* Pour le signal sur le GRID */
        /* ************************** */
        ptlosig = ptlotrs->GRID->SIG;
        if (ptlotrs->GRID != ptlocon) {
            niveau_last++;

            /* Le signal est marque MATCH, s'il ne l'est pas deja */
            if ((ptuser = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) == NULL) {
                ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                loop = -1;
            }
            else loop = (int)(long)ptuser->DATA;

            /* Le signal est ajoute sur le premier tableau */
            tab_ajoute(ptmatrice, niveau_last, (void *)ptlosig);
            tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long) TYPE_S);

            /* On cherche les signaux identiques */
            if (!fclFindCorrespondingLosig(ptmatrice, niveau, POID_GRID, prevcon, precniveau, loop, -1)) return 0;
            /* on part sur le signal trouve */
            if (loop >= 0) niveau_last--;
            else if (!parcours_trs_ins(ptlotrs->GRID)) return 0;
        }

        /* ************************** */
        /* Pour le signal sur le BULK */
        /* ************************** */
        if (ptlotrs->BULK) ptlosig = ptlotrs->BULK->SIG;
        if (ptlotrs->BULK != ptlocon && ptlotrs->BULK && ptlosig && SPI_IGNORE_BULK == 'N') {
            niveau_last++;

            /* Le signal est marque MATCH, s'il ne l'est pas deja */
            if ((ptuser = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) == NULL) {
                ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                loop = -1;
            }
            else loop = (int)(long)ptuser->DATA;

            /* Le signal est ajoute sur le premier tableau */
            tab_ajoute(ptmatrice, niveau_last, (void *) ptlosig);
            tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_S);

            /* On cherche les signaux identiques */
            if (!fclFindCorrespondingLosig(ptmatrice, niveau, POID_BULK, prevcon, precniveau, loop, -1)) return 0;
            /* on part sur le signal trouve */
            if (loop >= 0) niveau_last--;
            else if (!parcours_trs_ins(ptlotrs->BULK)) return 0;
        }
    }
    else if (sourcetype == TYPE_I) {
        for (ptnextlocon = ptloins->LOCON; ptnextlocon; ptnextlocon = ptnextlocon->NEXT) {
            if (ptnextlocon != ptlocon) {
                ptlosig = ptnextlocon->SIG;
                niveau_last++;

                /* Le signal est marque MATCH, s'il ne l'est pas deja */
                if ((ptuser = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                    loop = -1;
                }
                else loop = (int)(long)ptuser->DATA;

                /* Le signal est ajoute sur le premier tableau */
                tab_ajoute(ptmatrice, niveau_last, (void *) ptlosig);
                tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_S);

                /* On cherche les signaux identiques */
                if (!fclFindCorrespondingLosig(ptmatrice, niveau, getlabellocon(ptnextlocon), prevcon, precniveau, loop, -1)) return 0;
                /* on part sur le signal trouve */
                if (loop >= 0) niveau_last--;
                else if (!parcours_trs_ins(ptnextlocon)) return 0;
            }
        }
    }

    return 1;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                             PARCOURS_TRS_INS                           ##
   ##                                                                        ##
   ## Parcours les transistors ou instances connectes a un signal donne      ## 
   ####====================================================================####
   ## A partir d'un signal d'un sous-graphe, parcours l'ensemble des         ##
   ## transistors qui y sont conectes. Ce parcours doit etre identique dans  ##
   ## le circuit. Si on cherche a passer par un transistor qui a deja ete    ##
   ## parcouru, ce rebouclage doit egalement exister dans le circuit         ##
   ## Plusieurs Solutions peuvent exister dans le circuit. Elles sont alors  ##
   ## memoriser dans une matrice.                                            ##
   ##                                                                        ##
   ####====================================================================####
 */
static int
parcours_trs_ins(ptlocon)
    locon_list     *ptlocon;
{
    fcl_label       label;        /* label du connecteur             */
    chain_list     *ptfils;        /* Liste des fils du Vect courrant */
    chain_list     *ptfils_courrant;        /* Pointeur sur le fils courrant   */
    ptype_list     *ptuser;        /* ptype MATCH du vecteur courrant */
    losig_list     *ptlosig;        /* ptr sur signal precedent        */
    lotrs_list     *ptlotrs;        /* ptr sur trs a parcourir         */
    loins_list     *ptloins;        /* ptr sur instance a parcourir         */
    int             niveau = niveau_last;        /* niveau courrant               */
    matrice_list   *ptm;        /* pointeur sur une solution     */
    locon_list     *ptnextlocon;
    int             loop;

    if (FCL_TRACE_LEVEL > 5) {
        int             i;

        printf("parcours trs\n");
        displaymatrice(ptmatrice,niveau_last, FALSE); 
        for (i = 0, ptm = ptmatrice; ptm; i++, ptm = ptm->NEXT);
        printf("n=%d     nb=%d\n", niveau_last, i);
    }

    ptlosig = (losig_list *) ((ptmatrice->tab)[niveau]);

    /* si le signal cle correspond a une alim il faut que le */
    /* signal de la solution correspond a la meme alim       */

    if (ptlosig->TYPE == CNS_SIGVDD || ptlosig->TYPE == CNS_SIGVSS) {
        for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
            if (((losig_list *) ((ptm->tab)[niveau]))->TYPE != ptlosig->TYPE) {
                /* La solution courrante n'est pas bonne */
                ptm = tab_del(ptmatrice, ptm);
            }
        }
        if (ptmatrice->NEXT->NEXT == NULL) {
            if (FCL_TRACE_LEVEL > 5) {
                printf("phase 2 match failed: power supply in model has no correspondance\n");
            }
            return 0;
        }
        else return 1;
    }

    /* Test that instance connector couplings have not been broken */
    if (!check_couplings()) return 0;

    /* On ne parcours que les signaux INTERNES */
    if (ptlosig->TYPE != INTERNAL) return 1;

    /* Il faut tester les rebouclages du graphe */
/*    if (!rebouclage()) {
        if (FCL_TRACE_LEVEL > 5) {
            printf("phase 2 match failed: no loop to signal in model but all solutions have loop\n");
        }
        return 0;
    }*/

    /* On construit la liste des successeurs du vecteur du */
    /* sous-graphe courrant. Cette liste contient en       */
    /* premier les vecteurs deja MATCH (s'ils existent)    */

    ptfils = fils_sig(ptlosig, ptlocon);

    /* On parcourt la liste en essayant de faire un parcourt */
    /* identique dans le circuit                             */
    for (ptfils_courrant = ptfils; ptfils_courrant; ptfils_courrant = ptfils_courrant->NEXT) {
        niveau_last++;
        ptnextlocon = (locon_list *)ptfils_courrant->DATA;

        if (ptnextlocon->TYPE == 'T') {
            ptlotrs = (lotrs_list *)ptnextlocon->ROOT;

            /* Le transistor est marque MATCH, s'il ne l'est pas deja */
            if ((ptuser = getptype(ptlotrs->USER, FCL_MATCH_PTYPE)) == NULL) {
                ptlotrs->USER = addptype(ptlotrs->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                loop = -1;
            }
            else loop = (int)(long)ptuser->DATA;

            /* Le trs est ajoute sur le premier tableau */
            tab_ajoute(ptmatrice, niveau_last, (void *)ptlotrs);
            tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_T);

            label = getlabellocon(ptnextlocon);

            /* On cherche les transistors identiques */
            if (!fclFindCorrespondingLotrs(ptmatrice, niveau, label, loop)) {
                freechain(ptfils);
                return 0;
            }
            if (loop >= 0) niveau_last--;
            else if (!parcours_sig(ptnextlocon, TYPE_T, niveau)) {
                freechain(ptfils);
                return 0;
            }
        }
        else if (ptnextlocon->TYPE == 'I') {
            ptloins = (loins_list *)ptnextlocon->ROOT;

            /* Instance est marque MATCH, s'il ne l'est pas deja */
            if ((ptuser = getptype(ptloins->USER, FCL_MATCH_PTYPE)) == NULL) {
                ptloins->USER = addptype(ptloins->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);
                loop = -1;
            }
            else loop = (int)(long)ptuser->DATA;

            /* Instance est ajoute sur le premier tableau */
            tab_ajoute(ptmatrice, niveau_last, (void *)ptloins);
            tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_I);

            label = getlabellocon(ptnextlocon);

            /* On cherche les instances identiques */
            if (!fclFindCorrespondingLoins(ptmatrice, niveau, label, loop)) {
                freechain(ptfils);
                return 0;
            }
            if (loop >= 0) niveau_last--;
            else if (!parcours_sig(ptnextlocon, TYPE_I, niveau)) {
                freechain(ptfils);
                return 0;
            }
        }
    }

    freechain(ptfils);
    return 1;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                            clear()                                     ##
   ##                                                                        ##
   ## Supprime les ptypes MATCH du sous-graphe apres une premiere recherche  ##
   ####====================================================================####
   ##  entree : pointeur sur la LOFIG                                        ##
   ##  sortie : rien                                                         ##
   ####====================================================================####
 */
static void
clear(ptlofig)
    lofig_list     *ptlofig;
{
    losig_list     *ptlosig;
    lotrs_list     *ptlotrs;
    loins_list     *ptloins;
    ptype_list     *ptype;

    /*  scan signal list  */
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        ptype = getptype(ptlosig->USER, FCL_MATCH_PTYPE);
        if (ptype != NULL) ptlosig->USER = delptype(ptlosig->USER, FCL_MATCH_PTYPE);
    }

    /*  scan lotrs list  */
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        ptype = getptype(ptlotrs->USER, FCL_MATCH_PTYPE);
        if (ptype != NULL) ptlotrs->USER = delptype(ptlotrs->USER, FCL_MATCH_PTYPE);
    }

    /*  scan loins list  */
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
        ptype = getptype(ptloins->USER, FCL_MATCH_PTYPE);
        if (ptype != NULL) ptloins->USER = delptype(ptloins->USER, FCL_MATCH_PTYPE);
    }
}


/*
   ####====================================================================####
   ##                                                                        ##
   ##                            init_phase2()                               ##
   ##                                                                        ##
   ## Supprime tous les ptypes du traitement de la phase 1                   ##
   ## et initialise les labels                                               ##
   ####====================================================================####
   ##  entree : pointeur sur la LOFIG                                        ##
   ##  sortie : rien                                                         ##
   ####====================================================================####
 */
static void
init_phase2(ptlofig)
    lofig_list     *ptlofig;
{
    losig_list     *ptlosig = (losig_list *) NULL;
    ptype_list     *ptype;
    int             i;

    /*  scan signal list  */
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {

        /* for each signal count the number of connection */
        ptype = getptype(ptlosig->USER, LOFIGCHAIN);
        i = CountChain((chain_list *)ptype->DATA);

        /* compensate for external connector */
        if (ptlosig->TYPE != 'I') i--;

        /* and save this value */
        ptype = getptype(ptlosig->USER, FCL_LABEL_PTYPE);
        if (ptype == NULL) {
            ptlosig->USER = addptype(ptlosig->USER, FCL_LABEL_PTYPE, (void *)(long)i);
        }
        else {
            ptype->DATA = (void *)(long)i;
        }
    }
}

static void
clean_phase2(ptlofig)
    lofig_list     *ptlofig;
{
    losig_list     *ptlosig = (losig_list *) NULL;
    ptype_list     *ptype;

    /*  scan signal list  */
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        ptype = getptype(ptlosig->USER, FCL_LABEL_PTYPE);
        if (ptype != NULL) {
            ptlosig->USER = delptype(ptlosig->USER, FCL_LABEL_PTYPE);
        }
    }
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                            rebouclage()                                ##
   ##                                                                        ##
   ## Test si un rebouclage existe dans chaques solutions,                   ##
   ## si c'est le cas ces solutions sont supprimees                          ##
   ####====================================================================####
   ##  entree :                                                              ##
   ##  sortie : une valeur d'etat                                            ##
   ####====================================================================####
 */
/*
static int
rebouclage()
{
    void           *pt_element;
    matrice_list   *ptm;
    int             niveau;

    for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
        pt_element = ((ptm->tab)[niveau_last]);

        for (niveau = niveau_last - 1; niveau >= 0; niveau--) {
            if (((ptm->tab)[niveau]) == pt_element) {
                ptm = tab_del(ptmatrice, ptm);
                break;
            }
        }
    }

    if (ptmatrice->NEXT->NEXT == NULL) return 0;

    return 1;
}
*/
/*
   ####====================================================================####
   ##                                                                        ##
   ##                            check_couplings()                           ##
   ##                                                                        ##
   ## Remove solutions violating instance connector couplings                ##
   ####====================================================================####
 */
static int
check_couplings()
{
    matrice_list   *ptm;
    losig_list     *ptnewmodelsig;
    losig_list     *ptmodelsig;
    losig_list     *ptlosig;
    num_list       *ptcouplinglist;
    num_list       *ptlevellist, *ptlevel;
    num_list       *ptnum, *ptnumintersection;
    chain_list     *ptnumlistchain;
    ptype_list     *ptuser;
    long            coupling_index;
    long            level;

    if ((ptmatrice->NEXT->tab)[niveau_last] != (void *)(long)TYPE_S) return 1;
    ptnewmodelsig = (losig_list *)(ptmatrice->tab)[niveau_last];
    if ((ptuser = getptype(ptnewmodelsig->USER, FCL_COUP_LIST_PTYPE)) == NULL) return 1;
    ptcouplinglist = (num_list *)ptuser->DATA;
    /* For each coupling index on the most recent signal */
    for (ptnum = ptcouplinglist; ptnum; ptnum = ptnum->NEXT) {
        coupling_index = ptnum->DATA;
        ptlevellist = addnum(NULL, (long)niveau_last);
        /* obtain list of levels of signals with identical coupling index */
        for (level = 0; level < niveau_last; level++) {
            if ((ptmatrice->NEXT->tab)[level] != (void *)(long)TYPE_S) continue;
            ptmodelsig = (losig_list *)(ptmatrice->tab)[level];
            if ((ptuser = getptype(ptmodelsig->USER, FCL_COUP_LIST_PTYPE)) == NULL) continue;
            if (getnum((num_list *)ptuser->DATA, coupling_index) != NULL) {
                ptlevellist = addnum(ptlevellist, level);
            }
        }
        if (ptlevellist->NEXT != NULL) { /* signal(s) with matching index exist */
            for (ptm = ptmatrice->NEXT->NEXT; ptm; ptm = ptm->NEXT) {
                ptnumlistchain = NULL;
                /* obtain list of coupling index lists for each of the identified levels */
                for (ptlevel = ptlevellist; ptlevel; ptlevel = ptlevel->NEXT) {
                    ptlosig = (losig_list *)(ptm->tab)[ptlevel->DATA];
                    if ((ptuser = getptype(ptlosig->USER, FCL_COUP_LIST_PTYPE)) == NULL) {
                        ptm = tab_del(ptmatrice, ptm);
                        break;
                    }
                    ptnumlistchain = addchain(ptnumlistchain, ptuser->DATA);
                }
                if (ptlevel != NULL) {
                    freechain(ptnumlistchain);
                    continue;
                }
                /* bad solution if the coupling list intersection is empty */
                if ((ptnumintersection = get_numlist_intersect(ptnumlistchain)) == NULL) {
                    ptm = tab_del(ptmatrice, ptm);
                }
                freenum(ptnumintersection);
            }
        }
        freenum(ptlevellist);
    }

    if (ptmatrice->NEXT->NEXT == NULL) return 0;

    return 1;
}

static num_list *
get_numlist_intersect(ptnumlistchain)
    chain_list *ptnumlistchain;
{
    num_list   *ptfirstnumlist;
    num_list   *ptcurnumlist;
    num_list   *ptnum;
    num_list   *ptnumintersection = NULL;
    chain_list *ptchain;
    long        curindex;
    int         intersect;

    if (ptnumlistchain->NEXT == NULL) return NULL;
    ptfirstnumlist = (num_list *)ptnumlistchain->DATA;
    for (ptnum = ptfirstnumlist; ptnum; ptnum = ptnum->NEXT) {
        curindex = ptnum->DATA;
        intersect = TRUE;
        for (ptchain = ptnumlistchain->NEXT; ptchain; ptchain = ptchain->NEXT) {
            ptcurnumlist = (num_list *)ptchain->DATA;
            if (getnum(ptcurnumlist, curindex) == NULL) {
                intersect = FALSE;
                break;
            }
        }
        if (intersect) ptnumintersection = addnum(ptnumintersection, curindex);
    }
    return ptnumintersection;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                        test_non_match                                  ##
   ##                                                                        ##
   ##  Parcours les branches non parcourues                                  ## 
   ####====================================================================####
   ## Cherche si des transistors n'ont pas ete parcourus, s'il en existe     ##
   ## ces branches sont connectees au signaux externes. Il faut donc a partir## 
   ## du transistor non MATCH chercher un signal externe, puis continuer     ##
   ## le parcours dans les deux graphes a partir de ce signal                ##
   ##                                                                        ##
   ####====================================================================####
 */
static int
test_non_match(ptlofig_m)
    lofig_list     *ptlofig_m;
{
    lotrs_list     *ptlotrs;
    loins_list     *ptloins;
    losig_list     *ptlosig;
    locon_list     *ptnextlocon;
    ptype_list     *ptype;
    fcl_label       label;
    int             all_matched;
    int             matching;
    int             i;

    do {
        all_matched = TRUE;
        matching = FALSE;
        /* parcours tous les transistors du sous-graphe */
        for (ptlotrs = ptlofig_m->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
            if ((ptype = getptype(ptlotrs->USER, FCL_MATCH_PTYPE)) == NULL) {
                /* Une branche non parcourue existe */
                all_matched = FALSE;

                /* On cherche un signal externe deja MATCH */
                if ((ptlosig = ptlotrs->DRAIN->SIG)->TYPE == EXTERNAL &&
                    (ptype = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) != NULL) {

                    niveau_last++;
                    matching = TRUE;

                    /* Le transistor est marque MATCH */
                    ptlotrs->USER = addptype(ptlotrs->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);

                    /* Le trs est ajoute sur le premier tableau */
                    tab_ajoute(ptmatrice, niveau_last, ptlotrs);
                    tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_T);

                    label = getlabelsigtrs(ptlotrs, ptlosig);

                    for (i = 0; i <= niveau_last; i++) {
                        if ((ptmatrice->tab)[i] == ptlosig) break;
                    }

                    /* On cherche les transistors identiques */
                    if (!fclFindCorrespondingLotrs(ptmatrice, i, label, -1)) return 0;
                    if (!parcours_sig(ptlotrs->DRAIN, TYPE_T, (int)(long)ptype->DATA)) return 0;
                    break;
                }

                if ((ptlosig = ptlotrs->SOURCE->SIG)->TYPE == EXTERNAL &&
                    (ptype = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) != NULL) {

                    niveau_last++;
                    matching = TRUE;

                    /* Le transistor est marque MATCH */
                    ptlotrs->USER = addptype(ptlotrs->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);

                    /* Le trs est ajoute sur le premier tableau */
                    tab_ajoute(ptmatrice, niveau_last, ptlotrs);
                    tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long) TYPE_T);

                    label = getlabelsigtrs(ptlotrs, ptlosig);


                    for (i = 0; i <= niveau_last; i++) {
                        if ((ptmatrice->tab)[i] == ptlosig) break;
                    }

                    /* On cherche les transistors identiques */
                    if (!fclFindCorrespondingLotrs(ptmatrice, i, label, -1)) return 0;
                    if (!parcours_sig(ptlotrs->SOURCE, TYPE_T, (int)(long) ptype->DATA)) return 0;
                    break;
                }

                if ((ptlosig = ptlotrs->GRID->SIG)->TYPE == EXTERNAL &&
                    (ptype = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) != NULL) {

                    niveau_last++;
                    matching = TRUE;

                    /* Le transistor est marque MATCH */
                    ptlotrs->USER = addptype(ptlotrs->USER, FCL_MATCH_PTYPE, (void *)(long) niveau_last);

                    /* Le trs est ajoute sur le premier tableau */
                    tab_ajoute(ptmatrice, niveau_last, ptlotrs);
                    tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long) TYPE_T);

                    label = getlabelsigtrs(ptlotrs, ptlosig);

                    for (i = 0; i <= niveau_last; i++) {
                        if ((ptmatrice->tab)[i] == (void *) ptlosig) break;
                    }

                    /* On cherche les transistors identiques */
                    if (!fclFindCorrespondingLotrs(ptmatrice, i, label, -1)) return 0;
                    if (!parcours_sig(ptlotrs->GRID, TYPE_T, (int)(long) ptype->DATA)) return 0;
                    break;
                }
            }
        }
        /* parcours tous les instances du sous-graphe */
        for (ptloins = ptlofig_m->LOINS; ptloins; ptloins = ptloins->NEXT) {
            if ((ptype = getptype(ptloins->USER, FCL_MATCH_PTYPE)) == NULL) {
                /* Une branche non parcourue existe */
                all_matched = FALSE;

                /* On cherche un signal externe deja MATCH */
                for (ptnextlocon = ptloins->LOCON; ptnextlocon; ptnextlocon = ptnextlocon->NEXT) {
                    if ((ptlosig = ptnextlocon->SIG)->TYPE == EXTERNAL &&
                        (ptype = getptype(ptlosig->USER, FCL_MATCH_PTYPE)) != NULL) {

                        niveau_last++;
                        matching = TRUE;

                        /* Instance est marque MATCH */
                        ptloins->USER = addptype(ptloins->USER, FCL_MATCH_PTYPE, (void *)(long)niveau_last);

                        /* Instance est ajoute sur le premier tableau */
                        tab_ajoute(ptmatrice, niveau_last, ptloins);
                        tab_ajoute(ptmatrice->NEXT, niveau_last, (void *)(long)TYPE_I);

                        label = getlabellocon(ptnextlocon);

                        for (i = 0; i <= niveau_last; i++) {
                            if ((ptmatrice->tab)[i] == ptlosig) break;
                        }

                        /* On cherche les instances identiques */
                        if (!fclFindCorrespondingLoins(ptmatrice, i, label, -1)) return 0;
                        if (!parcours_sig(ptnextlocon, TYPE_I, (int)(long)ptype->DATA)) return 0;
                        break;
                    }
                }
            }
        }
        /* nothing possible */
        if (all_matched == FALSE && matching == FALSE && niveau_last > 2) {
           fprintf(stderr, "[FCL WAR] model '%s' is disjoint - there are unmatched transistors\n", ptlofig_m->NAME);
           break;
        }
    } while (all_matched == FALSE && matching == TRUE);

    return 1;
}

