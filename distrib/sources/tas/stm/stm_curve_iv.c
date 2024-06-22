/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curve_iv.c                                              */
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

stm_curve* stm_curve_iv_c( timing_model *model,
                           float         fin,
                           float         cout,
                           float         t0,
                           float         tmax
                         )
{
  return stm_curve_iv_pi( model, fin, cout, 1.0, 1e9, t0, tmax );
}

stm_curve* stm_curve_iv_pi( timing_model *model,
                            float         fin,
                            float         c1out,
                            float         c2out,
                            float         rout,
                            float         t0,
                            float         tmax
                          )
{
  stm_simu_tanh_param         slope;
  stm_simu_pi_load_parameter param;
  timing_iv                  *iv;
  char                        ret;
  stm_curve                  *curve;
  float                       ve, vs;
  int                         ne, ns;
  char                        r;
  float                       i;
  int                         nbe;
  int                         nbs;
  float                       te, ts;
  float                       vdd, vth;
  int                         n;
  float                       q;

  curve = stm_curve_alloc( model, t0, tmax, 0 );

  // Comportement dynamique

  param = stm_simu_param_standard;
  param.CURVE = curve;
  param.TMAX  = tmax*1000.0;
  param.T0    = t0*1000.0;
  
  vdd = stm_mod_vdd( model );
  vth = stm_mod_vth( model );
  
  iv = model->UMODEL.IV;
  if( !iv )
    return NULL;
    
  slope.VT = stm_modiv_in_vi( iv );
  slope.VF = stm_modiv_in_vf( iv );
  slope.F  = fin * 1000.0 ;
  if( slope.VT < slope.VF )
    slope.VI = 0.0;
  else
    slope.VI = vdd;

  ret = stm_simu_pi_load_ts( (char(*)(void*,float,float,float*))
                                                              stm_modiv_calc_is,
                             (char(*)(void*,float,float*))stm_simu_tanh,
                             iv,
                             &slope,
                             rout,
                             c1out + stm_modiv_cconf( iv, fin ),
                             c2out,
                             stm_modiv_ti_vi( iv ),
                             vth,
                             NULL,
                             NULL,
                             &param
                           );
  
  // Superpose la capacité de conflit.
  
  // 1) récupère te, ts
  te=0.0;
  ts=0.0;
  for( n=0 ; n < curve->TIME.NBPOINTS && (te==0.0 || ts==0.0) ; n++ ) {
    if( ( curve->TIME.DATA[n].VE   >= slope.VT && 
          curve->TIME.DATA[n-1].VE <= slope.VT ) ||
        ( curve->TIME.DATA[n].VE   <= slope.VT && 
          curve->TIME.DATA[n-1].VE >= slope.VT )   )
      te=curve->TIME.DATA[n].T;
    if( ( curve->TIME.DATA[n].VS   >= vth && 
          curve->TIME.DATA[n-1].VS <= vth ) ||
        ( curve->TIME.DATA[n].VS   <= vth && 
          curve->TIME.DATA[n-1].VS >= vth )   )
      ts=curve->TIME.DATA[n].T;
  }
  
  // 2) calcule du conflit
  q=iv->IV_CONF.PCONF0 + fin * iv->IV_CONF.PCONF1;
  q=q/(ts-te)/1000.0*vdd/2.0;
  
  // 3) remplissage du tableau
  for( n=0 ; n < curve->TIME.NBPOINTS && curve->TIME.DATA[n].T < ts ; n++ ) {
    if( curve->TIME.DATA[n].T >= te )
      curve->TIME.DATA[n].ICONF = q ;
  }
  
  // Comportement statique

  nbe = 30;
  nbs = 30;

  stm_alloc_static( curve, nbe+1, nbs+1 );

  for( ne = 0 ; ne <= nbe ; ne++ ) {

    ve = stm_mod_vdd(model)*ne/nbe;
    curve->STATIC.VE[ne] = fabs(ve) ;
    
    for( ns = 0 ; ns <= nbs ; ns++ ) {
    
      vs = stm_mod_vdd(model)*ns/nbs;
      curve->STATIC.VS[ns] = fabs(vs) ;
      
      // On joue le jeu : on recalcule i
      
      r = stm_modiv_calc_is( iv, ve, vs, &i );
      if( r )
        *( STM_CURVE_STATIC_GET_PTR_I( curve, ne, ns ) ) = fabs(i) ;
      else
        *( STM_CURVE_STATIC_GET_PTR_I( curve, ne, ns ) ) = -1.0 ;
    }
  }

  return curve;
}
