/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_curvemcc.c                                              */
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

float stm_get_q( float imax, float t, float F, float B, float U )
{
  float q;
  float BU ;
  float BU2 ;

  BU  = B * U ;
  BU2 = BU * BU ;

  if( t > 0.0 )
  {
    if (BU==1)
      q = imax * ( t - F * (LOG_2))
          + F * imax * (2 * log(1.0 + exp(-2.0 * t / F)));
    else
      q = imax * ( t - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
          + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * t / F)) + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * t / F)));
  }
  else
    q = 0.0 ;
  return q;
}

float stm_get_ic_imax (float t, float U, float imax, float F, float bn)
{
    double ic;
    
    if(t < 0.0){
        ic = 0.0;
    }else{
        ic = imax * ((1.0 + bn * U) * tanh( t / F) * tanh( t / F)) / (1.0 + bn * U * tanh( t / F));
    }

    return (float)ic;
        
}

float stm_get_ic (float v, float vt, float vi, float vf, float an, float bn)
{
    if(vi < vf){
        if(v < vt)
            return 0.0;
        else
            return (an * (v-vt)*(v-vt)/(1 + bn*(v-vt)));
    }else if(vi > vf){
        if(v > vi - vt)
            return 0.0;
        else
            return (an * (vi-v-vt)*(vi-v-vt)/(1 + bn*(vi-v-vt)));
    }else{
        return 0.0;
    }
    
}
float stm_get_v (float t, float vt, float vi, float vf, float F)
{
  float t0 ;
  float v ;


  if(vi < vf){
    t0 =  F*(vi-vt)/(vf-vt) ;
    if( t < 0.0 )
      v = vi ;
    else {
      if ( t < -t0 )
        v = vt + (vf-vt)*(t+t0)/F + vi ;
      else
        v = vt + (vf-vt)*tanh((t+t0)/F) + vi ;
    }
  }
  else {
    if(vi > vf){
      t0 = F*(vt-vi)/(vf-vt) ;
      if( t < 0.0 )
        v = vi ;
      else {
        if( t < t0 ) 
          v = vi + (vf-vt)*t/F ;
        else
          v = (vf-vt)*tanh((t-t0)/F) + vt ;
      }
    }
    else{
      v = 0.0 ;
    }
  }

  return v ;
}

float stm_get_v_fromvt (float t, float vt, float vi, float vf, float F)
{
    double vx;
    
    if(vi < vf){
        vx = (vt + (vf - vt) * tanh(t / F));
        if(vx < vi)
            return vi;
        else if(vx > vf)
            return vf;
        else
            return vx;
    }else if(vi > vf){
        vx = ((vi - vt) - (vi - vt - vf) * tanh(t / F));
        if(vx > vi)
            return vi;
        else if(vx < vf)
            return vf;
        else
            return vx;
    }else{
        return 0.0;
    }
}

float stm_get_v_deltat (float t, float deltat, float vt, float vi, float vf, float F)
{

    if(vi < vf){
        if (t < deltat)
            return vi;
        else
            return (vt + (vf - vt) * tanh((t - deltat) / F));
    }else if(vi > vf){
        if (t < deltat)
            return vi;
        else
            return ((vi - vt) - (vi - vt - vf) * tanh((t - deltat) / F));
    }else{
        return 0.0;
    }
}

float stm_get_iconf (timing_model *model, float t, float vi, float vf, float F, float deltat, float ts)
{
    float t0;
    float vt     = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VT];
    float vddmax = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VDDMAX];
    float pconf0 = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_PCONF0];
    float pconf1 = model->UMODEL.SCM->PARAMS.DUAL->DP[STM_PCONF1];
    float cconf;
    float q;
    
    if(deltat < F)
        cconf = pconf0 + pconf1 * (F - deltat);
    else
        cconf = pconf0;
    
    q = cconf * vddmax / 2.0;

    if(vi < vf){
        t0 = stm_get_t(vt, vt, vi, vf, F);
    }else{
        t0 = stm_get_t(vddmax - vt, vt, vi, vf, F);
    }

    if((t < t0) || (t > ts))
        return 0.0;
    else
        return (q / (ts - t0) / 1000.0);
}

