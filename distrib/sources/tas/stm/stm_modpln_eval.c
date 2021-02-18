/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modpln_eval.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/****************************************************************************/
/* function stm_pln_eval_rec()                                              */
/*                                                                          */
/* fonction recursive d'evaluation d'un polynome                           */
/****************************************************************************/
double stm_pln_eval_rec(chain_list *VAR, chain_list *DEG, chain_list **COEF_DEF, float *COEF, long *k)
{
    long    i;
    long    max_deg;
    long    coef;
    double  value;
    double  sum = 0.0;

    if((VAR == NULL) || (DEG == NULL)){
        coef = (long)((*COEF_DEF)->DATA);
        *COEF_DEF = (*COEF_DEF)->NEXT;

        /* si coef present, alors on retourne sa valeur */
        /* sinon, on retourne 0 */
        if(coef){
            *k = *k + 1;
            return (double)(COEF[*k - 1]);
        }else{
            return 0.0;
        }
    }
    max_deg = (long)(DEG->DATA);
    value = (double)((long)(VAR->DATA))/STM_PRESCALE;
    
    for(i=0; i<= max_deg; i++)
        sum += pow(value, i) * stm_pln_eval_rec(VAR->NEXT, DEG->NEXT, COEF_DEF, COEF, k);
        
    return sum;
}

/****************************************************************************/
/* function stm_modpln_slew()                                               */
/*                                                                          */
/* fonction d'evaluation d'un polynome modele de front                      */
/****************************************************************************/
float stm_modpln_slew (timing_polynom *pln, float slew, float load)
{

    chain_list *VAR = NULL;
    chain_list *DEG = NULL;
    chain_list *COEF_DEF = NULL;
    float      *COEF = pln->COEF_TAB;
    double      sum = 0;
    long        k = 0;
    long        i = 0;
    long        j = 0;
    long        *coef_def = pln->COEF_DEF;
    long        nbvar_degree = pln->DEG_NBVAR;
    long        nbvar;
    long        nbvarmask;
    long        degree;
    long        coef;
    long        nbcoef = 1;
    
    if (!pln)
        return 0.0;

    /* masque pour le nombre de variables */
    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = nbvar_degree & nbvarmask;
    
    /* masque pour le degre de chaque variables */
    nbvar_degree = nbvar_degree >> (sizeof(long) * 4);
    
    /* remplissage de la chain_list de degres */
    for(i = 0; i < nbvar; i++){
        degree = nbvar_degree & STM_DEGMASK;
        DEG = addchain( DEG, (void*)degree);
        nbvar_degree = nbvar_degree >> STM_DEGSHIFT;
        nbcoef *= degree + 1;
    }

    /* remplissage de la chain_list de position des coefs */
    for(i = 0; i < nbcoef; i++){
        coef = coef_def[j] & STM_COEF_DEFMASK;
        COEF_DEF = addchain( COEF_DEF, (void*)coef);
        coef_def[j] = coef_def[j] >> 1;
        if(!((i + 1) % (sizeof(long) * 8)))
            j++;
    }
    COEF_DEF = reverse(COEF_DEF);
    
    /* remplissage de la chain_list de valeurs des variables */
    VAR = addchain( VAR, (void*)((long)(slew*STM_PRESCALE)));
    VAR = addchain( VAR, (void*)((long)(load*STM_PRESCALE)));

    /* appel a la fonction recurssive */
    sum = stm_pln_eval_rec(VAR, DEG, &COEF_DEF, COEF, &k);

    freechain(VAR);
    freechain(COEF_DEF);
    freechain(DEG);

    return sum;  /* slew evaluation */
}

