/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_good_print.c                                     */
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

void stm_modscm_good_print (FILE *f, goodparams *params)
{
    int i;
    int index;

    fprintf (f, "\t\tscm_good (\n");

    /* link 0 */
    if(params->L0){
        fprintf (f, "\t\t\tlink_out (\n");
        fprintf (f, "\t\t\t\t(ci cf k3 k4 k5)\n");
        fprintf (f, "\t\t\t\t(");
        fprintf (f, "%.5g ", params->L0[STM_CI_0]);
        fprintf (f, "%.5g ", params->L0[STM_CF_0]);
        fprintf (f, "%.5g ", params->L0[STM_K3_0]);
        fprintf (f, "%.5g ", params->L0[STM_K4_0]);
        fprintf (f, "%.5g)\n", params->L0[STM_K5_0]);
        fprintf (f, "\t\t\t)\n");
    }
    /* links 1 to n - 1 */
    if(params->LI){
        fprintf (f, "\t\t\tlink_pass (\n");
        fprintf (f, "\t\t\t\t(%d)\n", (int)params->LI[STM_NB_I_LINKS]);
        fprintf (f, "\t\t\t\t(ci cf acti bcti k3 k4 k5)\n");
        for (i = 0; i < params->LI[STM_NB_I_LINKS]; i++) {
            index = 1 + STM_NB_LINK_I_PARAMS * i;
            fprintf (f, "\t\t\t\t(");
            fprintf (f, "%.5g ", params->LI[index + STM_CI_I]);
            fprintf (f, "%.5g ", params->LI[index + STM_CF_I]);
            fprintf (f, "%.5g ", params->LI[index + STM_ACTI_I]);
            fprintf (f, "%.5g ", params->LI[index + STM_BCTI_I]);
            fprintf (f, "%.5g ", params->LI[index + STM_K3_I]);
            fprintf (f, "%.5g ", params->LI[index + STM_K4_I]);
            fprintf (f, "%.5g)\n", params->LI[index + STM_K5_I]);
        }
        fprintf (f, "\t\t\t)\n");
    }

    /* link n */
    if(params->LN){
        fprintf (f, "\t\t\tlink_dual (\n");
        fprintf (f, "\t\t\t\t(ci cf acti bcti)\n");
        fprintf (f, "\t\t\t\t(");
        fprintf (f, "%.5g ", params->LN[STM_CI_N]);
        fprintf (f, "%.5g ", params->LN[STM_CF_N]);
        fprintf (f, "%.5g ", params->LN[STM_ACTI_N]);
        fprintf (f, "%.5g)\n", params->LN[STM_BCTI_N]);
        fprintf (f, "\t\t\t)\n");
    }
    /* dual part */
    fprintf (f, "\t\t\tdual (\n");
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
    fprintf (f, "%.5g)\n", params->DP[STM_QINIT] );
    fprintf (f, "\t\t\t)\n");

    fprintf (f, "\t\t)\n");
}