float stm_get_t (float v, float vt, float vi, float vf, float F)
{
    double t0, a;

    if(vi < vf){
        if((vt / (vf - vt)) < 1) {
          t0 = F * atanh(vt / (vf - vt));
          return (F * atanh((v - vt)/(vf - vt)) + t0);
        } else {
           if( vt/2.0 > vf - 2.0*(vf-vt) ) {
             t0 = F * atanh(vt/2.0 / (vf - vt));
             if (v < vt/2.0)
               // return ((2 * t0) * (v - vt) / vt + 2 * t0) ;
               return 2*t0*v/vt ;
             else
               return (F * atanh((v - vt)/(vf - vt)) + 2*t0);
           }
           else {
             a = (vf-vt)/F;
             t0 = vt/a ;
             if( v > vt )
               return (F * atanh((v - vt)/(vf - vt)) + t0);
             else
               return v/a ;
           }
        }
    }else if(vi > vf){
        if((vt / (vi - vt - vf)) < 1) {
          t0 = F * atanh(vt / (vi - vt - vf));
          return (F * atanh((vi - vt - v) / (vi - vt - vf)) + t0) ;
        } else {
           if( vi-vt/2.0 < 2*(vi-vt-vf) ) {
             t0 = F * atanh(vt/2.0 / (vi - vt - vf));
             if (v > vi-vt/2.0)
                //return ((2 * t0) * (v - (vi - vt)) / (vi - vt) + 2 * t0) ;
                return -2.0*t0*(v-vi)/vt;
             else
                return (F * atanh((vi - vt - v) / (vi - vt - vf)) + 2 * t0) ;
           }
           else {
             a = -(vi-vt-vf)/F ;
             t0 = -vt/a ;
             if( v > vi-vt )
               return (v-vi)/a;
             else
               return (F * atanh((vi - vt - v) / (vi - vt - vf)) + t0) ;
             
           }
        }
    }else{
        return 0.0;
    }
}

float stm_get_t_fromvt (float v, float vt, float vi, float vf, float F)
{
    if(vi < vf){
        return (F * atanh((v - vt)/(vf - vt)));
    }else if(vi > vf){
        return (F * atanh((vi - vt - v) / (vi - vt - vf))) ;
    }else{
        return 0.0;
    }
}

float stm_curve_calc_is(timing_model *model, float ve, float vs)
{
    double an        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_AN];
    double bn        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_BN];
    double vt        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VT];
    double imax      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_IMAX];
    double rlin      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RLIN];
    double vsat      = imax * rlin;
    
    if(ve < vt)
        return 0.0;
    else if(vs < vsat)
        return (float)(vs * (an * (ve * ve))/(1 + (bn * ve))/(vsat));
    else
        return (float)((an * (ve * ve))/(1 + (bn * ve)));
}

