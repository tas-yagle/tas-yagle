/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_input.c                                                  */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"

SIM_FLOAT sim_input_get_latest_event( sim_model *model )
{
  sim_input *input;
  SIM_FLOAT     tmax=0.0;
  SIM_FLOAT     t;
  SIM_FLOAT  period;
  char      *pattern;
  SIM_FLOAT  slope;
  SIM_FLOAT  start;
  
  input = NULL;
  while( (input = sim_input_scan( model, input )) ) {
    switch( sim_input_get_type( input ) ) {
    case SIM_SLOPE:
      switch( sim_input_get_slope_type( input ) ) {
      case SIM_SLOPE_PATTERN:
        sim_input_get_slope_pattern( input, NULL, NULL, &period, &pattern );
        t = ((SIM_FLOAT)strlen(pattern)) * period;
        break;
      case SIM_SLOPE_SINGLE:
        sim_input_get_slope_single( input, NULL, &slope, &start );
        t = start + slope;
        break;
      default :
        t=0.0;
      }
      break;
    case SIM_STUCK:
      t=0.0;
      break;
    default :
      t=0.0;
    }
    if( t > tmax ) tmax = t;
  }

  return tmax;
}

sim_input* sim_input_scan( sim_model *model, sim_input *scan )
{
  if( !scan ) return model->LINPUT;
  return scan->NEXT;
}

sim_input* sim_input_get_newinput( sim_model *model, char *locon, char locate )
{
  sim_input *input;
  char      *inputname;

  input = sim_input_get( model, locon );
  if( input )
    sim_input_clear( model, locon );

  input = sim_input_alloc();
  input->NEXT = model->LINPUT;
  model->LINPUT = input;
  if (locate == SIM_INPUT_LOCON) {
    input->LOCON_NAME = locon;
    inputname = input->LOCON_NAME;
  }
  else {
    input->LOSIG_NAME = locon;
    inputname = input->LOSIG_NAME;
  }
  addhtitem( model->HTINPUT, inputname, (long)input );

  return input;
}

void sim_input_set_slope_pattern( sim_model *model, 
                                  char *locon, 
                                  SIM_FLOAT trise, 
                                  SIM_FLOAT tfall, 
                                  SIM_FLOAT period, 
                                  char *pattern 
                                )
{
  sim_input *input;
  locon = namealloc( locon );
  sim_node_clean (model,locon);
  input = sim_input_get_newinput( model, locon, SIM_INPUT_LOCON ); // locate to pass in arg

  input->TYPE = SIM_SLOPE;
  input->UINPUT.INPUT_SLOPE.TYPE = SIM_SLOPE_PATTERN;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.TRISE   = trise;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.TFALL   = tfall;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PERIOD  = period;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PATTERN = strdup( pattern );
  input->VSS=sim_parameter_get_vss(model, 'i');
  input->VDD=sim_parameter_get_alim(model, 'i');
}

void sim_input_set_slope_mimic( sim_model *model, 
                                  char *locon, 
                                  char *refnode, 
                                  char *refvddnode, 
                                  char *refvssnode, 
                                  int revert 
                                )
{
  sim_input *input;
  locon = namealloc( locon );
  sim_node_clean (model,locon);
  input = sim_input_get_newinput( model, locon, SIM_INPUT_LOCON ); // locate to pass in arg

  input->TYPE = SIM_MIMIC;
  input->UINPUT.INPUT_MIMIC.REF_VSSNODE=refvssnode;
  input->UINPUT.INPUT_MIMIC.REF_VDDNODE=refvddnode;
  input->UINPUT.INPUT_MIMIC.REF_NODE=refnode;
  input->UINPUT.INPUT_MIMIC.revert=revert;
  input->VSS=sim_parameter_get_vss(model, 'i');
  input->VDD=sim_parameter_get_alim(model, 'i');
}

void  sim_input_get_slope_pattern( sim_input *input,
                                   SIM_FLOAT *trise, 
                                   SIM_FLOAT *tfall, 
                                   SIM_FLOAT *period, 
                                   char **pattern 
                                 )
{

  if( !input )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->TYPE != SIM_SLOPE )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->UINPUT.INPUT_SLOPE.TYPE != SIM_SLOPE_PATTERN )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( trise )   *trise = input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.TRISE;
  if( tfall )   *tfall= input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.TFALL;
  if( period )  *period= input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PERIOD;
  if( pattern ) *pattern= input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PATTERN;

}

