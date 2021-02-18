/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_merge.h                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_MERGE_H
#define STM_MOD_MERGE_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

	   int stm_mod_dupNotTbl(timing_model *orig, timing_model **dest);

extern timing_model *stm_mod_mergecd (char *name, timing_model *dtmod, timing_model *dsmod, timing_model *cstrmod, float constr, float load);
extern timing_model *stm_mod_mergecc (char *name, timing_model *dtmod, timing_model *dsmod, timing_model *cstrmod, float constr, float load);
extern timing_model *stm_mod_mergec (char *name, timing_model *dtmod, timing_model *ckmod, float constr, float load, float ckload);
extern timing_model *stm_mod_merge (char *name, timing_model *modA, timing_model *modB, float delay, float load, float slew);
extern timing_model *stm_mod_multidelaymerge (timing_model *smodelA, timing_model *dmodelA, float capa, timing_model *dmodelB);
extern timing_model *stm_mod_multislewmerge (timing_model *smodelA, float capa, timing_model *smodelB);
extern timing_model *stm_mod_multidelaymerge_n (chain_list *smodels, chain_list *dmodels);
extern timing_model *stm_mod_multislewmerge_n (chain_list *smodels);

#endif
