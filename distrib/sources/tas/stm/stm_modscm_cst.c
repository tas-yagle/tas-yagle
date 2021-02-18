/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_cst.c                                            */
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

timing_scm *stm_modscm_cst_create (float *csttab)
{
    timing_scm *scm = (timing_scm*)mbkalloc (sizeof (struct timing_scm));
    scm->TYPE       = STM_MODSCM_CST;
    scm->PARAMS.CST = (cstparams*)mbkalloc (sizeof (struct cstparams));
    scm->PARAMS.CST->DS = (float*)mbkalloc (STM_NB_CST_PARAMS * sizeof (float));
    
    scm->PARAMS.CST->DS[STM_CST_DELAY] = csttab[STM_CST_DELAY];
    scm->PARAMS.CST->DS[STM_CST_SLEW]  = csttab[STM_CST_SLEW];

    return scm;
}

/****************************************************************************/

void stm_modscm_cst_destroy (cstparams *params)
{
    if (!params)
        return;

    if (params->DS)
        mbkfree (params->DS);

    mbkfree (params);
}
