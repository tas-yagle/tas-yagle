/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_good_eval.h                                      */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_GOOD_EVAL_H
#define STM_MODSCM_GOOD_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_modscm_good_cconf (goodparams *params, float slew);
extern float stm_modscm_good_imax (goodparams *params);
extern float stm_modscm_good_vth (goodparams *params);
extern float stm_modscm_good_slew (goodparams *params, float load, float slew);
extern float stm_modscm_good_slope (goodparams *params, float load, float slew);
extern float stm_modscm_good_delay (goodparams *params, float load, float slew);
extern double stm_modscm_good_capaduallink (goodparams *tab, int *conflag, float load, float slew);
extern double stm_f6 (double imax, double c, double vt, double vddmax);
extern float stm_modscm_good_vf_input (goodparams *params);
extern float stm_modscm_good_vdd_input (goodparams *params);
 
#endif
