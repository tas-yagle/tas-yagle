/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_eval.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_EVAL_H
#define STM_MODTBL_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

extern float stm_modtbl_constraint (timing_table *table, float inputslew, float clockslew);
extern float stm_modtbl_delay (timing_table *table, float load, float slew);
extern float stm_modtbl_slew (timing_table *table, float load, float slew);
extern float stm_modtbl_interpol1Dlinear (timing_table *table, float x);
extern float stm_modtbl_interpol2Dbilinear (timing_table *table, float x, float y);
extern float stm_modtbl_lowerslew (timing_table *table, float slew);
extern float stm_modtbl_upperslew (timing_table *table, float slew);
extern float stm_modtbl_lowerload (timing_table *table, float load);
extern float stm_modtbl_upperload (timing_table *table, float load);
extern float stm_modtbl_loadparam (timing_table *table, float load, float slew);
extern float stm_modtbl_clockslewparam (timing_table *table, float clockslew, float slew);
extern float stm_modtbl_dataslewparam (timing_table *table, float clockslew, float slew);
extern float stm_modtbl_slewparam (timing_table *table, float load, float slew);
extern float stm_modtbl_maxwfixload (timing_table *tab, float load);
extern float stm_modtbl_maxwfixslew (timing_table *tab, float slew);
extern float stm_modtbl_minwfixload (timing_table *tab, float load);
extern float stm_modtbl_minwfixslew (timing_table *tab, float slew);

extern float stm_modtbl_interpol1DCalc(float x1, float x2, float y1, float y2, float x);
extern float stm_modtbl_value_minslew_maxcapa (timing_table *table);

#endif