stm_curve* stm_curve_mcc_c_old( timing_model *model,
                            float fin, 
                            float cout, 
                            float t0, 
                            float tmax 
                          )
{
    char      *STM_PLOT_VS ;
    char       type ;
    stm_curve *curve;
    double vt        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VT];
    double vddmax    = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VDDMAX];
    double threshold = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_THRESHOLD];
    int i, nbpoints = (int)tmax - (int)t0;
    double vs, vi, vf, ts, tp, tinput, toutput, fout, ic, iconf, t = 0.0 ;
    int ne, ns;
    int nbe;
    int nbs;
    double ve ;

    STM_PLOT_VS = getenv("STM_PLOT_VS_WITH");
    type = 0;
    
    if( STM_PLOT_VS ) {
      if( strcasecmp( STM_PLOT_VS, "tanh" )==0 )
        type='t';
      if( strcasecmp( STM_PLOT_VS, "q" )==0 )
        type='q';
      if( !type ) {
        printf( "bad value for variable STM_PLOT_VS\n" );
        
      }
    }
    if( !type )
      type='t';
  
    if(model->VF < vddmax/2.0){
        vi = 0.0;
        vf = vddmax;
    }else{
        vi = vddmax;
        vf = 0.0;
    }


    curve = stm_curve_alloc( model, t0, tmax , nbpoints);
  
    stm_curve_add_time_data(curve, 0.0, vi, vf, 0.0, 0.0);

    switch( type ) {
    
    case 't' :
    
      tp        = stm_modscm_dual_delay (model->UMODEL.SCM->PARAMS.DUAL, fin, NULL, cout);
      fout      = stm_modscm_dual_slew (model->UMODEL.SCM->PARAMS.DUAL, fin, NULL, NULL, cout);
      tinput    = stm_get_t (threshold, vt, vi, vf, fin);
      toutput   = stm_get_t (threshold, vt, vf, vi, fout);
      ts        = tinput + tp - toutput;
      
      for(i = 1; i < nbpoints; i++) {
        ve = stm_get_v (t, vt, vi, vf, fin);
        vs = stm_get_v (t - ts, vt, vf, vi, fout);
        stm_curve_add_time_data(curve, t + t0, ve, vs, ic, iconf);
        t++;
      }
      
      break ;
      
    case 'q' :
    
      tinput    = stm_get_t (vt, vt, vi, vf, fin);
      
      for( i=1 ; i < nbpoints ; i++ ) {
        vs = i*vddmax/nbpoints ;
        t = stm_modscm_dual_calculduts_threshold( model->UMODEL.SCM->PARAMS.DUAL, fin, cout, vs, NULL );
        ve = stm_get_v (t+tinput, vt, vi, vf, fin);
        if( model->VF < vddmax/2.0 )
          vs = vddmax-vs ;
        stm_curve_add_time_data(curve, t + t0 + tinput, ve, vs, ic, iconf);
      }

      break ;
    }
    
  // Comportement statique

  nbe = 10;
  nbs = 10;

  stm_alloc_static( curve, nbe, nbs );

  for( ne = 0 ; ne < nbe ; ne++ ) {

    ve = stm_mod_vdd(model)*ne/nbe;
    curve->STATIC.VE[ne] = ve ;
    
    for( ns = 0 ; ns < nbs ; ns++ ) {
    
      vs = stm_mod_vdd(model)*ns/nbs;
      curve->STATIC.VS[ns] = vs ;
      
      *( STM_CURVE_STATIC_GET_PTR_I( curve, ne, ns ) ) = stm_curve_calc_is( model, ve, vs); ;
    }
  }


    
    return curve;
}

