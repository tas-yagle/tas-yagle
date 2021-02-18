/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_path_sign.h                                      */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_PATH_SIGN_H
#define STM_MODSCM_PATH_SIGN_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

#ifndef NEWSIGN
extern void stm_modscm_path_signature (pathparams *params, char *s);
#else
extern float stm_modscm_path_signature (pathparams *params, char *s);
extern int stm_modscm_path_same_params (pathparams *params0, pathparams *params1);
#endif
extern void stm_modscm_path_change_params (pathparams *params);

#endif
