/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_cst_sign.h                                       */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_CST_SIGN_H
#define STM_MODSCM_CST_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modscm_cst_signature (cstparams *params, char *s);
#else
extern float stm_modscm_cst_signature (cstparams *params, char *s);
extern int stm_modscm_cst_same_params (cstparams *params0, cstparams *params1);
#endif
extern void stm_modscm_cst_change_params (cstparams *params);

#endif
