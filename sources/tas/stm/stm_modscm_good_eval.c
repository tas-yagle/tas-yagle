/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_good_eval.c                                      */
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
float stm_modscm_good_vf_input (goodparams *params)
{
    double vddin     = (double)params->DP[STM_VDDIN];
    double vt        = (double)params->DP[STM_VT];
    return (float)(vddin + vt);
}

float stm_modscm_good_vdd_input (goodparams *params)
{
    return params->DP[STM_VDDMAX];
}

float stm_modscm_good_cconf (goodparams *params, float slew)
{
    return params->DP[STM_CAPAI] + (params->DP[STM_PCONF0] +  params->DP[STM_PCONF1] * slew)
           * params->DP[STM_IRAP];
}

/****************************************************************************/

float stm_modscm_good_imax (goodparams *params)
{
    return params->DP[STM_IMAX];
}

/****************************************************************************/

float stm_modscm_good_vth (goodparams *params)
{
    return params->DP[STM_THRESHOLD];
}

/****************************************************************************/

float stm_modscm_good_slope (goodparams *params, float slew, float load)
{
    double capa;
    double pconf0;
    double pconf1;
    double capai;
    double irap;
    double vddin;
    double vt;
    double threshold;
    double imax;
    double an;
    double bn;
    int    conflag = 0;

    pconf0    = (double)params->DP[STM_PCONF0];
    pconf1    = (double)params->DP[STM_PCONF1];
    capai     = (double)params->DP[STM_CAPAI];
    irap      = (double)params->DP[STM_IRAP];
    vddin     = (double)params->DP[STM_VDDIN];
    vt        = (double)params->DP[STM_VT];
    threshold = (double)params->DP[STM_THRESHOLD];
    imax      = (double)params->DP[STM_IMAX];
    an        = (double)params->DP[STM_AN];
    bn        = (double)params->DP[STM_BN];

    capa = stm_modscm_good_capaduallink (params, &conflag, load, slew);

    return (float)stm_modscm_dual_calslope (imax, an, bn, vddin, vt, threshold, capa, (double)slew);
}

/****************************************************************************/

double stm_modscm_good_capaduallink (goodparams *tab, int *conflag, float load, float slew)
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
        capa += tab->DP[STM_PCONF0] + tab->DP[STM_PCONF1] * slew;
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
                capa += tab->DP[STM_PCONF0] + tab->DP[STM_PCONF1] * slew;
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
            capa += tab->DP[STM_PCONF0] + tab->DP[STM_PCONF1] * slew;
            *conflag = 1;
        }
    }
    capa += tab->DP[STM_CAPAI] 
         + (tab->DP[STM_PCONF0] + tab->DP[STM_PCONF1] * slew) * tab->DP[STM_IRAP];

    return capa;
}

/****************************************************************************/

double stm_f6 (double imax, double c, double vt, double vddmax) 
{
    return (1.2 * (vddmax - vt)) * c / (imax * 1000);
}

/****************************************************************************/
void stm_modscm_good_sdpath (goodparams *params, float load, float slew, double* val, double* front, int conflag)
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
                capa += params->DP[STM_PCONF0] + params->DP[STM_PCONF1]*slew;
                conflag=1;
            }
            index = 1;
            for(n = 0; n < i; n++){
                capa = params->LI[index + STM_CI_I] 
                     + (params->LI[index + STM_ACTI_I]+params->LI[index + STM_BCTI_I] * load) * capa
                     / (params->LI[index + STM_ACTI_I]+params->LI[index + STM_BCTI_I] * load + capa);
                if((params->LI[index + STM_CF_I] > 0)&&(!conflag)){
                    capa += params->DP[STM_PCONF0] + params->DP[STM_PCONF1]*slew;
                    conflag=1;
                }
                index += STM_NB_LINK_I_PARAMS;
            }
            
            index = 1 + (i -1) * STM_NB_LINK_I_PARAMS;
        
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
            capa += params->DP[STM_PCONF0] + params->DP[STM_PCONF1]*slew;
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
    
float stm_modscm_good_delay (goodparams *params, float slew, float load)
{
    double capa;
    int conflag = 0;
    double front, val;
    double tp, ts, te, fout, U, F;
    double delayrc = (double)params->DP[STM_DRC];


    capa = stm_modscm_good_capaduallink (params, &conflag, load, slew);

    stm_modscm_dual_cal_UF (params->DP[STM_IMAX],params->DP[STM_AN],params->DP[STM_BN], params->DP[STM_VDDIN], 0.0, (double)slew, &U, &F);
    
    ts = stm_modscm_dual_calts(params->DP[STM_IMAX],
                               params->DP[STM_AN],
                               params->DP[STM_BN],
                               U,
                               params->DP[STM_THRESHOLD],
                               capa,
                               F,
                               0.0,
                               params->DP[STM_VT],    /* unused if ci=0.0 */
                               params->DP[STM_VDDIN], /* unused if ci=0.0 */
                               0.0,
                               NULL,
                               NULL,
                               slew,
                               NULL,
                               0,
                               0.0,
                               0.0,
                               0.0,
                               NULL
                              );
    
    te = stm_modscm_dual_calte(params->DP[STM_VDDIN],
                               params->DP[STM_VT],
                               params->DP[STM_INPUT_THR],
                               slew);

    tp = ts - te;

    fout = stm_modscm_dual_calslew(params->DP[STM_IMAX],
                                   params->DP[STM_AN],
                                   params->DP[STM_BN],
                                   U,
                                   params->DP[STM_VT], 
                                   params->DP[STM_VDDMAX], 
                                   capa,
                                   slew,
                                   F,
                                   ts);


    front = fout;
    
/*  ancien front rc
    front = stm_f6(params->DP[STM_IMAX], capa, params->DP[STM_VT], params->DP[STM_VDDMAX]);*/

    stm_modscm_good_sdpath (params, load, slew, &val, &front, conflag);

    return (float)(tp + val + delayrc);
}

/****************************************************************************/
    
float stm_modscm_good_slew (goodparams *params, float slew, float load)
{
    double capa;
    int conflag = 0;
    double front, val;
    double ts, fout, U, F;


    capa = stm_modscm_good_capaduallink (params, &conflag, load, slew);

    stm_modscm_dual_cal_UF (params->DP[STM_IMAX],params->DP[STM_AN],params->DP[STM_BN], params->DP[STM_VDDIN], 0.0, (double)slew, &U, &F);
  
    ts = stm_modscm_dual_calts(params->DP[STM_IMAX],
                               params->DP[STM_AN],
                               params->DP[STM_BN],
                               U,
                               params->DP[STM_THRESHOLD],
                               capa,
                               F,
                               0.0,
                               params->DP[STM_VT],    /* unused if ci=0.0 */
                               params->DP[STM_VDDIN], /* unused if ci=0.0 */
                               0.0,
                               NULL,
                               NULL,
                               slew,
                               NULL,
                               0,
                               0.0,
                               0.0,
                               0.0,
                               NULL
                              );
    
    fout = stm_modscm_dual_calslew(params->DP[STM_IMAX],
                                   params->DP[STM_AN],
                                   params->DP[STM_BN],
                                   U,
                                   params->DP[STM_VT], 
                                   params->DP[STM_VDDMAX], 
                                   capa,
                                   slew,
                                   F,
                                   ts);


    front = fout;

/*  ancien front rc
    front = stm_f6(params->DP[STM_IMAX], capa, params->DP[STM_VT], params->DP[STM_VDDMAX]);*/

    stm_modscm_good_sdpath (params, load, slew, &val, &front, conflag);

    return (float)front;
}
