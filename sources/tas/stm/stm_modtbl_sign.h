/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_sign.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_SIGN_H
#define STM_MODSCM_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modtbl_signature (timing_table *tbl, char *s);
#else
extern float stm_modtbl_signature (timing_table *tbl, char *s);
#endif

#endif
