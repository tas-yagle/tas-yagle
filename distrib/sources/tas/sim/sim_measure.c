/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_obj.c                                                    */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"

sim_measure* sim_measure_scan( sim_model *model, sim_measure *measure )
{
  if( measure == NULL )
    return model->LMEASURE;
  return measure->NEXT;
}

sim_measure* sim_measure_alloc( void )
{
  sim_measure   *newmeasure;

  newmeasure = (sim_measure*)mbkalloc( sizeof( sim_measure ) );
  
  newmeasure->NEXT = NULL;
  newmeasure->TYPE = SIM_MEASURE_NO;
  newmeasure->WHAT = SIM_MEASURE_NO;
  newmeasure->DETAIL = NULL;
  newmeasure->NODENAME = NULL;

  return newmeasure;
}

sim_measure_detail* sim_measure_detail_alloc( void )
{
  sim_measure_detail   *newmeasure;

  newmeasure = (sim_measure_detail*)mbkalloc( sizeof( sim_measure_detail ) );
  
  newmeasure->NEXT = NULL;
  newmeasure->NODE_NAME = NULL;
  newmeasure->PRINT_NAME = NULL;
  newmeasure->DATA = NULL;

  return newmeasure;
}

void sim_measure_detail_free( sim_measure_detail *measure )
{
  mbkfree( measure->NODE_NAME );
  mbkfree( measure->PRINT_NAME );
  if( measure->DATA ) {
    mbkfree( measure->DATA );
    measure->DATA = NULL;
  }
  mbkfree( measure );
}

void sim_measure_free( sim_measure *measure )
{
  sim_measure_detail_clear_list( measure );
  if (measure->NODENAME != NULL)
    freechain (measure->NODENAME);
  mbkfree( measure );
}

sim_measure *sim_measure_set_locon( sim_model *model, char *locon )
{
  sim_measure *newmeasure;

  if( (newmeasure = sim_measure_get( model, locon, SIM_MEASURE_LOCON, SIM_MEASURE_VOLTAGE )) ) 
    return newmeasure;

  newmeasure = sim_measure_alloc();

  newmeasure->NEXT = model->LMEASURE;
  model->LMEASURE = newmeasure;
  newmeasure->TYPE = SIM_MEASURE_LOCON;
  newmeasure->WHERE.LOCON_NAME = locon;
  newmeasure->WHAT = SIM_MEASURE_VOLTAGE;

  return newmeasure;
}

void sim_measure_set_signal( sim_model *model, char *signal )
{
  sim_measure *newmeasure;

  if( sim_measure_get( model, signal, SIM_MEASURE_SIGNAL, SIM_MEASURE_VOLTAGE )
    ) 
    return;

  newmeasure = sim_measure_alloc();

  newmeasure->NEXT = model->LMEASURE;
  model->LMEASURE = newmeasure;
  newmeasure->TYPE = SIM_MEASURE_SIGNAL;
  newmeasure->WHERE.SIGNAL_NAME = signal;
  newmeasure->WHAT = SIM_MEASURE_VOLTAGE;

  return;
}

sim_measure *sim_measure_set_nodelist (sim_measure *measure, chain_list *nodelist)
{
  if (!measure) return NULL;
  measure->NODENAME = dupchainlst (nodelist);
  return measure;
}

void sim_measure_current( sim_model *model, char *locon )
{
  sim_measure *newmeasure;

  if( sim_measure_get( model, locon, SIM_MEASURE_LOCON, SIM_MEASURE_CURRENT ) ) 
    return;

  newmeasure = sim_measure_alloc();

  newmeasure->NEXT = model->LMEASURE;
  model->LMEASURE = newmeasure;
  newmeasure->TYPE = SIM_MEASURE_LOCON;
  newmeasure->WHERE.LOCON_NAME = locon;
  newmeasure->WHAT = SIM_MEASURE_CURRENT;
  newmeasure->NODENAME = NULL;

  return;

}

