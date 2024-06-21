/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modpln_eval.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODPLN_EVAL_H
#define STM_MODPLN_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H
 
extern float stm_modpln_delay (timing_polynom *pln, float load, float slew);
extern float stm_modpln_slew (timing_polynom *pln, float load, float slew);
extern float stm_modpln_constraint (timing_polynom *pln, float inslew, float ckslew);

#endif
