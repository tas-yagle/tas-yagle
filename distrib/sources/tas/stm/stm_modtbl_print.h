/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_print.h                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODTBL_PRINT_H
#define STM_MODTBL_PRINT_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void stm_modtbl_print (FILE *f, timing_table *table);
extern void stm_modtbl_templateprint (FILE *f, timing_ttable *templ);
extern void stm_modtbl_datprint (FILE *f, timing_table *table);
extern void stm_modtbl_plotprint (FILE *f, char *datfile, timing_table *table);

#endif
