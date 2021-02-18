/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modpln_sign.c                                           */
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
float stm_modpln_signature (timing_polynom *pln, char *s)
{
    s[0] = 'X'; /* not implemented */
    return 0;
}

#else
void stm_modpln_signature (timing_polynom *pln, char *s)
{
    s[0] = 'X'; /* not implemented */
}
#endif
