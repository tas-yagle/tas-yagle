/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm.h                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_H
#define STM_MODSCM_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/* constructors */
extern timing_scm *stm_modscm_create (float *tab0, float *tabi, float *tabn, float *tabd, float *tabcst, long type);
extern timing_scm *stm_modscm_duplicate (timing_scm *scm);

/* destructors */
extern void stm_modscm_destroy (timing_scm *scm);

#endif
