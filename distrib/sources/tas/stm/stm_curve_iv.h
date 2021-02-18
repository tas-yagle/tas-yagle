/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve_iv.h                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot, Olivier Bichaut                            */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CURVE_IV_H
#define STM_CURVE_IV_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern stm_curve* stm_curve_iv_c( timing_model *model, float fin, float cout, float t0, float tmax );
extern stm_curve* stm_curve_iv_pi( timing_model *model, float fin, float c1out, float c2out, float rout, float t0, float tmax );

#endif
