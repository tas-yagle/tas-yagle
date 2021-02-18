/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_cell_print.c                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CELL_PRINT_H
#define STM_CELL_PRINT_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void stm_cell_print (FILE *f, timing_cell *cell);
extern void stm_cell_printfct (FILE *f, timing_cell *cell);

#endif
