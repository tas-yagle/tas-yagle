/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot, Olivier Bichaut                            */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/


/******************************************************************************\
Memory allocation function
\******************************************************************************/

stm_curve* stm_curve_alloc( timing_model *model, float t0, float tmax , int nbpoints)
{
  stm_curve *curve;
  int i;

  curve = (stm_curve*)mbkalloc( sizeof( stm_curve ) );
  curve->MODEL    = model;
  curve->TIME.NBPOINTS = 0;
  if(nbpoints){
      curve->TIME.DATA = (stm_curve_data*)mbkalloc(nbpoints * sizeof( stm_curve_data ) );
      for(i = 0; i < nbpoints; i++){
          curve->TIME.DATA[i].T  = 0.0;
          curve->TIME.DATA[i].VE = 0.0;
          curve->TIME.DATA[i].VS = 0.0;
          curve->TIME.DATA[i].I  = 0.0;
          curve->TIME.DATA[i].ICONF = 0.0;
      }
      curve->TIME.DATAFREE = nbpoints;
  }else{
      curve->TIME.DATA     = NULL;
      curve->TIME.DATAFREE = 0;
  }
  curve->T0       = t0;
  curve->TMAX     = tmax;

  curve->STATIC.NBVE = 0;
  curve->STATIC.NBVS = 0;
  curve->STATIC.VE   = NULL;
  curve->STATIC.VS   = NULL;
  curve->STATIC.I    = NULL;
  return curve ;
}

void stm_curve_free( stm_curve *curve )
{
  if( curve->TIME.DATA )
    mbkfree( curve->TIME.DATA );
  if( curve->STATIC.VE )
    mbkfree( curve->STATIC.VE );
  if( curve->STATIC.VS )
    mbkfree( curve->STATIC.VS );
  if( curve->STATIC.I )
    mbkfree( curve->STATIC.I );
  mbkfree( curve );
}

void stm_curve_add_time_data( stm_curve *curve, 
                              float t, 
                              float ve, 
                              float vs, 
                              float i, 
                              float iconf
                            )
{
  if( curve->TIME.DATAFREE == 0 ) {
    curve->TIME.DATA = mbkrealloc( curve->TIME.DATA, 
                           (curve->TIME.NBPOINTS + 128)*sizeof( stm_curve_data )
                         );
    curve->TIME.DATAFREE = 128;
  }

  curve->TIME.DATAFREE--;
  curve->TIME.DATA[curve->TIME.NBPOINTS].T  = t;
  curve->TIME.DATA[curve->TIME.NBPOINTS].VE = ve;
  curve->TIME.DATA[curve->TIME.NBPOINTS].VS = vs;
  curve->TIME.DATA[curve->TIME.NBPOINTS].I  = i;
  curve->TIME.DATA[curve->TIME.NBPOINTS].ICONF = iconf;
  curve->TIME.NBPOINTS++;
}

void stm_alloc_static( stm_curve *curve, int nbe, int nbs )
{
  curve->STATIC.NBVE = nbe;
  curve->STATIC.NBVS = nbs;
  curve->STATIC.VE = mbkalloc( sizeof( float ) * nbe );
  curve->STATIC.VS = mbkalloc( sizeof( float ) * nbs );
  curve->STATIC.I  = mbkalloc( sizeof( float ) * nbs *nbe );
}

/******************************************************************************\
Calcule des transitions.
\******************************************************************************/

stm_curve* stm_curve_c( timing_model *model, 
                        float fin, 
                        float cout, 
                        float t0, 
                        float tmax 
                      )
{
  stm_curve *curve;
  
  switch( model->UTYPE ) {
    case STM_MOD_MODSCM :
      curve = stm_curve_mcc_c( model, fin, cout, t0, tmax );
      break;
    /*
    case STM_MOD_MODIV :
      curve = stm_curve_iv_c( model, fin, cout, t0, tmax );
      break;
    */
    default :
      curve = NULL;
  }

  return curve;
}

