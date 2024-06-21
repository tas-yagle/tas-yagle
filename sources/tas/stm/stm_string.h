/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_string.h                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_STRING_H
#define STM_STRING_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void stm_end (char *s);
extern int stm_msb (char *vect_ident);
extern int stm_lsb (char *vect_ident);
extern char *stm_basename (char *vect_ident);
extern char *stm_vect (char *ident, int p);
extern char *stm_unquote (char *qstr);
extern char *stm_pack (char *s);
extern char *stm_rename (char *str);

#endif

