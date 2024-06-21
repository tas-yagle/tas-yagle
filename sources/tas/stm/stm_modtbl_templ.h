/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_templ.h                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_TEMPL_H
#define STM_MODTBL_TEMPL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void stm_modtbl_findtemplates (timing_cell *cell) ;
extern timing_ttable *stm_modtbl_addtemplate (char *name, int nx, int ny, char xtype, char ytype) ;
extern timing_ttable *stm_modtbl_gettemplate (char *name) ;
extern void stm_modtbl_freetemplate (char *name) ;
extern timing_ttable *stm_modtbl_createtemplate (char *name, int nx, int ny, char xtype, char ytype) ;
extern void stm_modtbl_destroytemplate (timing_ttable *ttemplate) ;
extern void stm_modtbl_settemplateXrange (timing_ttable *ttemplate, chain_list *xrange, float scale) ;
extern void stm_modtbl_settemplateYrange (timing_ttable *ttemplate, chain_list *yrange, float scale) ;
extern timing_ttable *stm_modtbl_storetemplate (char *templ, timing_ttable *ttemplate) ;
extern int stm_modtbl_sametemplate (timing_ttable *temp1, timing_ttable *temp2) ;
extern timing_ttable *stm_modtbl_getmtemplate (timing_model *model) ;
extern char *stm_modtbl_splittemplname (char *templ, int *n) ;
extern char *stm_modtbl_templname (timing_ttable *templ) ;
extern timing_ttable *stm_modtbl_replacetemplate (chain_list *chain, timing_ttable *ttemplate) ;

#endif
