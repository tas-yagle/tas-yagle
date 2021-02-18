/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modfct_eval.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODFCT_EVAL_H
#define STM_MODFCT_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float  stm_modfct_delay (timing_function *fct, float load, float slew);
extern float  stm_modfct_slew (timing_function *fct, float load, float slew);
extern float  stm_modfct_constraint (timing_function *fct, float inslew, float ckslew);
extern void   stm_set_computed_delay (double value);
extern void   stm_set_computed_slope (double value);
extern double stm_get_computed_delay ();
extern double stm_get_computed_slope ();
extern double stm_get_output_load ();
extern double stm_get_input_slope ();
extern double stm_get_command_slope ();
extern void   stm_set_output_load (double val);
extern void   stm_set_input_slope (double val);
extern void   stm_set_command_slope (double val);
extern void   stm_set_current_arc(timing_function *arc);
extern timing_function *stm_get_current_arc();
extern void   stm_call_simulation();
extern void   stm_call_simulation_env();
extern void stm_call_ctk_env();
extern void   stm_call_func (ExecutionContext *ec, timing_function *fct);
#endif
