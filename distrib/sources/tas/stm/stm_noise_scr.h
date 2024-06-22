/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_noise_scr.h                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

#ifndef STM_NOISE_SCR_H
#define STM_NOISE_SCR_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

noise_scr* stm_noise_scr_create ( float default_invth );
void stm_noise_scr_destroy      ( noise_scr *model );
extern void stm_noise_scr_update       ( noise_scr *model, float resi, float invth );
extern void stm_noise_scr_update_resi  ( noise_scr *model, float resi );
extern void stm_noise_scr_update_invth ( noise_scr *model, float invth );
extern float stm_noise_scr_resi        ( noise_scr *model );
extern float stm_noise_scr_invth       ( noise_scr *model );
noise_scr* stm_noise_scr_duplicate( noise_scr *src_model );
#endif
