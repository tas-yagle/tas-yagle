/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_phase1.c                                                */
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

partition_list *init_phase1();
partition_list *phase1();
partition_list *relabel_sig();
partition_list *relabel_trs();
partition_list *corrompre_trs();
partition_list *corrompre_sig();
void           *getcle();


/*
   ####====================================================================####
   ##                                                                        ##
   ##                             VARIABLES GLOBALES                         ##
   ##                                                                        ##
   ####====================================================================####
 */

static chain_list     *ptchain_losig = NULL;        /* List des signaux corrompus a traiter */
static chain_list     *ptchain_lotrs = NULL;        /* List des trs corrompus a traiter     */


/*
   ####====================================================================####
   ##                                                                        ##
   ##                           init_phase1()                                ##
   ##                                                                        ## 
   ## Initialise les labels des vecteurs d'une netlist. Le poid des labels   ##
   ## est fonction pour les transistors de son type (Tn ou Tp) et pour les   ##
   ## signaux du nombre de connexion. Les connecteurs ont un label qui       ##
   ## depend du type de connecteur (Drain, Source, ...)                      ##
   ####====================================================================####
   ##  entree : pointeur sur une liste de logfig (memoire ou circuit )       ##
   ##         : caractere du type de liste (M=memoire, C=Circuit)            ## 
   ##  sortie : pointeur sur une liste de partition                          ##
   ####====================================================================####
 */
partition_list *
init_phase1(ptlofig, type)
    lofig_list     *ptlofig;
    char            type;
{
    losig_list     *ptlosig = NULL;
    lotrs_list     *ptlotrs = NULL;
    locon_list     *ptlocon = NULL;
    ptype_list     *ptype = NULL;
    partition_list *ptpartition = NULL;
    fcl_label       i;

    /*  scan lotrs list  */
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        ptype = getptype(ptlotrs->USER, FCL_LABEL_PTYPE);

        /* for each transistor init his LABEL */
        if (ptype == (ptype_list *) NULL) {
            if (MLO_IS_TRANSN(ptlotrs->TYPE))
                ptlotrs->USER = addptype(ptlotrs->USER, FCL_LABEL_PTYPE, (void *)(long) POID_TN);
            else
                ptlotrs->USER = addptype(ptlotrs->USER, FCL_LABEL_PTYPE, (void *)(long) POID_TP);
        }
        else {
            if (MLO_IS_TRANSN(ptlotrs->TYPE))
                ptype->DATA = (void *)(long) POID_TN;
            else
                ptype->DATA = (void *)(long) POID_TP;
        }

        /* and save the trs in the corresponding partition    */
        ptpartition = addpartition(ptpartition, ptlotrs, getlabeltrs(ptlotrs), 'T');

        /* shared transistors corrupt neighbouring signals */
        ptype = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE);
        if (ptype != NULL) {
            if (((long)ptype->DATA & FCL_SHARE_TRANS) != 0) {
                fclCorruptLosig(ptlotrs->SOURCE->SIG);
                fclCorruptLosig(ptlotrs->DRAIN->SIG);
                fclCorruptLosig(ptlotrs->GRID->SIG);
                if (ptlotrs->BULK && ptlotrs->BULK->SIG) fclCorruptLosig(ptlotrs->BULK->SIG);
            }
        }

        /* To avoid using bulk signals as key vectors when ignoring bulk */
        if (type == 'M' && SPI_IGNORE_BULK == 'Y' && ptlotrs->BULK && ptlotrs->BULK->SIG) {
            fclCorruptLosig(ptlotrs->BULK->SIG);
        }
    }

    /*  scan signal list  */
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        if (ptlosig->TYPE == 'I') { /* LE SIGNAL EST INTERNE */

            /* Corrupted memory signals correspond to shared transistors */
            if (type == 'C' || getlabelsig(ptlosig) != FCL_CORRUPT) {
                /* for each signal count the number of connection */
                ptype = getptype(ptlosig->USER, (long) LOFIGCHAIN);
                i = CountChain((chain_list *) ptype->DATA);
    
                /* and save this value */
                ptype = getptype(ptlosig->USER, FCL_LABEL_PTYPE);
                if (ptype == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, FCL_LABEL_PTYPE, (void *)(long)i);
                }
                else {
                    ptype->DATA = (void *)(long) i;
                }
    
                /* and save the signal in the corresponding partition */
                ptpartition = addpartition(ptpartition, ptlosig, i, 'S');
            }
        }
        else { /* LE SIGNAL EST EXTERNE */

            if (type == 'M') {
                /* Le signal est externe, il est donc corrompu */
                /* Le signal est marque corrompu et ajoute a   */
                /* la liste des signaux corrompus              */
                fclCorruptLosig(ptlosig);
                ptchain_losig = addchain(ptchain_losig, (void *) ptlosig);
            }
            else {
                /* for each signal count the number of connection */
                ptype = getptype(ptlosig->USER, (long) LOFIGCHAIN);
                i = CountChain((chain_list *) ptype->DATA);

                /* and save this value */
                ptype = getptype(ptlosig->USER, (long) FCL_LABEL_PTYPE);
                if (ptype == NULL) {
                    ptlosig->USER = addptype(ptlosig->USER, (long) FCL_LABEL_PTYPE, (void *)(long)i);
                }
                else ptype->DATA = (void *)(long)i;
            }
        }

        if (FCL_TRACE_LEVEL > 3) {
            printf("    nb de connexion de %ld '%s' (%c): %d\n", ptlosig->INDEX, getsigname(ptlosig), ptlosig->TYPE, getlabelsig(ptlosig));
        }
    }

    /*  scan locon list from FIG */
    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if (FCL_TRACE_LEVEL > 3) {
            printf("    connecteur %s : %d\n", ptlocon->NAME, getlabellocon(ptlocon));
        }
    }

    return ptpartition;
}

