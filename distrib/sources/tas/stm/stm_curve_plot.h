/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve_plot.h                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot, Olivier Bichaut                            */
/*                                                                          */
/****************************************************************************/

#ifndef STM_CURVE_PLOT_H
#define STM_CURVE_PLOT_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
void stm_curve_plot_filename( stm_curve *curve, char *nameout, char *suffix, char *buffer );
extern int stm_curve_plot( stm_curve *curve , char *nameout);
int stm_curve_plot_static( stm_curve *curve, char *nameout );
#endif
