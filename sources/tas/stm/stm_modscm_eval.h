/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_eval.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_EVAL_H
#define STM_MODSCM_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_modscm_cconf (timing_scm *scm, float slew);
extern float stm_modscm_imax (timing_scm *scm);
extern float stm_modscm_vth (timing_scm *scm);
extern float stm_modscm_delay (timing_scm *scm, float slew, stm_pwl *pwl, float load);
extern float stm_modscm_slew (timing_scm *scm, float slew, stm_pwl *pwl, stm_pwl **ptpwl, float load);
extern float stm_modscm_slope (timing_scm *scm, float load, float slew);
extern void  stm_modscm_timing( timing_scm *dscm, timing_scm *fscm, float fin, stm_pwl *pwlin, stm_driver* , float r, float c1, float c2, float *delay, float *fout, stm_pwl **pwlout, char *modelname );
extern float stm_modscm_slewparam (timing_scm *scm, float slew, float load);
extern float stm_modscm_loadparam (timing_scm *scm, float slew, float load);
extern float stm_modscm_capaeq (timing_scm *scm, float slew, float r, float c1, float c2, char *signame );
extern float stm_modscm_vf_input (timing_scm *scm);
extern float stm_modscm_vdd_input (timing_scm *scm);

#endif
