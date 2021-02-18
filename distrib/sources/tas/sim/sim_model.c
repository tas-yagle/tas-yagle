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
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"

ht *SIM_MODEL_LIST=NULL;

void sim_init( void )
{
  if( !SIM_MODEL_LIST ) {
    SIM_MODEL_LIST = addht( 10 );
  }
}

lofig_list* sim_model_get_lofig( sim_model *model )
{
  if( !model ) return NULL;
  return model->FIG;
}

void sim_model_set_lofig( sim_model *model, lofig_list *lofig )
{
  model->FIG = lofig;
}

sim_model* sim_model_alloc( void )
{
  sim_model *newmodel;

  newmodel = (sim_model *)mbkalloc( sizeof( sim_model ) );
  newmodel->LINPUT      = NULL;
  newmodel->LMEASURE    = NULL;
  newmodel->TRANSLATION = NULL;
  newmodel->LIC         = NULL;
  newmodel->FIG         = NULL;
  newmodel->FIGNAME     = NULL;
  newmodel->HTINPUT     = NULL;
  newmodel->OUTPUT_FILE = NULL;
  newmodel->OUTPUT_READ = 0;
//  newmodel->CONTEXT_HT  = NULL;
  newmodel->USER        = NULL;
  /* extraction fields */
  newmodel->LLABELS     = NULL;
  mfe_duprules(&(newmodel->LRULES),SIM_EXTRACT_RULE);
  newmodel->LARGS       = NULL;
  newmodel->LPRN        = NULL;
  newmodel->NBMC = 0;
  strcpy(newmodel->SUBCKTFILENAME,"");
  return newmodel;
}

void sim_model_free( sim_model *model )
{
  if( model->LINPUT      || model->LMEASURE || model->HTINPUT      || 
      model->OUTPUT_FILE || model->LIC      || model->TRANSLATION  ||
      model->LLABELS     || model->LRULES   || model->LARGS        ||
      model->LPRN)
    sim_error( "sim_model_free() : model %s not empty.\n", model->FIGNAME );
  mbkfree( model );
}

sim_model* sim_model_create_new( char *name )
{
  sim_model *newmodel;
  
  sim_init();
  
  newmodel = sim_model_alloc();
  newmodel->FIGNAME = name ;

  sim_parameter_init( newmodel );
  sim_input_init( newmodel );
  return newmodel;
}


sim_model* sim_model_create( char *name, int nbmc )
{
  sim_model *newmodel;
  
  sim_init();

  SIM_VT_SLOPEIN = -1.0;
  
  if( sim_model_get( name ) ) {
    sim_error( "sim_create_model() : model %d already declared.\n", name );
  }

  newmodel=sim_model_create_new(name);
  addhtitem( SIM_MODEL_LIST, newmodel->FIGNAME, (long)newmodel );
  newmodel->NBMC = nbmc ;

  return newmodel;
}

sim_model* sim_model_get( char *name )
{
  sim_model    *model;
  
  sim_init();

  model = (sim_model*)gethtitem( SIM_MODEL_LIST, name );

  if( model == (sim_model*)EMPTYHT )
    return NULL;

  return model;
}

void sim_model_clear_bypointer( sim_model *model)
{
  
  sim_init();

  sim_input_clean( model );
  sim_measure_clean( model );
  sim_ic_clean( model ); 
  sim_parameter_clean( model );
  sim_translate_clean( model);

  mfe_freerules(&(model->LRULES));
  if( model->NBMC > 0 )
    mfe_freelabels(&(model->LLABELS),1);
  else
    mfe_freelabels(&(model->LLABELS),0);
  sim_freeMeasAllArg(model);
  sim_freePrn(model);

  mbkfree( model->OUTPUT_FILE );
  model->OUTPUT_FILE = NULL;

  sim_model_free( model );

/*  if (model->CONTEXT_HT!=NULL) delht(model->CONTEXT_HT);
  model->CONTEXT_HT=NULL;*/
}

