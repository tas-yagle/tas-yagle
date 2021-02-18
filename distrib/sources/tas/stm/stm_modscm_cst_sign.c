/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual_sign.c                                     */
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
float stm_modscm_cst_signature (cstparams *params, char *s)
{
   return params->DS[STM_CST_DELAY]*123+params->DS[STM_CST_SLEW];
}
#else
void stm_modscm_cst_signature (cstparams *params, char *s)
{
   sprintf (s + strlen (s), "scmc_");
   sprintf (s + strlen (s), "%.5g_", params->DS[STM_CST_DELAY]);
   sprintf (s + strlen (s), "%.5g", params->DS[STM_CST_SLEW]);
}
#endif
void stm_modscm_cst_change_params (cstparams *params)
{
#ifdef NEWSIGN
   float precision = 0.01;
   params->DS[STM_CST_DELAY]=mbk_rounddouble(params->DS[STM_CST_DELAY], STM_NEWSIGN_ROUND_PRECISION);
   params->DS[STM_CST_SLEW]=mbk_rounddouble(params->DS[STM_CST_SLEW], STM_NEWSIGN_ROUND_PRECISION);
#endif
}
#ifdef NEWSIGN
int stm_modscm_cst_same_params (cstparams *params0, cstparams *params1)
{
  if (params0->DS[STM_CST_DELAY]==params1->DS[STM_CST_DELAY]
      && params0->DS[STM_CST_SLEW]==params1->DS[STM_CST_SLEW]) return 1;
  return 0;
}
#endif
