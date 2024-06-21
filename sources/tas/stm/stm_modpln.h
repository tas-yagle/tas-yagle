/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modpln.h                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODPLN_H
#define STM_MODPLN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/* constructors */
extern timing_polynom *stm_modpln_create (long nbvar, long *degree, long *coef_def, float *coef, timing_varpolynom **var);
extern timing_polynom *stm_modpln_duplicate (timing_polynom *pln);
extern timing_polynom *stm_modpln_create_fequa (float r, float s, float c, float f, float t);
extern timing_polynom *stm_modpln_create_fequaforslew (float f, float c);
extern timing_polynom *stm_modpln_cst_create(float cst);

/* destructors */
extern void stm_modpln_destroy (timing_polynom *pln);

#endif