void sim_model_clear( char *name )
{
  sim_model    *model;
  
  sim_init();

  model = (sim_model*)gethtitem( SIM_MODEL_LIST, name );

  if( model == (sim_model*)EMPTYHT )
    return ;

  if ( model->USER )
    freeptype (model->USER);

  sim_model_clear_bypointer(model);

  delhtitem( SIM_MODEL_LIST, name );

/*  if (model->CONTEXT_HT!=NULL) delht(model->CONTEXT_HT);
  model->CONTEXT_HT=NULL;*/
}

void sim_set_output_file( sim_model *model, char *file )
{
  if (model->OUTPUT_FILE)
      mbkfree (model->OUTPUT_FILE);
  model->OUTPUT_FILE = mbkstrdup(file);
}

char* sim_get_output_file( sim_model *model )
{
  return model->OUTPUT_FILE;
}

/*****************************************************************************\
Fonction de modification des parametres d'un model sim
*******************************************************************************

FUNCTION : sim_update_model_interf_name

Fonction qui met a jour les noms des differents slope,init,measure ...du model

\*****************************************************************************/
void sim_update_model_locon_name (sim_model *model,
                                  char *oldname,
                                  char *newname)
{
  sim_ic      *ic = NULL;
  sim_input   *input = NULL;
  sim_measure *measure = NULL;

  if ((measure = sim_measure_get( model, 
                                 oldname,
                                 SIM_MEASURE_LOCON, 
                                 SIM_MEASURE_VOLTAGE ))) {
       sim_measure_clear (model,oldname,
                          SIM_MEASURE_LOCON,
                          SIM_MEASURE_VOLTAGE
                         );
       sim_measure_set_locon (model,newname);
  }

  // update init
  if ((ic = sim_ic_get (model, oldname, SIM_IC_LOCON)))
    ic->WHERE.LOCON_NAME = newname;

  // update input slope or out capa
  if ((input = sim_input_get (model,oldname)))
    input->LOCON_NAME = newname;
}

/*****************************************************************************\

FUNCTION : sim_update_model_signal_name

Prefixe tous les noms des signaux

\*****************************************************************************/
void sim_update_model_signal_name (sim_model *model, char *prefix)
{
  sim_measure *measure = NULL;
  sim_ic      *ic = NULL;
  
  // update measure
  while( (measure = sim_measure_scan( model, measure )) ) {
    switch( sim_measure_get_what( measure ) ) {
      case SIM_MEASURE_VOLTAGE :
        switch( sim_measure_get_type( measure ) ) {
          case SIM_MEASURE_SIGNAL : 
            measure->WHERE.SIGNAL_NAME = concatname (prefix,
                                                     sim_measure_get_name (measure));
            break;
        }
    }
  }

  // update ic
  while( (ic = sim_ic_scan( model, ic )) ) {
    switch( sim_ic_get_locate( ic ) ) {
      case SIM_IC_SIGNAL :
        ic->WHERE.SIGNAL_NAME = concatname (prefix,sim_ic_get_name (ic));
        break;
    }
  }
}

/*****************************************************************************\

FUNCTION : sim_shift_input

Shift input start time

\*****************************************************************************/
void sim_shift_input (sim_model *model, SIM_FLOAT shift)
{
  lofig_list *lofig;
  locon_list *locon;
  sim_input  *input = NULL;
  char        type,level,subtype;
  SIM_FLOAT   trise, tstart;
  SIM_FLOAT (*func)(SIM_FLOAT t, void *data);
  void      *userdata;
  SIM_FLOAT *data,*olddata;

  lofig = sim_model_get_lofig( model );
  for( locon = lofig->LOCON ; locon ; locon = locon->NEXT ) {
    input = sim_input_get( model, locon->NAME );
    type = sim_input_get_type( input );

    if( type == SIM_NC )
      continue;

    switch( type ) {
      case SIM_SLOPE :
        subtype = sim_input_get_slope_type( input );
        switch( subtype ) {
          case SIM_SLOPE_SINGLE :
            sim_input_get_slope_single( input, &level, &trise, &tstart );
            if ( tstart+shift >= 0.0 )
              input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TSTART = tstart+shift;
            else
              input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TSTART = 0.0;
            break;
          case SIM_SLOPE_PATTERN :
            break;
        }
        break;

      case SIM_FUNC:
        sim_input_get_func( input, &func, &userdata ) ;
        data = (SIM_FLOAT*)(input->UINPUT.INPUT_FUNC.USER_DATA);
        olddata = (SIM_FLOAT*)userdata;
        if ( olddata[0]+shift >= 0.0 )
          data[0] = olddata[0]+shift;
        else
          data[0] = 0.0;
        break;
    }
  }
}


