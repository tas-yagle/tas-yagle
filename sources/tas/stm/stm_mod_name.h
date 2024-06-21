/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_mod_name.h                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MOD_NAME_H
#define STM_MOD_NAME_H

#include STM_H

extern char *stm_slwmname_extract (char *name);
extern char *stm_dlymname_extract (char *name);
extern char *stm_mod_name (char *input, char in_tr, char *output, char out_tr, unsigned int mtype, char minmax, int num);

#endif