/****************************************************************************/
/* function stm_modpln_delay()                                              */
/*                                                                          */
/* fonction d'evaluation d'un polynome modele de delai                      */
/****************************************************************************/
float stm_modpln_delay (timing_polynom *pln, float slew, float load)
{

    chain_list *VAR = NULL;
    chain_list *DEG = NULL;
    chain_list *COEF_DEF = NULL;
    float      *COEF = pln->COEF_TAB;
    double      sum = 0;
    long        k = 0;
    long        i = 0;
    long        j = 0;
    long        *coef_def = pln->COEF_DEF;
    long        nbvar_degree = pln->DEG_NBVAR;
    long        nbvar;
    long        nbvarmask;
    long        degree;
    long        coef;
    long        nbcoef = 1;
    long        coefdef_inter = coef_def[j];
    
    if (!pln)
        return 0.0;

    /* masque pour le nombre de variables */
    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = nbvar_degree & nbvarmask;
    
    /* masque pour le degre de chaque variables */
    nbvar_degree = nbvar_degree >> (sizeof(long) * 4);

    /* remplissage de la chain_list de degres */
    for(i = 0; i < nbvar; i++){
        degree = nbvar_degree & STM_DEGMASK;
        DEG = addchain( DEG, (void*)degree);
        nbvar_degree = nbvar_degree >> STM_DEGSHIFT;
        nbcoef *= degree + 1;
    }

    /* remplissage de la chain_list de position des coefs */
    for(i = 0; i < nbcoef; i++){
        coef = coefdef_inter & STM_COEF_DEFMASK;
        COEF_DEF = addchain( COEF_DEF, (void*)coef);
        coefdef_inter = coefdef_inter >> 1;
        if(!((i + 1) % (sizeof(long) * 8))){
            j++;
            coefdef_inter = coef_def[j];
        }
    }
    COEF_DEF = reverse(COEF_DEF);
    
    /* remplissage de la chain_list de valeurs des variables */
    VAR = addchain( VAR, (void*)((long)(slew*STM_PRESCALE)));
    VAR = addchain( VAR, (void*)((long)(load*STM_PRESCALE)));

    /* appel a la fonction recurssive */
    sum = stm_pln_eval_rec(VAR, DEG, &COEF_DEF, COEF, &k);

    freechain(VAR);
    freechain(COEF_DEF);
    freechain(DEG);

    return sum; /* delay evaluation */
}

/****************************************************************************/
/* function stm_modpln_constraint()                                         */
/*                                                                          */
/* fonction d'evaluation d'un polynome modele de contrainte                 */
/****************************************************************************/
float stm_modpln_constraint (timing_polynom *pln, float inslew, float ckslew)
{
    chain_list *VAR = NULL;
    chain_list *DEG = NULL;
    chain_list *COEF_DEF = NULL;
    float      *COEF = pln->COEF_TAB;
    double      sum = 0;
    long        k = 0;
    long        i = 0;
    long        j = 0;
    long        *coef_def = pln->COEF_DEF;
    long        nbvar_degree = pln->DEG_NBVAR;
    long        nbvar;
    long        nbvarmask;
    long        degree;
    long        coef;
    long        nbcoef = 1;
    long        coefdef_inter = coef_def[j];
    
    if (!pln)
        return 0.0;

    /* masque pour le nombre de variables */
    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = nbvar_degree & nbvarmask;
    
    /* masque pour le degre de chaque variables */
    nbvar_degree = nbvar_degree >> (sizeof(long) * 4);
    
    /* remplissage de la chain_list de degres */
    for(i = 0; i < nbvar; i++){
        degree = nbvar_degree & STM_DEGMASK;
        DEG = addchain( DEG, (void*)degree);
        nbvar_degree = nbvar_degree >> STM_DEGSHIFT;
        nbcoef *= degree + 1;
    }

    /* remplissage de la chain_list de position des coefs */
    for(i = 0; i < nbcoef; i++){
        coef = coefdef_inter & STM_COEF_DEFMASK;
        COEF_DEF = addchain( COEF_DEF, (void*)coef);
        coefdef_inter = coefdef_inter >> 1;
        if(!((i + 1) % (sizeof(long) * 8))){
            j++;
            coefdef_inter = coef_def[j];
        }
    }
    COEF_DEF = reverse(COEF_DEF);
    
    /* remplissage de la chain_list de valeurs des variables */
    VAR = addchain( VAR, (void*)((long)(inslew*STM_PRESCALE)));
    VAR = addchain( VAR, (void*)((long)(ckslew*STM_PRESCALE)));
    
    /* appel a la fonction recurssive */
    sum = stm_pln_eval_rec(VAR, DEG, &COEF_DEF, COEF, &k);

    freechain(VAR);
    freechain(COEF_DEF);
    freechain(DEG);

    return sum; /* constraint evaluation */
}
