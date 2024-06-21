/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_cst.h                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_CST_H
#define STM_MODSCM_CST_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/* constructors */
extern timing_scm *stm_modscm_cst_create (float *csttab);
extern void stm_modscm_cst_destroy (cstparams *params);

#endif
