/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_partition.c                                             */
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
  ##                           add_partition()                              ##
  ##                                                                        ##
  ## Ajoute un element dans une partition, si cette partition n'existe pas  ##
  ## elle est alors creee                                                   ##
  ##                                                                        ##
  ####====================================================================####
  ##  entree : pointeur sur la liste des partitions                         ##
  ##         : pointeur sur la structure de l'element ajoute                ##
  ##         : valeur du label de l'element                                 ##
  ##         : type de l'element (TRS ou SIG)                               ##
  ##  sortie : un pointeur sur le debut des partitions                      ##
  ####====================================================================####
*/
partition_list *
addpartition(ptpartition, ptdata, valeur, type)
    partition_list *ptpartition;
    void           *ptdata;
    fcl_label       valeur;
    char            type;
{
    partition_list *pt;

    pt = getpartition(ptpartition, valeur, type);

    if (pt == (partition_list *) NULL) {
        pt = (partition_list *) mbkalloc(sizeof(partition_list));

        pt->NEXT = ptpartition;
        pt->LABEL = valeur;
        pt->NBELEM = 0;
        pt->TYPE = type;
        pt->DATA = (void *) NULL;
        ptpartition = pt;
    }

    pt->DATA = (void *) addchain((chain_list *) pt->DATA, ptdata);
    pt->NBELEM++;

    return ptpartition;
}


