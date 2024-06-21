/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_meta.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_META_H
#define STM_MODTBL_META_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
extern void stm_mod_destroy_model(timing_model *stm);

extern void stm_modtbl_morph2_delay_fstm(timing_model *stm, double *s_axis, int ns, double *l_axis, int nl, double slew, double load, double ci0);
extern void stm_modtbl_morph2_slew_fstm(timing_model *stm, double *s_axis, int ns, double *l_axis, int nl, double slew, double load, double ci0);

extern void stm_modtbl_morph2_delay2D_fstm(timing_model *stm, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern void stm_modtbl_morph2_slew2D_fstm(timing_model *stm, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern void stm_modtbl_morph2_delay1Dslewfix_fstm(timing_model *stm, double *l_axis, int nl, double slew, double ci0);
extern void stm_modtbl_morph2_delay1Dloadfix_fstm(timing_model *stm, double *s_axis, int ns, double load);
extern void stm_modtbl_morph2_slew1Dslewfix_fstm(timing_model *stm, double *l_axis, int nl, double slew, double ci0);
extern void stm_modtbl_morph2_slew1Dloadfix_fstm(timing_model *stm, double *s_axis, int ns, double load);

extern timing_table *stm_modtbl_create_delay2D_fstm(timing_model *stm, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_slew2D_fstm(timing_model *stm, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_delay1Dslewfix_fstm(timing_model *stm, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_delay1Dloadfix_fstm(timing_model *stm, double *s_axis, int ns, double load);
extern timing_table *stm_modtbl_create_slew1Dslewfix_fstm(timing_model *stm, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_slew1Dloadfix_fstm(timing_model *stm, double *s_axis, int ns, double load);

extern timing_table *stm_modtbl_create_delay2D_ffct(timing_function *fct, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_slew2D_ffct(timing_function *fct, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_delay1Dslewfix_ffct(timing_function *fct, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_delay1Dloadfix_ffct(timing_function *fct, double *s_axis, int ns, double load);
extern timing_table *stm_modtbl_create_slew1Dslewfix_ffct(timing_function *fct, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_slew1Dloadfix_ffct(timing_function *fct, double *s_axis, int ns, double load);

extern timing_table *stm_modtbl_create_delay2D_fscm (timing_scm *scm, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_slew2D_fscm (timing_scm *scm, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_delay1Dslewfix_fscm (timing_scm *scm, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_delay1Dloadfix_fscm (timing_scm *scm, double *s_axis, int ns, double load);
extern timing_table *stm_modtbl_create_slew1Dslewfix_fscm (timing_scm *scm, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_slew1Dloadfix_fscm (timing_scm *scm, double *s_axis, int ns, double load);

extern timing_table *stm_modtbl_create_delay2D_fpln (timing_polynom *pln, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_slew2D_fpln (timing_polynom *pln, double *s_axis, int ns, double *l_axis, int nl, double ci0);
extern timing_table *stm_modtbl_create_delay1Dslewfix_fpln (timing_polynom *pln, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_delay1Dloadfix_fpln (timing_polynom *pln, double *s_axis, int ns, double load);
extern timing_table *stm_modtbl_create_slew1Dslewfix_fpln (timing_polynom *pln, double *l_axis, int nl, double slew, double ci0);
extern timing_table *stm_modtbl_create_slew1Dloadfix_fpln (timing_polynom *pln, double *s_axis, int ns, double load);

extern void stm_modtbl_fitonmax (timing_table **tab_A, timing_table **tab_B);

#endif
