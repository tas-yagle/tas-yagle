/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_false_eval.h                                     */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_FALSE_EVAL_H
#define STM_MODSCM_FALSE_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_modscm_false_imax (falseparams *params);
extern float stm_modscm_false_vth (falseparams *params);
extern float stm_modscm_false_slew (falseparams *params, float load, float slew);
extern float stm_modscm_false_slope (falseparams *params);
extern float stm_modscm_false_delay (falseparams *params, float load, float slew);
extern void stm_modscm_false_sdpath (falseparams *params, float load, float slew, double* val, double* front, int conflag);
extern float stm_modscm_false_vf_input (falseparams *params);
extern float stm_modscm_false_vdd_input (falseparams *params);

#endif
