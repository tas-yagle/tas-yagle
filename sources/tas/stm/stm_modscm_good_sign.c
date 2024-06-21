/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_good_sign.c                                     */
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
#ifdef NEWSIGN
float stm_modscm_good_signature (goodparams *params, char *s)
{
  int i, j, index;
  float sign=0;
  
  if(params->L0){
      for (i=0; i<STM_NB_LINK_0_PARAMS; i++)
        sign+=params->L0[i]*(i+27);
  }
  /* links 1 to n - 1 */
  if(params->LI){
      sign+=params->LI[STM_NB_I_LINKS]*17;
      for (i = 0; i < params->LI[STM_NB_I_LINKS]; i++) {
          index = 1 + STM_NB_LINK_I_PARAMS * i;
          for (j=0; j<STM_NB_LINK_I_PARAMS; j++)
            sign+=params->LI[index + j]*(i+j+17);
      }
  }

  /* link n */
  if(params->LN){
      for (i=0; i<STM_NB_LINK_N_PARAMS; i++)
        sign+=params->LN[i]*(i+13);
  }

  /* dual part */
  for (i=0; i<STM_NB_DUAL_PARAMS; i++)
    sign+=params->DP[i]*(i+7);

  return sign;
}
#else
void stm_modscm_good_signature (goodparams *params, char *s)
{
    int i;
    int index;
    float precision = 0.01;

    sprintf (s + strlen (s), "scmg_");

    /* link 0 */
    if(params->L0){
        sprintf (s + strlen (s), "lo_");
        sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->L0[STM_CI_0], precision));
        sprintf (s + strlen (s), "%.5g_", params->L0[STM_CF_0]);
        sprintf (s + strlen (s), "%.5g_", params->L0[STM_K3_0]);
        sprintf (s + strlen (s), "%.5g_", params->L0[STM_K4_0]);
        sprintf (s + strlen (s), "%.5g_", params->L0[STM_K5_0]);
    }
    /* links 1 to n - 1 */
    if(params->LI){
        sprintf (s + strlen (s), "lp_");
        sprintf (s + strlen (s), "%d_", (int)params->LI[STM_NB_I_LINKS]);
        for (i = 0; i < params->LI[STM_NB_I_LINKS]; i++) {
            index = 1 + STM_NB_LINK_I_PARAMS * i;
            sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->LI[index + STM_CI_I], precision));
            sprintf (s + strlen (s), "%.5g_", params->LI[index + STM_CF_I]);
            sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->LI[index + STM_ACTI_I], precision));
            sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->LI[index + STM_BCTI_I], precision));
            sprintf (s + strlen (s), "%.5g_", params->LI[index + STM_K3_I]);
            sprintf (s + strlen (s), "%.5g_", params->LI[index + STM_K4_I]);
            sprintf (s + strlen (s), "%.5g_", params->LI[index + STM_K5_I]);
        }
    }

    /* link n */
    if(params->LN){
        sprintf (s + strlen (s), "ld_");
        sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->LN[STM_CI_N], precision));
        sprintf (s + strlen (s), "%.5g_", params->LN[STM_CF_N]);
        sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->LN[STM_ACTI_N], precision));
        sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->LN[STM_BCTI_N], precision));
    }
    /* dual part */
    sprintf (s + strlen (s), "d_");
    sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->DP[STM_PCONF0], precision));
    sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->DP[STM_PCONF1], precision));
    sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->DP[STM_CAPAI], precision));
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_IRAP]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_VDDIN]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_VT]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_THRESHOLD]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_IMAX]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_AN]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_BN]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_VDDMAX]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_RSAT]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_RLIN]);
    sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->DP[STM_DRC], precision));
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_RBR]);
    sprintf (s + strlen (s), "%d_", mbk_float_approx_interval(params->DP[STM_CBR], precision));
    sprintf (s + strlen (s), "%.5g", params->DP[STM_INPUT_THR]);
}
#endif
/****************************************************************************/