stm_curve* stm_curve_mcc_c( timing_model *model, 
                        float fin, 
                        float cout, 
                        float t0, 
                        float tmax 
                      )
{
    char *STM_PLOT_VS_WITH_Q   = getenv("STM_PLOT_VS_WITH_Q");
    char *STM_PLOT_VS_END_TANH = getenv("STM_PLOT_VS_END_TANH");
    char *STM_PLOT_VS_END_RLIN = getenv("STM_PLOT_VS_END_RLIN");

    stm_curve *curve;
    double an        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_AN];
    double bn        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_BN];
    double vt        = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VT];
    double vddmax    = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VDDMAX];
    double imax      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_IMAX];
    double vddin     = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_VDDIN];
    double threshold = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_THRESHOLD];
    double capai     = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_CAPAI];
    double rbr       = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RBR];
    double cbr       = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_CBR];
    double pconf0    = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_PCONF0];
    double pconf1    = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_PCONF1];
    double irap      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_IRAP];
    double delayrc   = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_DRC];
    double rsat      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RSAT];
    double rlin      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_RLIN];
    int i, nbpoints = (int)tmax - (int)t0;
    double vs, vi, vf, ts, tp, tinput, toutput, fout, ic, iconf, t = 0.0, deltat, tvt ;
    int ne, ns;
    int nbe;
    int nbs;
    double ttp;
    double tf; 
    double U, F, Ur, Fr, im, Fs, tm, Fm, k4Q, to, Q, ve, te, ceqrsat ;
    double capa, capa0, cconf, thrlin, vsf, fo ;
    double crsatf;
    int j;
    
    if(model->VF < vddmax/2.0){
        vi = 0.0;
        vf = vddmax;
    }else{
        vi = vddmax;
        vf = 0.0;
    }

    tp = stm_modscm_dual_delay (model->UMODEL.SCM->PARAMS.DUAL, fin, NULL, cout);
    fout = stm_modscm_dual_slew (model->UMODEL.SCM->PARAMS.DUAL, fin, NULL, NULL, cout);
    tinput = stm_get_t (threshold, vt, vi, vf, fin);
    toutput = stm_get_t (threshold, vt, vf, vi, fout);
    ts = tinput + tp - toutput;
   
    if( vi < vf )
      ttp = stm_get_t( vt, vt, vi, vf, fin );
    else
      ttp = stm_get_t( (vi-vt), vt, vi, vf, fin );

    curve = stm_curve_alloc( model, t0, tmax , nbpoints);
  
    if((rbr > 0.0) && (cbr > 0.0)){
      stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, fin, &Ur, &Fr);
      stm_modscm_dual_slopei(0.7, bn, Ur, imax, Fr, rsat, vddmax, threshold, &im, &Fs);
      stm_modscm_dual_modifslope( vddin, vt, fin, vddmax, rbr, cbr, im, Fs, &tm, &Fm);
    }
    else {
        Fm = fin;
    }
    
    stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, Fm, &U, &F);
    
    if((capai > 0.00) && (capai/(cout + (pconf0 + pconf1 * fin) * irap) > STM_OVERSHOOT))
      deltat = stm_modscm_dual_calts (imax, an, bn, U, threshold, 0.0, F, 0.0, vt, vddin, capai, NULL, NULL, fin, NULL, 0, 0.0, 0.0, 0.0, NULL );
    else
      deltat = 0.0;
    deltat += delayrc ;
    if(deltat < fin)
      cconf = pconf0 + pconf1 * (fin - deltat);
    else
      cconf = pconf0;
    capa0 = cconf * irap + cout;
    
    to = stm_modscm_dual_calts (imax, an, bn, U, threshold, capa0, F, 0.0, vt, vddin, capai, NULL, NULL, fin, NULL, 0, 0.0, 0.0, 0.0, NULL );

    capa = capa0 ;

    if((rbr > 0.0) && (cbr > 0.0)){
      k4Q = stm_modscm_dual_get_k4Q( an, bn, vddin, fin, Ur, Fr, U, F, rbr, cbr, im, Fs, vddmax, rsat, rlin, -1.0, -1.0, vt );
      Q = stm_modscm_dual_get_Q( to, k4Q, an, bn, vddin, fin, Ur, Fr, U, F, rbr, cbr, im, Fs, tm, vddmax, vt, rsat, rlin, -1.0, -1.0, fout, threshold);
      capa -= 2.0 * Q / vddmax;
    }

    if( rsat > 0.0 ) {
      fo = stm_modscm_dual_calslew( imax, an, bn, U, vt, vddmax, capa, fin, F, to) ;
      te = stm_modscm_dual_calte( vddin, vt, vt, fin );
      /*
      ceqrsat = stm_modscm_dual_ceqrsat( vt, vddmax, fo, rsat, threshold, te, to ) ;
      capa = capa + ceqrsat ;
      */
    }

    tvt = stm_get_t (vt, vt, 0.0, U + vt, F);

    thrlin = ( imax - vddmax / rsat ) * rlin * rsat / ( rsat - rlin ) ;
    if( vi>vf )
      thrlin = vddmax - thrlin ;

    stm_curve_add_time_data(curve, 0.0, vi, vf, 0.0, 0.0);
    tf=-1.0;
    for(i = 1; i < nbpoints; i++){
      ve = stm_get_v (t, vt, vi, vf, fin);
      if((rbr > 0.0) && (cbr > 0.0))
        ic = stm_modscm_dual_get_ir( t - tvt, an, bn, vddin, fin, Ur, Fr, rbr, cbr/1000.0, imax, Fs, vddmax, rlin, rsat, -1.0, -1.0, vt, vddmax/2.0 ) ;
      else
        ic = stm_get_ic_imax (t - tvt, U, imax, F, bn) ;
      if( STM_PLOT_VS_WITH_Q ) {
        ceqrsat=0.0;
        for( j=3 ; j ; j-- ) {
          vs = ( stm_get_q(imax,t-ttp,F,bn,U) - (capai/1000.0) * fabs(ve-vi) ) / ((capa+ceqrsat)/1000.0) ;
          ceqrsat = stm_modscm_dual_ceqrsat( imax, vt, vddmax, fo, vddin, fin, rsat, rlin, an, bn, vs, threshold, te, to ) ;
        }
        vs = (vi>vf) ? vs : vf-vs ;

        
        if( STM_PLOT_VS_END_TANH ) {
          if( vi>vf ) {
            if( vs > threshold ) {
              if( tf < 0.0 ) 
                tf = t - stm_get_t( vs, vt, vf, vi, fout ) ;
              vs = stm_get_v (t - tf, vt, vf, vi, fout);
            }
          }
          else {
            if( vs < threshold ) {
              if( tf < 0.0 )
                tf = t - stm_get_t( vs, vt, vf, vi, fout )  ;
              vs = stm_get_v (t - tf, vt, vf, vi, fout);
            }
          }
        }
        if( STM_PLOT_VS_END_RLIN ) {
          if( vi>vf ) {
            if( vs > thrlin ) {
              if( tf < 0.0 ) {
                printf("rlin vs=%g, t=%g\n", vs, t );
                vsf    = vs ;
                tf     = t ;
                crsatf = ceqrsat ;
              }
              vs = vsf + (vddmax-vsf) * ( 1.0 - exp( -(t - tf)/(rlin*((capa+capai+crsatf)/1000.0)) ) );
            }
          }
          else {
            if( vs < thrlin ) {
              if( tf < 0.0 ) {
                printf("rlin vs=%g, t=%g\n", vs, t );
                vsf    = vs ;
                tf     = t ;
                crsatf = ceqrsat ;
              }
              vs = vsf - vsf * ( 1.0 - exp( -(t - tf)/(rlin*((capa+capai+crsatf)/1000.0)) ) );
            }
          }
        }
      }
      else
        vs = stm_get_v (t - ts, vt, vf, vi, fout);

      iconf = stm_get_iconf (model, t, vi, vf, fin, deltat, (float)(tinput+tp));
      stm_curve_add_time_data(curve, t + t0, ve, vs, ic, iconf);
      t ++;
    }
    
  // Comportement statique

  nbe = 10;
  nbs = 10;

  stm_alloc_static( curve, nbe, nbs );

  for( ne = 0 ; ne < nbe ; ne++ ) {

    ve = stm_mod_vdd(model)*ne/nbe;
    curve->STATIC.VE[ne] = ve ;
    
    for( ns = 0 ; ns < nbs ; ns++ ) {
    
      vs = stm_mod_vdd(model)*ns/nbs;
      curve->STATIC.VS[ns] = vs ;
      
      *( STM_CURVE_STATIC_GET_PTR_I( curve, ne, ns ) ) = stm_curve_calc_is( model, ve, vs); ;
    }
  }


    
    return curve;
}
stm_curve* stm_curve_mcc_pi( timing_model *model, 
                        float fin, 
                        float c1out,
                        float c2out,
                        float rout,
                        float t0, 
                        float tmax 
                      )
{
    double imax      = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_IMAX];
    double threshold = (double)model->UMODEL.SCM->PARAMS.DUAL->DP[STM_THRESHOLD];
    double cconf, load;
    
    cconf = stm_modscm_cconf (model->UMODEL.SCM, fin);
    load = stm_capaeq (imax, rout, c1out + cconf, c2out, threshold, "unknown" );

    return stm_curve_mcc_c( model, fin, load, t0, tmax );

}

