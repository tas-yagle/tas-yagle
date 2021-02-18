/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modiv_eval.h                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODIV_EVAL_H
#define STM_MODIV_EVAL_H


extern float stm_modiv_delay_c( timing_iv *iv, float fin, float c, float vth, float vdd );
extern float stm_modiv_delay_pi( timing_iv *iv, float fin, float r, float c1, float c2, float vth, float vdd );
extern float stm_modiv_slew_c( timing_iv *iv, float fin, float c, float vth, float vdd );
extern float stm_modiv_slew_pi( timing_iv *iv, float fin, float r, float c1, float c2, float vth, float vdd );

char stm_modiv_calc_is( timing_iv *iv, float ve, float vs, float *is );
char stm_modiv_eval_pi( timing_iv *iv, float fin, float r, float c1, float c2, float vth, float vdd, float *ts, float *fs );
char stm_modiv_eval_c( timing_iv *iv, float fin, float c, float vth, float vdd, float *ts, float *fs );

float stm_modiv_cconf( timing_iv *iv, float fin, float load );
float stm_modiv_calcte( timing_iv *iv, float fin, float vdd );

extern void stm_modiv_timing_pi( timing_iv *iv, float fin, float r, float c1, float c2, float vth, float vdd, float *delay, float *fout );
extern void stm_modiv_timing_c( timing_iv *iv, float fin, float c, float vth, float vdd, float *delay, float *fout );

#endif
