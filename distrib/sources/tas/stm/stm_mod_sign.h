/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_sign.c                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_SIGN_H
#define STM_MOD_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

#ifndef NEWSIGN
extern void stm_mod_signature (timing_model *tmodel, char *s);
#else
extern float stm_mod_signature (timing_model *tmodel, char *s);
extern int stm_mod_same_params (timing_model *tmodel0, timing_model *tmodel1);
#endif
extern void stm_mod_change_params (timing_model *tmodel);

#endif
