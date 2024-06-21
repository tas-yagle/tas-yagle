/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_noise.c                                                 */
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

void stm_noise_init( timing_model *model )
{
  model->NOISETYPE = STM_NOISE_NULL;
  model->NOISEMODEL.PTR = NULL;
}

void stm_noise_create( timing_model *model, char type )
{

  if( stm_noise_getmodeltype( model ) != STM_NOISE_NULL )
    avt_errmsg (STM_ERRMSG, "034", AVT_FATAL);
    
  switch( type ) {
    case STM_NOISE_SCR:
      model->NOISETYPE = STM_NOISE_SCR ;
      model->NOISEMODEL.SCR = stm_noise_scr_create(stm_mod_vdd(model)/2);
      break;
  }
}

void stm_noise_destroy( timing_model *model )
{
  switch(model->NOISETYPE) {
    case STM_NOISE_SCR:
      stm_noise_scr_destroy( model->NOISEMODEL.SCR );
      break;
  }

  model->NOISETYPE = STM_NOISE_NULL;
}

char stm_noise_getmodeltype( timing_model *model )
{
  if( model ) return (model->NOISETYPE);
  return STM_NOISE_NULL;
}

noise_scr* stm_noise_getmodel_scr( timing_model *model )
{
  return model->NOISEMODEL.SCR;
}

void stm_noise_duplicate( timing_model *src_model, timing_model *dst_model )
{
  switch( src_model->NOISETYPE ) {
  case STM_NOISE_SCR:
    dst_model->NOISETYPE      = STM_NOISE_SCR;
    dst_model->NOISEMODEL.SCR = stm_noise_scr_duplicate( src_model->NOISEMODEL.SCR );
    break;
  default:
    dst_model->NOISETYPE      = STM_NOISE_NULL;
  }
}

