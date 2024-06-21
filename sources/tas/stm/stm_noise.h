/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_noise.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_NOISE_H
#define STM_NOISE_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern void       stm_noise_init         ( timing_model *model );
extern void       stm_noise_create       ( timing_model *model, char type );
extern void       stm_noise_destroy      ( timing_model *model );
extern char       stm_noise_getmodeltype ( timing_model *model );
extern noise_scr* stm_noise_getmodel_scr ( timing_model *model );
extern void       stm_noise_duplicate    ( timing_model *src_model, timing_model *dst_model );
#endif
