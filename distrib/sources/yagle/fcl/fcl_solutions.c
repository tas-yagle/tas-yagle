/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_solutions.c                                             */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

static int      mark_cell_links();
static void     mark_cell_interface();

static void     mark_cell();
static befig_list *make_cell_befig();
static char    *get_instance_name();
static chain_list *get_instance_abl();
static loins_list *build_instance();
static void     write_solution();
static void     make_fcl_locon();
static void     make_correspondance_list();
static void     fcl_markconstraints();

int
fclTestSolutions(ptmatrice, ptlofig_c, ptlofig_m, ptbefig_m, instance)
    matrice_list   *ptmatrice;
    lofig_list     *ptlofig_c;
    lofig_list     *ptlofig_m;
    befig_list     *ptbefig_m;
    int            *instance;
{
    losig_list     *ptlosig;        /* SIGNAL de la solution */
    losig_list     *ptlosig_model;  /* SIGNAL du model       */
    lotrs_list     *ptlotrs;        /* TRS de la solution    */
    lotrs_list     *ptlotrs_model;  /* TRS du model          */
    loins_list     *ptloins;        /* INS de la solution    */
    loins_list     *ptloins_model;  /* INS du model          */
    matrice_list   *ptsolution;
    ht             *nameht;
    ht             *instanceht;
    ptype_list     *ptuser, *ptuser2;
    long            type;
    long            mark;
    long            i;

    if (FCL_DISPLAY_MATRIX) displaymatrice(ptmatrice, niveau_last, TRUE);
    for (ptsolution = ptmatrice->NEXT->NEXT; ptsolution; ptsolution = ptsolution->NEXT) {    
        /* check that all transistors of solution are available */
        for (i = 0; i <= niveau_last; i++) {
            if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_T) {
                ptlotrs = (lotrs_list *) (ptsolution->tab)[i];
                if ((ptuser = getptype(ptlotrs->USER, FCL_MARK_PTYPE)) != NULL) {
                    mark = (long)ptuser->DATA;
                    if ((mark & FCL_NOSHARE) == FCL_NOSHARE) break;
                }
            }
        }
        if (i <= niveau_last) continue;
        
        /* mark the solution */
        
        instanceht = addht(40);
        nameht = addht(40);

        if (FCL_BUILD_CELLS) {
            type = (long) getptype(ptlofig_m->USER, FCL_MODEL_PTYPE)->DATA | CNS_UNKNOWN;
            FCL_CELL_LIST = addcell(FCL_CELL_LIST, type, NULL, ptbefig_m);
        }

        for (i = 0; i <= niveau_last; i++) {
            if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_T) {
                /* Le chemin passe par un transistor */

                ptlotrs_model = (lotrs_list *) (ptmatrice->tab)[i];
                ptlotrs = (lotrs_list *) (ptsolution->tab)[i];

                addhtitem(instanceht, ptlotrs_model, (long) ptlotrs);

                if (FCL_TRACE_LEVEL > 1) {

                    printf("t - %d : S-%ld G-%ld D-%ld ", (int) ptlotrs->TYPE, ptlotrs->SOURCE->SIG->INDEX, ptlotrs->GRID->SIG->INDEX, ptlotrs->DRAIN->SIG->INDEX);
                    if (ptlotrs->TRNAME != NULL)
                        printf("'%s' ", ptlotrs->TRNAME);

                    printf("| t - %d : S-%ld G-%ld D-%ld", (int) ptlotrs_model->TYPE, ptlotrs_model->SOURCE->SIG->INDEX, ptlotrs_model->GRID->SIG->INDEX, ptlotrs_model->DRAIN->SIG->INDEX);
                    if (ptlotrs_model->TRNAME != NULL)
                        printf(" '%s'", ptlotrs_model->TRNAME);
                    printf("\n");
                }

                /* mark for sharing or no sharing */
                if ((ptuser = getptype(ptlotrs_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & FCL_SHARE_TRANS) != 0) {
                        if ((ptuser2 = getptype(ptlotrs->USER, FCL_MARK_PTYPE)) == NULL) {
                            ptlotrs->USER = addptype(ptlotrs->USER, FCL_MARK_PTYPE, (void*)FCL_SHARE_TRANS);
                        }
                        else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_SHARE_TRANS);
                    }
                    else {
                        if ((ptuser2 = getptype(ptlotrs->USER, FCL_MARK_PTYPE)) == NULL) {
                            ptlotrs->USER = addptype(ptlotrs->USER, FCL_MARK_PTYPE, (void*)FCL_NOSHARE);
                        }
                        else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_NOSHARE);
                    }
                }
                else {
                    if ((ptuser2 = getptype(ptlotrs->USER, FCL_MARK_PTYPE)) == NULL) {
                        ptlotrs->USER = addptype(ptlotrs->USER, FCL_MARK_PTYPE, (void*)FCL_NOSHARE);
                    }
                    else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_NOSHARE);
                }
                
                /* transfer the user markings */
                if ((ptuser = getptype(ptlotrs_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if ((ptuser2 = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                        ptlotrs->USER = addptype(ptlotrs->USER, FCL_TRANSFER_PTYPE, ptuser->DATA);
                    }
                    else ptuser2->DATA = (void *)((long)ptuser2->DATA | (long)ptuser->DATA);
                }

                if (FCL_BUILD_CELLS) {
                    ptlosig = (void *) ptlotrs->GRID->SIG;
                    if (mark_cell_links(ptlosig, ptmatrice, ptsolution) == FALSE) return FALSE;
                }
            }
            else if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_I) {
                /* Le chemin passe par une instance */

                ptloins_model = (loins_list *) (ptmatrice->tab)[i];
                ptloins = (loins_list *) (ptsolution->tab)[i];

                addhtitem(instanceht, ptloins_model, (long) ptloins);

                if (FCL_TRACE_LEVEL > 1) {
                    printf("i - %s | i - %s\n", ptloins->INSNAME, ptloins_model->INSNAME);
                }

                /* mark for sharing or no sharing */
                if ((ptuser = getptype(ptloins_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if (((long)ptuser->DATA & FCL_SHARE_TRANS) != 0) {
                        if ((ptuser2 = getptype(ptloins->USER, FCL_MARK_PTYPE)) == NULL) {
                            ptloins->USER = addptype(ptloins->USER, FCL_MARK_PTYPE, (void*)FCL_SHARE_INS);
                        }
                        else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_SHARE_TRANS);
                    }
                    else {
                        if ((ptuser2 = getptype(ptloins->USER, FCL_MARK_PTYPE)) == NULL) {
                            ptloins->USER = addptype(ptloins->USER, FCL_MARK_PTYPE, (void*)FCL_NOSHARE);
                        }
                        else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_NOSHARE);
                    }
                }
                else {
                    if ((ptuser2 = getptype(ptloins->USER, FCL_MARK_PTYPE)) == NULL) {
                        ptloins->USER = addptype(ptloins->USER, FCL_MARK_PTYPE, (void*)FCL_NOSHARE);
                    }
                    else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_NOSHARE);
                }
                
                /* transfer the user markings */
                if ((ptuser = getptype(ptloins_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if ((ptuser2 = getptype(ptloins->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                        ptloins->USER = addptype(ptloins->USER, FCL_TRANSFER_PTYPE, ptuser->DATA);
                    }
                    else ptuser2->DATA = (void *)((long)ptuser2->DATA | (long)ptuser->DATA);
                }
            }
            else {
                /* Le chemin passe par un signal */

                ptlosig_model = (losig_list *) (ptmatrice->tab)[i];
                ptlosig = (losig_list *) (ptsolution->tab)[i];
                addhtitem(instanceht, ptlosig_model, (long) ptlosig);

                if (FCL_TRACE_LEVEL > 1) {

                    if (ptlosig->NAMECHAIN != NULL) {
                        printf("s - %c : %s ", ptlosig->TYPE, (char *) (ptlosig->NAMECHAIN->DATA));
                    }
                    else
                        printf("s - %c : ", ptlosig->TYPE);

                    if (ptlosig_model->NAMECHAIN != NULL) {
                        printf("| s - %c : %s \n", ptlosig_model->TYPE, (char *) (ptlosig_model->NAMECHAIN->DATA));
                    }
                    else
                        printf("| s - %c : \n", ptlosig_model->TYPE);
                }

                if (ptlosig->NAMECHAIN != NULL && ptlosig_model->NAMECHAIN != NULL) {
                    addhtitem(nameht, ptlosig_model->NAMECHAIN->DATA, (long) ptlosig);
                }

                if ((ptuser = getptype(ptlosig_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    if ((ptuser2 = getptype(ptlosig->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                        ptlosig->USER = addptype(ptlosig->USER, FCL_TRANSFER_PTYPE, ptuser->DATA);
                    }
                    else ptuser2->DATA = (void *)((long)ptuser2->DATA | (long)ptuser->DATA);
                    if (((long)ptuser->DATA & FCL_VSS) == FCL_VSS) ptlosig->TYPE = CNS_SIGVSS;
                    if (((long)ptuser->DATA & FCL_VDD) == FCL_VDD) ptlosig->TYPE = CNS_SIGVDD;
                }

                if (FCL_BUILD_CELLS && ptlosig_model->TYPE == EXTERNAL) {
                    mark_cell_interface(ptlosig_model, ptlosig);
                }
            }
        }

        if (FCL_TRACE_LEVEL > 1) {
            printf("MATCH\n");
        }
        if (FCL_FILE) write_solution(ptmatrice, ptsolution, ptlofig_m->NAME, *instance);

        if (FCL_BUILD_INSTANCES) {
            FCL_INSTANCE_LIST = addchain(FCL_INSTANCE_LIST, build_instance(ptmatrice, ptsolution, ptlofig_m, instanceht, *instance));
            if (ptbefig_m != NULL) {
                FCL_INSTANCE_BEFIGS = addchain(FCL_INSTANCE_BEFIGS, make_cell_befig(ptbefig_m, nameht, *instance));
            }
        }

        if (FCL_BUILD_CELLS && ptbefig_m != NULL) {
            mark_cell(FCL_CELL_LIST, ptbefig_m, nameht);
            FCL_CELL_LIST->BEFIG = make_cell_befig(ptbefig_m, nameht, *instance);
        }

        fcl_markconstraints(ptlofig_c, ptlofig_m, nameht);
        (*instance)++;
        
        delht(nameht);
        delht(instanceht);
        if (FCL_BUILD_CORRESP) {
            make_correspondance_list(ptmatrice, ptsolution);
            if (FCL_TRACE_LEVEL > 1 && ptsolution->NEXT != NULL) {
                printf("Alternative solutions exist\n");
            }
            return TRUE;
        }
    }

    return TRUE;
}

static int
mark_cell_links(ptlosig, ptmatrice, ptsolution)
    losig_list     *ptlosig;
    matrice_list   *ptmatrice;
    matrice_list   *ptsolution;
{
    losig_list     *ptlosig_model;
    locon_list     *ptlocon;
    ptype_list     *ptuser;
    chain_list     *ptchain;
    int             marque;
    long            i;

    for (i = 0; i <= niveau_last; i++) {
        if (ptlosig == (losig_list *) (ptsolution->tab)[i]) {
            /* le TRS courrant a sur la grille un signal */
            /* qui appartient a la solution trouvee.     */

            ptlosig_model = (losig_list *) (ptmatrice->tab)[i];

            marque = TRUE;

            if (ptlosig_model->TYPE == EXTERNAL) {
                /* Le signal du model est externe, si son connecteur externe */
                /* est de direction IN, alors le signal ne doit pas avoir de */
                /* ptype de rebouclage sur CELL_LIST sauf si FCL_NEVER       */

                ptuser = getptype(ptlosig_model->USER, LOFIGCHAIN);
                for (ptchain = (chain_list *) ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptlocon = ((locon_list *) ptchain->DATA);
                    if (ptlocon->TYPE == 'E') break;
                }

                if (ptlocon->DIRECTION == IN) {
                    if ((ptuser = getptype(ptlosig_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                        if (((long)ptuser->DATA & FCL_NEVER) != FCL_NEVER) {
                            marque = FALSE;
                        }
                    }
                    else marque = FALSE;
                }
            }

            if (marque) {
                /* Sur le signal de la solution est ajoute un ptype */
                /* servant de lien avec la structure des cells  */

                ptuser = getptype(ptlosig->USER, FCL_CELL_PTYPE);
                if (ptuser != NULL) {
                    if (((chain_list *) ptuser->DATA)->DATA != FCL_CELL_LIST) {
                        ptuser->DATA = addchain(ptuser->DATA, FCL_CELL_LIST);
                    }
                }
                else {
                    ptlosig->USER = addptype(ptlosig->USER, FCL_CELL_PTYPE, addchain(NULL, FCL_CELL_LIST));
                }
            }
            break;
        }
    }
    return TRUE;
}

static void
mark_cell_interface(ptlosig_model, ptlosig)
    losig_list     *ptlosig_model;
    losig_list     *ptlosig;
{
    locon_list     *ptlocon;
    ptype_list     *ptype;
    chain_list     *ptchain;

    ptype = getptype(ptlosig_model->USER, (long) LOFIGCHAIN);
    for (ptchain = (chain_list *) ptype->DATA; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = ((locon_list *) ptchain->DATA);
        if ((ptype = getptype(ptlosig_model->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptype->DATA & FCL_NEVER) == FCL_NEVER) continue;
        }

        if (ptlocon->TYPE == 'E') {
            if (FCL_CELL_LIST->BEFIG != NULL
                && (ptlocon->DIRECTION == IN || ptlocon->DIRECTION == INOUT
                    || ptlocon->DIRECTION == TRANSCV)) {
                if (getptype(ptlosig->USER, FCL_FORCECONE_PTYPE) == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, FCL_FORCECONE_PTYPE, NULL);
                }
            }
            if (ptlocon->DIRECTION == OUT || ptlocon->DIRECTION == INOUT || ptlocon->DIRECTION == TRISTATE || ptlocon->DIRECTION == TRANSCV) {

                ptype = getptype(ptlosig->USER, FCL_CELL_PTYPE);
                if (ptype != NULL) {
                    if (((chain_list *) ptype->DATA)->DATA != FCL_CELL_LIST) {
                        ptype->DATA = addchain(ptype->DATA, FCL_CELL_LIST);
                    }
                }
                else {
                    ptlosig->USER = addptype(ptlosig->USER, FCL_CELL_PTYPE, addchain(NULL, FCL_CELL_LIST));
                }
            }
            break;
        }
    }
}

static void
mark_cell(ptcell, ptbefig_model, nameht)
    cell_list      *ptcell;
    befig_list     *ptbefig_model;
    ht             *nameht;
{
    losig_list     *ptlosig;
    bepor_list     *ptbepor;
    ptype_list     *ptuser;
    chain_list     *sigchain = NULL;

    for (ptbepor = ptbefig_model->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
        if ((ptlosig = (losig_list *) gethtitem(nameht, ptbepor->NAME)) == (losig_list *) EMPTYHT) {
            fprintf(stderr, "[FCL ERR] - unknown name '%s' in model behaviour\n", ptbepor->NAME);
            fclExit();
        }
        if (ptlosig->TYPE != CNS_SIGVSS && ptlosig->TYPE != CNS_SIGVDD)
            sigchain = addchain(sigchain, ptlosig);
        if (ptbepor->DIRECTION != 'I') {
            if ((ptuser = getptype(ptlosig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                ptuser->DATA = (void *) ((long) ptuser->DATA | FCL_CELLOUT);
            }
            else
                ptlosig->USER = addptype(ptlosig->USER, FCL_TRANSFER_PTYPE, (void *) FCL_CELLOUT);
        }
    }
    ptcell->USER = addptype(ptcell->USER, YAG_SIGLIST_PTYPE, sigchain);
}

static befig_list *
make_cell_befig(ptbefig_model, nameht, num)
    befig_list     *ptbefig_model;
    ht             *nameht;
    int             num;
{
    befig_list     *ptbefig;
    bepor_list     *ptbepor;
    beout_list     *ptbeout;
    berin_list     *ptberin;
    bereg_list     *ptbereg;
    beaux_list     *ptbeaux;
    beaux_list     *ptbedly;
    bebus_list     *ptbebus;
    bebux_list     *ptbebux;
    biabl_list     *ptbiabl;
    chain_list     *ptabl, *ptvalabl, *ptcndabl;
    char           *name;
    char           *prefix;
    char            buffer[80];

    sprintf(buffer, "%s_%d", ptbefig_model->NAME, num);
    ptbefig = beh_addbefig(NULL, namealloc(buffer));
    ptbefig->TIME_UNIT = ptbefig_model->TIME_UNIT;
    sprintf(buffer, "%s_%d_", ptbefig_model->NAME, num);
    prefix = namealloc(buffer);

    for (ptbepor = ptbefig_model->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
        name = get_instance_name(nameht, ptbepor->NAME, prefix);
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, ptbepor->DIRECTION, ptbepor->TYPE);
    }
    ptbefig->BEPOR = (bepor_list *) reverse((chain_list *) ptbefig->BEPOR);

    for (ptbeout = ptbefig_model->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        name = get_instance_name(nameht, ptbeout->NAME, prefix);
        if (ptbeout->ABL != NULL) {
            ptabl = get_instance_abl(nameht, ptbeout->ABL, prefix);
        }
        else
            ptabl = NULL;
        ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, name, ptabl, NULL,0);
        ptbefig->BEOUT->TIME = ptbeout->TIME;
    }
    ptbefig->BEOUT = (beout_list *) reverse((chain_list *) ptbefig->BEOUT);

    for (ptberin = ptbefig_model->BERIN; ptberin; ptberin = ptberin->NEXT) {
        name = get_instance_name(nameht, ptberin->NAME, prefix);
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
    }
    ptbefig->BERIN = (berin_list *) reverse((chain_list *) ptbefig->BERIN);

    for (ptbereg = ptbefig_model->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        name = get_instance_name(nameht, ptbereg->NAME, prefix);
        ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, name, NULL, NULL,0);
        for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptvalabl = get_instance_abl(nameht, ptbiabl->VALABL, prefix);
            ptcndabl = get_instance_abl(nameht, ptbiabl->CNDABL, prefix);
            ptbefig->BEREG->BIABL = beh_addbiabl(ptbefig->BEREG->BIABL, ptbiabl->LABEL, ptcndabl, ptvalabl);
            ptbefig->BEREG->BIABL->TIME = ptbiabl->TIME;
            ptbefig->BEREG->BIABL->FLAG = ptbiabl->FLAG;
        }
        ptbefig->BEREG->BIABL = (biabl_list *) reverse((chain_list *) ptbefig->BEREG->BIABL);
    }
    ptbefig->BEREG = (bereg_list *) reverse((chain_list *) ptbefig->BEREG);

    for (ptbeaux = ptbefig_model->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        name = get_instance_name(nameht, ptbeaux->NAME, prefix);
        ptabl = get_instance_abl(nameht, ptbeaux->ABL, prefix);
        ptbefig->BEAUX = beh_addbeaux(ptbefig->BEAUX, name, ptabl, NULL,0);
        ptbefig->BEAUX->TIME = ptbeaux->TIME;
    }
    ptbefig->BEAUX = (beaux_list *) reverse((chain_list *) ptbefig->BEAUX);

    for (ptbedly = ptbefig_model->BEDLY; ptbedly; ptbedly = ptbedly->NEXT) {
        name = get_instance_name(nameht, ptbedly->NAME, prefix);
        ptabl = get_instance_abl(nameht, ptbedly->ABL, prefix);
        ptbefig->BEDLY = beh_addbeaux(ptbefig->BEDLY, name, ptabl, NULL,0);
    }
    ptbefig->BEDLY = (beaux_list *) reverse((chain_list *) ptbefig->BEDLY);

    for (ptbebus = ptbefig_model->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        name = get_instance_name(nameht, ptbebus->NAME, prefix);
        ptbefig->BEBUS = beh_addbebus(ptbefig->BEBUS, name, NULL, NULL,0,0);
        for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptvalabl = get_instance_abl(nameht, ptbiabl->VALABL, prefix);
            ptcndabl = get_instance_abl(nameht, ptbiabl->CNDABL, prefix);
            ptbefig->BEBUS->BIABL = beh_addbiabl(ptbefig->BEBUS->BIABL, ptbiabl->LABEL, ptcndabl, ptvalabl);
            ptbefig->BEBUS->BIABL->TIME = ptbiabl->TIME;
            ptbefig->BEBUS->BIABL->FLAG = ptbiabl->FLAG;
        }
        ptbefig->BEBUS->BIABL = (biabl_list *) reverse((chain_list *) ptbefig->BEBUS->BIABL);
    }
    ptbefig->BEBUS = (bebus_list *) reverse((chain_list *) ptbefig->BEBUS);

    for (ptbebux = ptbefig_model->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        name = get_instance_name(nameht, ptbebux->NAME, prefix);
        ptbefig->BEBUX = beh_addbebux(ptbefig->BEBUX, name, NULL, NULL,0,0);
        for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptvalabl = get_instance_abl(nameht, ptbiabl->VALABL, prefix);
            ptcndabl = get_instance_abl(nameht, ptbiabl->CNDABL, prefix);
            ptbefig->BEBUX->BIABL = beh_addbiabl(ptbefig->BEBUX->BIABL, ptbiabl->LABEL, ptcndabl, ptvalabl);
            ptbefig->BEBUX->BIABL->TIME = ptbiabl->TIME;
            ptbefig->BEBUX->BIABL->FLAG = ptbiabl->FLAG;
        }
        ptbefig->BEBUX->BIABL = (biabl_list *) reverse((chain_list *) ptbefig->BEBUX->BIABL);
    }
    ptbefig->BEBUX = (bebux_list *) reverse((chain_list *) ptbefig->BEBUX);

    return ptbefig;
}

static char    *
get_instance_name(nameht, modelname, prefix)
    ht             *nameht;
    char           *modelname;
    char           *prefix;
{
    losig_list     *ptlosig;
    char            buffer[80];

    if ((ptlosig = (losig_list *) gethtitem(nameht, modelname)) == (losig_list *) EMPTYHT) {
        sprintf(buffer, "%s%s", prefix, modelname);
        return namealloc(buffer);
    }
    if (ptlosig->TYPE == CNS_SIGVSS) {
        FCL_NEED_ZERO = TRUE;
        return namealloc("yag_zero");
    }
    if (ptlosig->TYPE == CNS_SIGVDD) {
        FCL_NEED_ONE = TRUE;
        return namealloc("yag_one");
    }
    return (char *) ptlosig->NAMECHAIN->DATA;
}

static chain_list *
get_instance_abl(nameht, expr, prefix)
    ht             *nameht;
    chain_list     *expr;
    char           *prefix;
{
    if (ATOM(expr)) {
        if (strcmp(VALUE_ATOM(expr), "'d'")
            && strcmp(VALUE_ATOM(expr), "'z'")
            && strcmp(VALUE_ATOM(expr), "'1'")
            && strcmp(VALUE_ATOM(expr), "'0'")) {
            return createAtom(get_instance_name(nameht, (char *) expr->DATA, prefix));
        }
        else
            return createAtom(VALUE_ATOM(expr));
    }
    else {
        chain_list     *auxExpr;
        chain_list     *oldExpr = expr;
        chain_list     *expr1;

        auxExpr = createExpr(OPER(expr));
        expr1 = auxExpr;

        while ((expr = CDR(expr))) {
            /* on optimise a fond la recopie de la dorsale */

            expr1->NEXT = addchain(NULL, (void *) get_instance_abl(nameht, CAR(expr), prefix));
            expr1 = CDR(expr1);
        }

        /* on sauvegarde la poubelle */
        ((chain_list *) auxExpr->DATA)->NEXT = CDR(CAR(oldExpr));
        return (auxExpr);
    }
}

void
fclMarkInstances(ptlofig, instances, delete, fromcnsfig)
    lofig_list     *ptlofig;
    chain_list     *instances;
    int             delete;
    int             fromcnsfig;
{
    chain_list     *ptchain, *ptchain1, *ptchain2;
    chain_list     *translist;
    chain_list     *inslist;
    chain_list     *ptloconchain;
    loins_list     *ptins, *ptusedins;
    lotrs_list     *ptlotrs, *ptparatrans;
    lotrs_list     *ptprevtrans, *ptnexttrans;
    locon_list     *ptlocon;
    locon_list     *pttranscon;
    losig_list     *ptlosig;
    num_list       *ptnum;
    ptype_list     *ptuser, *ptuser2;
    int             inner;

    FCL_LOCON_LIST = NULL;
    for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
        translist = NULL;
        inslist = NULL;
        ptins = (loins_list *)ptchain->DATA;
        /* mark transistors */
        ptuser = getptype(ptins->USER, FCL_TRANSLIST_PTYPE);
        if (ptuser != NULL) translist = (chain_list *)ptuser->DATA;
        for (ptchain1 = translist; ptchain1; ptchain1 = ptchain1->NEXT) {
            ptlotrs = (lotrs_list *)ptchain1->DATA;
            if ((ptuser2 = getptype(ptlotrs->USER, MBK_TRANS_PARALLEL)) != NULL) {
                for (ptchain2 = (chain_list *)ptuser2->DATA; ptchain2; ptchain2 = ptchain2->NEXT) {
                    ptparatrans = (lotrs_list *)ptchain2->DATA;
                    if ((ptuser = getptype(ptparatrans->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                        ptparatrans->USER = addptype(ptparatrans->USER, FCL_TRANSFER_PTYPE, (void *)FCL_NEVER);
                    }
                    else ptuser->DATA = (void *)((long)ptuser->DATA | FCL_NEVER);
                }
            }
            else {
                if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                    ptlotrs->USER = addptype(ptlotrs->USER, FCL_TRANSFER_PTYPE, (void *)FCL_NEVER);
                }
                else ptuser->DATA = (void *)((long)ptuser->DATA | FCL_NEVER);
            }
        }
        /* mark instances */
        ptuser = getptype(ptins->USER, FCL_INSLIST_PTYPE);
        if (ptuser != NULL) inslist = (chain_list *)ptuser->DATA;
        for (ptchain1 = inslist; ptchain1; ptchain1 = ptchain1->NEXT) {
            ptusedins = (loins_list *)ptchain1->DATA;
            if ((ptuser = getptype(ptusedins->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                ptusedins->USER = addptype(ptusedins->USER, FCL_TRANSFER_PTYPE, (void *)FCL_NEVER);
            }
            else ptuser->DATA = (void *)((long)ptuser->DATA | FCL_NEVER);
        }
        for (ptlocon = ptins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            ptlosig = ptlocon->SIG;
            /* create pnode list for recognised instances */
            /*if (ptlosig->PRCN) {
                ptloconchain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA;
                for (;ptloconchain; ptloconchain = ptloconchain->NEXT) {
                    pttranscon = (locon_list *)ptloconchain->DATA;
                    if (pttranscon->TYPE != 'T') continue;
                    ptlotrs = pttranscon->ROOT;
                    inner = FALSE;
                    if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                        if (((long)ptuser->DATA & FCL_NEVER) == FCL_NEVER) {
                            inner = TRUE;
                        }
                    }
                    if (ptlocon->DIRECTION == IN || ptlocon->DIRECTION == UNKNOWN) {
                        if (inner == TRUE) {
                            for (ptnum = pttranscon->PNODE; ptnum; ptnum = ptnum->NEXT) {
                                ptlocon->PNODE = addnum(ptlocon->PNODE, ptnum->DATA);
                            }
                        }
                    }
                    else {
                        if (inner == FALSE) {
                            for (ptnum = pttranscon->PNODE; ptnum; ptnum = ptnum->NEXT) {
                                ptlocon->PNODE = addnum(ptlocon->PNODE, ptnum->DATA);
                            }
                        }
                    }
                }
                ptlocon->PNODE = (num_list *)reverse((chain_list *)ptlocon->PNODE);
            }*/
            if (!fromcnsfig) {
                if (ptlosig->TYPE == CNS_SIGINT) {
                    ptlosig->TYPE = CNS_SIGEXT;
                    make_fcl_locon(ptlosig);
                }
            }
        }
        if (!fromcnsfig) {
            ptins->NEXT = ptlofig->LOINS;
            ptlofig->LOINS = ptins;
        }
    }

    if (delete && !fromcnsfig) {
        ptprevtrans = NULL;
        for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptnexttrans) {
            ptnexttrans = ptlotrs->NEXT;
            if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL && ((long)ptuser->DATA & FCL_NEVER) != 0) {
                if (ptprevtrans != NULL) {
                    ptprevtrans->NEXT = ptnexttrans;
                }
                else ptlofig->LOTRS = ptnexttrans;
                ptlotrs->USER = delptype(ptlotrs->USER, FCL_TRANSFER_PTYPE);
                mbkfree(ptlotrs->GRID);
                mbkfree(ptlotrs->SOURCE);
                mbkfree(ptlotrs->DRAIN);
                if (ptlotrs->BULK) mbkfree(ptlotrs->BULK);
                mbkfree(ptlotrs);
            }
            else ptprevtrans = ptlotrs;
        }
    }

    if (!fromcnsfig) {
        for (ptlocon = FCL_LOCON_LIST; ptlocon; ptlocon = ptlocon->NEXT) {
            ptlocon->SIG->TYPE = 'I';
        }
        if ((ptuser = getptype(ptlofig->USER, FCL_LOCON_PTYPE)) != NULL) {
            ptuser->DATA = append((chain_list *)ptuser->DATA, (chain_list *)FCL_LOCON_LIST);
        }
        else ptlofig->USER = addptype(ptlofig->USER, FCL_LOCON_PTYPE, FCL_LOCON_LIST);
    }
}

static loins_list *
build_instance(ptmatrice, ptsolution, ptlofig_m, instanceht, num)
    matrice_list   *ptmatrice;
    matrice_list   *ptsolution;
    lofig_list     *ptlofig_m;
    ht             *instanceht;
    int             num;
{
    locon_list     *ptlocon;
    locon_list     *ptnewcon;
    losig_list     *ptlosig, *ptlosig_model;
    loins_list     *ptloins;
    lotrs_list     *ptlotrs, *ptlotrs_model;
    chain_list     *transchain = NULL;
    ptype_list     *ptuser;
    char            buffer[256];
    int             i;

    for (i = 0; i <= niveau_last; i++) {
        if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_T) {
            ptlotrs = (lotrs_list *) (ptsolution->tab)[i];
            transchain = addchain(transchain, ptlotrs);
            ptlotrs_model = (lotrs_list *)(ptmatrice->tab)[i];
            ptuser = getptype(ptlotrs_model->USER, FCL_CORRESP_PTYPE);
            if (ptuser != NULL) ptuser->DATA = ptlotrs;
            else ptlotrs_model->USER = addptype(ptlotrs_model->USER, FCL_CORRESP_PTYPE, ptlotrs);
        }
        else if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_S) {
            ptlosig = (losig_list *) (ptsolution->tab)[i];
            ptlosig_model = (losig_list *)(ptmatrice->tab)[i];
            ptuser = getptype(ptlosig_model->USER, FCL_CORRESP_PTYPE);
            if (ptuser != NULL) ptuser->DATA = ptlosig;
            else ptlosig_model->USER = addptype(ptlosig_model->USER, FCL_CORRESP_PTYPE, ptlosig);
        }
    }
    
    fclMarkTrans(transchain);

    ptloins = (loins_list *)mbkalloc(sizeof(loins_list));
    sprintf(buffer, "%s_%d", ptlofig_m->NAME, num);
    ptloins->INSNAME = namealloc(buffer);
    ptloins->FIGNAME = ptlofig_m->NAME;
    ptloins->LOCON = NULL;
    ptloins->USER = NULL;
    ptloins->NEXT = NULL;

    for (ptlocon = ptlofig_m->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptlosig = (losig_list *)gethtitem(instanceht, ptlocon->SIG);
        if (ptlosig == (losig_list *)EMPTYHT) {
            fprintf(stderr, "[FCL ERR] connector '%s' in model '%s' has no correspondance in circuit\n", ptlocon->NAME, ptlofig_m->NAME);
            ptlosig = NULL;
            continue;
        }
        ptnewcon = (locon_list *)mbkalloc(sizeof(locon_list));
        ptnewcon->NAME = ptlocon->NAME;
        ptnewcon->DIRECTION = ptlocon->DIRECTION;
        ptnewcon->TYPE = 'I';
        ptnewcon->SIG  = ptlosig;
        ptnewcon->ROOT = (void *)ptloins;
        ptnewcon->USER = NULL;
        ptnewcon->PNODE= NULL;
        ptnewcon->NEXT = ptloins->LOCON;
        ptloins->LOCON  = ptnewcon;
    }
    ptloins->LOCON = (locon_list *)reverse((chain_list *)ptloins->LOCON);
    
    ptloins->USER = addptype(ptloins->USER, FCL_TRANSLIST_PTYPE, transchain);

    fclUnmarkTrans(transchain);
    
    geniusExecuteAction(ptloins);
    return ptloins;
}

