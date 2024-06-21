/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm.c                                                */
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

timing_scm *stm_modscm_duplicate (timing_scm *scm)
{
    timing_scm *dupscm;

    if (!scm)
        return NULL;

    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            dupscm = stm_modscm_dual_create (scm->PARAMS.DUAL->DP);
            break;
        case STM_MODSCM_GOOD:
            dupscm = stm_modscm_good_create (scm->PARAMS.GOOD->L0,
                                             scm->PARAMS.GOOD->LI,
                                             scm->PARAMS.GOOD->LN,
                                             scm->PARAMS.GOOD->DP);
            break;
        case STM_MODSCM_FALSE:
            dupscm = stm_modscm_false_create (scm->PARAMS.FALS->L0,
                                              scm->PARAMS.FALS->LI,
                                              scm->PARAMS.FALS->LN,
                                              scm->PARAMS.FALS->FP);
            break;
        case STM_MODSCM_PATH:
            dupscm = stm_modscm_path_create  (scm->PARAMS.PATH->L0,
                                              scm->PARAMS.PATH->LI,
                                              scm->PARAMS.PATH->LN,
                                              scm->PARAMS.PATH->PP);
            break;
        case STM_MODSCM_CST:
            dupscm = stm_modscm_cst_create  (scm->PARAMS.CST->DS);
            break;
    }
    
    return dupscm;
}

/****************************************************************************/

timing_scm *stm_modscm_create (float *tab0, float *tabi, float *tabn, float *tabd, float* tabcst, long type)
{
    timing_scm *scm = (timing_scm*)mbkalloc (sizeof (struct timing_scm));
    
    switch (type) {
        case STM_MODSCM_DUAL:
            scm = stm_modscm_dual_create (tabd);
            break;
        case STM_MODSCM_GOOD:
            scm = stm_modscm_good_create (tab0, tabi, tabn, tabd);
            break;
        case STM_MODSCM_FALSE:
            scm = stm_modscm_false_create (tab0, tabi, tabn, tabd);
            break;
        case STM_MODSCM_PATH:
            scm = stm_modscm_path_create (tab0, tabi, tabn, tabd);
            break;
        case STM_MODSCM_CST:
            scm = stm_modscm_cst_create (tabcst);
            break;
    }

    return scm;
}

/****************************************************************************/

void stm_modscm_destroy (timing_scm *scm)
{
    if (!scm)
        return;
    
    switch (scm->TYPE) {
        case STM_MODSCM_DUAL:
            stm_modscm_dual_destroy (scm->PARAMS.DUAL);
            break;
        case STM_MODSCM_GOOD:
            stm_modscm_good_destroy (scm->PARAMS.GOOD);
            break;
        case STM_MODSCM_FALSE:
            stm_modscm_false_destroy (scm->PARAMS.FALS);
            break;
        case STM_MODSCM_PATH:
            stm_modscm_path_destroy (scm->PARAMS.PATH);
            break;
        case STM_MODSCM_CST:
            stm_modscm_cst_destroy (scm->PARAMS.CST);
            break;
    }

    mbkfree (scm);
}
