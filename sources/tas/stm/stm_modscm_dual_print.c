/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual_print.c                                     */
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

void stm_modscm_dual_print (FILE *f, dualparams *params)
{
    fprintf (f, "\t\tscm_dual (\n");
    
    fprintf (f, "\t\t\tdual (\n");

    if( V_BOOL_TAB[ __TAS_ENHANCED_CAPAI ].VALUE ) {
      fprintf (f, "\t\t\t\t(pconf0 pconf1 capai capai0 capai1 capai2 capai3 vt0 vt0c irap vddin vt threshold imax an bn vddmax rsat rlin drc rbr cbr ithr rint vint chalf rconf kf kr qinit)\n");
      fprintf (f, "\t\t\t\t(");
      fprintf (f, "%.5g ", params->DP[STM_PCONF0]);
      fprintf (f, "%.5g ", params->DP[STM_PCONF1]);
      fprintf (f, "%.5g ", params->DP[STM_CAPAI]);
      fprintf (f, "%.5g ", params->DP[STM_CAPAI0]);
      fprintf (f, "%.5g ", params->DP[STM_CAPAI1]);
      fprintf (f, "%.5g ", params->DP[STM_CAPAI2]);
      fprintf (f, "%.5g ", params->DP[STM_CAPAI3]);
      fprintf (f, "%.5g ", params->DP[STM_VT0]);
      fprintf (f, "%.5g ", params->DP[STM_VT0C]);
      fprintf (f, "%.5g ", params->DP[STM_IRAP]);
      fprintf (f, "%.5g ", params->DP[STM_VDDIN]);
      fprintf (f, "%.5g ", params->DP[STM_VT]);
      fprintf (f, "%.5g ", params->DP[STM_THRESHOLD]);
      fprintf (f, "%.5g ", params->DP[STM_IMAX]);
      fprintf (f, "%.5g ", params->DP[STM_AN]);
      fprintf (f, "%.5g ", params->DP[STM_BN]);
      fprintf (f, "%.5g ", params->DP[STM_VDDMAX]);
      fprintf (f, "%.5g ", params->DP[STM_RSAT]);
      fprintf (f, "%.5g ", params->DP[STM_RLIN]);
      fprintf (f, "%.5g ", params->DP[STM_DRC]);
      fprintf (f, "%.5g ", params->DP[STM_RBR]);
      fprintf (f, "%.5g ", params->DP[STM_CBR]);
      fprintf (f, "%.5g ", params->DP[STM_INPUT_THR]);
      fprintf (f, "%.5g ", params->DP[STM_RINT]);
      fprintf (f, "%.5g ", params->DP[STM_VINT]);
      fprintf (f, "%.5g ", params->DP[STM_CHALF]);
      fprintf (f, "%.5g ", params->DP[STM_RCONF]);
      fprintf (f, "%.5g ", params->DP[STM_KF]);
      fprintf (f, "%.5g ", params->DP[STM_KRT]);
      fprintf (f, "%.5g)\n ", params->DP[STM_QINIT]);
    }
    else {
      fprintf (f, "\t\t\t\t(pconf0 pconf1 capai irap vddin vt threshold imax an bn vddmax rsat rlin drc rbr cbr ithr rint vint chalf rconf kf kr qinit)\n");
      fprintf (f, "\t\t\t\t(");
      fprintf (f, "%.5g ", params->DP[STM_PCONF0]);
      fprintf (f, "%.5g ", params->DP[STM_PCONF1]);
      fprintf (f, "%.5g ", params->DP[STM_CAPAI]);
      fprintf (f, "%.5g ", params->DP[STM_IRAP]);
      fprintf (f, "%.5g ", params->DP[STM_VDDIN]);
      fprintf (f, "%.5g ", params->DP[STM_VT]);
      fprintf (f, "%.5g ", params->DP[STM_THRESHOLD]);
      fprintf (f, "%.5g ", params->DP[STM_IMAX]);
      fprintf (f, "%.5g ", params->DP[STM_AN]);
      fprintf (f, "%.5g ", params->DP[STM_BN]);
      fprintf (f, "%.5g ", params->DP[STM_VDDMAX]);
      fprintf (f, "%.5g ", params->DP[STM_RSAT]);
      fprintf (f, "%.5g ", params->DP[STM_RLIN]);
      fprintf (f, "%.5g ", params->DP[STM_DRC]);
      fprintf (f, "%.5g ", params->DP[STM_RBR]);
      fprintf (f, "%.5g ", params->DP[STM_CBR]);
      fprintf (f, "%.5g ", params->DP[STM_INPUT_THR]);
      fprintf (f, "%.5g ", params->DP[STM_RINT]);
      fprintf (f, "%.5g ", params->DP[STM_VINT]);
      fprintf (f, "%.5g ", params->DP[STM_CHALF]);
      fprintf (f, "%.5g ", params->DP[STM_RCONF]);
      fprintf (f, "%.5g ", params->DP[STM_KF]);
      fprintf (f, "%.5g ", params->DP[STM_KRT]);
      fprintf (f, "%.5g)\n ", params->DP[STM_QINIT]);
    }
    
    
    fprintf (f, "\t\t\t)\n");
    fprintf (f, "\t\t)\n");
}
