/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_cst_eval.c                                       */
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

float stm_modscm_cst_slew (cstparams *params)
{
    return params->DS[STM_CST_SLEW];
}

/****************************************************************************/
    
float stm_modscm_cst_imax (cstparams *params)
{
    cstparams *tp;
    tp = params;

    //avt_errmsg (STM_ERRMSG, "026", AVT_ERROR);
    return 0.001;
}

/****************************************************************************/
    
float stm_modscm_cst_vth (cstparams *params)
{
    cstparams *tp;
    tp = params;

    //avt_errmsg (STM_ERRMSG, "027", AVT_ERROR);
    return 0.0;
}

/****************************************************************************/
    
float stm_modscm_cst_slope (cstparams *params)
{
    cstparams *tp;
    tp = params;

    //avt_errmsg (STM_ERRMSG, "028", AVT_ERROR);
    return 0.0;
}

/****************************************************************************/
    
float stm_modscm_cst_delay (cstparams *params)
{
    return params->DS[STM_CST_DELAY];
}
