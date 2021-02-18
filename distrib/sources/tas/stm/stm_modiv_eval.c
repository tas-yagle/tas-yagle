/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_solver.c                                                */
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

/******************************************************************************\
Global definition
\******************************************************************************/

void stm_modiv_timing_pi( timing_iv *iv,
                          float fin,
                          float r,
                          float c1,
                          float c2,
                          float vth,
                          float vdd,
                          float *delay,
                          float *fout
                        )
{
  float te ;

  if( stm_modiv_eval_pi( iv, fin, r, c1, c2, vth, vdd, delay, fout ) ) {
    if( delay ) {
      te = stm_modiv_calcte( iv, fin, vdd );
      *delay = *delay - te;
    }
  }
  else {
    if( delay ) *delay = 0.0 ;
    if( fout  ) *fout  = 0.0 ;
  }
}

void stm_modiv_timing_c( timing_iv *iv,
                          float fin,
                          float c,
                          float vth,
                          float vdd,
                          float *delay,
                          float *fout
                        )
{
  stm_modiv_timing_pi( iv, fin, 1e9, c, 1.0, vth, vdd, delay, fout );
}

/******************************************************************************\
Delay for a ground capacitance load
\******************************************************************************/

float stm_modiv_delay_c( timing_iv *iv, 
                         float fin, 
                         float c, 
                         float vth, 
                         float vdd 
                       )
{
  float delay;
  float te;

  if( stm_modiv_eval_c( iv, fin, c, vth, vdd, &delay, NULL ) ) {
    te = stm_modiv_calcte( iv, fin, vdd );
    delay = delay - te;
    return delay;
  }
  else
    delay = 0.0;

  return delay ;
}

/******************************************************************************\
Delay for a pi cell load
\******************************************************************************/
float stm_modiv_delay_pi( timing_iv *iv,
                          float fin,
                          float r,
                          float c1,
                          float c2,
                          float vth,
                          float vdd
                        )

{
  float delay;
  float te;

  if( stm_modiv_eval_pi( iv, fin, r, c1, c2, vth, vdd, &delay, NULL ) ) {
    te = stm_modiv_calcte( iv, fin, vdd );
    delay = delay - te;
  }
  else
    delay = 0.0;

  return delay;
}

/******************************************************************************\
Output slew for a ground capacitance load
\******************************************************************************/

float stm_modiv_slew_c( timing_iv *iv, 
                        float fin, 
                        float c, 
                        float vth,
                        float vdd
                      )
{
  float slope;

  if( stm_modiv_eval_c( iv, fin, c, vth, vdd, NULL, &slope ) )
    return slope;

  return 0.0;
}

/******************************************************************************\
Output slew for a pi cell load
\******************************************************************************/

float stm_modiv_slew_pi( timing_iv *iv,
                         float      fin,
                         float      r,
                         float      c1,
                         float      c2,
                         float      vth,
                         float      vdd
                       )
{
  float slope;

  if( stm_modiv_eval_pi( iv, fin, r, c1, c2, vth, vdd, NULL, &slope ) )
    return slope;

  return 0.0;
}

/******************************************************************************\
Evalutation current
return value :
  1 - Ok
  0 - Ko
\******************************************************************************/