/*****************************************************************************\

FUNCTION : sim_model_merge_ic

\*****************************************************************************/
void sim_model_merge_ic (sim_model *srcmodel, sim_model *model)
{
  sim_ic      *ic = NULL;
  char         ic_locate;

  while( (ic = sim_ic_scan( model, ic )) ) {
    switch( sim_ic_get_locate( ic ) ) {
      case SIM_IC_LOCON :
        ic_locate = SIM_IC_LOCON;
        break;
      case SIM_IC_SIGNAL:
        ic_locate = SIM_IC_SIGNAL;
        break;
    }
    switch (sim_ic_get_type( ic )) {
      case SIM_IC_VOLTAGE: sim_ic_set_voltage (srcmodel,
                                               sim_ic_get_name (ic),
                                               ic_locate,
                                               sim_ic_get_voltage(ic));
                           break;
      case SIM_IC_LEVEL  : sim_ic_set_level   (srcmodel,
                                               sim_ic_get_name (ic),
                                               ic_locate,
                                               sim_ic_get_level(ic));
                           break;
    }
  }
}

/*****************************************************************************\

FUNCTION : sim_model_merge_measure

\*****************************************************************************/
void sim_model_merge_measure (sim_model *srcmodel, sim_model *model)
{
  sim_measure *measure = NULL;

  while( (measure = sim_measure_scan( model, measure )) ) {
    switch( sim_measure_get_what( measure ) ) {
      case SIM_MEASURE_VOLTAGE :
        switch( sim_measure_get_type( measure ) ) {
          case SIM_MEASURE_LOCON : 
            sim_measure_set_locon (srcmodel,sim_measure_get_name (measure));
            break;
          case SIM_MEASURE_SIGNAL: 
            sim_measure_set_signal (srcmodel,sim_measure_get_name (measure));
            break;
        }
        break;
      case SIM_MEASURE_CURRENT :
        break;
    }
  }
}

