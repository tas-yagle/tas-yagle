/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual.c                                           */
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

timing_scm *stm_modscm_dual_create (float *params)
{
    int i;
    timing_scm *scm = (timing_scm*)mbkalloc (sizeof (struct timing_scm));
    scm->TYPE   = STM_MODSCM_DUAL;
    scm->PARAMS.DUAL = (dualparams*)mbkalloc (sizeof (struct dualparams));
    scm->PARAMS.DUAL->DP = (float*)mbkalloc (STM_NB_DUAL_PARAMS * sizeof (float));

    for( i=0 ; i<STM_NB_DUAL_PARAMS ; i++)
      scm->PARAMS.DUAL->DP[i] = params[i] ;

    return scm;
}

/****************************************************************************/

void stm_modscm_dual_destroy (dualparams *params)
{
    if (!params)
        return;

    if (params->DP)
        mbkfree (params->DP);

    mbkfree (params);
}