char stm_modiv_calc_is( timing_iv *iv, float ve, float vs, float *is )
{
  int   ne;
  int   ns;
  float i00;
  float i01;
  float i10;
  float i11;
  float i0x;
  float i1x;
  
  for( ne=1 ; ne<iv->NVE ; ne++ ) 
    if( ve >= iv->VE[ne-1] && ve <= iv->VE[ne] )
      break;

  if( ne >= iv->NVE )
    ne = iv->NVE-1;
    
  for( ns=1 ; ns<iv->NVS ; ns++ ) 
    if( vs >= iv->VS[ns-1] && vs <= iv->VS[ns] )
      break;

  if( ns >= iv->NVS )
    ns = iv->NVS-1;

  i00 = stm_modiv_getis( iv, ne-1, ns-1 );
  i01 = stm_modiv_getis( iv, ne-1, ns   );
  i10 = stm_modiv_getis( iv, ne,   ns-1 );
  i11 = stm_modiv_getis( iv, ne,   ns   );

  i0x = ( i01 - i00 ) * (vs - iv->VS[ns-1])/(iv->VS[ns] - iv->VS[ns-1]) + i00 ;
  i1x = ( i11 - i10 ) * (vs - iv->VS[ns-1])/(iv->VS[ns] - iv->VS[ns-1]) + i10 ;

  *is = ( i1x - i0x ) * (ve - iv->VE[ne-1])/(iv->VE[ne] - iv->VE[ne-1]) + i0x ;

  return 1;
}

/******************************************************************************\
Evaluation of delay (ts) and slew (fs) for a pi load
Return value :
  1 - Ok
  0 - Ko

Units for parameters :
  delay         : picosecond
  resistance    : ohm
  capacitance   : femto-farad
  voltage       : volt
\******************************************************************************/

char stm_modiv_eval_pi( timing_iv *iv,
                        float      fin,
                        float      r,
                        float      c1,
                        float      c2,
                        float      vth,
                        float      vdd,
                        float     *ts,
                        float     *fs
                      )
{
  stm_simu_tanh_param slope;
  char                ret;
  float               cconf;
  char                *plotcurvename=NULL;
 
  if( !iv )
    return 0;

  slope.VT = stm_modiv_in_vt( iv );
  slope.VF = stm_modiv_in_vf( iv );
  slope.VI = stm_modiv_in_vi( iv );
  slope.F  = fin * 1000.0 ;
 
  cconf = stm_modiv_cconf( iv, fin, c1 );

  ret = stm_simu_pi_load_ts( (char(*)(void*,float,float,float*))
                                                              stm_modiv_calc_is,
                             (char(*)(void*,float,float*))stm_simu_tanh,
                             iv,
                             &slope,
                             r,
                             c1 + cconf,
                             c2,
                             stm_modiv_ti_vi( iv ),
                             vth,
                             ts,
                             fs,
                             NULL,
                             plotcurvename
                           );

  if( ret ) {
    if( fs )
      *fs = vdd / ((*fs)*1000.0) * 0.6 ;
    if( ts )
      *ts = (*ts) / 1000.0 ;
  }
  return ret;
}

/******************************************************************************\
Evaluation of delay and slew for a single capacitance load
\******************************************************************************/

char stm_modiv_eval_c( timing_iv *iv,
                       float      fin,
                       float      c,
                       float      vth,
                       float      vdd,
                       float     *ts,
                       float     *fs
                     )
{
  return stm_modiv_eval_pi( iv,
                            fin,
                            1.0e9,
                            c,
                            1.0,
                            vth,
                            vdd,
                            ts,
                            fs
                          );
}

/******************************************************************************\
Evaluation of conflict capacitance
\******************************************************************************/

float stm_modiv_cconf( timing_iv *iv, float fin, float load )
{
  float c ;
  float pconf1 ;

  pconf1 = iv->IV_CONF.PCONF1 ;
  if( pconf1*fin < 10.0*load ) 
    pconf1 = 1.2 * pconf1 ;

  c = iv->IV_CONF.CI + ( iv->IV_CONF.PCONF0 + pconf1 * fin ) * iv->IV_CONF.IRAP;

  return c;
}

/******************************************************************************\
Evaluation of commutation input instant
\******************************************************************************/

float stm_modiv_calcte( timing_iv *iv, float fin, float vdd )
{
  float t;
  float vi;
  float vf;
  float vs;
  float vt;

  vi = stm_modiv_in_vi( iv );
  vt = stm_modiv_in_vt( iv );
  vf = stm_modiv_in_vf( iv );
  vs = stm_modiv_in_vth( iv );
  
  t = stm_get_t( vs, vt, vi, vf, fin );

  return t;
}
