/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modpln.c                                                */
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
/* function stm_modpln_duplicate()                                          */
/*                                                                          */
/****************************************************************************/
timing_polynom *stm_modpln_duplicate (timing_polynom *pln)
{
    long deg_nbvar = pln->DEG_NBVAR;
    long nbvar;
    long nbvarmask;
    long degree[100];
    long i;
    
    if (!pln)
        return NULL;
    
    /* masque pour le nombre de variables */
    nbvarmask = ULONG_MAX >> (sizeof(long) * 4);
    nbvar = deg_nbvar & nbvarmask;
  
    /* masque pour le degre de chaque variables */
    deg_nbvar = deg_nbvar >> (sizeof(long) * 4);
    for(i = 0; i < nbvar; i++){
        degree[i] = deg_nbvar & STM_DEGMASK;
        deg_nbvar = deg_nbvar >> STM_DEGSHIFT;
    }

    return stm_modpln_create (nbvar, degree, pln->COEF_DEF, pln->COEF_TAB, pln->VAR);
}

/****************************************************************************/
/* function stm_modpln_create()                                             */
/*                                                                          */
/* creation d'un polynome a partir du nb de variables, du tableau contenant */
/* le degre de chaque variables, du tableau de position des coefs et du     */
/* tableau de valeurs des coefs.                                            */
/****************************************************************************/
timing_polynom *stm_modpln_create (long nbvar, long *degree, long *coef_def, float *coef, timing_varpolynom **var)
{
    timing_polynom *pln = (timing_polynom*)mbkalloc (sizeof (struct timing_polynom));
    long deg_nbvar = 0;
    long nbcoef = 1;
    long nbcoefreal = 0;
    long i = 0;
    long j = 0;
    long poscoef = 0;
    long coefdef_inter = coef_def[j];

    /****************************************/
    /*  ______________long_______________   */
    /* |     deg_var    |     nb_var     |  */
    /* |__________001010|______________10|  */
    /*                                      */
    /* degre de chaque variables sur 3 bits */
    /* ici on a 2 variables, la 1ere        */
    /* d'ordre 2 et la 2eme d'ordre 1       */
    /****************************************/
    
    /* creation du demi-long deg_var */
    for(i = nbvar - 1; i >= 0; i--){
        deg_nbvar = deg_nbvar | degree[i];
        deg_nbvar = deg_nbvar << STM_DEGSHIFT;
        nbcoef *= degree[i] + 1;
    }
    deg_nbvar = deg_nbvar >> STM_DEGSHIFT;
    
    /* ajout du nombre de variables */
    deg_nbvar = deg_nbvar << (sizeof(long) * 4);
    deg_nbvar = deg_nbvar | nbvar;
    
    pln->DEG_NBVAR =  deg_nbvar;
    
    /* allocation du tableau var en fonction du nb de var */
    if(var){
        pln->VAR = (timing_varpolynom**)mbkalloc (nbvar * sizeof (struct timing_varpolynom));
        for(i = 0; i < nbvar; i++){
            if(var[i]){
                pln->VAR[i] = var[i];
            }else
                break;
        }
    }else{
        pln->VAR = NULL;
    }

    /* allocation du tableau coef_def en fonction du nb de coefs */
    if(!(nbcoef % (sizeof(long) * 8))){
        pln->COEF_DEF = (long*)mbkalloc ( ((nbcoef / (sizeof(long) * 8))) * sizeof (long));
        for(i = 0; i < (long)(nbcoef / (sizeof(long) * 8)); i++)
            pln->COEF_DEF[i] = coef_def[i];
    }else{
        pln->COEF_DEF = (long*)mbkalloc ( ((nbcoef / (sizeof(long) * 8)) + 1) * sizeof (long));
        for(i = 0; i < (long)(nbcoef / (sizeof(long) * 8)) + 1; i++)
            pln->COEF_DEF[i] = coef_def[i];
    }
    
    /****************************************/
    /*  ______________long_______________   */
    /* |           coef_def[i]           |  */
    /* |000000000000000000000000000111111|  */
    /*                                      */
    /* un 1 indique que le coef est present */
    /* ici les 6 premiers coefs doivent etre*/
    /* pris en compte. Avec la meme config  */
    /* que ci-dessus, on a:                 */
    /* a + bx + cx² + dy + exy + fx²y       */
    /* a,b,c,d,e et f etant les coefs.      */
    /* COEF_TAB[0] = a                      */
    /* COEF_TAB[1] = b                      */
    /* COEF_TAB[2] = c                      */
    /* COEF_TAB[3] = d                      */
    /* COEF_TAB[4] = e                      */
    /* COEF_TAB[5] = f                      */
    /****************************************/
   
    for(i = 0; i < nbcoef; i++){
        poscoef = coefdef_inter & STM_COEF_DEFMASK;
        if(poscoef)
            nbcoefreal++; /* si coef present, print du coef */
        
        /* passage au coef suivant */
        coefdef_inter = coefdef_inter >> 1;

        /* si nbcoef > size of long alors passage au long suivant*/
        if(!((i + 1) % (sizeof(long) * 8))){
            j++;
            coefdef_inter = coef_def[j];
        }
    }
    pln->COEF_TAB = (float*)mbkalloc ( (nbcoefreal)* sizeof (float));
        for(i = 0; i < nbcoefreal; i++)
            pln->COEF_TAB[i] = coef[i];

    return pln;
}

