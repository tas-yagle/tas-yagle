/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual_sign.h                                     */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_DUAL_SIGN_H
#define STM_MODSCM_DUAL_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modscm_dual_signature (dualparams *params, char *s);
#else
extern float stm_modscm_dual_signature (dualparams *params, char *s);
extern int stm_modscm_dual_same_params (dualparams *params0, dualparams *params1);
#endif
extern void stm_modscm_dual_change_params (dualparams *params);

#endif
