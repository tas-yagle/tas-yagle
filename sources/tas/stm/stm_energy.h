/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy.h                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_ENERGY_H
#define STM_ENERGY_H


/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
extern void    stm_energy_init (timing_model *model);
extern void    stm_energy_destroy (timing_model *model);
extern void    stm_energy_duplicate (timing_model *src_model, timing_model *dst_model);
extern float   stm_energy_eval (timing_model *model, float load, float slew);
extern timing_model    *stm_energy_energy2tbl (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);
extern void    stm_energy_defaultenergy2tbl (timing_model *model, float slew, float load);

#endif
