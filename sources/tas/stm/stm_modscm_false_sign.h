/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_false_sign.h                                     */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_FALSE_SIGN_H
#define STM_MODSCM_FALSE_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modscm_false_signature (falseparams *params, char *s);
#else
extern float stm_modscm_false_signature (falseparams *params, char *s);
extern int stm_modscm_false_same_params (falseparams *params0, falseparams *params1);
#endif
extern void stm_modscm_false_change_params (falseparams *params);

#endif
