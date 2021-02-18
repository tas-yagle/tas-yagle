/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_findcell.c                                              */
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


/*
   ####====================================================================####
   ##                                                                        ##
   ##                            find_mem()                                  ##
   ##                                                                        ##
   ## Recherche d'un sous-graphe dans un graphe representant des netlists du ##
   ## point de vue transistor                                                ##
   ####====================================================================####
   ##  entree : pointeur sur la netlist logique du circuit                   ##
   ##         : pointeur sur la netlist logique du point memorisant          ##
   ##           a rechercher                                                 ##
   ##  sortie : pointeur sur la netlist logique du circuit avec les          ##
   ##           elements des points memoires marques                         ## 
   ####====================================================================####
 */
void 
fclFind(ptlofig_c, ptlofig_m, ptbefig_m)
    lofig_list     *ptlofig_c;
    lofig_list     *ptlofig_m;
    befig_list     *ptbefig_m;
{
    partition_list *ptsolution = NULL;
    void           *ptcle = NULL;
    time_t          start = 0;
    time_t          end = 0;
    struct rusage   END;
    struct rusage   START; 

    if (FCL_TRACE_LEVEL > 2) {
        printf("  PHASE 1 :\n");
        printf("   recherche des meilleurs candidats\n");
    }

    fclChrono(&START, &start);
    ptsolution = phase1(ptlofig_c, ptlofig_m, &ptcle);
    if (FCL_TRACE_LEVEL > 0) {
        printf("\n[FCL] Execution of Phase 1 took: ");
        fclChrono(&END,&end);
        fclPrintTime(&START,&END,start,end);
    }
    clean_phase1(ptlofig_c);
    clean_phase1(ptlofig_m);

    /* Test si des vecteurs candidats existent */
    if (ptsolution != (partition_list *) NULL) {

        if (FCL_TRACE_LEVEL > 2) {
            printf("####\n");
            if (ptsolution->TYPE == 'T') {
                printf("#### Vecteur cle      G : %ld\n",
                       ((lotrs_list *) ptcle)->GRID->SIG->INDEX);
                printf("####                  S : %ld\n",
                       ((lotrs_list *) ptcle)->SOURCE->SIG->INDEX);
                printf("####                  D : %ld\n",
                       ((lotrs_list *) ptcle)->DRAIN->SIG->INDEX);
            }
            else {
                printf("#### Vecteur cle        : %ld\n",
                       (((losig_list *) ptcle)->INDEX));
            }
            printf("####\n");
            printf("#### Nombre de vecteurs candidats : %d (Label=%d)\n",
                   ptsolution->NBELEM,
                   ptsolution->LABEL);
            displaypartition(ptsolution);
            printf("####\n");

            printf("  PHASE 2 :\n");
        }

        /* On lance la deuxieme phase */
        fclChrono(&START, &start);
        fclPhase2(ptlofig_c, ptlofig_m, ptbefig_m, ptsolution, ptcle, FALSE);
        if (FCL_TRACE_LEVEL > 0) {
            printf("\n[FCL] Execution of Phase 2 took: ");
            fclChrono(&END,&end);
            fclPrintTime(&START,&END,start,end);
        }

        delpartition(ptsolution, ptsolution->LABEL, ptsolution->TYPE);
    }
    else {
        if (FCL_TRACE_LEVEL > 2)
            printf("Pas de vecteurs candidats\n");
    }

}
