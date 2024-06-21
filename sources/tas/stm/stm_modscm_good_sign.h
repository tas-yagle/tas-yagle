/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_good_sign.h                                     */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_GOOD_SIGN_H
#define STM_MODSCM_GOOD_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modscm_good_signature (goodparams *params, char *s);
#else
extern float stm_modscm_good_signature (goodparams *params, char *s);
extern int stm_modscm_good_same_params (goodparams *params0, goodparams *params1);
#endif
extern void stm_modscm_good_change_params (goodparams *params);

#endif