/*****************************************************************************\

FUNCTION : sim_model_merge_input

\*****************************************************************************/
void sim_model_merge_input (sim_model *srcmodel, sim_model *model,char *insname)
{
  lofig_list *lofig;
  losig_list *losig;
  sim_input  *input = NULL;
  char        type,level,subtype;
  SIM_FLOAT   trise, tfall,period,tstart;
  char       *pattern;
  SIM_FLOAT (*func)(SIM_FLOAT t, void *data);
  void      *userdata;
  char       locate;
  char      *newinputname;

  lofig = sim_model_get_lofig( model );
  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
    input = sim_input_get( model, getsigname (losig) );
    type = sim_input_get_type( input );

    if( type == SIM_NC )
      continue;
    if (input->LOSIG_NAME) {
      locate = SIM_INPUT_SIGNAL;
      newinputname = concatname (insname,sim_input_get_name (input));
    }
    else {
      locate = SIM_INPUT_LOCON;
      newinputname = sim_input_get_name (input);
    }
    if (sim_input_get(srcmodel,newinputname)) {
      sim_input_clear ( srcmodel,newinputname);
      //continue;
    }

    switch( type ) {
      case SIM_STUCK :
        subtype = sim_input_get_stuck_type( input );
        switch (subtype) {
          case SIM_STUCK_LEVEL :
            sim_input_set_stuck_level( srcmodel, sim_input_get_name (input),
                                       sim_input_get_stuck_level( input ));
            break;
          case SIM_STUCK_VALUE:
            sim_input_set_stuck_voltage ( srcmodel, sim_input_get_name(input),
                                          sim_input_get_stuck_voltage( input ));
            break;
        }
        break;
      case SIM_SLOPE :
        subtype = sim_input_get_slope_type( input );
        switch( subtype ) {
          case SIM_SLOPE_SINGLE :
            sim_input_get_slope_single( input, &level, &trise, &tstart );
            sim_input_set_slope_single( srcmodel, 
                                        newinputname, 
                                        level,
                                        trise,
                                        tstart,
                                        locate
                                        );
            break;
          case SIM_SLOPE_PATTERN :
            sim_input_get_slope_pattern( input, &trise, &tfall,
                                         &period, &pattern );
            sim_input_set_slope_pattern( srcmodel, sim_input_get_name (input),
                                         trise, tfall,period,pattern);
            break;
        }
        break;

      case SIM_FUNC:
        sim_input_get_func( input, &func, &userdata ) ;
        sim_input_set_func (srcmodel, newinputname, func, userdata,locate,NULL);
        break;

    }
  }
}

/*****************************************************************************\

FUNCTION : sim_model_merge

Merge deux modeles sim : - measure
                         - ic
                         - input

Le modele merge est srcmodel

\*****************************************************************************/
void sim_model_merge (sim_model *srcmodel, sim_model *model, char *insname)
{
  // merge ic
  sim_model_merge_ic (srcmodel,model);

  // merge measure
  sim_model_merge_measure (srcmodel,model);
  
  // merge input
  sim_model_merge_input (srcmodel,model,insname);
}

void sim_clean_netlist(sim_model *model)
{
  ptype_list *pt;
  locon_list *lc;
  if (model->FIG!=NULL)
  {
    for (lc=model->FIG->LOCON; lc!=NULL; lc=lc->NEXT)
      if ((pt=getptype(lc->USER, SIM_EXT_CAPA))!=NULL) mbkfree(pt->DATA);
  }
}

void sim_free_context (sim_model *model)
{
  if (model->FIG!=NULL)
  {
    sim_clean_netlist(model);
    freeflatmodel (model->FIG);
  }

  sim_model_clear_bypointer( model);
}

void sim_set_filename(sim_model *model, char *filename)
{
  strcpy(model->SUBCKTFILENAME, filename);
}

void sim_set_external_capa(sim_model *model, char *con, SIM_FLOAT value)
{
  locon_list *lc;
  ptype_list *pt;
  con=namealloc(con);

  if (model==NULL) return;

  for (lc=model->FIG->LOCON; lc!=NULL && lc->NAME!=con; lc=lc->NEXT) ;

  if (lc!=NULL)
  {
    if ((pt=getptype(lc->USER, SIM_EXT_CAPA))==NULL)
      pt=lc->USER=addptype(lc->USER, SIM_EXT_CAPA, mbkalloc(sizeof(SIM_FLOAT)));
    *(SIM_FLOAT *)pt->DATA=value;
  }
  else
    sim_error( "could not find connector '%s' in netlist %s\n", con, model->FIGNAME );   
}

SIM_FLOAT sim_get_external_capa(sim_model *model, locon_list *lc)
{
  ptype_list *pt;
  if ((pt=getptype(lc->USER, SIM_EXT_CAPA))==NULL) return 0;
  
  return *(SIM_FLOAT *)pt->DATA;
}

void sim_resetmeasures(sim_model *model)
{
  if( model->NBMC > 0 )
    mfe_freelabels(&(model->LLABELS),1);
  else
    mfe_freelabels(&(model->LLABELS),0);
  sim_freeMeasAllArg(model);
  sim_freePrn(model);
}

