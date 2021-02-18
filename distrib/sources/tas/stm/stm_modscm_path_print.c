/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_path_print.c                                    */
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

void stm_modscm_path_print (FILE *f, pathparams *params)
{
    int i;
    int index;

    fprintf (f, "\t\tscm_path (\n");


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
    fprintf (f, "\t\t\tpath (\n");
    fprintf (f, "\t\t\t\t(pconf0 vddmax)\n");
    fprintf (f, "\t\t\t\t(");
    fprintf (f, "%.5g ", params->PP[STM_PCONF0_F]);
    fprintf (f, "%.5g)\n", params->PP[STM_VDDMAX_P]);
    fprintf (f, "\t\t\t)\n");

    fprintf (f, "\t\t)\n");
}