static void
write_solution(ptmatrice, ptsolution, name, numins)
    matrice_list   *ptmatrice;
    matrice_list   *ptsolution;
    char           *name;
    int             numins;
{
    lotrs_list     *ptlotrs_model;
    lotrs_list     *ptlotrs;
    loins_list     *ptloins_model;
    loins_list     *ptloins;
    losig_list     *ptlosig_model;
    losig_list     *ptlosig;
    char            buffer[256];
    int             i;

    sprintf(buffer, "%s_%d", name, numins);
    fprintf(FCL_OUTPUT, "Name: '%s'\n\n", buffer);
 
    for (i = 0; i <= niveau_last; i++) {
        if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_T) {
            /* Le chemin passe par un transistor */

            ptlotrs_model = (lotrs_list *) (ptmatrice->tab)[i];
            ptlotrs = (lotrs_list *) (ptsolution->tab)[i];

            fprintf(FCL_OUTPUT, "t - %d : S-%ld G-%ld D-%ld ", (int) ptlotrs->TYPE, ptlotrs->SOURCE->SIG->INDEX, ptlotrs->GRID->SIG->INDEX, ptlotrs->DRAIN->SIG->INDEX);
            if (ptlotrs->TRNAME != NULL)
                fprintf(FCL_OUTPUT, "'%s' ", ptlotrs->TRNAME);

            fprintf(FCL_OUTPUT, "| t - %d : S-%ld G-%ld D-%ld", (int) ptlotrs_model->TYPE, ptlotrs_model->SOURCE->SIG->INDEX, ptlotrs_model->GRID->SIG->INDEX, ptlotrs_model->DRAIN->SIG->INDEX);
            if (ptlotrs_model->TRNAME != NULL)
                fprintf(FCL_OUTPUT, " '%s'", ptlotrs_model->TRNAME);
            fprintf(FCL_OUTPUT, "\n");
        }
        else if ((ptmatrice->NEXT->tab)[i] == (void *) TYPE_I) {
            /* Le chemin passe par une instance */

            ptloins_model = (loins_list *) (ptmatrice->tab)[i];
            ptloins = (loins_list *) (ptsolution->tab)[i];

            fprintf(FCL_OUTPUT, "i - '%s'", ptloins->INSNAME);
            fprintf(FCL_OUTPUT, "| i - '%s'", ptloins_model->INSNAME);
            fprintf(FCL_OUTPUT, "\n");
        }
        else {
            ptlosig_model = (losig_list *) (ptmatrice->tab)[i];
            ptlosig = (losig_list *) (ptsolution->tab)[i];

            if (ptlosig->NAMECHAIN != NULL) {
                fprintf(FCL_OUTPUT, "s - %c : %s ", ptlosig->TYPE, (char *) (ptlosig->NAMECHAIN->DATA));
            }
            else
                printf("s - %c : ", ptlosig->TYPE);

            if (ptlosig_model->NAMECHAIN != NULL) {
                fprintf(FCL_OUTPUT, "| s - %c : %s \n", ptlosig_model->TYPE, (char *) (ptlosig_model->NAMECHAIN->DATA));
            }
            else
                fprintf(FCL_OUTPUT, "| s - %c : \n", ptlosig_model->TYPE);
        }
    }
    fprintf(FCL_OUTPUT, "\n");
}

