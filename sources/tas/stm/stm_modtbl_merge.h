/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_merge.h                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_MERGE_H
#define STM_MODTBL_MERGE_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern timing_table *stm_modtbl_mergec (timing_table *dttab, timing_table *cktab, float cstr, float load, float ckload);
extern timing_table *stm_modtbl_mergecd (timing_table *dttab, timing_table *dstab, timing_table *cstrtab, float cstr, float load);
extern timing_table *stm_modtbl_mergecc (timing_table *dttab, timing_table *dstab, timing_table *cstrtab, float cstr, float load);
extern timing_table *stm_modtbl_merge (timing_table *tabA, timing_table *tabB, float delay, float load, float slew);
extern timing_table *stm_modtbl_multidelaymerge (timing_table *stabA, timing_table *dtabA, float capa, timing_table *dtabB);
extern timing_table *stm_modtbl_multidelaymerge_n (chain_list *stabs, chain_list *dtabs);
extern timing_table *stm_modtbl_multislewmerge (timing_table *stabA, float capa, timing_table *stabB);
extern timing_table *stm_modtbl_multislewmerge_n (chain_list *stabs);
extern int stm_modtbl_sametablesize (timing_table *tableA, timing_table *tableB);

#endif
