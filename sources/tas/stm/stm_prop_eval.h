/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_prop_eval.h                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_PROP_EVAL_H
#define STM_PROP_EVAL_H

#include STM_H

extern float stm_getfancap (timing_props *properties, float fanout);
extern float stm_getfanres (timing_props *properties, float fanout);

#endif
