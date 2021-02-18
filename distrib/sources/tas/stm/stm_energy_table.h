/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_energy_table.h                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_H
#define STM_MODTBL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

extern void             stm_energy_table_destroy (timing_table *table);
extern timing_table    *stm_energy_table_duplicate (timing_table *table);
extern float            stm_energy_table_eval (timing_table *etable, float load, float slew);
extern void             stm_energy_table_morph2_energy_fparams (timing_model *model, double *s_axis, int ns, double *l_axis, int nl, double slew, double load, double ci0);
extern void             stm_energy_table_morph2_energy2D_fparams (timing_model *model, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern void             stm_energy_table_morph2_energy1Dslewfix_fparams (timing_model *model, double *l_axis, int nl, double slew, double ci0);
extern void             stm_energy_table_morph2_energy1Dloadfix_fparams (timing_model *model, double *s_axis, int ns, double load);
extern timing_table    *stm_energy_table_create_energy2D_fparams (energyparams *eparams, double *s_axis, int ns, double *l_axis, int nl, double ci0, float vdd);
extern timing_table    *stm_energy_table_create_energy1Dslewfix_fparams (energyparams *eparams, double *l_axis, int nl, double slew, double ci0, float vdd);
extern timing_table    *stm_energy_table_create_energy1Dloadfix_fparams (energyparams *eparams, double *s_axis, int ns, double load, float vdd);

#endif
