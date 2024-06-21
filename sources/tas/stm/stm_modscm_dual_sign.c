/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual_sign.c                                      */
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
float stm_modscm_dual_signature (dualparams *params, char *s)
{
  float sign=0;
  int i;
  for (i=0; i<STM_NB_DUAL_PARAMS; i++)
    sign+=params->DP[i]*(i+1);
  return sign;
}
#else
void stm_modscm_dual_signature (dualparams *params, char *s)
{
    float precision = 0.01;
    sprintf (s + strlen (s), "scmd_");
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
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_INPUT_THR]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_RINT]);
    sprintf (s + strlen (s), "%.5g_", params->DP[STM_VINT]);
    sprintf (s + strlen (s), "%d_", mbk_float_approx_interval( params->DP[STM_CHALF], precision) );
    sprintf (s + strlen (s), "%.5g", params->DP[STM_RCONF]);
}
#endif
void stm_modscm_dual_change_params (dualparams *params)
{
    float precision = 0.01;
#ifdef NEWSIGN
    int i;
    for (i=0; i<=STM_KRT; i++)
        params->DP[i]=mbk_rounddouble(params->DP[i], STM_NEWSIGN_ROUND_PRECISION);
#else
    params->DP[STM_PCONF0] = mbk_float_approx_middle(params->DP[STM_PCONF0], precision);
    params->DP[STM_PCONF1] = mbk_float_approx_middle(params->DP[STM_PCONF1], precision);
    params->DP[STM_CAPAI] = mbk_float_approx_middle(params->DP[STM_CAPAI], precision);
    params->DP[STM_DRC] = mbk_float_approx_middle(params->DP[STM_DRC], precision);
    params->DP[STM_CBR] = mbk_float_approx_middle(params->DP[STM_CBR], precision);
#endif
}

#ifdef NEWSIGN
int stm_modscm_dual_same_params (dualparams *params0, dualparams *params1)
{
  return memcmp(params0->DP, params1->DP, STM_NB_DUAL_PARAMS * sizeof (float))==0;
}
#endif