/*
  ####====================================================================####
  ##                                                                        ##
  ##                         small_partition()                              ##
  ##                                                                        ##
  ## renvoie un pointeur sur la partition ayant le moins d'elements         ##
  ## Les autres partitions sont detruites                                   ##
  ##                                                                        ##
  ####====================================================================####
  ##  entree : pointeur sur la liste des partitions                         ##
  ##  sortie : un pointeur sur le debut des partitions                      ##
  ####====================================================================####
*/
partition_list *
smallestpartition(ptpartition)
    partition_list *ptpartition;
{
    for (; ptpartition->NEXT;) {
        if (ptpartition->NEXT->NBELEM < ptpartition->NBELEM) {
            ptpartition = delpartition(ptpartition, ptpartition->LABEL, ptpartition->TYPE);
        }
        else {
            ptpartition = delpartition(ptpartition, ptpartition->NEXT->LABEL, ptpartition->NEXT->TYPE);
        }
    }

    return ptpartition;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                           get_partition()                              ##
  ##                                                                        ##
  ## Recherche une partition suivant un label partitculier                  ##
  ##                                                                        ##
  ####====================================================================####
  ##  entree : pointeur sur la liste des partitions                         ##
  ##         : valeur du label de l'element                                 ##
  ##         : type de l'element (TRS ou SIG)                               ##
  ##  sortie : un pointeur sur la partition                                 ##
  ####====================================================================####
*/
partition_list *
getpartition(ptpartition, valeur, type)
    partition_list *ptpartition;
    fcl_label       valeur;
    char            type;
{
    partition_list *pt;

    for (pt = ptpartition; pt; pt = pt->NEXT)
        if (pt->LABEL == valeur && pt->TYPE == type)
            return pt;

    return (partition_list *) NULL;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                           delpartition()                               ##
  ##                                                                        ##
  ## Supprime  une partition suivant un label partitculier                  ##
  ##                                                                        ##
  ####====================================================================####
  ##  entree : pointeur sur la liste des partitions                         ##
  ##         : valeur du label de l'element                                 ##
  ##         : type de l'element (TRS ou SIG)                               ##
  ##  sortie : un pointeur sur la tete des partitions                       ##
  ####====================================================================####
*/
partition_list *
delpartition(ptpartition, valeur, type)
    partition_list *ptpartition;
    fcl_label       valeur;
    char            type;
{
    partition_list *pt;
    partition_list *pt_before = ptpartition;

    for (pt = ptpartition; pt; pt_before = pt, pt = pt->NEXT) {
        if (pt->LABEL == valeur && pt->TYPE == type) {
            if (pt_before == pt)
                /* On veut enlever l'element de tete */
                ptpartition = pt->NEXT;
            else
                pt_before->NEXT = pt->NEXT;

            freechain((chain_list *) pt->DATA);

            mbkfree(pt);
            return ptpartition;
        }
    }
    return ptpartition;
}

/*
  ####====================================================================####
  ##                                                                        ##
  ##                         removepartitionelement()                       ##
  ##                                                                        ##
  ## Supprime un element dans une partition, si cette partition est vide    ##
  ## elle est alors detruite                                                ##
  ##                                                                        ##
  ####====================================================================####
  ##  entree : pointeur sur la liste des partitions                         ##
  ##         : pointeur sur la structure de l'element enleve                ##
  ##         : valeur du label de l'element                                 ##
  ##         : type de l'element (TRS ou SIG)                               ##
  ##  sortie : un pointeur sur le debut des partitions                      ##
  ####====================================================================####
*/
partition_list *
removepartitionelement(ptpartition, valeur, type)
    partition_list *ptpartition;
    fcl_label       valeur;
    char            type;
{
    partition_list *pt;

    pt = getpartition(ptpartition, valeur, type);
    if (!--pt->NBELEM) ptpartition = delpartition(ptpartition, valeur, type);

    return ptpartition;
}


/*
  ####====================================================================####
  ##                                                                        ##
  ##                        comparepartition()                              ##
  ##                                                                        ##
  ## Compare les deux listes de partitions (model, circuit)                 ##
  ## Les partitions du circuit qui n'existent pas dans le model sont        ##
  ## supprime; Verifie qu'a chaque partition du model correspond une        ##
  ## partition dans le circuit                                              ##
  ##                                                                        ##
  ####====================================================================####
  ##  entree : pointeur sur la liste des partitions                         ##
  ##         : pointeur sur la structure de l'element enleve                ##
  ##         : valeur du label de l'element                                 ##
  ##         : type de l'element (TRS ou SIG)                               ##
  ##  sortie : un pointeur sur le debut des partitions                      ##
  ####====================================================================####
*/
partition_list *
comparepartition(ptpartition_c, ptpartition_m)
    partition_list *ptpartition_c;
    partition_list *ptpartition_m;
{
    partition_list *pt;
    partition_list *ptcandidat;
    partition_list *ptnext;
    chain_list     *ptchain;
    losig_list     *ptlosig;

    /* Supprimer toutes les partitions du circuit pour lesquelles */
    /* des partitions de poids identiques n'existent pas dans le  */
    /* graphe de la cellule memoire                               */
    /* Marquer corrompu les signaux  ou les transitors de ces     */
    /* partitions supprimees                                      */

    for (pt = ptpartition_c; pt; pt = ptnext) {
        if ((ptcandidat = getpartition(ptpartition_m, pt->LABEL, pt->TYPE)) == NULL || pt->LABEL == 0) {
            ptnext = pt->NEXT;
            ptpartition_c = delpartition(ptpartition_c, pt->LABEL, pt->TYPE);
        }
        else ptnext = pt->NEXT;
    }

    /* Verifie qu'a chaque partition du sous-graphe   */
    /* il existe une partition de taille au moins     */
    /* egale dans le graphe                           */
    for (pt = ptpartition_m; pt; pt = pt->NEXT) {
        if (pt->LABEL == 0 && pt->TYPE == 'S') {
            for (ptchain = pt->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptlosig = (losig_list *) (ptchain->DATA);
                fclCorruptLosig(ptlosig);
            }
        }
        else if ((ptcandidat = getpartition(ptpartition_c, pt->LABEL, pt->TYPE)) == NULL || ptcandidat->NBELEM < pt->NBELEM) {
            /* Pas d'isomorphisme                      */
            if (FCL_TRACE_LEVEL > 1) {
                printf("*** FCL Match Failed as partition label '%d' in model has no correspondance\n", pt->LABEL);
                displaypartition(pt);
            }

            while (ptpartition_c) {
                ptpartition_c = delpartition(ptpartition_c, ptpartition_c->LABEL, ptpartition_c->TYPE);
            }
            break;
        }
    }

    return ptpartition_c;
}


void
printpartition(ptpartition)
    partition_list *ptpartition;
{
    partition_list *pt;

    for (pt = ptpartition; pt; pt = pt->NEXT) {
        printf("Partition Type=%c label=%d nb elements=%d\n", pt->TYPE, pt->LABEL, pt->NBELEM);
    }
}

void
displaypartition(ptpartition)
    partition_list *ptpartition;
{
    losig_list     *ptsig;
    chain_list     *ptchain;

    if (ptpartition->TYPE == 'T')
        for (ptchain = ptpartition->DATA; ptchain; ptchain = ptchain->NEXT) {
            ptsig = ((lotrs_list *) ptchain->DATA)->GRID->SIG;
            if (ptsig->NAMECHAIN == NULL)
                printf("#### Vecteur candidat G : %ld\n", ptsig->INDEX);
            else
                printf("#### Vecteur candidat G : %s\n",
                       (char *) ptsig->NAMECHAIN->DATA);

            ptsig = ((lotrs_list *) ptchain->DATA)->SOURCE->SIG;
            if (ptsig->NAMECHAIN == NULL)
                printf("#### Vecteur candidat S : %ld\n", ptsig->INDEX);
            else
                printf("#### Vecteur candidat S : %s\n",
                       (char *) ptsig->NAMECHAIN->DATA);

            ptsig = ((lotrs_list *) ptchain->DATA)->DRAIN->SIG;
            if (ptsig->NAMECHAIN == NULL)
                printf("#### Vecteur candidat D : %ld\n", ptsig->INDEX);
            else
                printf("#### Vecteur candidat D : %s\n",
                       (char *) ptsig->NAMECHAIN->DATA);
        }
    else {
        for (ptchain = ptpartition->DATA; ptchain; ptchain = ptchain->NEXT) {
            ptsig = (losig_list *) ptchain->DATA;
            if (ptsig->NAMECHAIN == NULL)
                printf("#### Vecteur candidat : %ld\n", ptsig->INDEX);
            else
                printf("#### Vecteur candidat : %s\n",
                       (char *) ptsig->NAMECHAIN->DATA);
        }
    }

}