void sim_input_set_func( sim_model *model,
                         char *locon, 
                         SIM_FLOAT (*func)(SIM_FLOAT t, void *data),
                         void *user_data,
                         char locate,
                         char *deltavar
                       )
{
  sim_input *input;
  locon = namealloc( locon );
  input = sim_input_get_newinput( model, locon, locate);

  input->TYPE                         = SIM_FUNC;
  input->UINPUT.INPUT_FUNC.FUNC       = func;
  input->UINPUT.INPUT_FUNC.USER_DATA  = user_data;
  if( deltavar )
    input->DELTAVAR = strdup(deltavar);
  input->VSS=sim_parameter_get_vss(model, 'i');
  input->VDD=sim_parameter_get_alim(model, 'i');
}

void sim_input_get_func( sim_input *input, 
                         SIM_FLOAT (**func)( SIM_FLOAT, void* ),
                         void **data
                       )
{
  if( !input )
    sim_error( "sim_input_get_func() : not a valid input\n" );

  if( input->TYPE != SIM_FUNC )
    sim_error( "sim_input_get_func() : not a valid input\n" );

  *func = input->UINPUT.INPUT_FUNC.FUNC;
  *data = input->UINPUT.INPUT_FUNC.USER_DATA;
}

void  sim_input_set_stuck_level( sim_model *model, char *locon, char level )
{
  sim_input *input;
  locon = namealloc( locon );
  sim_node_clean (model,locon);
  input = sim_input_get_newinput( model, locon, SIM_INPUT_LOCON );

  input->TYPE                                       = SIM_STUCK;
  input->UINPUT.INPUT_STUCK.TYPE                    = SIM_STUCK_LEVEL;
  input->UINPUT.INPUT_STUCK.MODEL.STUCK_LEVEL.VALUE = level;
  input->VSS=sim_parameter_get_vss(model, 'i');
  input->VDD=sim_parameter_get_alim(model, 'i');
}

void  sim_input_set_stuck_voltage( sim_model *model, 
                                   char *locon, 
                                   SIM_FLOAT voltage 
                                 )
{
  sim_input *input;
  locon = namealloc( locon );
  sim_node_clean (model,locon);
  input = sim_input_get_newinput( model, locon, SIM_INPUT_LOCON );

  input->TYPE                                         = SIM_STUCK;
  input->UINPUT.INPUT_STUCK.TYPE                      = SIM_STUCK_VALUE;
  input->UINPUT.INPUT_STUCK.MODEL.STUCK_VOLTAGE.VALUE = voltage;
  input->VSS=sim_parameter_get_vss(model, 'i');
  input->VDD=sim_parameter_get_alim(model, 'i');
}

void  sim_input_set_slope_single( sim_model *model, 
                                  char *locon, 
                                  char level, 
                                  SIM_FLOAT trise, 
                                  SIM_FLOAT tstart ,
                                  char locate
                                )
{
  sim_input *input;
  locon = namealloc( locon );
  sim_node_clean (model,locon);
  input = sim_input_get_newinput( model, locon, locate);

  input->TYPE                                             = SIM_SLOPE;
  input->UINPUT.INPUT_SLOPE.TYPE                          = SIM_SLOPE_SINGLE;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TRANSITION = level;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TRISE      = trise;
  input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TSTART     = tstart;
  input->VSS=sim_parameter_get_vss(model, 'i');
  input->VDD=sim_parameter_get_alim(model, 'i');
}

char  sim_input_get_type( sim_input *input )
{
  if( !input ) return SIM_NC;
  return input->TYPE;
}

char*  sim_input_get_name( sim_input *input )
{
  if( !input ) return NULL;
  else if ( input->LOCON_NAME )
    return input->LOCON_NAME;
  else if ( input->LOSIG_NAME )
    return input->LOSIG_NAME;
  else
    return NULL;
}

char  sim_input_get_stuck_type( sim_input *input )
{
  if( !input )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->TYPE != SIM_STUCK )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  return input->UINPUT.INPUT_STUCK.TYPE;
}

char  sim_input_get_slope_type( sim_input *input )
{
  if( !input )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->TYPE != SIM_SLOPE )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  return input->UINPUT.INPUT_SLOPE.TYPE;
}

