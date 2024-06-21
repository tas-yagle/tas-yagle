/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_path.c                                          */
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
   
timing_scm *stm_modscm_path_create (float *tab0, float *tabi, float *tabn, float *tabp)
{
    int index;
    int i;
    timing_scm *scm = (timing_scm*)mbkalloc (sizeof (struct timing_scm));
    scm->TYPE   = STM_MODSCM_PATH;
    scm->PARAMS.PATH = (pathparams*)mbkalloc (sizeof (struct pathparams));
    
    /* link 0 */
    if (tab0) {
        scm->PARAMS.PATH->L0 = (float*)mbkalloc (STM_NB_LINK_0_PARAMS * sizeof (float));
        scm->PARAMS.PATH->L0[STM_CI_0] = tab0[STM_CI_0];
        scm->PARAMS.PATH->L0[STM_CF_0] = tab0[STM_CF_0];
        scm->PARAMS.PATH->L0[STM_K3_0] = tab0[STM_K3_0];
        scm->PARAMS.PATH->L0[STM_K4_0] = tab0[STM_K4_0];
        scm->PARAMS.PATH->L0[STM_K5_0] = tab0[STM_K5_0];
    } else
        scm->PARAMS.PATH->L0 = NULL;

    /* link i = 1 to n - 1 */
    if (tabi) {
        scm->PARAMS.PATH->LI = (float*)mbkalloc ((1 + tabi[STM_NB_I_LINKS] * STM_NB_LINK_I_PARAMS) * sizeof (float));
        scm->PARAMS.PATH->LI[STM_NB_I_LINKS] = tabi[STM_NB_I_LINKS];
        for (i = 0; i < tabi[STM_NB_I_LINKS]; i++) {
            index = 1 + i * STM_NB_LINK_I_PARAMS;
            scm->PARAMS.PATH->LI[index + STM_CI_I]   = tabi[index + STM_CI_I];
            scm->PARAMS.PATH->LI[index + STM_CF_I]   = tabi[index + STM_CF_I];
            scm->PARAMS.PATH->LI[index + STM_ACTI_I] = tabi[index + STM_ACTI_I];
            scm->PARAMS.PATH->LI[index + STM_BCTI_I] = tabi[index + STM_BCTI_I];
            scm->PARAMS.PATH->LI[index + STM_K3_I]   = tabi[index + STM_K3_I];
            scm->PARAMS.PATH->LI[index + STM_K4_I]   = tabi[index + STM_K4_I];
            scm->PARAMS.PATH->LI[index + STM_K5_I]   = tabi[index + STM_K5_I];
        }
    } else
        scm->PARAMS.PATH->LI = NULL;

    /* link n (active) */
    if (tabn) {
        scm->PARAMS.PATH->LN = (float*)mbkalloc (STM_NB_LINK_N_PARAMS * sizeof (float));
        scm->PARAMS.PATH->LN[STM_CI_N]   = tabn[STM_CI_N];
        scm->PARAMS.PATH->LN[STM_CF_N]   = tabn[STM_CF_N];
        scm->PARAMS.PATH->LN[STM_ACTI_N] = tabn[STM_ACTI_N];
        scm->PARAMS.PATH->LN[STM_BCTI_N] = tabn[STM_BCTI_N];
    } else 
        scm->PARAMS.PATH->LN = NULL;


    /* false part */
    scm->PARAMS.PATH->PP = (float*)mbkalloc (STM_NB_PATH_PARAMS * sizeof (float));
    scm->PARAMS.PATH->PP[STM_PCONF0] = tabp[STM_PCONF0];
    scm->PARAMS.PATH->PP[STM_VDDMAX_P] = tabp[STM_VDDMAX_P];

    return scm;
}
    
/****************************************************************************/

void stm_modscm_path_destroy (pathparams *params)
{
    if (!params)
        return;

    if (params->L0)
        mbkfree (params->L0);
    
    if (params->LI)
        mbkfree (params->LI);
    
    if (params->LN)
        mbkfree (params->LN);
    
    if (params->PP)
        mbkfree (params->PP);

    mbkfree (params);
}