/****************************************************************************/
/* function stm_modpln_destroy()                                            */
/*                                                                          */
/****************************************************************************/
void stm_modpln_destroy (timing_polynom *pln)
{
    if (!pln)
        return;

    if(pln->VAR)
        mbkfree(pln->VAR);
    if(pln->COEF_DEF)
        mbkfree(pln->COEF_DEF);
    if(pln->COEF_TAB)
        mbkfree(pln->COEF_TAB);
    
    mbkfree (pln);
}

/****************************************************************************/
/* function stm_modpln_create_fequa()                                       */
/*                                                                          */
/* creation d'un polynome a partir du parametre de front s de la resistance */
/* r, du delai t dans la line, de la capa c0 et du front f0                 */
/****************************************************************************/
timing_polynom *stm_modpln_create_fequa (float r, float s, float c, float f, float t)
{
    timing_polynom *pln = (timing_polynom*)mbkalloc (sizeof (struct timing_polynom));
    long degree;
    long nbvar;
    long deg_nbvar;
    timing_varpolynom *var[2];
     
    
    /****************************************/
    /*  ______________long_______________   */
    /* |     deg_var    |     nb_var     |  */
    /* |__________001001|______________10|  */
    /*                                      */
    /****************************************/
    degree = STM_RCSF_DEG;
    deg_nbvar = degree << (sizeof(long) * 4);
    nbvar = STM_RCSF_NBVAR;
    deg_nbvar = deg_nbvar | nbvar;
    
    pln->DEG_NBVAR = deg_nbvar;
    
    /****************************************/
    /*  ______________long_______________   */
    /* |           coef_def[0]           |  */
    /* |000000000000000000000000000000111|  */
    /*                                      */
    /* COEF_TAB[0] = t0'                    */
    /* COEF_TAB[1] = param de front         */
    /* COEF_TAB[2] = resistance             */
    /****************************************/
    pln->COEF_DEF = (long*)mbkalloc ( 1 * sizeof (long));
    pln->COEF_DEF[0] = STM_RCSF_CDEF;
    pln->COEF_TAB = (float*)mbkalloc ( 3 * sizeof (float));
    if(r < 0)
        pln->COEF_TAB[0] = t - f*s/1000.0;
    else if(s < 0)
        pln->COEF_TAB[0] = t - c*r/1000.0;
    else
        pln->COEF_TAB[0] = t - c*r/1000.0 - f*s/1000.0;
    if(s < 0)
        pln->COEF_TAB[1] = 0.0;
    else
        pln->COEF_TAB[1] = s/1000.0;
    if(r < 0)
        pln->COEF_TAB[2] = 0.0;
    else
        pln->COEF_TAB[2] = r/1000.0;
    
    var[0] = (timing_varpolynom*)mbkalloc (sizeof (struct timing_varpolynom));
    var[0]->TYPE  = STM_INPUT_SLEW;
    var[0]->THMIN =  stm_round (f / 2.0, 2);
    var[0]->THMAX =  stm_round (f * 2.0, 2);
    var[1] = (timing_varpolynom*)mbkalloc (sizeof (struct timing_varpolynom));
    var[1]->TYPE  = STM_LOAD;
    var[1]->THMIN =  stm_round (c / 2.0, 2);
    var[1]->THMAX =  stm_round (c * 2.0, 2);
    pln->VAR = (timing_varpolynom**)mbkalloc (2 * sizeof (struct timing_varpolynom));
    pln->VAR[0] = var[0];
    pln->VAR[1] = var[1];


    return pln;
}