static void
make_correspondance_list(ptmatrice, ptsolution)
    matrice_list   *ptmatrice;
    matrice_list   *ptsolution;
{
    fclcorresp_list *ptnewcorresp;
    int             i;

    for (i = 0; i <= niveau_last; i++) {
        ptnewcorresp = mbkalloc(sizeof(fclcorresp_list));
        ptnewcorresp->NEXT = FCL_REAL_CORRESP_LIST;
        FCL_REAL_CORRESP_LIST = ptnewcorresp;
        ptnewcorresp->TYPE = (char)(long)(ptmatrice->NEXT->tab)[i];
        ptnewcorresp->ORIG = (ptmatrice->tab)[i];
        ptnewcorresp->CORRESP = (ptsolution->tab)[i];
    }
}

static void
make_fcl_locon(ptlosig)
    losig_list *ptlosig;
{
    locon_list *ptlocon;
    ptype_list *ptuser;

    ptlocon = (locon_list *)mbkalloc(sizeof(locon_list));
    ptlocon->NAME      = (char *)ptlosig->NAMECHAIN->DATA;
    ptlocon->SIG       = ptlosig;
    ptlocon->ROOT      = NULL;
    ptlocon->DIRECTION = 'X';
    ptlocon->TYPE      = EXTERNAL;
    ptlocon->USER      = NULL;
    ptlocon->NEXT      = FCL_LOCON_LIST;
    ptlocon->PNODE     = NULL;

    FCL_LOCON_LIST = ptlocon;

    ptuser = getptype(ptlosig->USER, LOFIGCHAIN);
    if (ptuser != NULL) {
        ptuser->DATA = addchain(ptuser->DATA, ptlocon);
    }
}