char  sim_input_get_stuck_level( sim_input *input )
{
  if( !input )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->TYPE != SIM_STUCK )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->UINPUT.INPUT_STUCK.TYPE != SIM_STUCK_LEVEL )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  return input->UINPUT.INPUT_STUCK.MODEL.STUCK_LEVEL.VALUE;
}

SIM_FLOAT sim_input_get_stuck_voltage( sim_input *input )
{
  if( !input )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->TYPE != SIM_STUCK )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->UINPUT.INPUT_STUCK.TYPE != SIM_STUCK_VALUE )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  return input->UINPUT.INPUT_STUCK.MODEL.STUCK_VOLTAGE.VALUE;
}

void  sim_input_get_slope_single( sim_input *input, char *level, SIM_FLOAT *trise, SIM_FLOAT *tstart )
{
  if( !input )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->TYPE != SIM_SLOPE )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( input->UINPUT.INPUT_SLOPE.TYPE != SIM_SLOPE_SINGLE )
    sim_error( "sim_input_get_slope_pattern() : not a valid input\n" );

  if( level )  *level = input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TRANSITION;
  if( trise )  *trise = input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TRISE;
  if( tstart ) *tstart= input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_SINGLE.TSTART;
}

void  sim_input_clear( sim_model *model, char *locon )
{
  sim_input *input, *previnput;
 
  previnput = NULL;
  for( input = model->LINPUT ; input ; input = input->NEXT ) {
    if( input->LOCON_NAME == locon )
      break;
    previnput = input ;
  }

  if( !input ) return;

  if (previnput)
     previnput->NEXT= input->NEXT;
  else
     model->LINPUT = input->NEXT;

  sim_input_free( input );
  delhtitem( model->HTINPUT, locon );
}

sim_input* sim_input_get( sim_model *model, char *locon )
{
  sim_input *input;
  input = (sim_input*)gethtitem( model->HTINPUT, locon );
  if( input == (sim_input*)EMPTYHT ) 
    return NULL;
  return input;
}

void sim_input_free( sim_input *input )
{
  if( input->TYPE == SIM_SLOPE ) {
    if( input->UINPUT.INPUT_SLOPE.TYPE == SIM_SLOPE_PATTERN ) {
      mbkfree( input->UINPUT.INPUT_SLOPE.MODEL.SLOPE_PATTERN.PATTERN );
    }
  }
  else if( input->TYPE == SIM_FUNC ) {
      mbkfree( input->UINPUT.INPUT_FUNC.USER_DATA);
  }

  if( input->DELTAVAR )
    free(input->DELTAVAR);
  mbkfree( input );
}

sim_input* sim_input_alloc( void )
{
  sim_input *newsiminput;

  newsiminput = mbkalloc( sizeof( sim_input ) );
  newsiminput->NEXT       = NULL;
  newsiminput->LOCON_NAME = NULL;
  newsiminput->LOSIG_NAME = NULL;
  newsiminput->TYPE       = 0;
  newsiminput->DELTAVAR   = NULL;

  return newsiminput;
}

void sim_input_clean( sim_model *model )
{
  sim_input    *input, *nextinput;
  for( input = model->LINPUT ; input ; input = nextinput ) {
    nextinput = input->NEXT ;
    sim_input_free( input );
  }
  model->LINPUT = NULL;

  delht( model->HTINPUT );
  model->HTINPUT = NULL;
}