void stm_modscm_good_change_params (goodparams *params)
{
    int i, j;
    int index;
    float precision = 0.01;

#ifdef NEWSIGN
    /* link 0 */
    if(params->L0){
        for (i=0; i<STM_NB_LINK_0_PARAMS; i++)
          params->L0[i] = mbk_rounddouble(params->L0[i], STM_NEWSIGN_ROUND_PRECISION);
    }
    /* links 1 to n - 1 */
    if(params->LI){
        for (i = 0; i < params->LI[STM_NB_I_LINKS]; i++) {
            index = 1 + STM_NB_LINK_I_PARAMS * i;
            for (j=0; j<STM_NB_LINK_I_PARAMS; j++)
              params->LI[index + j] = mbk_rounddouble(params->LI[index + j], STM_NEWSIGN_ROUND_PRECISION);
        }
    }

    /* link n */
    if(params->LN){
        for (i=0; i<STM_NB_LINK_N_PARAMS; i++)
          params->LN[i] = mbk_rounddouble(params->LN[i], STM_NEWSIGN_ROUND_PRECISION);
    }

    /* dual part */
    for (i=0; i<STM_NB_DUAL_PARAMS; i++)
      params->DP[i] = mbk_rounddouble(params->DP[i], STM_NEWSIGN_ROUND_PRECISION);
#else
    /* link 0 */
    if(params->L0){
        params->L0[STM_CI_0] = mbk_float_approx_middle(params->L0[STM_CI_0], precision);
    }
    /* links 1 to n - 1 */
    if(params->LI){
        params->LI[STM_NB_I_LINKS] = mbk_float_approx_middle(params->LI[STM_NB_I_LINKS], precision);
        for (i = 0; i < params->LI[STM_NB_I_LINKS]; i++) {
            index = 1 + STM_NB_LINK_I_PARAMS * i;
            params->LI[index + STM_CI_I] = mbk_float_approx_middle(params->LI[index + STM_CI_I], precision);
            params->LI[index + STM_ACTI_I] = mbk_float_approx_middle(params->LI[index + STM_ACTI_I], precision);
            params->LI[index + STM_BCTI_I] = mbk_float_approx_middle(params->LI[index + STM_BCTI_I], precision);
        }
    }

    /* link n */
    if(params->LN){
        params->LN[STM_CI_N] = mbk_float_approx_middle(params->LN[STM_CI_N], precision);
        params->LN[STM_ACTI_N] = mbk_float_approx_middle(params->LN[STM_ACTI_N], precision);
        params->LN[STM_BCTI_N] = mbk_float_approx_middle(params->LN[STM_BCTI_N], precision);
    }
    /* dual part */
    params->DP[STM_PCONF0] = mbk_float_approx_middle(params->DP[STM_PCONF0], precision);
    params->DP[STM_PCONF1] = mbk_float_approx_middle(params->DP[STM_PCONF1], precision);
    params->DP[STM_CAPAI] = mbk_float_approx_middle(params->DP[STM_CAPAI], precision);
    params->DP[STM_DRC] = mbk_float_approx_middle(params->DP[STM_DRC], precision);
    params->DP[STM_CBR] = mbk_float_approx_middle(params->DP[STM_CBR], precision);
#endif
}

#ifdef NEWSIGN
int stm_modscm_good_same_params (goodparams *params0, goodparams *params1)
{
  if ((params0->L0==NULL || params1->L0==NULL) && params0->L0!=params1->L0) return 0;
  if (params0->L0!=NULL && memcmp(params0->L0, params1->L0, STM_NB_LINK_0_PARAMS * sizeof (float))!=0) return 0;
  if ((params0->LI==NULL || params1->LI==NULL) && params0->LI!=params1->LI) return 0;
  if (params0->LI!=NULL && params0->LI[STM_NB_I_LINKS]!=params1->LI[STM_NB_I_LINKS]) return 0;
  if (params0->LI!=NULL && memcmp(params0->LI, params1->LI, (1 + params0->LI[STM_NB_I_LINKS] * STM_NB_LINK_I_PARAMS))!=0) return 0;
  if ((params0->LN==NULL || params1->LN==NULL) && params0->LN!=params1->LN) return 0;
  if (params0->LN!=NULL && memcmp(params0->LN, params1->LN, STM_NB_LINK_N_PARAMS * sizeof (float))!=0) return 0;
  return memcmp(params0->DP, params1->DP, STM_NB_DUAL_PARAMS * sizeof (float))==0;
}
#endif