void
clean_phase1(ptlofig)
    lofig_list     *ptlofig;
{
    lotrs_list     *ptlotrs = NULL;
    ptype_list     *ptype;

    freechain(ptchain_losig);
    ptchain_losig = NULL;
    freechain(ptchain_lotrs);
    ptchain_lotrs = NULL;

    /*  scan lotrs list  */
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        ptype = getptype(ptlotrs->USER, (long) FCL_LABEL_PTYPE);
        if (ptype != NULL) ptlotrs->USER = delptype(ptlotrs->USER, FCL_LABEL_PTYPE);
    }
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                              phase1()                                  ##
   ##                                                                        ##
   ## Fait apparaitre des vecteurs candidats dans la netlist du circuit      ##
   ####====================================================================####
   ##  entree : pointeur sur la netlist du circuit                           ##
   ##         : pointeur sur la netlist du point memoire                     ## 
   ##         : pointeur sur le future vecteur cle                           ## 
   ##  sortie : pointeur sur le vecteur cle                                  ##
   ####====================================================================####
 */

static void freepartition(partition_list *ptpartition_c)
{
  while (ptpartition_c) {
     ptpartition_c = delpartition(ptpartition_c, ptpartition_c->LABEL, ptpartition_c->TYPE);
  }
}
partition_list *
phase1(ptlofig_c, ptlofig_m, ptcle)
    lofig_list     *ptlofig_c;
    lofig_list     *ptlofig_m;
    void          **ptcle;
{
    partition_list *ptpartition_c = (partition_list *) NULL;
    partition_list *ptpartition_m = (partition_list *) NULL;
    lotrs_list     *ptlotrs = (lotrs_list *) NULL;
    losig_list     *ptlosig = (losig_list *) NULL;

    if (FCL_TRACE_LEVEL > 4) {
        printf("   initialisation du point memoire\n");
    }
    ptpartition_m = init_phase1(ptlofig_m, 'M');
    if (FCL_TRACE_LEVEL > 4) {
        printf("   initialisation du circuit\n");
    }
    ptpartition_c = init_phase1(ptlofig_c, 'C');

    do {
        if ((ptpartition_c = comparepartition(ptpartition_c, ptpartition_m)) == NULL) {
            /* Pas de vecteurs candidats */
            *ptcle = (void *) NULL;
            freepartition(ptpartition_m);
            return (partition_list *) NULL;
        }

        if (FCL_TRACE_LEVEL > 4) {
            printf("\tcomparer les partitions   :\n");
            printf("CHIP\n");
            printpartition(ptpartition_c);
            printf("MEM\n");
            printpartition(ptpartition_m);
        }


        for (ptlosig = ptlofig_m->LOSIG; ptlosig && (getlabelsig(ptlosig) == FCL_CORRUPT); ptlosig = ptlosig->NEXT);
        if (!ptlosig) {
            for (ptlosig = ptlofig_c->LOSIG; ptlosig && (getpartition(ptpartition_c, getlabelsig(ptlosig), 'S') == NULL); ptlosig = ptlosig->NEXT);
            if (!ptlosig) {
                ptpartition_c = smallestpartition(ptpartition_c);
                *ptcle = getcle(ptpartition_c, ptpartition_m);
                freepartition(ptpartition_m);
                return ptpartition_c;
            }
        }

        if (FCL_TRACE_LEVEL > 4) {
            printf("   traitement des SIGNAUX ........:\n");
            printf("\tCHIP\n");
            printpartition(ptpartition_c);
            printf("\tMEM\n");
            printpartition(ptpartition_m);
        }

        ptpartition_c = relabel_sig(ptlofig_c, ptpartition_c, 'C');
        ptpartition_m = relabel_sig(ptlofig_m, ptpartition_m, 'M');

        if (FCL_TRACE_LEVEL > 4) {
            printf("\tafter signal relabelling       :\n");
            printf("\tCHIP\n");
            printpartition(ptpartition_c);
            printf("\tMEM\n");
            printpartition(ptpartition_m);
        }

        ptpartition_m = corrompre_sig(ptpartition_m);

        if (FCL_TRACE_LEVEL > 4) {
            printf("\tcorrompre les transistors :\n");
            printf("\tMEM\n");
            printpartition(ptpartition_m);
            printf("\tCHIP\n");
            printpartition(ptpartition_c);
        }

        if ((ptpartition_c = comparepartition(ptpartition_c, ptpartition_m)) == NULL) {
            *ptcle = NULL;
            freepartition(ptpartition_m);
            return NULL;
        }
        if (FCL_TRACE_LEVEL > 4) {
            printf("\tcomparer les partitions   :\n");
            printf("\tCHIP\n");
            printpartition(ptpartition_c);
            printf("\tMEM\n");
            printpartition(ptpartition_m);
        }

        for (ptlotrs = ptlofig_m->LOTRS; ptlotrs && (getlabeltrs(ptlotrs) == FCL_CORRUPT); ptlotrs = ptlotrs->NEXT);
        if (!ptlotrs) {
            for (ptlotrs = ptlofig_c->LOTRS; ptlotrs && (getpartition(ptpartition_c, getlabeltrs(ptlotrs), 'T') == NULL); ptlotrs = ptlotrs->NEXT);
            if (!ptlotrs) {
                ptpartition_c = smallestpartition(ptpartition_c);
                *ptcle = getcle(ptpartition_c, ptpartition_m);
                freepartition(ptpartition_m);
                return ptpartition_c;
            }
        }
        if (FCL_TRACE_LEVEL > 4) {
            printf("   traitement des TRS ............:\n");
        }

        ptpartition_c = relabel_trs(ptlofig_c, ptpartition_c, 'C');
        ptpartition_m = relabel_trs(ptlofig_m, ptpartition_m, 'M');

        if (FCL_TRACE_LEVEL > 4) {
            printf("\trelabel les transistors...:\n");
            printf("\tCHIP\n");
            printpartition(ptpartition_c);
            printf("\tMEM\n");
            printpartition(ptpartition_m);
        }

        ptpartition_m = corrompre_trs(ptpartition_m);
        if (FCL_TRACE_LEVEL > 4) {
            printf("\tcorrompre les signaux     :\n");
            printf("MEM\n");
            printpartition(ptpartition_m);
        }

    }
    while (1);
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                              relabel_sig()                             ##
   ##                                                                        ##
   ## Calcul les labels des signaux suivant les labels de ses voisins        ## 
   ####====================================================================####
   ##  entree : pointeur sur une liste de logfig (memoire ou circuit )       ##
   ##         : pointeur sur une liste de partition de vecteur a renommer    ## 
   ##         : caractere du type de liste (M=memoire, C=Circuit)            ## 
   ##  sortie : pointeur sur une liste de partition de vecteurs renommes     ##
   ####====================================================================####
 */
partition_list *
relabel_sig(ptlofig, ptpartition, type)
    lofig_list     *ptlofig;
    partition_list *ptpartition;
    char            type;
{
    losig_list     *ptlosig;
    locon_list     *ptlocon;
    chain_list     *ptchain;
    ptype_list     *ptype;
    fcl_label       newlabel;

    /* On parcourt tous les signaux de la figure */
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        /* Les signaux recalcules sont :
           tous les internes du circuits
           tous les internes du point memoire non corrompus */

        if (ptlosig->TYPE == 'I' && type == 'C' && getpartition(ptpartition, getlabelsig(ptlosig), 'S') == NULL) {
            /* le signal du circuit a ete elimine    */
            /* par l'operation de comparaison        */
            /* Il n'est plus dans aucune partition   */

            /* On recalcule son LABEL       */

            newlabel = 0;
            ptype = getptype(ptlosig->USER, (long) LOFIGCHAIN);
            for (ptchain = (chain_list *) ptype->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlocon = (locon_list *) ptchain->DATA;
                if (ptlocon->TYPE != 'T') continue;

                newlabel += getlabellocon(ptlocon) * getlabeltrs((lotrs_list *) ptlocon->ROOT);
            }
            /* Et on modifie le LABEL de la structure */
            ptype = getptype(ptlosig->USER, (long) FCL_LABEL_PTYPE);
            ptype->DATA = (void *) (long)((fcl_label)(long) (ptype->DATA) + newlabel);
            if (FCL_TRACE_LEVEL > 4) {
                printf("    new label for partitionless signal %ld '%s' (%c): %d\n", ptlosig->INDEX, getsigname(ptlosig), ptlosig->TYPE, getlabelsig(ptlosig));
            }
        }
        else {
            if (ptlosig->TYPE == 'I' && (type == 'C' || getlabelsig(ptlosig) != FCL_CORRUPT)) {
                /* le signal est enleve de sa partition */
                /* puis il est recalcule, et remis dans */
                /* une nouvelle partition               */
                ptpartition = removepartitionelement(ptpartition, getlabelsig(ptlosig), 'S');
                /* On recalcule son LABEL       */

                newlabel = 0;
                ptype = getptype(ptlosig->USER, (long) LOFIGCHAIN);
                for (ptchain = (chain_list *) ptype->DATA; ptchain; ptchain = ptchain->NEXT) {
                    ptlocon = (locon_list *) ptchain->DATA;
                    if (ptlocon->TYPE != 'T') continue;

                    newlabel += getlabellocon(ptlocon) * getlabeltrs((lotrs_list *) ptlocon->ROOT);
                }

                /* Et on modifie le LABEL de la structure */
                ptype = getptype(ptlosig->USER, (long) FCL_LABEL_PTYPE);
                ptype->DATA = (void *) (long)((fcl_label)(long) (ptype->DATA) + newlabel);

                /* Puis on rajoute le signal dans la partition */
                ptpartition = addpartition(ptpartition, (void *) ptlosig, getlabelsig(ptlosig), 'S');
                if (FCL_TRACE_LEVEL > 4) {
                    printf("    new label for signal %ld '%s' (%c): %d\n", ptlosig->INDEX, getsigname(ptlosig), ptlosig->TYPE, getlabelsig(ptlosig));
                }
            }
        }
    }
    return ptpartition;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                              relabel_trs()                             ##
   ##                                                                        ##
   ## Calcul les labels des transistors suivant les labels de ses voisins    ## 
   ####====================================================================####
   ##  entree : pointeur sur une liste de logfig (memoire ou circuit )       ##
   ##         : pointeur sur une liste de partition de vecteur a renommer    ## 
   ##         : caractere du type de liste (M=memoire, C=Circuit)            ## 
   ##  sortie : pointeur sur une liste de partition de vecteurs renommes     ##
   ####====================================================================####
 */
partition_list *
relabel_trs(ptlofig, ptpartition, type)
    lofig_list     *ptlofig;
    partition_list *ptpartition;
    char            type;
{
    lotrs_list     *ptlotrs;
    ptype_list     *ptype;
    fcl_label       newlabel;

    /* On parcourt tous les transistors de la figure */
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        /* Les transistors recalcules sont :
           tous ceux du circuit
           tous ceux du point memoire non corrompus */

        if (type == 'C' && getpartition(ptpartition, getlabeltrs(ptlotrs), 'T') == NULL) {
            /* le transi du circuit a ete elimine    */
            /* par l'operation de comparaison        */
            /* Il n'est plus dans aucune partition   */

            /* On recalcule son LABEL       */

            newlabel = getlabellocon(ptlotrs->DRAIN) *
                getlabelsig(ptlotrs->DRAIN->SIG) +
                getlabellocon(ptlotrs->GRID) *
                getlabelsig(ptlotrs->GRID->SIG) +
                getlabellocon(ptlotrs->SOURCE) *
                getlabelsig(ptlotrs->SOURCE->SIG);

            if (ptlotrs->BULK && ptlotrs->BULK->SIG) {
                if (ptlotrs->BULK->SIG->TYPE == 'I') {
                    newlabel += getlabellocon(ptlotrs->BULK) * getlabelsig(ptlotrs->BULK->SIG);
                }
            }

            /* Et on modifie le LABEL de la structure */
            ptype = getptype(ptlotrs->USER, (long) FCL_LABEL_PTYPE);
            ptype->DATA = (void *)(long) ((fcl_label)(long) (ptype->DATA) + newlabel);
        }
        else {
            if (type == 'C' || getlabeltrs(ptlotrs) != FCL_CORRUPT) {
                /* le transi est enleve de sa partition */
                /* puis il est recalcule, et remis dans */
                /* une nouvelle partition               */
                ptpartition = removepartitionelement(ptpartition, getlabeltrs(ptlotrs), 'T');
                /* On recalcule son LABEL       */

                newlabel = getlabellocon(ptlotrs->DRAIN) *
                    getlabelsig(ptlotrs->DRAIN->SIG) +
                    getlabellocon(ptlotrs->GRID) *
                    getlabelsig(ptlotrs->GRID->SIG) +
                    getlabellocon(ptlotrs->SOURCE) *
                    getlabelsig(ptlotrs->SOURCE->SIG);

                if (ptlotrs->BULK && ptlotrs->BULK->SIG) {
                    if (ptlotrs->BULK->SIG->TYPE == 'I') {
                        newlabel += getlabellocon(ptlotrs->BULK) * getlabelsig(ptlotrs->BULK->SIG);
                    }
                }

                /* Et on modifie le LABEL de la structure */
                ptype = getptype(ptlotrs->USER, (long) FCL_LABEL_PTYPE);
                ptype->DATA = (void *)(long) ((fcl_label)(long) (ptype->DATA) + newlabel);

                /* Puis on rajoute le transi dans la partition */
                ptpartition = addpartition(ptpartition, ptlotrs, getlabeltrs(ptlotrs), 'T');
            }
        }
    }
    return ptpartition;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                             corrompre_sig()                            ##
   ##                                                                        ##
   ## Marque corrompu les transistors suivant les signaux deja corrompus     ##
   ####====================================================================####
   ##  entree : pointeur sur une liste de lofig (memoire ou circuit )        ##
   ##         : pointeur sur une liste de partition de vecteur               ## 
   ##  sortie : pointeur sur une liste de partition de vecteur               ##
   ####====================================================================####
 */
partition_list *
corrompre_sig(ptpartition)
    partition_list *ptpartition;
{
    chain_list     *ptchain;
    chain_list     *ptchain2;
    lotrs_list     *ptlotrs;
    losig_list     *ptlosig;
    locon_list     *ptlocon;
    ptype_list     *ptype;

    for (ptchain = ptchain_losig; ptchain; ptchain = ptchain->NEXT) {
        ptlosig = (losig_list *) ptchain->DATA;
        ptype = getptype(ptlosig->USER, (long) LOFIGCHAIN);

        /* Corrompre tous les transistors des connecteurs */
        for (ptchain2 = (chain_list *) ptype->DATA; ptchain2; ptchain2 = ptchain2->NEXT) {
            ptlocon = (locon_list *) (ptchain2->DATA);
            if (ptlocon->TYPE == 'T') {
                /* Si pas 'T' on est sur un connecteur externe */

                if (ptlocon->NAME == MBK_BULK_NAME && (ptlosig->TYPE == CNS_SIGVSS || ptlosig->TYPE == CNS_SIGVDD)) continue;
                
                ptlotrs = (lotrs_list *) (ptlocon->ROOT);
                /* On marque le transistor corrompu */
                if (getlabeltrs(ptlotrs) != FCL_CORRUPT) {

                    /* On l'ajoute a la liste des nouveaux corrompus */
                    ptchain_lotrs = addchain(ptchain_lotrs, ptlotrs);

                    /* On l'enleve de sa partition */
                    ptpartition = removepartitionelement(ptpartition, getlabeltrs(ptlotrs), 'T');

                    fclCorruptLotrs(ptlotrs);
                }
            }
        }
    }

    freechain(ptchain_losig);
    ptchain_losig = NULL;

    return ptpartition;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                             corrompre_trs()                            ##
   ##                                                                        ##
   ## Marque corrompu les signaux suivant les transistors deja corrompus     ##
   ####====================================================================####
   ##  entree : pointeur sur une liste de logfig (memoire ou circuit )       ##
   ##         : pointeur sur une liste de partition de vecteur               ## 
   ##  sortie : pointeur sur une liste de partition de vecteur               ##
   ####====================================================================####
 */
partition_list *
corrompre_trs(ptpartition)
    partition_list *ptpartition;
{
    chain_list     *ptchain;
    losig_list     *ptlosig;
    lotrs_list     *ptlotrs;

    /* pour tous les transistors qui viennent d'etre marques corrompus */
    for (ptchain = ptchain_lotrs; ptchain; ptchain = ptchain->NEXT) {
        /* Corrompre le signal du connecteur DRAIN */
        ptlosig = ((lotrs_list *) (ptchain->DATA))->DRAIN->SIG;

        if (getlabelsig(ptlosig) != FCL_CORRUPT) {

            /* On l'ajoute a la liste des nouveaux corrompus */
            ptchain_losig = addchain(ptchain_losig, ptlosig);

            /* On l'enleve de sa partition */
            ptpartition = removepartitionelement(ptpartition, getlabelsig(ptlosig), 'S');

            fclCorruptLosig(ptlosig);
        }

        /* Corrompre le signal du connecteur SOURCE */
        ptlosig = ((lotrs_list *) (ptchain->DATA))->SOURCE->SIG;

        if (getlabelsig(ptlosig) != FCL_CORRUPT) {

            /* On l'ajoute a la liste des nouveaux corrompus */
            ptchain_losig = addchain(ptchain_losig, ptlosig);

            /* On l'enleve de sa partition */
            ptpartition = removepartitionelement(ptpartition, getlabelsig(ptlosig), 'S');

            fclCorruptLosig(ptlosig);
        }

        /* Corrompre le signal du connecteur GRID */
        ptlosig = ((lotrs_list *) (ptchain->DATA))->GRID->SIG;

        if (getlabelsig(ptlosig) != FCL_CORRUPT) {

            /* On l'ajoute a la liste des nouveaux corrompus */
            ptchain_losig = addchain(ptchain_losig, ptlosig);

            /* On l'enleve de sa partition */
            ptpartition = removepartitionelement(ptpartition, getlabelsig(ptlosig), 'S');

            fclCorruptLosig(ptlosig);
        }

        /* Corrompre le signal du connecteur BULK */
        ptlotrs = (lotrs_list *) (ptchain->DATA);
        if (ptlotrs->BULK && ptlotrs->BULK->SIG) {
            ptlosig = ((lotrs_list *) (ptchain->DATA))->BULK->SIG;

            if (getlabelsig(ptlosig) != FCL_CORRUPT) {
    
                /* On l'ajoute a la liste des nouveaux corrompus */
                ptchain_losig = addchain(ptchain_losig, ptlosig);

                /* On l'enleve de sa partition */
                ptpartition = removepartitionelement(ptpartition, getlabelsig(ptlosig), 'S');

                fclCorruptLosig(ptlosig);
            }
        }
    }

    freechain(ptchain_lotrs);
    ptchain_lotrs = NULL;

    return ptpartition;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                            getcle()                                    ##
   ##                                                                        ##
   ## Renvoi un pointeur sur un vecteur cle                                  ##
   ####====================================================================####
   ##  entree : pointeur sur la partition du circuit                         ##
   ##         : pointeur sur la partition du point memoire                   ##
   ##  sortie : pointeur sur un vecteur cle                                  ##
   ####====================================================================####
 */
void           *
getcle(ptpartition_c, ptpartition_m)
    partition_list *ptpartition_c;
    partition_list *ptpartition_m;
{
    partition_list *ptpartition;
    void           *vect_cle;

    ptpartition = getpartition(ptpartition_m, ptpartition_c->LABEL, ptpartition_c->TYPE);
    vect_cle = (((chain_list *) (ptpartition->DATA))->DATA);
//    freechain((chain_list *) (ptpartition->DATA));

    return (vect_cle);

}
