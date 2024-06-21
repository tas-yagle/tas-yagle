/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_pwl.c                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/


#ifndef STM_PWL_H
#define STM_PWL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern char STM_PWL_DEBUG_ALLOC;
extern stm_pwl *stm_pwl_create (float *tab, int n);
extern stm_pwl *stm_pwl_dup (stm_pwl *pwl);
extern void stm_pwl_destroy (stm_pwl *pwl);
double stm_get_t_pwl( stm_pwl *pwl, float v );
stm_pwl* stm_shrink_pwl( stm_pwl *pwin, float u0, float f0, float u1, float f1, float vt );
extern mbk_pwl* stm_pwl_to_mbk_pwl( stm_pwl *stmpwl, float vt, float vmax  );
extern stm_pwl* mbk_pwl_to_stm_pwl( mbk_pwl *mbkpwl );
extern void stm_pwl_finish(void);
#endif
