/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_false_eval.c                                     */
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
double stm_modscm_false_capalink (falseparams *tab, int *conflag, float load, float slew)
{
    float capa;
    float denom;
    float acti;
    float bcti;
    float ci;
    int i, index;

    *conflag = 0;
    
    /* link 0 */
    capa = tab->L0[STM_CI_0] + load;
    if (tab->L0[STM_CF_0] > 0) {
        capa += tab->FP[STM_PCONF0] + tab->FP[STM_PCONF1] * slew;
        *conflag=1;
    }

    /* link 1 -> n - 1 */
    if(tab->LI){
        for (i = 0; i < tab->LI[STM_NB_I_LINKS]; i++) {
            index = 1 + STM_NB_LINK_I_PARAMS * i;
            acti = tab->LI[index + STM_ACTI_I];
            bcti = tab->LI[index + STM_BCTI_I];
            ci   = tab->LI[index + STM_CI_I];
            denom = (acti + bcti * load + capa);
            if(denom < 0.001)
                capa = 0.0;
            else
                capa = ci + (acti + bcti * load) * capa / denom;

            if (tab->LI[index + STM_CF_I] > 0 && !*conflag) {
                capa += tab->FP[STM_PCONF0] + tab->FP[STM_PCONF1] * slew;
                *conflag = 1;
            }
        }
    }

    /* link n */
    if(tab->LN){
        acti = tab->LN[STM_ACTI_N];
        bcti = tab->LN[STM_BCTI_N];
        ci   = tab->LN[STM_CI_N];
        denom = (acti + bcti * load + capa);
        if(denom < 0.001)
            capa = 0.0;
        else
            capa = ci + (acti + bcti * load) * capa / denom;
    
        if (tab->LN[STM_CF_N] > 0 && !*conflag) {
            capa += tab->FP[STM_PCONF0] + tab->FP[STM_PCONF1] * slew;
            *conflag = 1;
        }
    }


    return capa;
}
/****************************************************************************/
void stm_modscm_false_sdpath (falseparams *params, float load, float slew, double* val, double* front, int conflag)
{
    int index;
    int n, i;
    double capa;
    float k3, k4, k5;

    *val = 0.0;

    if(params->LI){
        for(i = params->LI[STM_NB_I_LINKS]; i > 0; i--){
            capa = params->L0[STM_CI_0] + load;
            if((params->L0[STM_CF_0] > 0)&&(!conflag)){
                capa += params->FP[STM_PCONF0] + params->FP[STM_PCONF1]*slew;
                conflag=1;
            }
            index = 1;
            for(n = 0; n < i; n++){
                capa = params->LI[index + STM_CI_I] 
                     + (params->LI[index + STM_ACTI_I]+params->LI[index + STM_BCTI_I] * load) * capa
                     / (params->LI[index + STM_ACTI_I]+params->LI[index + STM_BCTI_I] * load + capa);
                if((params->LI[index + STM_CF_I] > 0)&&(!conflag)){
                    capa += params->FP[STM_PCONF0] + params->FP[STM_PCONF1]*slew;
                    conflag=1;
                }
                index += STM_NB_LINK_I_PARAMS;
            }
            index -= STM_NB_LINK_I_PARAMS;
        
            capa /= 1000.0 ;
	        k3 = params->LI[index + STM_K3_I];
            k4 = params->LI[index + STM_K4_I];
            k5 = params->LI[index + STM_K5_I];
            *val += k3*capa*((k3*capa)+ *front)/((k3*capa)+(k4* *front)) ;
            *front += k5*capa ;
        }
    }
    if(params->L0){
        capa = params->L0[STM_CI_0] + load;
        if((params->L0[STM_CF_0] > 0)&&(!conflag)){
            capa += params->FP[STM_PCONF0] + params->FP[STM_PCONF1]*slew;
            conflag=1;
        }
        capa /= 1000.0 ;
        k3 = params->L0[STM_K3_0];
        k4 = params->L0[STM_K4_0];
        k5 = params->L0[STM_K5_0];
        if((k3>0) && (k4>0) && (k5>0)){
            *val += k3*capa*((k3*capa)+ *front)/((k3*capa)+(k4* *front)) ;
            *front += k5*capa ;
        }
    }
}
/****************************************************************************/

float stm_modscm_false_slew (falseparams *params, float slew, float load)
{
    
    double capa;
    int conflag = 0;
    double front, val;
    
    capa = stm_modscm_false_capalink (params, &conflag, load, slew);
    front = slew + (params->FP[STM_RTOT_F] * capa)/1000;
    stm_modscm_false_sdpath (params, load, slew, &val, &front, conflag);
    
    return (float)front;
}

/****************************************************************************/
    
float stm_modscm_false_imax (falseparams *params)
{

    return params->FP[STM_VDDMAX_F] / (2 * params->FP[STM_RTOT_F]);
}

/****************************************************************************/

float stm_modscm_false_vth (falseparams *params)
{

    return params->FP[STM_VDDMAX_F] / 2;
}

/****************************************************************************/

float stm_modscm_false_slope (falseparams *params)
{

    return (float)(params->FP[STM_K_F] * 1000);
}

/****************************************************************************/
    
float stm_modscm_false_delay (falseparams *params, float slew, float load)
{
    
    double capa;
    int conflag = 0;
    double front, val;
    double tp;
    double delayrc = (double)params->FP[STM_DRC_F];
    
    
    capa = stm_modscm_false_capalink (params, &conflag, load, slew);
    tp = (params->FP[STM_RTOT_F] * capa)/1000 + params->FP[STM_K_F] * slew;

    front = slew + (params->FP[STM_RTOT_F] * capa)/1000;
    stm_modscm_false_sdpath (params, load, slew, &val, &front, conflag);
    return (float)(tp + val + delayrc);
}

/****************************************************************************/

float stm_modscm_false_vf_input (falseparams *params)
{
    return params->FP[STM_VF_INPUT_F];
}

float stm_modscm_false_vdd_input (falseparams *params)
{
    return params->FP[STM_VDDMAX_F];
}

