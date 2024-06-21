/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_path_eval.c                                     */
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

void stm_modscm_path_sdpath (pathparams *params, float load, double* val, double* front, double* slope)
{
    int index;
    int n, i;
    double capa;
    int conflag = 0;
    float k3, k4, k5;

    *val = 0.0;
    *slope = 0.0;

    if(params->LI){
        for(i = params->LI[STM_NB_I_LINKS]; i > 0; i--){
            capa = params->L0[STM_CI_0] + load;
            if((params->L0[STM_CF_0] > 0)&&(!conflag)){
                capa += params->PP[STM_PCONF0];
                conflag=1;
            }
            index = 1;
            for(n = 0; n < i; n++){
                capa = params->LI[index + STM_CI_I] 
                     + (params->LI[index + STM_ACTI_I]+params->LI[index + STM_BCTI_I] * load) * capa
                     / (params->LI[index + STM_ACTI_I]+params->LI[index + STM_BCTI_I] * load + capa);
                if((params->LI[index + STM_CF_I] > 0)&&(!conflag)){
                    capa += params->PP[STM_PCONF0];
                    conflag=1;
                }
                index += STM_NB_LINK_I_PARAMS;
            }
            index -= STM_NB_LINK_I_PARAMS;
        
            capa /= 1000.0 ;
	        k3 = params->LI[index + STM_K3_I];
            k4 = params->LI[index + STM_K4_I];
            k5 = params->LI[index + STM_K5_I];
            *val += k3*capa*((k3*capa)+ *front)/((k3*capa)+(k4* *front));
            *slope += k3*capa*(((k3*capa)+(k3*k4*capa))/(((k3*capa)+(k4* *front))*((k3*capa)+(k4* *front))));
            *front += k5*capa;
        }
    }
    if(params->L0){
        capa = params->L0[STM_CI_0] + load;
        if((params->L0[STM_CF_0] > 0)&&(!conflag)){
            capa += params->PP[STM_PCONF0];
            conflag=1;
        }
        capa /= 1000.0 ;
        k3 = params->L0[STM_K3_0];
        k4 = params->L0[STM_K4_0];
        k5 = params->L0[STM_K5_0];
        if((k3>0) && (k4>0) && (k5>0)){
            *val += k3*capa*((k3*capa)+ *front)/((k3*capa)+(k4* *front)) ;
            *slope += k3*capa*(((k3*capa)+(k3*k4*capa))/(((k3*capa)+(k4* *front))*((k3*capa)+(k4* *front))));
            *front += k5*capa ;
        }
    }
    *slope *= 1000;
}

/****************************************************************************/

float stm_modscm_path_imax (pathparams *params)
{
    return params->PP[STM_VDDMAX_P] / (2 * params->L0[STM_K3_0]);
}

/****************************************************************************/

float stm_modscm_path_vth (pathparams *params)
{
    return params->PP[STM_VDDMAX_P] / 2;
}

/****************************************************************************/

float stm_modscm_path_slew (pathparams *params, float slew, float load)
{
    
    double fout, val, slope;
    
    fout = slew;
    stm_modscm_path_sdpath (params, load, &val, &fout, &slope);
    
    return (float)fout;
}

/****************************************************************************/
    
float stm_modscm_path_slope (pathparams *params, float slew, float load)
{

    double val, fout, slope;
    
    fout = slew;
    stm_modscm_path_sdpath (params, load, &val, &fout, &slope);

    return (float)slope;
}

/****************************************************************************/
    
float stm_modscm_path_delay (pathparams *params, float slew, float load)
{
    
    double val, fout, slope;
    
    fout = slew;
    stm_modscm_path_sdpath (params, load, &val, &fout, &slope);
    
    return (float)((long)val);
}
/****************************************************************************/

float stm_modscm_path_vf_input (pathparams *params)
{
    return params->PP[STM_VDDMAX_P];
}
/****************************************************************************/

float stm_modscm_path_vdd_input (pathparams *params)
{
    return params->PP[STM_VDDMAX_P];
}

