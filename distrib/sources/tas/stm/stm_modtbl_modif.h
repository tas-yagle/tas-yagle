/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_modif.h                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_MODIF_H
#define STM_MODTBL_MODIF_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void stm_modtbl_scale_and_add_val (timing_table *table, float val, float factor);
extern void stm_modtbl_addtab (timing_table *tab, float *valtab, char type);
extern timing_table *stm_modtbl_Creduce (timing_table *table, float slew, float ckslew, char redmode);
extern timing_table *stm_modtbl_reduce (timing_table *table, float slew, float load, char redmode);
extern timing_table *stm_modtbl_neg (timing_table *table);
extern void stm_modtbl_shift (timing_table *tab, float load);
extern void stm_modtbl_shrinkslewaxis (timing_table *tab, double thmin, double thmax, int type);
extern void stm_modtbl_shrinkslewaxis_with_rate (timing_table *tab, double rate);
extern void stm_modtbl_shrinkslewaxis_scm2thr (timing_table *tab, double thmin, double thmax, int type, double vt, double vdd);
extern void stm_modtbl_shrinkslewdata (timing_table *tab, double thmin, double thmax, int type);
extern void stm_modtbl_shrinkslewdata_with_rate (timing_table *tab, double rate);
extern void stm_modtbl_shrinkslewdata_scm2thr (timing_table *tab, double thmin, double thmax, int type, double vt, double vdd, double vf);
extern void stm_modtbl_Ximportckslewaxis (timing_table *dsttab, timing_table *srctab);
extern void stm_modtbl_Yimportckslewaxis (timing_table *dsttab, timing_table *srctab);
extern void stm_modtbl_Ximportslewaxis (timing_table *dsttab, timing_table *srctab);
extern void stm_modtbl_Yimportslewaxis (timing_table *dsttab, timing_table *srctab);
extern void stm_modtbl_Ximportloadaxis (timing_table *dsttab, timing_table *srctab);
extern void stm_modtbl_Yimportloadaxis (timing_table *dsttab, timing_table *srctab);

#endif
