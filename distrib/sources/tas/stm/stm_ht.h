/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_ht.h                                                    */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_HT_H
#define STM_HT_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern stm_ht *stm_addht (unsigned long len);
extern void stm_delht (stm_ht *pTable);
extern long stm_gethtitem (stm_ht *pTable, char *key);
extern long stm_addhtitem (stm_ht *pTable, char *key, long value);
extern long stm_delhtitem (stm_ht *pTable, char *key);
extern void stm_reallocht (stm_ht *pTable);

#endif

