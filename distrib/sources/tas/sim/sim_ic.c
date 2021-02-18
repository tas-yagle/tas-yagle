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

sim_ic* sim_ic_get_newic( sim_model *model, char *name, char locate )
{
  sim_ic *ic;

  ic = sim_ic_get( model, name, locate );
  if( ic )
    sim_ic_clear( model, name, locate );

  ic = sim_ic_alloc();
  ic->NEXT = model->LIC ;
  model->LIC = ic;
  
  switch( locate ) {
  case SIM_IC_LOCON :
    ic->WHERE.LOCON_NAME = namealloc(name);
    ic->LOCATE = SIM_IC_LOCON;
    break;
  case SIM_IC_SIGNAL :
    ic->WHERE.SIGNAL_NAME = namealloc(name);
    ic->LOCATE = SIM_IC_SIGNAL;
    break;
  }

  ic->TYPE = SIM_IC_NOIC;

  return ic;
}

void sim_ic_set_voltage( sim_model *model, char *name, char locate, SIM_FLOAT voltage )
{
  sim_ic *ic;

  sim_node_clean(model, name);

  ic = sim_ic_get_newic( model, name, locate );

  ic->TYPE = SIM_IC_VOLTAGE;
  ic->UIC.VOLTAGE.VOLTAGE = voltage;
  ic->NODENAME = NULL;
  ic->VSS=sim_parameter_get_vss(model, 'i');
  ic->VDD=sim_parameter_get_alim(model, 'i');
}

sim_ic *sim_ic_set_level( sim_model *model, char *name, char locate, char level )
{
  sim_ic *ic;

  sim_node_clean(model, name);

  ic = sim_ic_get_newic( model, name, locate );

  ic->TYPE = SIM_IC_LEVEL;
  ic->UIC.LEVEL.LEVEL = level;
  ic->NODENAME = NULL;
  ic->VSS=sim_parameter_get_vss(model, 'i');
  ic->VDD=sim_parameter_get_alim(model, 'i');
  return ic;
}

sim_ic *sim_ic_set_nodelist (sim_ic *ic, chain_list *nodelist)
{
  if (!ic) return NULL;
  ic->NODENAME = dupchainlst(nodelist);
  return ic;
}

sim_ic* sim_ic_get( sim_model *model, char *name, char locate )
{
  sim_ic *ic;

  for( ic = model->LIC ; ic ; ic = ic->NEXT )
    if( ( sim_ic_get_locate( ic ) == locate )           &&
        ( strcmp( sim_ic_get_name( ic ), name ) == 0 )    )
      return ic;
  return NULL;
}

char sim_ic_get_type( sim_ic *ic )
{
  if( !ic )
    return SIM_IC_NOIC;
  return ic->TYPE;
}

chain_list *sim_ic_get_nodelist ( sim_ic *ic )
{
  if( !ic )
    return NULL;
  return ic->NODENAME;
}

char sim_ic_get_level( sim_ic *ic )
{
  if( !ic || ic->TYPE != SIM_IC_LEVEL )
    sim_error( "sim_ic_get_level() : not a valid ic\n" );
  return ic->UIC.LEVEL.LEVEL;
}

SIM_FLOAT sim_ic_get_voltage( sim_ic *ic )
{
  if( !ic || ic->TYPE != SIM_IC_VOLTAGE )
    sim_error( "sim_ic_get_level() : not a valid ic\n" );
  return ic->UIC.VOLTAGE.VOLTAGE;
}

char sim_ic_get_locate( sim_ic *ic )
{
  return( ic->LOCATE );
}

char* sim_ic_get_name( sim_ic *ic )
{
  if( !ic ) return NULL;

  switch( ic->LOCATE ) {
  case SIM_IC_SIGNAL:
    return ic->WHERE.SIGNAL_NAME;
    break;
  case SIM_IC_LOCON:
    return ic->WHERE.LOCON_NAME;
    break;
  }
  return NULL;
}

sim_ic* sim_ic_scan( sim_model *model, sim_ic *ic )
{
  if( !ic ) return model->LIC;
  return ic->NEXT;
}

void sim_ic_clean( sim_model *model )
{
  sim_ic *ic, *nextic;
  for( ic = model->LIC ; ic ; ic = nextic ) {
    nextic = ic->NEXT;
    sim_ic_free( ic );
  }
  model->LIC = NULL;
}

void sim_ic_clear( sim_model *model, char *name, char locate )
{
  sim_ic *scan, *prev;

  prev = NULL;

  for( scan = model->LIC ; scan ; scan = scan->NEXT ) {
    if( ( sim_ic_get_locate( scan ) == locate ) &&
        ( strcmp( sim_ic_get_name( scan ), name ) == 0 ) )
      break;
    prev = scan ;
  }

  if (!scan) return;

  if (prev)
    prev->NEXT= scan->NEXT ;
  else
    model->LIC = scan->NEXT ;

  sim_ic_free( scan );
}

void sim_ic_free( sim_ic *ic )
{
  if (ic->NODENAME != NULL)
    freechain (ic->NODENAME);
  mbkfree( ic );
}

sim_ic* sim_ic_alloc( void )
{
  sim_ic *ic;
  ic = (sim_ic*)mbkalloc( sizeof( sim_ic ) );
  return ic;
}

SIM_FLOAT sim_ic_get_vdd( sim_ic *ic )
{
  return ic->VDD;
}

SIM_FLOAT sim_ic_get_vss( sim_ic *ic )
{
  return ic->VSS;
}

