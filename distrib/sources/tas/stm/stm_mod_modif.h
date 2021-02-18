/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_modif.h                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_MODIF_H
#define STM_MOD_MODIF_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

extern timing_model *stm_mod_Creduce (char *name, timing_model *tmodel, float slew, float ckslew, char redmode);
extern timing_model *stm_mod_reduce (char *name, timing_model *tmodel, float slew, float load, char redmode, char modeltype);
extern timing_model *stm_mod_neg (timing_model *model);
extern void stm_mod_shift (timing_model *model, float load);
extern void stm_mod_shrinkslewaxis (timing_model *model, double thmin, double thmax, int type);
extern void stm_mod_shrinkslewdata (timing_model *model, double thmin, double thmax, int type);
extern void stm_mod_shrinkslewaxis_with_rate (timing_model *model, double rate);
extern void stm_mod_shrinkslewdata_with_rate (timing_model *model, double rate);

#endif
