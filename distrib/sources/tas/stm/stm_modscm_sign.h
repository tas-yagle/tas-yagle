/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_sign.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_SIGN_H
#define STM_MODSCM_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modscm_signature (timing_scm *scm, char *s);
#else
extern float stm_modscm_signature (timing_scm *scm, char *s);
int stm_modscm_change_params (timing_scm *scm0, timing_scm *scm0);
extern int stm_modscm_same_params (timing_scm *scm0, timing_scm *scm1);
#endif
extern void stm_modscm_change_params (timing_scm *scm);

#endif
