/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modpln_print.c                                          */
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
/* function stm_modpln_print()                                              */
/*                                                                          */
/* drive un modele polynome                                                 */
/****************************************************************************/
void stm_modpln_print (FILE *f, timing_polynom *pln)
{
    float      *COEF = pln->COEF_TAB;
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
    double      valcoef;
    long        coefdef_inter = coef_def[j];
    
    fprintf (f, "\t\tspdm (\n");

    /* masque pour le nombre de variables */
    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = nbvar_degree & nbvarmask;
   
    /* print des seuils de valabilite des variables */
    if(pln->VAR){
        for(i = 0; i < nbvar; i++){
            if(pln->VAR[i]->TYPE == STM_INPUT_SLEW)
                fprintf (f, "\t\t\tinput_slew_axis");
            else if(pln->VAR[i]->TYPE == STM_CLOCK_SLEW)
                fprintf (f, "\t\t\tclock_slew_axis");
            else if(pln->VAR[i]->TYPE == STM_LOAD)
                fprintf (f, "\t\t\tload_axis");
            fprintf (f, " varthmin (%.2f)", pln->VAR[i]->THMIN);
            fprintf (f, " varthmax (%.2f)\n", pln->VAR[i]->THMAX);
        }
    }    
    
    
    /* masque pour le degre de chaque variables */
    nbvar_degree = nbvar_degree >> (sizeof(long) * 4);

    /* print des degres de chaque variables */
    fprintf (f, "\t\t\torders (");
    for(i = 0; i < nbvar; i++){
        degree = nbvar_degree & STM_DEGMASK;
        if (i != nbvar - 1)
            fprintf (f, "%ld ",degree);
        else
            fprintf (f, "%ld",degree);
        nbvar_degree = nbvar_degree >> STM_DEGSHIFT;
        nbcoef *= degree + 1;
    }
    fprintf (f, ")\n");
    
    /* print des coefs */
    fprintf (f, "\t\t\tcoefs (");
    for(i = 0; i < nbcoef; i++){
        coef = coefdef_inter & STM_COEF_DEFMASK;
        if(coef){
            valcoef = COEF[k]; /* si coef present, print du coef */
            k++;
        }else{
            valcoef = 0.0; /* sinon on met un 0 */
        }
        if (i != nbcoef - 1)
            fprintf (f, "%.4g ",valcoef);
        else
            fprintf (f, "%.4g",valcoef);
        
        /* mise a la ligne tous les 10 coefs */
        if(!((i+1)%10))
            fprintf (f, "\n\t\t\t       ");

        /* passage au coef suivant */
        coefdef_inter = coefdef_inter >> 1;

        /* si nbcoef > size of long alors passage au long suivant*/
        if(!((i + 1) % (sizeof(long) * 8))){
            j++;
            coefdef_inter = coef_def[j];
        }
    }
    fprintf (f, ")\n");
    
    fprintf (f, "\t\t)\n");
    
}


