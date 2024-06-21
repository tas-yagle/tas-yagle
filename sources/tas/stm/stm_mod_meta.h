/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_meta.h                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_META_H
#define STM_MOD_META_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

extern float stm_mod_getcaracslewmin (timing_model *model);
extern float stm_mod_getcaracslewoutmin (timing_model *model);
extern float stm_mod_getcaracloadmin (timing_model *model);
extern float stm_mod_getcaracslewmax (timing_model *model);
extern float stm_mod_getcaracslewoutmax (timing_model *model);
extern float stm_mod_getcaracloadmax (timing_model *model);
extern void stm_mod_setcaracslew (timing_model *model, float slew);
extern void stm_mod_setcaracslewout (timing_model *model, float slew);
extern void stm_mod_setcaracload (timing_model *model, float load);

extern timing_model *stm_mod_scm2tbl_delay (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0, float slew, float load);
extern timing_model *stm_mod_scm2tbl_slew (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0, float slew, float load);
extern timing_model *stm_mod_pln2tbl_delay (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);
extern timing_model *stm_mod_pln2tbl_slew (timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);

extern void stm_getAxis(timing_model *model, float *x, long *nx, float *y, long *ny);
extern long stm_genloadaxis (double *loads, float load_min, float load_max);
extern long stm_genslewaxis (double *slews, float slew_min, float slew_max);
extern float *stm_dyna_slews (int n, float slew);
extern float *stm_dyna_loads (int n, float avg_r, float load);
extern void stm_mod_defaultscm2tbl_slew (timing_model *model, float slew, float load);
extern void stm_mod_defaultscm2tbl_delay (timing_model *model, float slew, float load);
extern void stm_mod_defaultpln2tbl_slew (timing_model *model);
extern void stm_mod_defaultpln2tbl_delay (timing_model *model);
extern float stm_round (float d, int precision);

extern timing_model *stm_mod_fct2tbl_delay(timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);
extern timing_model *stm_mod_fct2tbl_slew(timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);
extern void stm_mod_defaultfct2tbl_delay(timing_model *model, char xtype, char ytype);
extern void stm_mod_defaultfct2tbl_slew(timing_model *model, char xtype, char ytype);

extern timing_model *stm_mod_stm2tbl_delay(timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);
extern timing_model *stm_mod_stm2tbl_slew(timing_model *model, float *slews, long ns, float *loads, long nl, float ci0);
extern void stm_mod_defaultstm2tbl_delay(timing_model *model, char xtype, char ytype, float slew, float load);
extern void stm_mod_defaultstm2tbl_slew(timing_model *model, char xtype, char ytype, float slew, float load);
extern void stm_mod_shrinkslewaxis_thr2scm(timing_model *model, double *slews_scaled, double *slews, long ns);
extern double stm_mod_shrinkslew_thr2scm(timing_model *model, double fin);
extern double stm_mod_shrinkslew_scm2thr(timing_model *model, double fout);
extern void stm_mod_shrinkslewaxis_scm2thr(timing_model *model, double *slews);
extern void stm_mod_shrinkslewdata_scm2thr(timing_model *model);
extern int stm_if_thresholds(void);
extern double stm_thr2scm(double fin, double thmin, double thmax, double vt, double vf, double vdd, char type);
extern double stm_scm2thr(double fout, double thmin, double thmax, double vt, double vf, double vdd, char type);
#endif
