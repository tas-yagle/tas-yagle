/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl.h                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_H
#define STM_MODTBL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H


/* constructors */
extern timing_table *stm_modtbl_create (int nx, int ny, char xtype, char ytype);
extern timing_table *stm_modtbl_create_ftemplate (timing_ttable *ttemplate);
extern timing_table *stm_modtbl_create_fequac (float sdt, float sck, float fdt, float fck, float t);
extern timing_table *stm_modtbl_create_fequa (float r, float s, float c, float f, float t);
extern timing_table *stm_modtbl_create_fcst (float value);
extern timing_table *stm_modtbl_duplicate (timing_table *table);
extern float *stm_modtbl_createset1D (int nx);
extern float **stm_modtbl_createset2D (int nx, int ny);

/* destructors */
extern void stm_modtbl_destroy (timing_table *table);;
extern void stm_modtbl_destroyset2D (float **set, int x);
extern void stm_modtbl_destroyset1D (float *set);

/* value setters */
extern void stm_modtbl_setXrange (timing_table *table, chain_list *xrange, float scale);
extern void stm_modtbl_setYrange (timing_table *table, chain_list *yrange, float scale);
extern void stm_modtbl_set1Dval (timing_table *table, int xpos, float value);
extern void stm_modtbl_setXrangeval (timing_table *table, int xpos, float value);
extern void stm_modtbl_setYrangeval (timing_table *table, int ypos, float value);
extern void stm_modtbl_set2Dval (timing_table *table, int xpos, int ypos, float value);
extern void stm_modtbl_set1Dset (timing_table *table, chain_list *xdata, float scale);
extern void stm_modtbl_set2Dset (timing_table *table, chain_list *xydata, float scale);
extern void stm_modtbl_setconst (timing_table *tab, float value);

/* value accessors */
extern int stm_modtbl_isnull (float value);
extern float stm_modtbl_initval ();
extern float stm_modtbl_getslewaxisval (timing_table *tab, int i);
extern float stm_modtbl_getckslewaxisval (timing_table *tab, int i);
extern float stm_modtbl_getloadaxisval (timing_table *tab, int i);
extern float *stm_modtbl_getloadaxis (timing_table *tab, int *nval);
extern float *stm_modtbl_getslewaxis (timing_table *tab, int *nval);
extern int stm_modtbl_getnslew (timing_table *table);
extern int stm_modtbl_getnckslew (timing_table *table);
extern int stm_modtbl_getnload (timing_table *table);

#endif
