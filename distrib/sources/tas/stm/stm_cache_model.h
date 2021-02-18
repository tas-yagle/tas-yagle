/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_cache_model.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CACHE_MODEL_H
#define STM_CACHE_MODEL_H
extern  char    STM_PRELOAD;
extern  mbkcache*   STM_CACHE;

extern void     stm_cache_setfilepos (char *cellname, char *modelname, MBK_OFFSET_MAX *pos);
extern FILE    *stm_cache_getfilepos (char *cellname, timing_model *model, MBK_OFFSET_MAX *pos);
extern void     stm_enable_cache (char (*isactive)(void*,void*), unsigned long int (*fn_load)(void*,void*), unsigned long int (*fn_free)(void*,void*));
extern unsigned long int    stm_cache_parse_model (char *cellname, timing_model *model);
extern unsigned long int    stm_cache_free_model (char *cellname, timing_model *model);
extern void     stm_cache_refresh_model (char *cellname, timing_model *model);
extern char     stm_cache_isactive (char *cellname, timing_model *model);
extern FILE    *stm_cache_getfile (timing_cell *cell);
extern void     stm_cache_setfile (char *cellname, FILE *f, char *filename, char *extension);
extern void     stm_cache_clearfile (char *cellname);




#endif
