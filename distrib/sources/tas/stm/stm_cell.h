/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_cell.h                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CELL_H
#define STM_CELL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/* constructors */                       
extern timing_cell *stm_cell_create (char *cellname);
                                             
/* destructors */                            
extern void stm_cell_destroy (timing_cell *cell);

/* modifiers */
extern char *stm_cell_addmodel (timing_cell *cell, timing_model *model, int noshare);
//extern void stm_cell_addsc (timing_cell *cell, sim_model *sc);
extern void stm_cell_delmodel (timing_cell *cell, timing_model *model, int force);
extern int stm_cell_share_decrement (timing_cell *cell, timing_model *model, int n);
extern int stm_cell_share_increment (timing_cell *cell, timing_model *model, int n);
extern int stm_cell_share_getn (timing_cell *cell, timing_model *model);
extern void stm_cleanfigmodel (char *cellname, ht *htable);
#endif
