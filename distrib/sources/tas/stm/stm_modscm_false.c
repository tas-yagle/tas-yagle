/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_false.c                                          */
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
/*
     index                    | param.  | signif.
    --------------------------+---------+------------
    0                         | n       | # links
    --------------------------+---------+------------
    1                         | Ci      | link 0
    1 + 1                     | Cf      |
    1 + 2                     | k3      |
    1 + 3                     | k4      |
    1 + 4                     | k5      |
    --------------------------+---------+------------
    6 + 7 * (i - 1)           | Ci      | link i 
    6 + 7 * (i - 1) + 1       | Cf      | 1 to n - 1
    6 + 7 * (i - 1) + 2       | acti    |
    6 + 7 * (i - 1) + 3       | bcti    |
    6 + 7 * (i - 1) + 4       | k3      |
    6 + 7 * (i - 1) + 5       | k4      |
    6 + 7 * (i - 1) + 6       | k5      |
    --------------------------+---------+------------
    6 + 7 * (n - 2)           | Ci      | link n
    6 + 7 * (n - 2) + 1       | Cf      |
    6 + 7 * (n - 2) + 2       | acti    |
    6 + 7 * (n - 2) + 3       | bcti    |
    --------------------------+---------+------------
*/ 
   
timing_scm *stm_modscm_false_create (float *tab0, float *tabi, float *tabn, float *tabf)
{
    int index;
    int i;
    timing_scm *scm = (timing_scm*)mbkalloc (sizeof (struct timing_scm));
    scm->TYPE   = STM_MODSCM_FALSE;
    scm->PARAMS.FALS = (falseparams*)mbkalloc (sizeof (struct falseparams));
    
    /* link 0 */
    if (tab0) {
        scm->PARAMS.FALS->L0 = (float*)mbkalloc (STM_NB_LINK_0_PARAMS * sizeof (float));
        scm->PARAMS.FALS->L0[STM_CI_0] = tab0[STM_CI_0];
        scm->PARAMS.FALS->L0[STM_CF_0] = tab0[STM_CF_0];
        scm->PARAMS.FALS->L0[STM_K3_0] = tab0[STM_K3_0];
        scm->PARAMS.FALS->L0[STM_K4_0] = tab0[STM_K4_0];
        scm->PARAMS.FALS->L0[STM_K5_0] = tab0[STM_K5_0];
    } else
        scm->PARAMS.FALS->L0 = NULL;

    /* link i = 1 to n - 1 */
    if (tabi) {
        scm->PARAMS.FALS->LI = (float*)mbkalloc ((1 + tabi[STM_NB_I_LINKS] * STM_NB_LINK_I_PARAMS) * sizeof (float));
        scm->PARAMS.FALS->LI[STM_NB_I_LINKS] = tabi[STM_NB_I_LINKS];
        for (i = 0; i < tabi[STM_NB_I_LINKS]; i++) {
            index = 1 + i * STM_NB_LINK_I_PARAMS;
            scm->PARAMS.FALS->LI[index + STM_CI_I]   = tabi[index + STM_CI_I];
            scm->PARAMS.FALS->LI[index + STM_CF_I]   = tabi[index + STM_CF_I];
            scm->PARAMS.FALS->LI[index + STM_ACTI_I] = tabi[index + STM_ACTI_I];
            scm->PARAMS.FALS->LI[index + STM_BCTI_I] = tabi[index + STM_BCTI_I];
            scm->PARAMS.FALS->LI[index + STM_K3_I]   = tabi[index + STM_K3_I];
            scm->PARAMS.FALS->LI[index + STM_K4_I]   = tabi[index + STM_K4_I];
            scm->PARAMS.FALS->LI[index + STM_K5_I]   = tabi[index + STM_K5_I];
        }
    } else
        scm->PARAMS.FALS->LI = NULL;

    /* link n (active) */
    if (tabn) {
        scm->PARAMS.FALS->LN = (float*)mbkalloc (STM_NB_LINK_N_PARAMS * sizeof (float));
        scm->PARAMS.FALS->LN[STM_CI_N]   = tabn[STM_CI_N];
        scm->PARAMS.FALS->LN[STM_CF_N]   = tabn[STM_CF_N];
        scm->PARAMS.FALS->LN[STM_ACTI_N] = tabn[STM_ACTI_N];
        scm->PARAMS.FALS->LN[STM_BCTI_N] = tabn[STM_BCTI_N];
    } else 
        scm->PARAMS.FALS->LN = NULL;


    /* false part */
    scm->PARAMS.FALS->FP = (float*)mbkalloc (STM_NB_FALSE_PARAMS * sizeof (float));
    scm->PARAMS.FALS->FP[STM_PCONF0] = tabf[STM_PCONF0];
    scm->PARAMS.FALS->FP[STM_PCONF1] = tabf[STM_PCONF1];
    scm->PARAMS.FALS->FP[STM_RTOT_F] = tabf[STM_RTOT_F];
    scm->PARAMS.FALS->FP[STM_K_F]    = tabf[STM_K_F];
    scm->PARAMS.FALS->FP[STM_VDDMAX_F]  = tabf[STM_VDDMAX_F];
    scm->PARAMS.FALS->FP[STM_DRC_F]  = tabf[STM_DRC_F];
    scm->PARAMS.FALS->FP[STM_VF_INPUT_F]  = tabf[STM_VF_INPUT_F];

    return scm;
}
    
/****************************************************************************/

void stm_modscm_false_destroy (falseparams *params)
{
    if (!params)
        return;

    if (params->L0)
        mbkfree (params->L0);
    
    if (params->LI)
        mbkfree (params->LI);
    
    if (params->LN)
        mbkfree (params->LN);
    
    if (params->FP)
        mbkfree (params->FP);

    mbkfree (params);
}
