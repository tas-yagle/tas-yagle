/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_pwth.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/


#ifndef STM_PWTH_H
#define STM_PWTH_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern stm_pwl *stm_pwth_create (float *tab, int n);
extern void stm_pwl_destroy (stm_pwl *pwl);
extern int stm_pwl_to_tanh( stm_pwl *pwl, double vt, double vdd, stm_pwth *pwth, int *ntanh, float oldslew );
int stm_get_v_pwth_fn( stm_pwth_fn *args, double t, double *v );
extern double stm_get_v_pwth (float t, float vt, float vdd, char sens, stm_pwth *pwth );
extern double stm_get_t_pwth (float vddin, float vt, float seuil, stm_pwth *pwth );
extern void stm_pwl_debug_pwl_to_tanh( stm_pwl *pwl, double fold, dualparams *params, char *filename, double vt, double vdd, double fin, stm_pwl *pwlin, double load, double tp, char eventin, char eventout ) ;
extern void stm_pwl_debug_pwl_q( double vt, double vdd, double fin, stm_pwl *pwlin, dualparams *params, char *filename, double tp );
void stm_plot_pwth( stm_pwth *pwth, char *filename, float t0, float vddin, float vtn, float vth, char sens );
#endif
