/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_dual.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_DUAL_H
#define STM_MODSCM_DUAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/* constructors */
extern timing_scm *stm_modscm_dual_create (float *params);
extern void stm_modscm_dual_destroy (dualparams *params);

#endif