static void
fcl_markconstraints(ptlofig_c, ptlofig_m, nameht)
    lofig_list *ptlofig_c;
    lofig_list *ptlofig_m;
    ht         *nameht;
{
    losig_list *ptlosig;
    chain_list *ptchain, *ptlistchain, *newchain;
    ptype_list *ptuser, *ptptype;
    inffig_list *ifl;

    if ((ifl=getloadedinffig(ptlofig_c->NAME))==NULL)
      ifl=addinffig(ptlofig_c->NAME);

    if ((ptuser = getptype(ptlofig_m->USER, FCL_CMPUP_PTYPE)) != NULL) {
        for (ptlistchain = (chain_list *)ptuser->DATA; ptlistchain; ptlistchain = ptlistchain->NEXT) {
            newchain = NULL;
            for (ptchain = (chain_list *)ptlistchain->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlosig = (losig_list *)gethtitem(nameht, ptchain->DATA);
                if (ptlosig != (losig_list *)EMPTYHT) {
                    newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
                }
                else {
                    printf("FCL: Constrained signal '%s' does not exist\n", (char *)ptchain->DATA);
                }
            }
            inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CMPU, "", newchain, NULL);
//            ifl->LOADED.INF_CMPU = addchain(ifl->LOADED.INF_CMPU, newchain);
        }
    }
    if ((ptuser = getptype(ptlofig_m->USER, FCL_CMPDN_PTYPE)) != NULL) {
        for (ptlistchain = (chain_list *)ptuser->DATA; ptlistchain; ptlistchain = ptlistchain->NEXT) {
            newchain = NULL;
            for (ptchain = (chain_list *)ptlistchain->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlosig = (losig_list *)gethtitem(nameht, ptchain->DATA);
                if (ptlosig != (losig_list *)EMPTYHT) {
                    newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
                }
                else {
                    printf("FCL: Constrained signal '%s' does not exist\n", (char *)ptchain->DATA);
                }
            }
            inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_CMPD, "", newchain, NULL);
//            ifl->LOADED.INF_CMPD = addchain(ifl->LOADED.INF_CMPD, newchain);
        }
    }
    if ((ptuser = getptype(ptlofig_m->USER, FCL_MUXUP_PTYPE)) != NULL) {
        for (ptlistchain = (chain_list *)ptuser->DATA; ptlistchain; ptlistchain = ptlistchain->NEXT) {
            newchain = NULL;
            for (ptchain = (chain_list *)ptlistchain->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlosig = (losig_list *)gethtitem(nameht, ptchain->DATA);
                if (ptlosig != (losig_list *)EMPTYHT) {
                    newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
                }
                else {
                    printf("FCL: Constrained signal '%s' does not exist\n", (char *)ptchain->DATA);
                }
            }
            inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_MUXU, "", newchain, NULL);
//            ifl->LOADED.INF_MUXU = addchain(ifl->LOADED.INF_MUXU, newchain);
        }
    }
    if ((ptuser = getptype(ptlofig_m->USER, FCL_MUXDN_PTYPE)) != NULL) {
        for (ptlistchain = (chain_list *)ptuser->DATA; ptlistchain; ptlistchain = ptlistchain->NEXT) {
            newchain = NULL;
            for (ptchain = (chain_list *)ptlistchain->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlosig = (losig_list *)gethtitem(nameht, ptchain->DATA);
                if (ptlosig != (losig_list *)EMPTYHT) {
                    newchain = addchain(newchain, ptlosig->NAMECHAIN->DATA);
                }
                else {
                    printf("FCL: Constrained signal '%s' does not exist\n", (char *)ptchain->DATA);
                }
            }
            inf_AddList(INF_FIG, INF_LOADED_LOCATION, INF_MUXD, "", newchain, NULL);
//            ifl->LOADED.INF_MUXD = addchain(ifl->LOADED.INF_MUXD, newchain);
        }
    }
    if ((ptuser = getptype(ptlofig_m->USER, FCL_NETOUTPUT_PTYPE)) != NULL) {
        for (ptptype = (ptype_list *)ptuser->DATA; ptptype; ptptype = ptptype->NEXT) {
            ptlosig = (losig_list *)gethtitem(nameht, ptptype->DATA);
            if (ptlosig != (losig_list *)EMPTYHT) {
                ptlosig->USER = addptype(ptlosig->USER, YAG_VAL_S_PTYPE, (void *)ptptype->TYPE);
            }
            else {
                printf("FCL: Oriented signal '%s' does not exist\n", (char *)ptptype->DATA);
            }
        }
    }
}

