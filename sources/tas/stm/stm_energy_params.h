/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_params.h                                         */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_ENERGY_PARAMS_H
#define STM_ENERGY_PARAMS_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

extern void            stm_energy_params_destroy (energyparams *params) ;
extern energyparams   *stm_energy_params_duplicate (energyparams *params);
extern energyparams   *stm_energy_params_create (float *params);
extern float           stm_energy_params_eval (energyparams *eparams, float load, float slew, float vdd);
extern timing_model   *stm_energy_params2tbl (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);

#endif
