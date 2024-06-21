/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_cst_eval.h                                       */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_CST_EVAL_H
#define STM_MODSCM_CST_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_modscm_cst_imax (cstparams *params);
extern float stm_modscm_cst_vth (cstparams *params);
extern float stm_modscm_cst_delay (cstparams *params);
extern float stm_modscm_cst_slew (cstparams *params);
extern float stm_modscm_cst_slope (cstparams *params);

#endif