void sim_measure_clear( sim_model *model, char *name, char type, char what )
{
  sim_measure *measure, *prevmeasure;

  prevmeasure = NULL;
  for( measure = model->LMEASURE ; measure ; measure = measure->NEXT ) {
    if( !strcmp( sim_measure_get_name( measure ), name ) &&
        sim_measure_get_type( measure ) == type          &&
        sim_measure_get_what( measure ) == what             )
      break;
    prevmeasure = measure;
  }
  if( !measure ) return ;

  if (prevmeasure)
    prevmeasure->NEXT= measure->NEXT;
  else
    model->LMEASURE = measure->NEXT;

  sim_measure_free( measure );
}

char sim_measure_get_type( sim_measure *measure )
{
  if( !measure ) return SIM_MEASURE_NO;
  return measure->TYPE;
}

chain_list *sim_measure_get_nodelist ( sim_measure *measure )
{
  if( !measure ) return NULL;
  return measure->NODENAME;
}

char sim_measure_get_what( sim_measure *measure )
{
  if( !measure ) return SIM_MEASURE_NO;
  return measure->WHAT;
}

char* sim_measure_get_name( sim_measure *measure )
{
  if( measure->TYPE == SIM_MEASURE_LOCON ) return measure->WHERE.LOCON_NAME;
  if( measure->TYPE == SIM_MEASURE_SIGNAL ) return measure->WHERE.SIGNAL_NAME;
  sim_error( "sim_measure_get_name() : unknown TYPE_n" );
  return NULL;
}

sim_measure* sim_measure_get( sim_model *model, char *name, char type, char what )
{
  sim_measure *measure;

  measure=NULL;
  while( (measure = sim_measure_scan( model, measure )) )
    if( !strcmp( sim_measure_get_name( measure ), name ) &&
        sim_measure_get_type( measure ) == type          &&
        sim_measure_get_what( measure ) == what             )
      break;

  return measure;
}

void sim_measure_set_detail( sim_measure *measure, char *nodename,char *printname )
{
  sim_measure_detail    *detail;

  detail = sim_measure_detail_alloc();
  detail->NEXT = measure->DETAIL;
  measure->DETAIL = detail;

  detail->NODE_NAME  = strdup( nodename );
  detail->PRINT_NAME = strdup( printname );
}

char* sim_measure_detail_get_nodename( sim_measure_detail *detail )
{
  if( !detail ) return NULL;
  return detail->NODE_NAME;
}

char* sim_measure_detail_get_name( sim_measure_detail *detail )
{
  if( !detail ) return NULL;
  return detail->PRINT_NAME;
}

SIM_FLOAT* sim_measure_detail_get_data( sim_measure_detail *detail )
{
  if( !detail ) return NULL;
  return detail->DATA;
}

void sim_measure_detail_set_data( sim_measure_detail *detail, SIM_FLOAT *data )
{
  if( !detail ) return;
  detail->DATA = data;
}

void sim_measure_detail_clear_list( sim_measure *measure )
{
  sim_measure_detail    *detail, *next;
  for( detail = measure->DETAIL ; detail ; detail = next ) {
    next = detail->NEXT;
    sim_measure_detail_free( detail );
  }
  measure->DETAIL = NULL;
}

sim_measure_detail* sim_measure_detail_scan( sim_measure *measure, sim_measure_detail *scan )
{
  if( !scan )
    return measure->DETAIL;
  return scan->NEXT;
}

void sim_measure_clean( sim_model *model )
{
  sim_measure *measure, *nextmeasure;
  for( measure = model->LMEASURE ; measure ; measure = nextmeasure ) {
    nextmeasure = measure->NEXT;
    sim_measure_free( measure );
  }
  model->LMEASURE = NULL;
}

void sim_measure_detail_clean( sim_model *model )
{
  sim_measure *measure;
  measure = NULL;
  while( (measure = sim_measure_scan( model, measure ) ) )
    sim_measure_detail_clear_list( measure );
}