/****************************************************************************/
/* function stm_modpln_create_fequaforslew()                                */
/*                                                                          */
/****************************************************************************/
timing_polynom *stm_modpln_create_fequaforslew (float f, float c)
{
    timing_polynom *pln = (timing_polynom*)mbkalloc (sizeof (struct timing_polynom));
    long degree;
    long nbvar;
    long deg_nbvar;
    timing_varpolynom *var[1];

    /****************************************/
    /*  ______________long_______________   */
    /* |     deg_var    |     nb_var     |  */
    /* |_____________001|______________01|  */
    /*                                      */
    /****************************************/
    degree = 0x1;
    deg_nbvar = degree << (sizeof(long) * 4);
    nbvar = 0x1;
    deg_nbvar = deg_nbvar | nbvar;
    
    pln->DEG_NBVAR = deg_nbvar;
    
    /****************************************/
    /*  ______________long_______________   */
    /* |           coef_def[0]           |  */
    /* |000000000000000000000000000000010|  */
    /*                                      */
    /* COEF_TAB[0] = resistance             */
    /****************************************/
    pln->COEF_DEF = (long*)mbkalloc ( 1 * sizeof (long));
    pln->COEF_DEF[0] = 0x2;
    pln->COEF_TAB = (float*)mbkalloc ( 1 * sizeof (float));
        pln->COEF_TAB[0] = f / c;
        
    var[0] = (timing_varpolynom*)mbkalloc (sizeof (struct timing_varpolynom));
    var[0]->TYPE  = STM_LOAD;
    var[0]->THMIN =  stm_round (c / 2.0, 2);
    var[0]->THMAX =  stm_round (c * 2.0, 2);
    pln->VAR = (timing_varpolynom**)mbkalloc (sizeof (struct timing_varpolynom));
    pln->VAR[0] = var[0];

    return pln;
}

/****************************************************************************/
/* function stm_modpln_cst_create()                                         */
/*                                                                          */
/****************************************************************************/
timing_polynom *stm_modpln_cst_create(float cst)
{
    timing_polynom *pln = (timing_polynom*)mbkalloc (sizeof (struct timing_polynom));
    long degree;
    long nbvar;
    long deg_nbvar;

    degree = 0;
    deg_nbvar = degree << (sizeof(long) * 4);
    nbvar = 1;
    deg_nbvar = deg_nbvar | nbvar;
    
    pln->DEG_NBVAR = deg_nbvar;
    
    pln->COEF_DEF = (long*)mbkalloc ( 1 * sizeof (long));
    pln->COEF_DEF[0] = 0x1;
    pln->COEF_TAB = (float*)mbkalloc ( 1 * sizeof (float));
    pln->COEF_TAB[0] = cst;

    return pln;
}

