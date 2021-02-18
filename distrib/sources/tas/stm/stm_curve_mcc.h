/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve_mcc.h                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot, Olivier Bichaut                            */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CURVE_MCC_H
#define STM_CURVE_MCC_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_get_q( float imax, float t, float F, float B, float U );
extern float stm_get_ic_imax (float t, float U, float imax, float F, float bn);
extern float stm_get_v (float t, float vt, float vi, float vf, float F);
extern float stm_get_v_fromvt (float t, float vt, float vi, float vf, float F);
extern float stm_get_t (float v, float vt, float vi, float vf, float F);
extern float stm_get_t_fromvt (float v, float vt, float vi, float vf, float F);
extern stm_curve* stm_curve_mcc_c( timing_model *model, float fin, float cout, float t0, float tmax );
extern stm_curve* stm_curve_mcc_pi( timing_model *model, float fin, float c1out, float c2out, float rout, float t0, float tmax );

#endif
