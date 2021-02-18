/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_eval.c                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_EVAL_H
#define STM_MOD_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

extern float stm_mod_constraint (timing_model *tmodel, float inputslew, float clockslew);
extern float stm_mod_slew (timing_model *tmodel, float load, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame);
extern float stm_mod_delay (timing_model *tmodel, float load, float slew, stm_pwl *pwl, char *signame);
extern float stm_mod_slew_pi (timing_model* tmodel, float c1, float c2, float r, float slew, stm_pwl *pwl, stm_pwl **ptpwl, char *signame);
extern float stm_mod_delay_pi (timing_model *tmodel, float c1, float c2, float r, float slew, stm_pwl *pwl, char *signame);
extern float stm_mod_loadparam (timing_model *tmodel, float load, float slew);
extern float stm_mod_clockslewparam (timing_model *tmodel, float clockslew, float slew);
extern float stm_mod_dataslewparam (timing_model *tmodel, float clockslew, float slew);
extern float stm_mod_slewparam (timing_model *tmodel, float load, float slew);
extern float stm_mod_imax (timing_model *model);
extern float stm_mod_vt (timing_model *model);
extern float stm_mod_default_vt ();
extern float stm_mod_vf (timing_model *model);
extern float stm_mod_vf_input (timing_model *model);
extern float stm_mod_default_vfd ();
extern float stm_mod_default_vfu ();
extern float stm_mod_vth (timing_model *model);
extern float stm_mod_default_vth ();
extern float stm_mod_vdd (timing_model *model);
extern float stm_mod_vdd_input (timing_model *model);
extern float stm_mod_default_vdd ();
extern float stm_mod_rlin (timing_model *model);
extern void  stm_mod_driver (timing_model *model, float *r, float *v);
extern float stm_mod_default_rlin (void);
extern float stm_mod_vsat (timing_model *model);
extern float stm_mod_default_vsat (void);
extern void stm_mod_timing( timing_model *dmodel, timing_model *fmodel, float fin, stm_pwl *pwlin, stm_driver *driver, float load, float *delay, float *fout, stm_pwl **pwlout, char *signame, char *inputname, char dirin, char dirout );
extern void stm_mod_timing_pi( timing_model *dmodel, timing_model *fmodel, float fin, stm_pwl *pwlin, stm_driver *driver, float c1, float c2, float r, float *delay, float *fout, stm_pwl **pwlout, char *signame, char *inputname, char dirin, char dirout );
#endif
