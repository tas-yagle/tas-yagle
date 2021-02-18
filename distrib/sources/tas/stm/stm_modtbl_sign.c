/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_sign.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
#ifdef NEWSIGN
float stm_modtbl_signature (timing_table *tbl, char *s)
{
   s[0] = 'X'; /* not implemented */
   return 0;
}
#else
void stm_modtbl_signature (timing_table *tbl, char *s)
{
    timing_table *ttbl;
    ttbl = tbl;
    s[0] = 'X'; /* not implemented */
}
#endif
