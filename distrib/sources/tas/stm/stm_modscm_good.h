/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_good.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_GOOD_H
#define STM_MODSCM_GOOD_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/* constructors */
extern timing_scm *stm_modscm_good_create (float *tab0, float *tabi, float *tabn, float *tabd);

/* destructors */
extern void stm_modscm_good_destroy (goodparams *params);

#endif
