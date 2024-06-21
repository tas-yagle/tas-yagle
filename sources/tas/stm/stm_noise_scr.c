/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_noise_scr.c                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

noise_scr* stm_noise_scr_create( float default_invth )
{
  noise_scr *model;

  model = (noise_scr*)mbkalloc( sizeof( noise_scr ) );
  model->R = 0.0;
  model->INVTH = default_invth;

  return model;
}

void stm_noise_scr_destroy( noise_scr *model )
{
  mbkfree( model );
}

void stm_noise_scr_update( noise_scr *model, float resi, float invth )
{
  model->R = resi;
  model->INVTH = invth;
}

void stm_noise_scr_update_resi( noise_scr *model, float resi )
{
  model->R = resi;
}

void stm_noise_scr_update_invth( noise_scr *model, float invth )
{
  model->INVTH = invth;
}

float stm_noise_scr_resi( noise_scr *model )
{
  return model->R;
}

float stm_noise_scr_invth( noise_scr *model )
{
  return model->INVTH;
}

noise_scr* stm_noise_scr_duplicate( noise_scr *src_model )
{
  noise_scr *dst_model;

  dst_model         = stm_noise_scr_create(src_model->INVTH);
  dst_model->R      = src_model->R ;
  dst_model->INVTH  = src_model->INVTH ;

  return dst_model;
}
