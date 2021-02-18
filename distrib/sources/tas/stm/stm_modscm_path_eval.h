/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_path_eval.h                                     */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_PATH_EVAL_H
#define STM_MODSCM_PATH_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_modscm_path_imax (pathparams *params);
extern float stm_modscm_path_vth (pathparams *params);
extern float stm_modscm_path_slew (pathparams *params, float load, float slew);
extern float stm_modscm_path_slope (pathparams *params, float load, float slew);
extern float stm_modscm_path_delay (pathparams *params, float load, float slew);
extern void stm_modscm_path_sdpath (pathparams *params, float load, double* val, double* front, double* slope);
extern float stm_modscm_path_vf_input (pathparams *params);
extern float stm_modscm_path_vdd_input (pathparams *params);

#endif