void sim_input_init( sim_model *model )
{
  model->HTINPUT = addht(5);
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_tanh_slope (SIM_FLOAT t, void *data)
{
    SIM_FLOAT st = ((SIM_FLOAT*)data)[0];
    SIM_FLOAT tt = ((SIM_FLOAT*)data)[1];
    SIM_FLOAT vh = ((SIM_FLOAT*)data)[2];
    SIM_FLOAT vl = ((SIM_FLOAT*)data)[3];
    SIM_FLOAT alim   = ((SIM_FLOAT*)data)[4]-((SIM_FLOAT*)data)[6];
    SIM_FLOAT sense  = ((SIM_FLOAT*)data)[5];
    SIM_FLOAT a, val;

    if (sense > 0) {
        if (t < st) 
            val=0;
        else {
            a = (atanh (vh) - atanh (vl)) / tt;
            val=alim * tanh (a * (t - st));
        }
    }
    else {
        if (t < st)
            val= alim;
        else {
            a = (atanh (1 - vl) - atanh (1 - vh)) / tt;
            val= - alim * tanh (a * (t - st)) + alim;
        }
    }

    return ((SIM_FLOAT*)data)[6]+val;
}

/*---------------------------------------------------------------------------*/

SIM_FLOAT sim_tanh_slope_tas (SIM_FLOAT t, void *data)
{
    SIM_FLOAT st = ((SIM_FLOAT*)data)[0]; // start time
    SIM_FLOAT F = ((SIM_FLOAT*)data)[1]; // slope
    SIM_FLOAT vi = ((SIM_FLOAT*)data)[2]; // Vfinal
    SIM_FLOAT vf = ((SIM_FLOAT*)data)[3]; // Vinit
    SIM_FLOAT vt = ((SIM_FLOAT*)data)[4];
    SIM_FLOAT sense  = ((SIM_FLOAT*)data)[5];
    SIM_FLOAT t0, val;

    F = elpThr2Scm (F, SIM_VTH_LOW, SIM_VTH_HIGH, vt, vf, vi<vf? vf : vi, sense>0? elpRISE:elpFALL) ;

    if (sense > 0) {
        if (t < st) 
            val= 0;
        else {
            t0 = F*vt/(vf-vt) ;
            if( t > st+t0 ) 
              val = vt+(vf-vt)*tanh((t-st-t0)/F);
            else {
              if( t > st )
                val = (vf-vt)*(t-st-t0)/F + vt ;
              else
                val = vi ;
            }
        }
    }
    else {
        if (t < st)
            val= vi;
        else {
            t0 = F*vt/(vi-vt-vf) ;
            if( t > st+t0 )
              val = (vi-vt)-(vi-vt-vf)*tanh( (t-st-t0)/F );
            else {
              if( t > st )
                val = -(vi-vt-vf)*(t-st-t0)/F+(vi-vt);
              else
                val = vi ;
            }
        }
    }
    return ((SIM_FLOAT*)data)[6]+val;
}

/*---------------------------------------------------------------------------*/

void *sim_builtin_tanh_tas ()
{
    return &sim_tanh_slope_tas;
}

void *sim_builtin_tanh ()
{
    return &sim_tanh_slope;
}

void *sim_builtin_ramp ()
{
    return NULL;
}


/*---------------------------------------------------------------------------*/

void sim_input_set_slope_tanh (sim_model *model,
                               char *vnode,
                               double start_time, 
                               double transition_time,
                               char sense,
                               char *deltavar )
{
  SIM_FLOAT t_time = SIM_UNIT_X_TO_Y (transition_time, 1.0, SIM_UNIT_TIME);
  SIM_FLOAT s_time = SIM_UNIT_X_TO_Y (start_time, 1.0, SIM_UNIT_TIME);
  SIM_FLOAT *data = (SIM_FLOAT*)mbkalloc (7 * sizeof (SIM_FLOAT)); 

  switch (sense) {
      case SIM_RISE:
            if ( SIM_VT_SLOPEIN > 0.0 ) {
              data[0] = s_time;
              data[1] = t_time;
              data[2] = 0.0;
              data[3] = sim_parameter_get_alim (model, 'i')-sim_parameter_get_vss (model, 'i');
              data[4] = SIM_VT_SLOPEIN; //SIM_VT VTN
              data[5] = 1.0;
              data[6] = sim_parameter_get_vss (model, 'i');
              sim_input_set_func    (model, 
                                     vnode,
                                     sim_builtin_tanh_tas(), 
                                     data,
                                     SIM_INPUT_LOCON,
                                     deltavar);
            }
            else {
              data[0] = s_time;
              data[1] = t_time;
              data[2] = sim_parameter_get_slopeVTHH (model);
              data[3] = sim_parameter_get_slopeVTHL (model);
              data[4] = sim_parameter_get_alim (model, 'i')-sim_parameter_get_vss (model, 'i');
              data[5] = 1.0;
              data[6] = sim_parameter_get_vss (model, 'i');
              sim_input_set_func (model, 
                                  vnode,
                                  sim_builtin_tanh(),
                                  data,
                                  SIM_INPUT_LOCON,
                                  deltavar);
            }
            break;
        case SIM_FALL:
            if ( SIM_VT_SLOPEIN > 0.0 ) {
              data[0] = s_time;
              data[1] = t_time;
              data[2] = sim_parameter_get_alim (model, 'i')-sim_parameter_get_vss (model, 'i'); 
              data[3] = 0.0;
              data[4] = SIM_VT_SLOPEIN; //SIM_VT VTP
              data[5] = -1.0;
              data[6] = sim_parameter_get_vss (model, 'i');
              sim_input_set_func (model, 
                                  vnode, 
                                  sim_builtin_tanh_tas(), 
                                  data,
                                  SIM_INPUT_LOCON,
                                  deltavar);
            }
            else {
              data[0] = s_time;
              data[1] = t_time;
              data[2] = sim_parameter_get_slopeVTHH (model);
              data[3] = sim_parameter_get_slopeVTHL (model);
              data[4] = sim_parameter_get_alim (model, 'i')-sim_parameter_get_vss (model, 'i');
              data[5] = -1.0;
              data[6] = sim_parameter_get_vss (model, 'i');
              sim_input_set_func (model, 
                                  vnode,
                                  sim_builtin_tanh(),
                                  data,
                                  SIM_INPUT_LOCON,
                                  deltavar);
            }
            break;
        default:
            fprintf (stderr, "sim_input_set_slope_tanh : unknown sense on %s\n",vnode);
    }
}

void sim_translate_clean(sim_model *model)
{
  sim_translate *st, *nst;
  for (st=model->TRANSLATION; st!=NULL; st=nst)
    {
      nst=st->NEXT;
      mbkfree(st);
    }
  model->TRANSLATION = NULL;
}
char *sim_input_get_alias(sim_model *model, char *src, sim_translate **ret_st)
{
  sim_translate *st;
  if (ret_st!=NULL) *ret_st=NULL;
  for (st=model->TRANSLATION; st!=NULL && st->signal!=src; st=st->NEXT) ;
  if (st==NULL) return src;
  if (ret_st!=NULL) *ret_st=st;
  return st->equiv;
}

void sim_input_add_alias(sim_model *model, char *src, char *dest)
{
  sim_translate *ret;
  sim_input_get_alias(model, src, &ret);
  if (ret==NULL)
    {
      ret=(sim_translate *)mbkalloc(sizeof(sim_translate));
      ret->NEXT=model->TRANSLATION;
      model->TRANSLATION=ret;
    }
    
  ret->signal=src;
  ret->equiv=dest;
}

/*****************************************************************************\

FUNCTION : sim_input_get_tstart

Get starting time of the input
\*****************************************************************************/
SIM_FLOAT sim_input_get_tstart (sim_model *model, char *loconname)
{
  lofig_list *lofig;
  sim_input  *input = NULL;
  char        type,level,subtype;
  SIM_FLOAT   trise, tstart = -1.0;
  SIM_FLOAT (*func)(SIM_FLOAT t, void *data);
  void      *userdata;
  SIM_FLOAT *data,*olddata;

  lofig = sim_model_get_lofig( model );

  input = sim_input_get( model, loconname );
  type = sim_input_get_type( input );

  if( type == SIM_NC )
    return -1.0;

  switch( type ) {
    case SIM_SLOPE :
      subtype = sim_input_get_slope_type( input );
      switch( subtype ) {
        case SIM_SLOPE_SINGLE :
          sim_input_get_slope_single( input, &level, &trise, &tstart );
          break;
        case SIM_SLOPE_PATTERN :
          sim_input_get_slope_pattern( input,
                                       NULL, 
                                       NULL, 
                                      &tstart, 
                                       NULL
                                     );
          break;
      }
      break;

    case SIM_FUNC:
      sim_input_get_func( input, &func, &userdata ) ;
      data = (SIM_FLOAT*)(input->UINPUT.INPUT_FUNC.USER_DATA);
      olddata = (SIM_FLOAT*)userdata;
      tstart = olddata[0];
      break;
  }
  return tstart;
}

SIM_FLOAT sim_input_get_vdd( sim_input *input )
{
  return input->VDD;
}

SIM_FLOAT sim_input_get_vss( sim_input *input )
{
  return input->VSS;
}