stm_curve* stm_curve_pi( timing_model *model, 
                         float fin, 
                         float c1out,
                         float c2out,
                         float rout,
                         float t0, 
                         float tmax 
                       )
{
  stm_curve *curve;
  
  switch( model->UTYPE ) {
    case STM_MOD_MODSCM :
      curve = stm_curve_mcc_pi( model, fin, c1out, c2out, rout, t0, tmax );
      break;
    /*  
    case STM_MOD_MODIV :
      curve = stm_curve_iv_pi( model, fin, c1out, c2out, rout, t0, tmax );
      break;
    */
    default :
      curve = NULL;
  }

  return curve;
}

void stm_model_plot( char            *filename,
                     timing_model    *model,
                     float            fin,
                     stm_pwl         *pwlin,
                     stm_driver      *driver,
                     float            rl,
                     float            c1,
                     float            c2,
                     char             evtin,
                     char             evtout,
                     float            delaytas,
                     float            slewtas,
                     char            *config
                   )
{
  float                  vdd ;
  float                  vmin ;
  float                  vmax ;
  float                  vstep ;
  float                  tstep ;
  timing_scm            *scmmodel ;
  dualparams            *scmdual ;
  float                  ts ;
  float                  vs ;
  float                  te ;
  float                  ve ;
  float                  v ;
  FILE                  *filein ;
  FILE                  *fileinpwl ;
  FILE                  *fileout ;
  FILE                  *fileoutth ;
  FILE                  *filecmd ;
  char                   buffer[1024] ;
  stm_pwth               pwthin[10] ;
  int                    nthin ;
  stm_pwth              *ptthin ;
  float                  vddin ;
  float                  vt ;
  float                  delta ;
  float                  deltai ;
  curveparam             param ;
  float                  qsat ;
  float                  load ;
  float                  loadinit ;
  float                  capa ;
  float                  capa0 ;
  float                  chalf ;
  float                  tmin, cc, cl, q, qcf ;
  float                  r ;
  double                 vcap[3], qcapai, vin ;
  int                    k ;
  double                 tin ;
  float                  tepwl ;
  float                  deltapwl ;

  vdd    = model->VDD ;
  vddin  = vdd ;
  vt     = model->VT ;
  ptthin = NULL ;
  vmin   = 0.02 * vdd ;
  vmax   = 0.98 * vdd ;
  vstep  = 0.01 * vdd ;

  if (evtin=='U') deltai = tpiv_inverter_config_t0r*1e12 ;
  else deltai = tpiv_inverter_config_t0f*1e12 ;


  switch( model->UTYPE ) {
  
  case STM_MOD_MODSCM :
    param.TYPE = STM_CURVE_DUAL ;
    scmmodel = model->UMODEL.SCM ;
    if( scmmodel->TYPE != STM_MODSCM_DUAL ) return ;
    scmdual = scmmodel->PARAMS.DUAL ;
    vt = scmdual->DP[STM_VT] ;
    if( c2>0.0 && rl>0.0 )
      load = stm_modscm_dual_capaeq( scmdual, NULL, fin, rl, c1, c2, scmdual->DP[STM_THRESHOLD], NULL );
    else
      load = c1 ;
    if( c2>0.0 && rl>0.0 )
      loadinit = stm_modscm_dual_capaeq( scmdual, NULL, fin, rl, c1, c2, vdd/20.0, NULL );
    else
      loadinit = c1 ;
    stm_modscm_dual_fill_param( scmdual, fin, pwlin, driver, load, &param.MODEL.DUAL.PARAMTIMING ) ;
    sprintf( buffer, "%s_ve_pwl.dat", filename );
    if( !param.MODEL.DUAL.PARAMTIMING.NEWCTKMODEL ) {
      if (param.MODEL.DUAL.PARAMTIMING.NTHSHRK!=0)
        stm_plot_pwth( param.MODEL.DUAL.PARAMTIMING.PWTHSHRK, 
                       buffer, 
                       deltai, 
                       param.MODEL.DUAL.PARAMTIMING.VDDIN, 
                       param.MODEL.DUAL.PARAMTIMING.VT, 
                       param.MODEL.DUAL.PARAMTIMING.THRESHOLD, 
                       evtin 
                     ) ;
    }
    
    param.MODEL.DUAL.EVTOUT = evtout ;
    param.MODEL.DUAL.VDD   = vdd ;
    vddin = scmdual->DP[STM_VDDIN] ;
    
    if( pwlin ) {
      if( stm_pwl_to_tanh( pwlin, vt, vddin+vt, pwthin, &nthin, fin ) )
        ptthin = pwthin ;
    }
    
    if( ptthin )
      te = stm_get_t_pwth( vddin, vt, vdd/2.0, ptthin );
    else {
      te = stm_modscm_dual_calte (vddin, vt, vdd/2.0, fin );
    }
    
    tepwl = te ;
    break ;
    
  case STM_MOD_MODIV :
    param.TYPE           = STM_CURVE_IV ;
    param.MODEL.IV.FIN   = fin ;
    param.MODEL.IV.LOAD  = load ;
    param.MODEL.IV.VDD   = vdd ;
    param.MODEL.IV.MODEL = model->UMODEL.IV ;
    te = stm_modiv_calcte( param.MODEL.IV.MODEL, param.MODEL.IV.FIN, param.MODEL.IV.VDD );
    break ;

  default : 
    return ;
  }

  /******* configuration ***********************************/

  delta = deltai-te ;
  deltapwl = deltai-tepwl ;
  
  sprintf( buffer, "%s_ve.dat", filename );
  filein = fopen( buffer, "w" );
  sprintf( buffer, "%s_vs.dat", filename );
  fileout = fopen( buffer, "w" );
    
  /******* drive output voltage from stm model *************/

  if( param.MODEL.DUAL.PARAMTIMING.NEWCTKMODEL ) {
    newctktrace debug ;
    double      ceqrsat ;

    printf( "WARNING : STM plot only valid at threshold !\n" );

    sprintf( buffer, "%s_ve_pwl.dat", filename );
    fileinpwl = fopen( buffer, "w" );
    param.MODEL.DUAL.PARAMTIMING.EVTIN = evtin ;
    param.MODEL.DUAL.PARAMTIMING.CAPA  += load ;
    param.MODEL.DUAL.PARAMTIMING.CAPA0 += load ;
    param.MODEL.DUAL.PARAMTIMING.CHALF += load ;
    
    stm_modscm_dual_calts_qsat( &(param.MODEL.DUAL.PARAMTIMING), 
                                param.MODEL.DUAL.PARAMTIMING.THRESHOLD, 
                                NULL, 
                                NULL, 
                                &ceqrsat, 
                                NULL 
                              );
    stm_modscm_dual_calts_newctk( &(param.MODEL.DUAL.PARAMTIMING), 
                                  param.MODEL.DUAL.PARAMTIMING.THRESHOLD, 
                                  ceqrsat,
                                  NULL
                                );
    debug.DT   = deltai - param.MODEL.DUAL.PARAMTIMING.TIN ;
    debug.FIN  = fileinpwl ;
    debug.FOUT = fileout ;

    stm_modscm_dual_calts_newctk( &(param.MODEL.DUAL.PARAMTIMING), 
                                  param.MODEL.DUAL.PARAMTIMING.THRESHOLD, 
                                  ceqrsat,
                                  &debug
                                );
    fclose( fileinpwl );
  }
  else {

    capa = param.MODEL.DUAL.PARAMTIMING.CAPA ;
    capa0 = param.MODEL.DUAL.PARAMTIMING.CAPA0 ;
    chalf = param.MODEL.DUAL.PARAMTIMING.CHALF ;

    tmin = stm_modscm_dual_calte( vddin, vt, 0.0, fin );
    tstep = -tmin/100.0 ;
    
    r  = scmdual->DP[STM_RCONF] ;
    cc = param.MODEL.DUAL.PARAMTIMING.CAPAIE[0]/1000.0 ;
    cl = (capa + loadinit)/1000.0 ;

    
    for( ts=tmin ; ts<0.0 ; ts = ts+tstep ) {
      v = -r*cc*(vdd-vt)/fin*(1.0-exp(-(ts-tmin)/(r*(cl+cc))));
      if( param.MODEL.DUAL.EVTOUT == 'U' )
        vs = v ;
      else
        vs = param.MODEL.DUAL.VDD-v ;
      fprintf( fileout, "%g %g %g\n", (ts+deltapwl)*1e-12, vs, 0.0 );
    }

    ts=0.0 ;
    v = -r*cc*(vdd-vt)/fin*(1.0-exp(-(ts-tmin)/(r*(cl+cc))));
    if( param.MODEL.DUAL.EVTOUT == 'U' )
      vs = v ;
    else
      vs = param.MODEL.DUAL.VDD-v ;
    fprintf( fileout, "%g %g %g\n", (ts+deltapwl)*1e-12, vs, 0.0 );

    vcap[0] = scmdual->DP[STM_VT0] ;
    vcap[1] = vdd/2.0 ;
    vcap[2] = scmdual->DP[STM_VT0C] ;
    qsort( vcap, 3, sizeof(double), (int(*)(const void*,const void*))mbk_qsort_dbl_cmp );

    k=0 ;
    qcapai = 0.0;
    
    while( v <=0.0 )
    {
      vin=vt+(vdd-vt)*tanh(ts/fin);
      while( k<3 && vin > vcap[k] ) {
        k++ ;
        qcapai = qcapai + vcap[k-1]*(param.MODEL.DUAL.PARAMTIMING.CAPAIE[k-1]-param.MODEL.DUAL.PARAMTIMING.CAPAIE[k])/1000.0;
      }
      
      cc  = param.MODEL.DUAL.PARAMTIMING.CAPAIE[k]/1000.0;

      if( param.MODEL.DUAL.PARAMTIMING.ICF0 > 0.0 && param.MODEL.DUAL.PARAMTIMING.STRANS > 0.0 ) {
        tin = ts ;
        if( ts > param.MODEL.DUAL.PARAMTIMING.STRANS )
          tin = param.MODEL.DUAL.PARAMTIMING.STRANS ;
        qcf = -param.MODEL.DUAL.PARAMTIMING.ICF0*( tin-tin*tin/(2.0*param.MODEL.DUAL.PARAMTIMING.STRANS) );
        //qcf = -param.MODEL.DUAL.PARAMTIMING.ICF0*ts ;
      }
      else
        qcf = 0.0 ;
        
      q = stm_get_q( param.MODEL.DUAL.PARAMTIMING.IMAX, ts, param.MODEL.DUAL.PARAMTIMING.F, param.MODEL.DUAL.PARAMTIMING.BN, param.MODEL.DUAL.PARAMTIMING.U );
      v = (q-qcf-qcapai-param.MODEL.DUAL.PARAMTIMING.QINIT-cc*( param.MODEL.DUAL.PARAMTIMING.VT + param.MODEL.DUAL.PARAMTIMING.VDDIN*tanh(ts/param.MODEL.DUAL.PARAMTIMING.FIN) ))/cl ;
      if( param.MODEL.DUAL.EVTOUT == 'U' )
        vs = v ;
      else
        vs = param.MODEL.DUAL.VDD-v ;
      fprintf( fileout, "%g %g %g\n", (ts+deltapwl)*1e-12, vs, 0.0 );
      ts = ts+tstep ;
    }

    for( v = vmin ; v <= vmax ; v = v + vstep ) {
   
      qsat = 0.0 ;
      
      switch( param.TYPE ) {
      case STM_CURVE_DUAL :
        if( param.MODEL.DUAL.PARAMTIMING.INTEGNUMERIC ) {
          param.MODEL.DUAL.PARAMTIMING.CAPA  = capa  + load ;
          param.MODEL.DUAL.PARAMTIMING.CAPA0 = capa0 + load ;
          param.MODEL.DUAL.PARAMTIMING.CHALF = chalf + load ;
          stm_modscm_dual_calts_final_numeric(  &param.MODEL.DUAL.PARAMTIMING, &v, &ts, 1 );
        }
        else {
          if( V_BOOL_TAB[ __AVT_NUMSOL_FOR_PILOAD ].VALUE && c2>0 && rl>0.0 ) {
            stm_modscm_dual_calts_final_numeric_pi(  &param.MODEL.DUAL.PARAMTIMING, rl, c1, c2, &v, &ts, 1 );
          }
          else {
            if( c2>0.0 && rl>0.0 ) {
              param.MODEL.DUAL.PARAMTIMING.CAPA = capa ;
              load = stm_modscm_dual_capaeq( scmdual, &param.MODEL.DUAL.PARAMTIMING, fin, rl, c1, c2, v, NULL );
            }
            else
              load = c1 ;
            param.MODEL.DUAL.PARAMTIMING.CAPA  = capa  + load ;
            param.MODEL.DUAL.PARAMTIMING.CAPA0 = capa0 + load ;
            param.MODEL.DUAL.PARAMTIMING.CHALF = chalf + load ;
            ts = stm_modscm_dual_calts_with_param_timing( &param.MODEL.DUAL.PARAMTIMING, v );
            qsat = param.MODEL.DUAL.PARAMTIMING.CEQRSAT * v ;
          }
        }
        if( param.MODEL.DUAL.EVTOUT == 'U' )
          vs = v ;
        else
          vs = param.MODEL.DUAL.VDD-v ;
        break ;
      case STM_CURVE_IV :
        stm_modiv_eval_c( param.MODEL.IV.MODEL, 
                          param.MODEL.IV.FIN,
                          param.MODEL.IV.LOAD,
                          v,
                          param.MODEL.IV.VDD,
                          & ts,
                          NULL
                        );
        vs = v ;
        break ;
      }
      
      fprintf( fileout, "%g %g %g\n", (ts+deltapwl)*1e-12, vs, qsat*1e-12  );
    }
  }


  /******* drive input voltage from stm model **************/
 
  for( v = vmin ; v <= vmax ; v = v + vstep ) {

    switch( param.TYPE ) {
    case STM_CURVE_DUAL :
      if( ptthin )
        te = stm_get_t_pwth( vddin, vt, v, ptthin );
      else
        te = stm_modscm_dual_calte (vddin, vt, v, fin );
      if( evtin == 'U' )
        ve = v ;
      else
        ve = vdd-v ;
      break ;

    case STM_CURVE_IV :
      te = stm_get_t( v, param.MODEL.IV.MODEL->IV_INPUT.VT, param.MODEL.IV.MODEL->IV_INPUT.VI, param.MODEL.IV.MODEL->IV_INPUT.VF, param.MODEL.IV.FIN );
      ve = v ;
      break ;
    }
      
    fprintf( filein, "%g %g\n", (te+delta)*1e-12, ve );
  }

  fclose( fileout );
  fclose( filein );

  /******* drive tas output voltage ************************/
  
  if( slewtas > 0.0 ) {
  
    sprintf( buffer, "%s_vs_tas.dat", filename );
    fileoutth = fopen( buffer, "w" );
  
    ts = stm_modscm_dual_calte (vddin, vt, vdd/2.0, slewtas );
    delta = deltai + delaytas - ts ;

    for( v = vmin ; v <= vmax ; v = v + vstep ) {
  
      ts = stm_modscm_dual_calte (vddin, vt, v, slewtas );
      if( evtout == 'U' )
        ve = v ;
      else
        ve = vdd-v ;
        
      fprintf( fileoutth, "%g %g\n", (ts+delta)*1e-12, ve );
    }
  
    fclose( fileoutth );

  }

  /******* drive gnuplot command file******** *************/

  sprintf( buffer, "%s.plt", filename );
  filecmd = fopen( buffer, "w" );
  if( !filecmd )
    return ;

  fprintf( filecmd, "plot '%s_vs.dat' using 1:2 title \"vs\", '%s_ve.dat' using 1:2 title \"ve\"\n", filename, filename );
  fprintf( filecmd, "pause -1\n" );

  fclose( filecmd );

  config = NULL ;
}
