/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modfct.h                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODFCT_H
#define STM_MODFCT_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/* constructors */

extern timing_function *stm_modfct_duplicate (timing_function *fct);
extern timing_function *stm_modfct_create (char *ins, char *localname, APICallFunc *cf);

/* destructors */
    
extern void stm_modfct_destroy (timing_function *fct);

#endif
