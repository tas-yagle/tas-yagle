/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_eval.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"

double TESTIM = -1.0 ;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
float stm_modscm_dual_vf_input (dualparams *params)
{
    double vddin     = (double)params->DP[STM_VDDIN];
    double vt        = (double)params->DP[STM_VT];
    return (float)(vddin + vt);
}

float stm_modscm_dual_vdd_input (dualparams *params)
{
    return params->DP[STM_VDDMAX];
}

/*******************************************************************************/
/* sert au calcul de la prise en compte des maillons entre activelink et source*/
/*******************************************************************************/

void stm_modscm_dual_slopei(double k, double B, double U, double IMAX, double Fe, double rsat, double vdd, double threshold, double *imax, double *Fs)
{
    double delta, tm, x;
    delta = k * k * B * B * U * U + 4.0 * k * (1.0 + B * U);
    x = (k * B * U + sqrt(delta)) / 2.0 / (1.0 + B * U);
    tm = Fe * atanh(x);
    *Fs = tm / k;
    if( rsat > 0.0 )
      *imax = IMAX-(vdd-threshold)/rsat;
    else
      *imax = IMAX ;
}

/****************************************************************************/

double stm_modscm_dual_voltage_rc( double t, double rbr, double cbr, double imax, double Fs )
{
  double v;

  if( t > 0.0 ) 
    v = rbr * rbr * imax * cbr / Fs * ( 1.0 - exp( -t / (rbr * cbr)) - ( t > Fs ? ( 1.0 - exp(-( t - Fs) / (rbr * cbr))) : 0.0 ) );
  else
    v = 0.0 ;

  return v ;
}

/****************************************************************************/

void stm_modscm_dual_modifslope(double vddin, double vt, double F, double vdd, double rbr, double cbr, double imax, double Fs, double *te, double *Fm)
{
    double Ve;
    
    cbr /= 1000.0;
    
    // *te = F * atanh( ( vdd/2 - vt ) / vddin );
    // Ve = vddin * tanh( *te / F) + vt + rbr * rbr * imax * cbr / Fs * ( 1.0 - exp(-(*te) / (rbr * cbr)) - (*te>Fs?(1.0 - exp(-(*te - Fs) / (rbr * cbr))):0.0));
    
    /* Instant ou la tension d'entrée passe au millieu de son excursion : (vddin-vt)/2+vt */
    *te = F * 0.549 ; 
    Ve = vddin * tanh( *te / F) + vt + stm_modscm_dual_voltage_rc( *te, rbr, cbr, imax, Fs );
    *Fm = *te / atanh( (Ve - vt) / vddin );

    vdd=0.0; /* unused */
}

/****************************************************************************/

double stm_modscm_dual_get_ir( double t, 
                               double A, 
                               double B, 
                               double vin, 
                               double F, 
                               double Ur, 
                               double Fr, 
                               double rbr, 
                               double cbr, 
                               double imax, 
                               double Fs, 
                               double vdd,
                               double rnt, 
                               double rns,
                               double rni,
                               double vint,
                               double vt, 
                               double vout 
                             )
{
    double V, Vr ;
    double b, br ;
    double ir ;
    double i ;

    /* Valeurs rapportées */
    Vr = Ur * tanh( t / Fr );

    /* Valeurs réélles */
    V  = vin * tanh( t / F) ;
    b  = stm_modscm_dual_voltage_rc( t, rbr, cbr, imax, Fs );

    /* Valeur du bruit rapporté proportionnel à Vm / Vr */
    br = b * Vr / V ;

    if( !isfinite( br ) )
      return 0.0 ;
    
    /* Nouvelle tension d'entrée rapportée */
    Vr = Vr + br ;
    ir = A * Vr * Vr / ( 1.0 + B * Vr );

    i = stm_modscm_dual_calc_i( imax, ir, A, B, rnt, rns, rni, vint, vdd, vt, vt+V+b, vout );
    return i ;
}

/****************************************************************************/
double stm_modscm_dual_get_ia( double t, 
                               double A, 
                               double B, 
                               double Ua, 
                               double Fa, 
                               double vin, 
                               double fin, 
                               double imax, 
                               double vdd,
                               double rnt, 
                               double rns,
                               double rni,
                               double vint,
                               double vt, 
                               double vout 
                             )
{
    double V ;
    double ia ;
    double i ;

    V = Ua * tanh(t / Fa); 
    ia =  A * V * V / (1.0 + B * V);
    V = vt + vin * tanh( t/fin );
    i = stm_modscm_dual_calc_i( imax, ia, A, B, rnt, rns, rni, vint, vdd, vt, V, vout );

    return i ;
}

/****************************************************************************/

double stm_modscm_dual_get_k4Q( double A, 
                                double B, 
                                double vin, 
                                double Fin, 
                                double Ur, 
                                double Fr, 
                                double Ua, 
                                double Fa, 
                                double rbr, 
                                double cbr, 
                                double imax, 
                                double Fs, 
                                double vdd,
                                double rns,
                                double rnt,
                                double rni,
                                double vint,
                                double vt
                              )
{
    double k, ia, ir ;
    double vout ;
    
    cbr /= 1000.0;
    k    = 1.0 ;
    if( rni>0.0 && vint > 0.0 )
      vout = vdd/2.0 ;
    else
      vout = vdd ;
    
    do {
        k = k * 1.2 ;
        ir = stm_modscm_dual_get_ir( k*Fs, A, B, vin, Fin, Ur, Fr, rbr, cbr, imax, Fs, vdd, rnt, rns, rni, vint, vt, vout );
        ia = stm_modscm_dual_get_ia( k*Fs, A, B, Ua, Fa, vin, Fin, imax, vdd, rnt, rns, rni, vint, vt, vout );
    }
    while ( fabs(ir-ia)/ir > 0.01 );

    return k;
}

/****************************************************************************/

double stm_modscm_dual_get_Q( double ts, 
                              double k, 
                              double A, 
                              double B, 
                              double vin, 
                              double Fin, 
                              double Ur, 
                              double Fr, 
                              double Ua, 
                              double Fa, 
                              double rbr, 
                              double cbr, 
                              double imax, 
                              double Fs, 
                              double tm,
                              double vdd,
                              double vt,
                              double rns,
                              double rnt,
                              double rni,
                              double vint,
                              double fout,
                              double threshold
                            )
{
    double s, Q;
    double ir, ia ;
    double vout ;
    double d, dt ;

    cbr /= 1000.0;
    vout = vdd ;
    
    if(ts < tm) {
      Q = 0.0;
    }
    else { 
      if((ts >= tm) && (ts < Fs)) {
        vout = vdd/2.0 ;
        ia = stm_modscm_dual_get_ia(ts, A, B, Ua, Fa, vin, Fin, imax, vdd, rnt, rns, rni, vint, vt, vout ) ;
        ir = stm_modscm_dual_get_ir(ts, A, B, vin, Fin, Ur, Fr, rbr, cbr, imax, Fs, vdd, rnt, rns, rni, vint, vt, vout ) ;
        Q = 0.5 * (ts - tm) * ( ir - ia );
      }
      else {
        dt = fout * atanh((threshold-vt)/(vdd-vt)) ;
        d  = ts - dt ;
        vout = vdd - ( vt + (vdd-vt)*tanh((Fs-d)/fout) );
        ia = stm_modscm_dual_get_ia(Fs, A, B, Ua, Fa, vin, Fin, imax, vdd, rnt, rns, rni, vint, vt, vout) ;
        ir = stm_modscm_dual_get_ir(Fs, A, B, vin, Fin, Ur, Fr, rbr, cbr, imax, Fs, vdd, rnt, rns, rni, vint, vt, vout ) ;
        s = (k * Fs / 2.0 - tm / 2.0) * ( ir - ia );
        
        if((ts >= Fs) && (ts < k*Fs)) {
          vout = vdd/2.0 ;
          ir = stm_modscm_dual_get_ir(ts, A, B, vin, Fin, Ur, Fr, rbr, cbr, imax, Fs, vdd, rnt, rns, rni, vint, vt, vout ) ;
          ia = stm_modscm_dual_get_ia(ts, A, B, Ua, Fa, vin, Fin, imax, vdd, rnt, rns, rni, vint, vt, vout) ;
          
          Q = s - 0.5 * (k * Fs - ts) * ( ir - ia );
        }
        else
          Q = s;
      }
    }
    return Q * 1000.0;
}

/****************************************************************************/

float stm_modscm_dual_cconf (dualparams *params, float slew)
{
    return params->DP[STM_CAPAI] + (params->DP[STM_PCONF0] +  params->DP[STM_PCONF1] * slew)
           * params->DP[STM_IRAP];
}

/****************************************************************************/

float stm_modscm_dual_imax (dualparams *params)
{
    return params->DP[STM_IMAX];
}

/****************************************************************************/
    
float stm_modscm_dual_vth (dualparams *params)
{
    return params->DP[STM_THRESHOLD];
}

/****************************************************************************/
void stm_modscm_dual_cal_UF (double imax,
                              double an,
                              double bn,
                              double vddin,
                              double vx,
                              double fin,
                              double *U,
                              double *F)
{

    *U = (imax * bn + sqrt (pow ((imax * bn), 2) + 4.0 * an * imax)) / (2.0 * an);

    *F = fin * *U / vddin;

    *F *= (*U - vx) / *U ;
    *U -= vx ;
} 
/****************************************************************************/
    
double stm_modscm_dual_calculduts (dualparams *params, float slew, float load, stm_pwl *pwl)
{
    double threshold = (double)params->DP[STM_THRESHOLD];
    double ts ;
    ts = stm_modscm_dual_calculduts_threshold( params, slew, load, threshold, pwl );

    return ts ;

}
/****************************************************************************/

char stm_modscm_dual_slew_for_pwl( paramforslewpwl *param, float v, float *t )
{
  *t = stm_modscm_dual_calts_with_param_timing( param->DUALSCM, v );
  return 1;
}

/****************************************************************************/
float stm_modscm_dual_slew (dualparams *params, float slew, stm_pwl *pwl, stm_pwl **ptpwl, float load)
{
  float f ;
  double vt ;
  double vdd ;
  double s1old, s1new, s2old, s2new, seuil1, seuil2, ts1, ts2 ;
  char oldmode = 0;
  float tabpwl[30];
  double vs, ts ;
  int i;

  if( oldmode ) {
    f = stm_modscm_dual_slew_old( params, slew, load );
  }
  else {
    vt = (double)params->DP[STM_VT];
    vdd = (double)params->DP[STM_VDDMAX];

    s1old = vt ;
    s1new = 0.3*vdd ;

    s2old = 0.75 * (vdd-vt) + vt ;
    s2new = 0.8*vdd ;

    if( s1old > s1new ) seuil1 = s1old ; else seuil1 = s1new ;
    if( s2old > s2new ) seuil2 = s2old ; else seuil2 = s2new ;

    ts1 = stm_modscm_dual_calculduts_threshold( params, slew, load, seuil1, pwl );
    ts2 = stm_modscm_dual_calculduts_threshold( params, slew, load, seuil2, pwl );

    f = stm_thr2scm( ts2-ts1, 
                     seuil1/vdd, 
                     seuil2/vdd, 
                     vt, 
                     vdd, 
                     vdd, 
                     STM_UP 
                   );
   
    if( ptpwl ) {
      i=0;

      vs = 0.2*vdd ;
      ts = stm_modscm_dual_calculduts_threshold( params, slew, load, vs, pwl );
      tabpwl[i++] = ts ; tabpwl[i++]=vs ;
     
      vs = 0.5*vdd ;
      ts = stm_modscm_dual_calculduts_threshold( params, slew, load, vs, pwl );
      tabpwl[i++] = ts ; tabpwl[i++]=vs ;
      
      vs = 0.8*vdd ;
      ts = stm_modscm_dual_calculduts_threshold( params, slew, load, vs, pwl );
      tabpwl[i++] = ts ; tabpwl[i++]=vs ;
      
      vs = 0.9*vdd ;
      ts = stm_modscm_dual_calculduts_threshold( params, slew, load, vs, pwl );
      tabpwl[i++] = ts ; tabpwl[i++]=vs ;
      
      vs = 0.95*vdd ;
      ts = stm_modscm_dual_calculduts_threshold( params, slew, load, vs, pwl );
      tabpwl[i++] = ts ; tabpwl[i++]=vs ;
      *ptpwl = stm_pwl_create( tabpwl, 5 );

      /*
      int n, j ;
      static char printed=0;
      n=6;
      j=0;
      for(i=1;i<=n;i++) {
        vs = ((float)i)/((float)(n+1))*vdd ;
        if( vs < vt )
          continue ;
        ts = stm_modscm_dual_calculduts_threshold( params, slew, load, vs, pwl );
        tabpwl[j++] = ts ;
        tabpwl[j++] = vs ;
      }

      if( !printed ) {
        printf( "---> %d points\n", j/2 );
        printed = 1 ;
      }
      *ptpwl = stm_pwl_create( tabpwl, j/2 );
      */
    }
  }

  return f ;
}

float stm_modscm_dual_slew_old (dualparams *params, float slew, float load)
{
    double cconf;
    double capa, capa0;
    double pconf0    = (double)params->DP[STM_PCONF0];
    double pconf1    = (double)params->DP[STM_PCONF1];
    double capai     = (double)params->DP[STM_CAPAI];
    double irap      = (double)params->DP[STM_IRAP];
    double vddin     = (double)params->DP[STM_VDDIN];
    double vt        = (double)params->DP[STM_VT];
    double threshold = (double)params->DP[STM_THRESHOLD];
    double imax      = (double)params->DP[STM_IMAX];
    double an        = (double)params->DP[STM_AN];
    double bn        = (double)params->DP[STM_BN];
    double delayrc   = (double)params->DP[STM_DRC];
    double rbr       = (double)params->DP[STM_RBR];
    double cbr       = (double)params->DP[STM_CBR];
    double rsat      = (double)params->DP[STM_RSAT];
    double rlin      = (double)params->DP[STM_RLIN];
    double vddmax    = (double)params->DP[STM_VDDMAX];
    double fout ;
    double deltat;
    double U, F, Ur, Fr, im, Fs, tm, Fm, k4Q, t0, Q;
    double ts1, ts2, seuil1, seuil2 ;

    if( pconf1*slew < 10.0*load ) 
      pconf1 = 1.2 * pconf1 ;

    /* Note :
       On n'appelle pas deux fois la fonction culcule du ts avec deux seuils 
       différents car on souhaite conserver la même caractéristique.
    */

    if((rbr > 0.0) && (cbr > 0.0)){
        stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, (double)slew, &Ur, &Fr);
        stm_modscm_dual_slopei(0.7, bn, Ur, imax, Fr, rsat, vddmax, vddmax, &im, &Fs);
        stm_modscm_dual_modifslope( vddin, vt, slew, vddmax, rbr, cbr, im, Fs, &tm, &Fm);
    }
    else {
        Fm = (double)slew;
    }
    stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, Fm, &U, &F);
    if((capai > 0.00) && (capai/(load + (pconf0 + pconf1 * slew) * irap) > STM_OVERSHOOT)){
        deltat = stm_modscm_dual_calts (imax, an, bn, U, threshold, 0.0, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
    }else{
        deltat = 0.0;
    }
    deltat += delayrc ;
    if(deltat < slew)
        cconf = pconf0 + pconf1 * (slew - deltat);
    else
        cconf = pconf0;
    capa0 = cconf * irap + (double)load;
    t0 = stm_modscm_dual_calts (imax, an, bn, U, threshold, capa0, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
    capa = capa0 ;
    if((rbr > 0.0) && (cbr > 0.0)){
        fout = stm_modscm_dual_calslew( imax, an, bn, U, vt, vddmax, capa, slew, F, t0);
        k4Q = stm_modscm_dual_get_k4Q( an, bn, vddin, slew, Ur, Fr, U, F, rbr, cbr, im, Fs, vddmax, rsat, rlin, -1.0, -1.0, vt );
        Q = stm_modscm_dual_get_Q( t0, k4Q, an, bn, vddin, slew, Ur, Fr, U, F, rbr, cbr, im, Fs, tm, vddmax, rsat, rlin, -1.0, -1.0, vt, fout, threshold);
        if( 2.0 * Q / vddmax < capa )
          capa -= 2.0 * Q / vddmax;
    }

    /*
    if( STM_DEFAULT_SMINR > 0.0 && STM_DEFAULT_SMAXR < 1.0 ) {
      seuil1 = STM_DEFAULT_SMINR*vddmax ;
      seuil2 = STM_DEFAULT_SMAXR*vddmax ;
    }
    else {
      seuil1 = vt ;
      seuil2 = 0.75 * (vddmax-vt) + vt ;
    }
    */
    
    seuil1 = 0.5*vddmax ;
    seuil2 = 0.8*vddmax ;

    {
      double s1old, s1new, s2old, s2new ;

      s1old = vt ;
      s1new = 0.4*vddmax ;

      s2old = 0.75 * (vddmax-vt) + vt ;
      s2new = 0.8*vddmax ;

      if( s1old > s1new ) seuil1 = s1old ; else seuil1 = s1new ;
      if( s2old > s2new ) seuil2 = s2old ; else seuil2 = s2new ;

    }
    
    if( rsat <= 0.0 ) {
      ts1 = stm_modscm_dual_calts (imax, an, bn, U, seuil1, capa, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
      ts2 = stm_modscm_dual_calts (imax, an, bn, U, seuil2, capa, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
    }
    else {
      ts1 = stm_modscm_dual_calts_rsat_full_range ( capai, imax, an, bn, U, vt, vddmax, threshold, seuil1, rsat, rlin, capa, slew, F, stm_modscm_dual_calte (vddin, vt, vt, (double)slew), t0, vddin, NULL );
      ts2 = stm_modscm_dual_calts_rsat_full_range ( capai, imax, an, bn, U, vt, vddmax, threshold, seuil2, rsat, rlin, capa, slew, F, stm_modscm_dual_calte (vddin, vt, vt, (double)slew), t0, vddin, NULL );
    }

    return stm_thr2scm( ts2-ts1, 
                        seuil1/vddmax, 
                        seuil2/vddmax, 
                        vt, 
                        vddmax, 
                        vddmax, 
                        STM_UP 
                      );
}
/****************************************************************************/

float stm_dv( float t, mbk_pwl *linetanh, float rdriver, float cin, float capai )
{
  float k ;
  int   n, m ;
  float dv ;
  float tmax ;
  float ta ;
  float a ;
  float t0 ;
  static char enable='x' ;

  if( V_BOOL_TAB[ __STM_ENABLE_DV ].VALUE )
    enable = 'y' ;
  else
    enable = 'n' ;
    
  if( enable == 'n' )
    return 0.0 ;

  t0=linetanh->DATA[0].X0;
  if( t < t0 )
    return 0.0 ;

  for( n = 0 ; n < linetanh->N ; n++ ) {
        
    if( n==linetanh->N-1 )
      tmax = linetanh->X1 ;
    else
      tmax = linetanh->DATA[n+1].X0 ;

    if( tmax > t )
      break ;
  }

  if( n >= linetanh->N )
    n = linetanh->N - 1 ;
    
  dv = 0.0 ;
  k = rdriver * ( cin + capai );
       
  for( m = 0 ; m <= n ; m++ ) {
    a  = linetanh->DATA[m].A ;
    ta = linetanh->DATA[m].X0;
    dv = dv + a*(1.0-exp(-(t-ta)/k)) ;
    if( m<n ) {
      ta = linetanh->DATA[m+1].X0 ;
      dv = dv - a*(1.0-exp(-(t-ta)/k)) ;
    }
  }
  dv = dv*rdriver*capai;

  return dv ;
}

double stm_dv_old( double t, double im, double Fs, double r, double cl, double cin, double ci )
{
  double k1 ;
  double c1 ;
  double dv ;
  
  k1 = r*( cl*cin + cl*ci + cin*ci );
  c1 = ci + cl ;

  if( t <= Fs )
    dv = im*r*ci/(Fs*c1)*(t-k1/c1*(1.0-exp(-c1*t/k1))) ;
  else
    dv = im*r*ci/(Fs*c1)*(t-k1/c1*(1.0-exp(-c1*t/k1))) -
         im*r*ci/(Fs*c1)*((t-Fs)-k1/c1*(1.0-exp(-c1*(t-Fs)/k1)));
  
  return dv ;
}

/****************************************************************************/
void stm_estim_output( stm_dual_param_timing *param, double global_t, double v )
{
  double t0 ;
  double tst0 ;
  double local_t ;
  double tslocal_t ;
  double dtlocal ;
  double dt ;

  t0   = stm_modscm_dual_calts( param->IMAX, 
                                param->AN, 
                                param->BN, 
                                param->U, 
                                param->THRESHOLD, 
                                param->CAPA0, 
                                param->F, 
                                param->DTPWL, 
                                param->VT, 
                                param->VDDIN, 
                                param->CAPAI, 
                                NULL,
                                NULL,
                                param->FIN, 
                                NULL, 
                                0, 
                                0,
                                0.0,
                                0.0,
                                NULL 
                              );
                              
  param->FOUT = stm_modscm_dual_calslew( param->IMAX, 
                                         param->AN, 
                                         param->BN, 
                                         param->U, 
                                         param->VT, 
                                         param->VDDMAX, 
                                         param->CAPA, 
                                         param->FIN, 
                                         param->F, 
                                         t0
                                       );

  if( v > 0.0 ) {
    tst0      = stm_modscm_dual_calte( param->VDDMAX-param->VT, param->VT, param->THRESHOLD, param->FOUT ) ;
    tslocal_t = stm_modscm_dual_calte( param->VDDMAX-param->VT, param->VT, v, param->FOUT ) ;  
    dtlocal   = t0-tst0 ;
    local_t   = dtlocal+tslocal_t ;
    dt = global_t - param->DTPWL - local_t ;
    t0 = t0 + dt ;
  }

  param->T0 = t0 + param->DTPWL ;
}

void  stm_modscm_dual_fill_param( dualparams            *params, 
                                  float                  fin,
                                  stm_pwl               *pwlin,
                                  stm_driver            *driver,
                                  float                  load, 
                                  stm_dual_param_timing *paramtiming
                                ) 
{
    double cconf;
    double capa, capa0;
    double pconf0    = (double)params->DP[STM_PCONF0];
    double pconf1    = (double)params->DP[STM_PCONF1];
    double capai     = (double)params->DP[STM_CAPAI];
    double capao     = (double)params->DP[STM_CAPAO];
    double irap      = (double)params->DP[STM_IRAP];
    double vddin     = (double)params->DP[STM_VDDIN];
    double vt        = (double)params->DP[STM_VT];
    double threshold = (double)params->DP[STM_THRESHOLD];
    double input_thr = (double)params->DP[STM_INPUT_THR];
    double imax      = (double)params->DP[STM_IMAX];
    double an        = (double)params->DP[STM_AN];
    double bn        = (double)params->DP[STM_BN];
    double delayrc   = (double)params->DP[STM_DRC];
    double rbr       = (double)params->DP[STM_RBR];
    double cbr       = (double)params->DP[STM_CBR];
    double rsat      = (double)params->DP[STM_RSAT];
    double rlin      = (double)params->DP[STM_RLIN];
    double vddmax    = (double)params->DP[STM_VDDMAX];
    double rint      = (double)params->DP[STM_RINT];
    double vint      = (double)params->DP[STM_VINT];
    double chalf     = (double)params->DP[STM_CHALF];
    double rconf     = (double)params->DP[STM_RCONF];
    double kf        = (double)params->DP[STM_KF];
    double qinit0    = (double)params->DP[STM_QINIT];
    double vt0       = (double)params->DP[STM_VT0];
    double vt0c      = (double)params->DP[STM_VT0C];
    double capaie[4] ;
    double vcap[3] ;
    char   integnumeric ;
    double deltat=0.0 ;
    double U, F, Ur, Fr, im, Fs, tm, Fm, k4Q, t0, Q, te, ts, koshoot;
    double fout ;
    double vsat ;
    double rsatsep ;
    stm_pwl *pwlshrk ;
    char newswitchmodel ;
    double qinit ;
    int n;
    int nbdv;
    stm_pwl *pwldv ;
    float t, v ;
    stmtanhdata tanhdata;
    mbk_pwl *linetanh;
    float lv, dv, tx, tmin, tminin, tmax, tmaxin, tmaxout, dt, tminctk ;
    static int fordebug = 0 ;
    mbk_pwl_param *param ;
    int ctkmodel ;
    float lt, ldv, s ;
    int localpwlin ;
    float rdriver ;
    float cin ;
    static char flag='n';
    double icf0 ;
    double strans ;
    int usecapaie ;

    fordebug++;

    capaie[0] = params->DP[STM_CAPAI0] ;
    capaie[1] = params->DP[STM_CAPAI1] ;
    capaie[2] = params->DP[STM_CAPAI2] ;
    capaie[3] = params->DP[STM_CAPAI3] ;
    if( capaie[0] == capaie[1] &&
        capaie[0] == capaie[2] &&
        capaie[3] == capaie[3]    )
      usecapaie = 0 ;
    else
      usecapaie = 1 ;
      

    vcap[0] = vt0 ;
    vcap[1] = vddmax/2.0 ;
    vcap[2] = vt0c ;
    
    qsort( vcap, 3, sizeof(double), (int(*)(const void*,const void*))mbk_qsort_dbl_cmp );

    ctkmodel = V_INT_TAB[ __STM_CTK_MODEL ].VALUE ;

    newswitchmodel = 0 ;
    if( vint > 0.0 && rint > 0.0 )
      newswitchmodel = 1 ;

    integnumeric = 0 ;

    if( newswitchmodel && ( vint > 0.0 && rint > 0.0 ) )
      integnumeric = 1 ;

    if( V_BOOL_TAB[ __STM_NUMERICAL_INTEGRATION ].VALUE ) {
      if( flag=='n' ) {
        printf( "numerical integration forced !\n" );
        flag='y';
      }
      integnumeric = 1 ;
    }

    qinit  = qinit0 ;
    icf0   = 0.0 ;
    strans = 0.0 ;
    if( rconf > 0.0 && kf > 0.0 && fin > 1.0 ) {
      tmax = kf*fin ;
      icf0 = capaie[0]*1e-15/(fin*1e-12)*(vddmax-vt)*(1.0-exp(-tmax*1e-12/(rconf*(capaie[0]+load)*1e-15))) ;
      if( V_BOOL_TAB[ __STM_NEW_OVERSHOOT ].VALUE ) {
        double r,k ;
        r = (vddmax-2*vt)/vddmax ;
        if( r>0.3 )
          k=V_FLOAT_TAB[__STM_OVERSHOOT_K_LVT].VALUE ;
        else
          k=V_FLOAT_TAB[__STM_OVERSHOOT_K_HVT].VALUE ;
        qinit = -k * (capai*1e-3)/fin*(vddmax-vt)*(tmax+rconf*(load*1e-3)*(exp(-tmax/(rconf*load*1e-3))-1.0));
      }
      else {
        Q = -(pconf0+load)/1000.0*((-icf0)*rconf)-capaie[0]/1000.0*vt-qinit0 ;
        qinit = qinit+Q ;
      }
    }

    /*     fin :  front d'entrée réel original.
           Fr :   front d'entrée rapporté sans prendre en compte le cbr.
           Ur :   tension d'entrée rapporté sans prendre en compte le cbr.
           Fs :   instant où on considère que le courant atteint imax.
           Fm :   front d'entrée réel prenant en compte le cbr.
           F :    front d'entrée rapporté prenant en compte cbr.
           U :    tension d'entrée rapporté prenant en compte cbr.
           t0 :   estimation ts sortie en vdd/2
           fout : estimation f sortie
           
    */
   
    capa = load+pconf0+pconf1*fin;
  
    if( !integnumeric && (rbr > 0.0) && (cbr > 0.0) ) {
      stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, (double)fin, &Ur, &Fr);
      stm_modscm_dual_slopei(0.7, bn, Ur, imax, Fr, rsat, vddmax, vddmax, &im, &Fs);
      stm_modscm_dual_modifslope( vddin, vt, fin, vddmax, rbr, cbr, im, Fs, &tm, &Fm);
    }
    else
      Fm = (double)fin ;

    stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, Fm, &U, &F);
    t0 = stm_modscm_dual_calts (imax, an, bn, U, threshold, capa, F, 0.0, vt, vddin, capai, NULL, NULL, fin, NULL, 0, 0.0, 0.0, 0.0, NULL );
    fout = stm_modscm_dual_calslew( imax, an, bn, U, vt, vddmax, capa, fin, F, t0);
  
    /* Prise en compte de la modification du front d'entree due au capai.
       Pour l'instant, nous l'appliquons que les technos pour lesquelles le
       vt est superieur à vdd/2 */
    paramtiming->NEWCTKMODEL = 0 ;
    paramtiming->EVTIN       = '-' ;
    paramtiming->VIN         = -1.0 ;

    if( driver ) { 
      if( ! V_BOOL_TAB[ __AVT_CTK_ON_INPUT ].VALUE || integnumeric==1 )
        driver = NULL ; 
    }

    if( driver ) {
      if( driver->c <= 0.0 ||
          driver->r <= 0.0    )
        driver = NULL ;
    }

    if( driver ) { 
          
      if( V_BOOL_TAB[ __AVT_NEW_CTK_ON_INPUT ].VALUE && V_BOOL_TAB[ __AVT_CTK_ON_INPUT ].VALUE ) {
        paramtiming->DRIVER.c = driver->c - capai ;
        if( V_BOOL_TAB[ __STM_RDRIVER_FROM_SLOPE ].VALUE ) {
          if( V_BOOL_TAB[ __AVT_BUG_RDRIVER ].VALUE ) 
            paramtiming->DRIVER.r = 0.693*fin/(driver->c/1000.0)/4.0 ;
          else
            paramtiming->DRIVER.r = 0.693*fin/(driver->c/1000.0) ;
        }
        else
          paramtiming->DRIVER.r = driver->r ;
        paramtiming->DRIVER.v = driver->v ;
        paramtiming->NEWCTKMODEL = 1 ;
      }
        
      if( ! V_BOOL_TAB[ __AVT_CTK_ON_INPUT ].VALUE   || 
          integnumeric==1                            ||
          V_BOOL_TAB[ __AVT_NEW_CTK_ON_INPUT ].VALUE 
        )
        driver = NULL ; 
    }

    if( driver ) {
      /* retire la capa de couplage comptée dans la capa d'entrée */
      cin = driver->c - capao ;
    }
    else {
      cin = -1.0 ;
    }
   
    localpwlin = 0 ;

    if( driver ) {
      if( V_BOOL_TAB[ __STM_RDRIVER_FROM_SLOPE ].VALUE ) {
        /* resistance du driver du front d'entrée : modèle integrales à 
           l'infinie égales */
        if( V_BOOL_TAB[ __AVT_BUG_RDRIVER ].VALUE )
          rdriver = 0.693*fin/(driver->c/1000.0)/4.0 ;
        else
          rdriver = 0.693*fin/(driver->c/1000.0) ;
      }
      else
        rdriver = driver->r ;
    }
    else
      rdriver = -1.0 ;

    /* calcul de l'écart entre le référentiel de l'entrée et de la sortie */
    if( threshold > vt )
      ts = fout*atanh( (threshold-vt)/(vddmax-vt));
    else
      ts = fout*(threshold-vt)/(vddmax-vt);
    dt = t0-ts ;

    if( !integnumeric && ( (driver && driver->r > 0.0 && driver->c > 0.0 && cin > 0.0 ) || usecapaie ) ) {
      /* Calcule du dV de tensions sur l'entrée à partir de l'estimation du front de sortie : 
         Comme on ne connait pas la transformée de laplace de la tanh, on passe en pwl */
      tanhdata.F   = fout ;
      tanhdata.VDD = vddmax ;
      tanhdata.VT  = vt ;
      tanhdata.T0  = dt ;
      
      /* calcul du dernier instant : quand les transitions atteignent 95% de leur valeurs finales */
      tmaxout = dt + fout*atanh( (0.95*vddmax-vt)/(vddmax-vt) );
      tmaxin  = fin*atanh( (0.95*(vddin+vt)-vt)/vddin ) ;
      if( tmaxout > tmaxin )
        tmax = 2.0*tmaxout ;
      else
        tmax = 2.0*tmaxin ;
        
      /* calcul du premier instant */
      tx = -fout*vt/(vddmax-vt);
      if( tx+dt > 0.0 )
        tmin = tx+dt ;
      else
        tmin = 0.0 ;

      tminctk = tmin ;
      if( V_BOOL_TAB[ __AVT_RDRIVER_UNDER_VSAT ].VALUE && driver->v > 0.0 ) {
        vsat = vddmax-driver->v ;
        if( vsat > vt ) {
          tminin = fin*atanh( ( vsat - vt )/vddin );
          if( tminin > tmin )
            tminctk = tminin ;
        }
      }
     
      if( usecapaie )
        tmin = 0.0 ;

      param = mbk_pwl_get_default_param( (char (*)(void*, double, double*))stmtanhfn, (void*) &tanhdata, tmin, tmax ) ;
      /* calcul des pwl : on impose le point vt */
      mbk_pwl_add_param_point( param, dt );
      
      linetanh = mbk_pwl_create( (char (*)(void*, double, double*))stmtanhfn, (void*) &tanhdata, tmin, tmax, param ) ;
      mbk_pwl_free_param( param );
      
      if( linetanh ) {
       
        switch( ctkmodel ) {
        
        case 1 : /* méthode du pwl equivalent */
        case 2 : /* méthode du pwl equivalent, limite haute */
        case 3 : /* méthode du pwl equivalent, limite basse */

          /* Determine le stm_pwl de l'entrée en retranchant les dv */
          pwldv      = (stm_pwl*)mbkalloc( sizeof(stm_pwl));
          pwldv->TAB = (float*)mbkalloc(sizeof(float)*2*V_INT_TAB[ __STM_INPUT_NB_PWL ].VALUE);
      
          nbdv = 0;
          lv   = 0.0 ;
          for( n=0 ; n<V_INT_TAB[ __STM_INPUT_NB_PWL ].VALUE ; n++ ) {
      
            t  = ((float)n)/((float)(V_INT_TAB[ __STM_INPUT_NB_PWL ].VALUE-1))*tmax;
            if( rdriver>0.0 && t>=tminctk )
              dv = stm_dv( t, linetanh, rdriver, cin/1000.0, capao/1000.0 );
            else
              dv = 0.0 ;
            v  = vt+vddin*tanh(t/fin) - dv;
            switch( ctkmodel ) {
            case 1 :
            case 3 :
              pwldv->TAB[2*nbdv+0] = t ;
              pwldv->TAB[2*nbdv+1] = v ;
              nbdv++;
              break ;
            case 2 :
              if( v > lv ) {
                pwldv->TAB[2*nbdv+0] = t ;
                pwldv->TAB[2*nbdv+1] = v ;
                nbdv++;
                lv = v ;
              }
              break ;
            }
          }
          pwldv->N = nbdv ;

          /* ecrasement du pwlin. a faire : calculer la forme de la tension
             d'entrée en prenant en compte le dv */

          pwlin = pwldv ;
          localpwlin = 1 ;
          break ;
        case 4 : /* méthode du front equivalent */
          lt = 0.0 ;
          ldv = 0.0 ;
          s  = 0.0 ;
            
          for( n=0 ; n<V_INT_TAB[ __STM_INPUT_NB_PWL ].VALUE ; n++ ) {
            t  = ((float)n)/((float)(V_INT_TAB[ __STM_INPUT_NB_PWL ].VALUE-1))*tmax;
            if( rdriver>0.0 ) 
              dv = stm_dv( t, linetanh, rdriver, cin/1000.0, capao/1000.0 );
            else
              dv = 0.0 ;
            s  = s + (t-lt)*(dv+ldv)/2.0 ;
            lt = t ;
            ldv = dv ;
          }
          fin = fin + s/(vddin*0.693);
          break ;
        }

        mbk_pwl_free_pwl(linetanh);
      }
    }

    /* transforme le pwl d'entrée proportionnellement au passage de 
       vddin, fin -> U, F. */
    paramtiming->NTHSHRK = 0;
    if( pwlin ) {
      pwlshrk = stm_shrink_pwl( pwlin, vddin, fin, U, F, vt );
      if( !stm_pwl_to_tanh( pwlshrk, vt, U+vt, paramtiming->PWTHSHRK, &paramtiming->NTHSHRK, F ) ) 
        paramtiming->NTHSHRK = 0;
      stm_pwl_destroy( pwlshrk );
     
      if( localpwlin ) {
        stm_pwl_destroy( pwlin );
        pwlin=NULL;
      }
    }

    if( V_BOOL_TAB[ __STM_QINIT_FOR_DT_CONF ].VALUE && icf0>0.0 ) {
      /* calcule l'instant où la sortie vaut 0, après absorbtion de l'overshoot */
      strans = stm_modscm_dual_calts (imax, an, bn, U, 0.0, 0.0, F, 0.0, vt, vddin, capaie[0], NULL, NULL, fin, NULL, 0, qinit, 0.0, 0.0, NULL );
      /* vérifie si cet instant est inférieur à l'instant où la branche conflictuelle se coupe */
      te = stm_modscm_dual_calte (vddin, vt, vt0c, (double)fin);
      if( te < strans )
        strans = te ;
      deltat = stm_modscm_dual_calts_final( imax, an, bn, U, 0.0, 0.0, F, vt, vddin, capaie[0], fin, 0.0, qinit, strans, icf0, NULL );
    }
    else
    {
      if(V_BOOL_TAB[__STM_QINIT_FOR_OVERSHOOT_CONF].VALUE)
        deltat = stm_modscm_dual_calts (imax, an, bn, U, threshold, 0.0, F, 0.0, vt, vddin, capai, NULL, NULL, fin, NULL, 0, qinit, 0.0, 0.0, NULL );
      else
        deltat = stm_modscm_dual_calts (imax, an, bn, U, threshold, 0.0, F, 0.0, vt, vddin, capai, NULL, NULL, fin, NULL, 0, 0, 0.0, 0.0, NULL );
    }

    koshoot = 0.0 ;
    if( (capai > 0.00) && (capai/(load + (pconf0 + pconf1 * fin) * irap) > STM_OVERSHOOT) ) {
        te = stm_modscm_dual_calte (vddin, vt, input_thr, (double)fin);
        koshoot = ( deltat/te ) * (deltat/te );
    }else{
        deltat = 0.0;
    }
    deltat += delayrc ;

    if( newswitchmodel ) {
      if( koshoot < 1.0 )
          cconf = pconf0 + irap * pconf1 * fin *( 1.0 - koshoot ) ;
      else
          cconf = pconf0;
      capa0 = cconf ;
    }
    else {
      if( koshoot < 1.0 )
        cconf = pconf0 + pconf1 * fin *( 1.0 - koshoot );
     else
        cconf = pconf0;
      capa0 = cconf * irap ;
    }

    if( capa0 + load < 0.0 )
      capa0 = 0.0 ;

    capa = capa0 ;

    t0 = -1.0 ;
    if( integnumeric == 0 )
      t0   = stm_modscm_dual_calts (imax, an, bn, U, threshold, capa0+load, F, 0.0, vt, vddin, capai, NULL, NULL, fin, NULL, 0, 0.0, 0.0, 0.0, NULL );

    if( integnumeric == 0 && (rbr > 0.0) && (cbr > 0.0)){
        // Calcule de la charge supplémentaire due à l'overshoot
        fout = stm_modscm_dual_calslew( imax, an, bn, U, vt, vddmax, capa+load, fin, F, t0);
        k4Q  = stm_modscm_dual_get_k4Q( an, bn, vddin, fin, Ur, Fr, U, F, rbr, cbr, im, Fs, vddmax, rsat, rlin, rint, vint, vt );
        Q    = stm_modscm_dual_get_Q( t0, k4Q, an, bn, vddin, fin, Ur, Fr, U, F, rbr, cbr, im, Fs, tm, vddmax, vt, rsat, rlin, rint, vint, fout, threshold );
        if( 2.0 * Q / vddmax < (capa+load) )
            capa -= 2.0 * Q / vddmax;
    }

    vsat = vddmax ;
    fout = 0.0 ;

    if( integnumeric==0 && rsat > 0.0 ) {
      rsatsep = vddmax/imax ;
      if( rsat > rsatsep && rsatsep > rlin ) {
        vsat  = vddmax - ( imax - vddmax / rsat ) * rlin * rsat / ( rsat - rlin ) ;
        if( vsat < 0.0 || vsat > vddmax ) 
          vsat = vddmax ;
        else {
          fout = stm_modscm_dual_calslew( imax, an, bn, U, vt, vddmax, capa+load, fin, F, t0);
        }
      }
    }


    paramtiming->IMAX       = imax ;
    paramtiming->AN         = an ;
    paramtiming->BN         = bn ;
    paramtiming->U          = U ;
    paramtiming->CAPA       = capa ;
    paramtiming->CAPA0      = capa0 ;
    paramtiming->CHALF      = chalf ;
    paramtiming->F          = F ;
    paramtiming->VT         = vt ;
    paramtiming->VDDIN      = vddin ;
    paramtiming->VDDMAX     = vddmax ;
    paramtiming->CAPAI      = capai ;
    paramtiming->FIN        = fin ;
    paramtiming->VSAT       = vsat ;
    paramtiming->VINT       = vint ;
    paramtiming->RLIN       = rlin ;
    paramtiming->RINT       = rint ;
    paramtiming->RSAT       = rsat ;
    paramtiming->FOUT       = fout ;
    paramtiming->T0         = t0 ;
    paramtiming->TE         = stm_modscm_dual_calte (vddin, vt, vt, fin) ;
    paramtiming->THRESHOLD  = threshold ;
    paramtiming->RBR        = rbr ;
    paramtiming->CBR        = cbr ;
    paramtiming->QINIT      = qinit ;
    paramtiming->INTEGNUMERIC = integnumeric ;
    paramtiming->DT         = deltat ;
    paramtiming->QSAT_VE    = -1.0 ;
    paramtiming->QSAT_TE    = -1.0 ;
    paramtiming->QSAT_Q0    = -1.0 ;
    paramtiming->DTPWL      = 0.0 ;
    for( n=0 ; n<4 ; n++ )
      paramtiming->CAPAIE[n] = capaie[n] ;
    for( n=0 ; n<3 ; n++ )
      paramtiming->VCAP[n]   = vcap[n] ;
 
    if(    ! V_BOOL_TAB[ __STM_QSAT_ENHANCED ].VALUE
        &&   V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE == 0  )
      paramtiming->COMPAT30P3 = 1 ;
    else
      paramtiming->COMPAT30P3 = 0 ;
    paramtiming->STRANS = strans ;
    paramtiming->ICF0   = icf0 ;
}

double stm_modscm_dual_get_qsat2( stm_dual_param_timing *param, double seuil, double ceqrsat )
{
  double qsat2 ;
  double tout ;
  double tst ;
  double dt ;

  if( param->THRESHOLD > param->VT )
    tst = param->FOUT * atanh( (param->THRESHOLD-param->VT)/(param->VDDMAX-param->VT) ) ;
  else 
    tst = param->FOUT * (param->THRESHOLD-param->VT)/(param->VDDMAX-param->VT) ;
  
  dt  = (param->T0-param->DTPWL)-tst;   // Ecart entre le référentiel de l'entrée et de la sortie
  
  if( seuil > param->VT )
    tout = param->FOUT * atanh( (seuil-param->VT)/(param->VDDMAX-param->VT) ) ;
  else 
    tout = param->FOUT * (seuil-param->VT)/(param->VDDMAX-param->VT) ;
 
  tout = tout + dt ;

  switch( V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE ) {
  case 1 :
    qsat2 = ceqrsat*seuil/(tout*tout) ;
    break ;
  case 2 :
    qsat2 = ceqrsat*seuil/((tout-param->DT)*(tout-param->DT)) ;
    break ;
  case 3 :
    qsat2 = ceqrsat*seuil/tout ;
    break ;
  default : 
    qsat2 = 0.0 ;
  }

  return qsat2 ;
}

void evalnewslope( double vt, 
                   double vddin, 
                   double lt, 
                   double lvin, 
                   double t, 
                   double f, 
                   double dvin, 
                   double *dtnew, 
                   double *fnew,
                   double *vin
                 )
{
  double vin0 ;
  double vinmin ;
  double t1 ;
  double t2 ;
  double v1 ;
  double v2 ;
  double k1 ;
  double k2 ;
  double fm ;
  double dtm ;
  
  vin0  = vt + vddin*tanh(t/f) ;

  /* compute minimal slope */
  //  vinmin = lvin + (vin0-lvin)/100.0 ;
  fm     = f*100.0 ;
  dtm    = lt-fm*atanh((lvin-vt)/vddin);
  vinmin = vt+vddin*tanh((t-dtm)/fm);

  *vin = vin0 - dvin ;
  if( *vin < vinmin ) {
    *vin = vinmin ;
  }

  v1 = lvin ;
  t1 = lt ;
  v2 = *vin ;
  t2 = t ;
  k1 = atanh((v1-vt)/vddin);
  k2 = atanh((v2-vt)/vddin);
  *dtnew = (t2*k1-t1*k2)/(k1-k2);
  *fnew  = (t1-t2)/(k1-k2);
}

/* résolution de q(t)=c.vs+ci.vi+qinit */
double stm_modscm_dual_calts_newctk( stm_dual_param_timing *param, 
                                     double threshold, 
                                     double capasup,
                                     newctktrace *debug
                                   )
{
  /* general */
  double        capa ;
  int           onemoretime ;
  int           findin ;
  int           findout ;
  int           ctk ;

  /* debug related variables */
  static int    fordebug=0 ;
  int           iter;
  char*         plotname=NULL ;
  FILE          *file ;
  double        dvpure=0.0 ;

  /* input crosstalk parameter */
  double        dvin ;
  double        dvinr ;
  /* output slope parameter */
  double        vout ;
  double        lvout ;
  mbk_pwl      *pwlout ;
  int           n ;
  double        tout ;
  /* interval parameter */
  double        t ;
  double        lt ;
  double        dq ;
  double        kin ;
  double        kout ;
  int           nin ;
  int           nout ;
  double        step ;
  double        qt ;
  double        qlt ;
  double        qorg ;
  double        ltfinal ;
  double        dqfinal ;
  /* input slope parameter */
  double        f ;
  double        dt ;
  double        fr ;
  double        dtr ;
  double        fnew ;
  double        dtnew ;
  double        frnew ;
  double        dtrnew ;
  double        ffinal ;
  double        dtfinal ;
  double        frfinal ;
  double        dtrfinal ;
  double        vin ;
  double        vinr ;
  double        lvin ;
  double        lvinr ;
  double        tin ;
  
  fordebug++ ;

  if( plotname ) {
    file    = mbkfopen( plotname, "inout.dat", "w" );
    printf( "r  = %g\n", param->DRIVER.r );
    printf( "ci = %g\n", param->DRIVER.c );
    printf( "cc = %g\n", param->CAPAI );
  }
  else
    file    = NULL ;
  
  /* general */
  capa  = param->CAPA + capasup ;

  /* interval parameter */
  n    = V_INT_TAB[ __STM_INPUT_NB_PWL ].VALUE ;
  nin  = 1 ;
  nout = 1 ;
  dq   = 0.0 ;

  /* output slope parameter */
  lvout     = (-1.0/capa)*(param->CAPAI*param->VT+param->QINIT);
  lt        = 0.0 ;
  pwlout    = mbk_pwl_alloc_pwl( 2*((int)(n+0.5)) );
  pwlout->N = 0 ;

  /* input slope parameter */
  f          = param->FIN ;
  dt         = 0.0 ;
  fr         = param->F ;
  dtr        = 0.0 ;
  lvin       = param->VT ;
  lvinr      = param->VT ;
  param->TIN = -1.0 ;

  findin      = 0 ;
  findout     = 0 ;
  onemoretime = 1 ;
 
  if( threshold <= lvin ) {
    param->TIN = f*(threshold-param->VT)/param->VDDIN ;
    findin = 1 ;
  }

  ctk = 1 ;
  iter = 0 ;
  
  do
  {
    iter++ ;

    kin  = ((double)nin)  / ((double)n) ;
    kout = ((double)nout) / ((double)n) ;
    
    /* working on interval [lt-t] */

    if( nin == n && nout == n ) {
      printf( "oups (fordebug=%d, threshold=%g)!\n", fordebug, threshold );
      exit(1);
    }

    /* compute t */
    if( nout < n ) {
      vout  = kout*param->VDDMAX ;
      qorg = stm_get_q( param->IMAX, lt  - dtr, fr, param->BN, param->U ) ;
      tout = stm_modscm_dual_calts_final( param->IMAX,
                                          param->AN, 
                                          param->BN, 
                                          param->U, 
                                          vout,
                                          capa, 
                                          fr,
                                          param->VT, 
                                          param->VDDIN, 
                                          param->CAPAI, 
                                          param->FIN, 
                                          dtr,
                                          param->QINIT + dq + qorg ,
                                          0.0,
                                          0.0,
                                          NULL
                                        )+dtr ;
    }
    else 
      tout = FLT_MAX ;

    if( nin < n ) {
      vin = param->VT+kin*param->VDDIN;
      tin = f*atanh((vin-param->VT)/param->VDDIN)+dt;
    }
    else
      tin = FLT_MAX ;

    if( tin < tout ) {
      t = tin ;
      qt   = stm_get_q( param->IMAX, t  - dtr, fr, param->BN, param->U ) ;
      qorg = stm_get_q( param->IMAX, lt  - dtr, fr, param->BN, param->U ) ;
      vout = 1.0/(capa/1000.0) * ( qt - qorg
                                   - (param->CAPAI/1000.0) * (param->VT+param->VDDIN*tanh(t/param->FIN))
                                   - param->QINIT 
                                   - dq
                                 );
      step=1 ;
    }
    else {
      t = tout ;
      step = 2 ;
    }
                             
    if( nin < n && nout < n ) {
    
      /* compute the input voltage variation caused by output voltage variation */
      pwlout->DATA[ pwlout->N ].X0 = lt ;
      pwlout->DATA[ pwlout->N ].A  = (vout-lvout)/(t-lt) ;
      pwlout->DATA[ pwlout->N ].B  = vout - pwlout->DATA[pwlout->N].A * t ;
      pwlout->X1 = t ;
      (pwlout->N)++ ;
      
      dvin = stm_dv( t, pwlout, param->DRIVER.r, param->DRIVER.c/1000.0, param->CAPAI/1000.0 ) ;
      dvinr = dvin*param->U/param->VDDIN ;

      /* compute the new f and dt for input */
      evalnewslope( param->VT, param->VDDIN, lt, lvin, t, f, dvin, &dtnew, &fnew, &vin );
      

      /* compute the new fr and dtr for input */
      evalnewslope( param->VT, param->U, lt, lvinr, t, fr, dvinr, &dtrnew, &frnew, &vinr );

      /* compute charges at lt and t with frnew/dtrnew */
      qt  = stm_get_q( param->IMAX, t  - dtrnew, frnew, param->BN, param->U ) ;
      qlt = stm_get_q( param->IMAX, lt - dtrnew, frnew, param->BN, param->U ) ;
    
      /* compute vout and the (dis)charge for this interval */
      vout = 1.0/(capa/1000.0) * ( qt 
                                   - qlt 
                                   - (param->CAPAI/1000.0) * (param->VT+param->VDDIN*tanh(t/param->FIN))
                                   - param->QINIT 
                                   - dq 
                                 );
    }
    else {
      fnew = f ;
      dtnew = dt ;
      frnew = fr ;
      dtrnew = dtr ;
      vout = FLT_MAX ;
      vin = FLT_MAX ;
      dvin = 0.0 ;
      dvinr = 0.0 ;
    }

    if( vin > threshold && !findin ) {
      param->TIN = dtnew + fnew*atanh((threshold-param->VT)/param->VDDIN);
      findin = 1 ;
    }

    if( vout > threshold && !findout ) {
      ffinal   = fnew ;
      dtfinal  = dtnew ;
      frfinal  = frnew ;
      dtrfinal = dtrnew ;
      dqfinal  = dq ;
      ltfinal  = lt ;
      findout  = 1 ;
    }
    
    if( nin < n && nout < n ) {
      pwlout->DATA[ pwlout->N-1 ].A  = (vout-lvout)/(t-lt) ;
      pwlout->DATA[ pwlout->N-1 ].B  = vout - pwlout->DATA[pwlout->N-1].A * t ;
    }

    /* prepare the next iteration : continue with FIN/F */
    if( nin < n ) {
      f   = param->FIN ;
      dt  = t - f*atanh((vin-param->VT)/param->VDDIN) ;
      fr  = param->F ;
      //dtr = t - fr*atanh((vin*param->U/param->VDDIN-param->VT)/param->U) ;
      dtr = t - fr*atanh((vinr-param->VT)/param->U) ;
    }
    
    if( step == 1 ) 
      nin++ ;
    else
      nout++ ;

    if( file ) {
      double vin0 ;
      vin0 = param->VT + param->U*tanh(t/param->FIN);
      dvpure = dvpure + param->CAPAI/(param->CAPAI+param->DRIVER.c)*(vout-lvout);
      fprintf( file, "%g %g %g %g %g %g\n", t, vin0, vin, vout, dvin, dvpure );
    }

    if( debug ) {
      if( param->EVTIN == 'U' ) {
        fprintf( debug->FIN,  "%g %g\n", (t+debug->DT)*1e-12, vin );
        fprintf( debug->FOUT, "%g %g\n", (t+debug->DT)*1e-12, param->VDDMAX - vout );
      }
      else {
        fprintf( debug->FIN,  "%g %g\n", (t+debug->DT)*1e-12, param->VDDMAX - vin );
        fprintf( debug->FOUT, "%g %g\n", (t+debug->DT)*1e-12, vout );
      }
    }
    
    dq = dq + qlt - qt ;
    lt    = t ;
    lvout = vout ;
    lvin  = vin ;
    lvinr = vinr ;

    if( findin && findout )
      onemoretime = 0 ;
  }
  while( onemoretime ) ;

  /* qorg = qlt ? */
  qorg = stm_get_q( param->IMAX, ltfinal - dtrfinal, frfinal, param->BN, param->U ) ;
  t = stm_modscm_dual_calts_final( param->IMAX,
                                   param->AN, 
                                   param->BN, 
                                   param->U, 
                                   threshold,
                                   capa, 
                                   frfinal,
                                   param->VT, 
                                   param->VDDIN, 
                                   param->CAPAI, 
                                   param->FIN, 
                                   dtrfinal,
                                   param->QINIT + dqfinal + qorg,
                                   0.0,
                                   0.0,
                                   NULL
                                 )+dtrfinal ;
  param->VIN = param->VT + param->VDDIN*tanh((t-dtfinal)/ffinal );

  if( !findin ) {
    param->TIN = dtfinal + ffinal*atanh((threshold-param->VT)/param->VDDIN);
  }

  mbk_pwl_free_pwl( pwlout );

  if( !isfinite( t ) || !isfinite( param->TIN ) ) {
    printf( "delay calculation error !!! (fordebug=%d,iter=%d)\n", fordebug,iter );
    exit(1);
  }
  
  if( file )
    fclose( file );

  return t ;
}

double stm_modscm_dual_calts_with_param_timing_classic( stm_dual_param_timing *paramtiming, double threshold, stm_qsat_v *qsatv )
{
  double   tf ;
  double   tsat ;
  double   ceqrsat=0.0 ;
  double   ceqrsat0=0.0 ;
  stm_qsat qsat ;
  double   localthreshold ;
  char     needceqrsat ;

  if( threshold >= paramtiming->VSAT ) {
    localthreshold = paramtiming->VSAT ;
    needceqrsat = 1 ;
  }
  else {
    localthreshold = threshold ;
    needceqrsat = 0 ;
  }
  
  /* pour assurer la compatibilité avec ancien modèle, on maintient le bug
     où l'on calcul ceqrsat à threshold, sans le borner par vsat */
  if( paramtiming->COMPAT30P3 /* && ! paramtiming->NEWCTKMODEL */ )
    stm_modscm_dual_calts_qsat( paramtiming, threshold, NULL, &qsat, &ceqrsat, needceqrsat ? &ceqrsat0 : NULL );
  else 
    stm_modscm_dual_calts_qsat( paramtiming, localthreshold, qsatv, &qsat, &ceqrsat, needceqrsat ? &ceqrsat0 : NULL );
    

  if( paramtiming->NEWCTKMODEL ) 
    tf = stm_modscm_dual_calts_newctk( paramtiming, localthreshold, ceqrsat, NULL );
  else {
    TESTIM = paramtiming->T0 ;
    tf = stm_modscm_dual_calts( paramtiming->IMAX,
                                paramtiming->AN, 
                                paramtiming->BN, 
                                paramtiming->U, 
                                localthreshold,
                                paramtiming->CAPA+ceqrsat, 
                                paramtiming->F,
                                paramtiming->DTPWL,
                                paramtiming->VT, 
                                paramtiming->VDDIN, 
                                paramtiming->CAPAI, 
                                paramtiming->CAPAIE, 
                                paramtiming->VCAP,
                                paramtiming->FIN, 
                                paramtiming->COMPAT30P3 ? paramtiming->PWTHSHRK : NULL,
                                paramtiming->COMPAT30P3 ? paramtiming->NTHSHRK  : 0,
                                paramtiming->QINIT,
                                paramtiming->STRANS,
                                paramtiming->ICF0,
                                &qsat
                              );
  }

  if( threshold > paramtiming->VSAT ) {
    tsat = tf ;
    /*
    tf = tsat - paramtiming->RLIN * paramtiming->CAPA/1000.0 * 
                log( 1.0 - ( threshold - paramtiming->VSAT ) / ( paramtiming->VDDMAX - paramtiming->VSAT ) );
    */
    tf = tsat - paramtiming->RLIN * (paramtiming->CAPA+ceqrsat0+paramtiming->CAPAI)/1000.0 * 
                log( 1.0 - ( threshold - paramtiming->VSAT ) / ( paramtiming->VDDMAX - paramtiming->VSAT ) );
  }

  if( threshold > 0.0 )
    paramtiming->CEQRSAT = ceqrsat/1000.0 + stm_get_qsat(&qsat,tf)/threshold ;
  else
    paramtiming->CEQRSAT = 0.0 ;

  return tf ;
}

double stm_compute_vout( stm_dual_param_timing *param, double *t, double *vqsat, stm_qsat_v *qsat_enh )
{
  double   qsol ;
  double   qtot ;
  double   vout ;
  double   vmin ;
  double   vmax ;
  double   ceqrsat ;
  stm_qsat qsat ;
  static int fordebug = 0 ;

  fordebug++ ;

  qsol = stm_get_q( param->IMAX, (*t - param->DTPWL), param->F, param->BN, param->U ) - (param->CAPAI/1000.0)*( param->VT + param->VDDIN*tanh(*t/param->FIN) ) - param->QINIT ;

  /* toujours vrai */
  vmax = qsol/(param->CAPA/1000.0) ;
  if( vmax > param->VDDMAX )
    vmax = 0.999*param->VDDMAX ;

  /* approximation */
  vmin = -((param->CAPAI/1000.0)*param->VT+param->QINIT)/(param->CAPA/1000.0) ;
 
  if( vmin > vmax ) {
    vmin = vmax - param->VDDMAX/10.0 ;
  }

  do {
    stm_modscm_dual_calts_qsat( param, vmin, qsat_enh, &qsat, &ceqrsat, NULL );
    qtot = ( (param->CAPA/1000.0) + (ceqrsat/1000.0) )*vmin + stm_get_qsat( &qsat, *t - param->DTPWL );
    if( qtot > qsol ) {
      vmin = vmin - param->VDDMAX/10.0 ;
    }
  }
  while( qtot > qsol );

  do {
    vout = ( vmin + vmax ) / 2.0 ;

    stm_modscm_dual_calts_qsat( param, vout, qsat_enh, &qsat, &ceqrsat, NULL );
    qtot = ( (param->CAPA/1000.0) + (ceqrsat/1000.0) )*vout + stm_get_qsat( &qsat, *t - param->DTPWL );

    if( qtot > qsol )
      vmax = vout ;
    else
      vmin = vout ;
  }
  while( (vmax-vmin)>param->VDDMAX/1000.0 );

  vout = ( vmin + vmax ) / 2.0 ;

  if( vqsat ) {
    stm_modscm_dual_calts_qsat( param, vout, qsat_enh, &qsat, &ceqrsat, NULL );
    *vqsat = vout * ceqrsat/1000.0 + stm_get_qsat( &qsat, *t - param->DTPWL ) ;
  }

  return vout ;
}

double stm_compute_vout_old( stm_dual_param_timing *paramtiming, double *t, double *qsat, stm_qsat_v *qsat_enh )
{
  double tmin, tmax ;
  double vmin, vmax ;
  double vout ;
  double tx ;
  int    iter = 20 ;

  vmin = 0.1*paramtiming->VDDMAX ;
  vmax = paramtiming->VSAT ;
  vout = -1.0 ;

  tmin = stm_modscm_dual_calts_with_param_timing_classic( paramtiming, vmin, qsat_enh ) ;

  if( tmin > *t ) {
  
    *t    = tmin ;
    vout  = vmin ;
    if( qsat )
      *qsat = paramtiming->CEQRSAT * vout ;
    
  }
  else {
  
    tmax = stm_modscm_dual_calts_with_param_timing_classic( paramtiming, vmax, qsat_enh ) ;
    
    if( tmax < *t ) {
    
      vout  = -1.0 ;
      if( qsat )
        *qsat = 0.0 ;
      
    }
    else {

      do {
        iter-- ;
        
        vout = vmin + (*t-tmin)/(tmax-tmin)*(vmax-vmin) ;
        tx   = stm_modscm_dual_calts_with_param_timing_classic( paramtiming, vout, qsat_enh ) ;
        if( tx < *t ) {
          tmin = tx ;
          vmin = vout ;
        }
        else {
          tmax = tx ;
          vmax = vout ;
        }
      }
      while( fabs(tx-*t) / *t > 0.01 && iter );

      if( !iter ) {
        avt_log( LOGDEBUG, 1, "stm_compute_vout() : convergence failled\n" );
      }
      
      *t = tx ;
      if( qsat )
        *qsat = paramtiming->CEQRSAT * vout ;
    }
  }

  return vout ;
}

void stm_modscm_dual_calts_qsat_v( stm_dual_param_timing *paramtiming, stm_qsat_v *qsat )
{
  qsat->A     = 1.0/(paramtiming->RSAT*(paramtiming->CAPA/1000.0));
  qsat->B     = (paramtiming->IMAX-paramtiming->VDDMAX/paramtiming->RSAT)/(paramtiming->CAPA/1000.0) ;
  qsat->T0    = paramtiming->QSAT_TE-paramtiming->DTPWL + (1.0/qsat->A)*log((paramtiming->VDDMAX+qsat->B/qsat->A-paramtiming->QSAT_VE)/(paramtiming->VDDMAX+qsat->B/qsat->A));
  qsat->TS    = paramtiming->QSAT_TE-paramtiming->DTPWL ;
  qsat->QSAT0 = paramtiming->QSAT_Q0 ;
}

/* Calcule le modèle de charge de saturation. Rempli la structure qsat suivant la configuration. Si le modèle ceqrsat est retenu,
qsat est configuré de façon à renvoyer 0.
si le pointeur ceqrsat0 est défini, il contiendra le ceqrsat intermédiaire utilisé pour configurer qsat */
void stm_modscm_dual_calts_qsat( stm_dual_param_timing *paramtiming, 
                                 double threshold, 
                                 stm_qsat_v *qsatv, 
                                 stm_qsat *qsat, 
                                 double *ceqrsat, 
                                 double *ceqrsat0 
                               )
{
  double qsat2 ;

  if( qsat ) {
    qsat->QSAT2   = 0.0 ;
    qsat->DT      = 0.0 ;
    qsat->TSATMAX = 0.0 ;
  }
  if( ceqrsat )
    *ceqrsat      = 0.0 ;

  if( threshold >= paramtiming->VDDMAX ) {
    avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_qsat() : threshold exceed vdd. set to vdd\n" );
    threshold = 0.999*paramtiming->VDDMAX ;
  }
  
  if( threshold > 0.0 ) {

    if( qsat )
      qsat->QSATV   = qsatv ;
    
    if( paramtiming->COMPAT30P3 ) {
         
      if( V_BOOL_TAB[ __STM_CEQRSAT_NUMERIC].VALUE ) {
        if( ceqrsat )
          *ceqrsat = stm_modscm_dual_ceqrsat_numeric( paramtiming, threshold );
      }
      else {
        if( ceqrsat )
          *ceqrsat = stm_modscm_dual_ceqrsat( paramtiming->IMAX,
                                              paramtiming->VT, 
                                              paramtiming->VDDMAX, 
                                              paramtiming->FOUT, 
                                              paramtiming->U, 
                                              paramtiming->F, 
                                              paramtiming->RSAT, 
                                              paramtiming->RLIN, 
                                              paramtiming->AN, 
                                              paramtiming->BN, 
                                              threshold, 
                                              paramtiming->THRESHOLD, 
                                              paramtiming->TE, 
                                              paramtiming->T0-paramtiming->DTPWL
                                            );
      }
      if( ceqrsat0 )
        *ceqrsat0 = *ceqrsat ;
    }
    else {
    
      if( threshold > 0.0 && paramtiming->RSAT && ( !qsat->QSATV || ceqrsat0 ) ) {
      
        if( V_BOOL_TAB[ __STM_CEQRSAT_NUMERIC].VALUE ) {
        
          *ceqrsat = stm_modscm_dual_ceqrsat_numeric( paramtiming, threshold );
          
        }
        else {
        
          if( ceqrsat )
            *ceqrsat = stm_modscm_dual_ceqrsat( paramtiming->IMAX,
                                                paramtiming->VT, 
                                                paramtiming->VDDMAX, 
                                                paramtiming->FOUT, 
                                                paramtiming->U, 
                                                paramtiming->F, 
                                                paramtiming->RSAT, 
                                                paramtiming->RLIN, 
                                                paramtiming->AN, 
                                                paramtiming->BN, 
                                                threshold, 
                                                paramtiming->THRESHOLD, 
                                                paramtiming->TE, 
                                                paramtiming->T0-paramtiming->DTPWL
                                              );
        }

        if( ceqrsat0 )
          *ceqrsat0 = *ceqrsat ;

        if( V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE == 1 ||
            V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE == 2 ||    
            V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE == 3    ) {
            
          qsat2 = stm_modscm_dual_get_qsat2( paramtiming, threshold, (*ceqrsat)/1000.0 );
          
          if( qsat2 > 0.0 ) {
              *ceqrsat = 0.0 ;
          }
        }
        else
          qsat2 = 0.0 ;

        if( qsat->QSATV )
          if( ceqrsat )
            *ceqrsat = 0.0 ;

        if( qsat ) {
          qsat->QSAT2   = qsat2 ;
          qsat->DT      = paramtiming->DT ;
          qsat->TSATMAX = paramtiming->IMAX/(2.0*qsat2);
        }
      }
    }
  }
  else {
    if( qsat )
      qsat->QSATV   = NULL ;
  }

  if( qsat ) {
    qsat->IMAX    = paramtiming->IMAX ;
    qsat->C       = (paramtiming->CAPA+*ceqrsat)/1000.0 ;
    qsat->VDD     = paramtiming->VDDMAX ;
  }
}

double stm_modscm_dual_calts_qbranch( stm_dual_param_timing *param, stm_qsat_v *qsatv, double vout, double t ) 
{
  double    q ;
  stm_qsat  qsat ;
  double    vqsat ;
  double    qbr ;
  double    ceqrsat ;
  
  q = stm_get_q( param->IMAX, t, param->F, param->BN, param->U );
  stm_modscm_dual_calts_qsat( param, vout, qsatv, &qsat, &ceqrsat, NULL );
  vqsat = stm_get_qsat( &qsat, t ) + ceqrsat*vout/1000.0 ;

  qbr = q - vqsat ;

  return qbr ;
}

double stm_modscm_dual_calts_with_param_timing( stm_dual_param_timing *paramtiming, double threshold )
{
  double     tf ;
  double     te ;
  double     vout_enh ;
  stm_qsat_v qsat ;
  stm_qsat_v *ptqsat ;
  double     vqsat ;
  int        computed = 0 ;
  static int fordebug = 0 ;
  double     vout ;
  double     lvout ;
  double     q ;
  double     backup_f ;
  double     backup_dtpwl ;
  double     backup_qinit ;
  double     tiabs ;
  double     ltiabs ;
  double     localthreshold ;
  int        i ;

  fordebug++;
    
  if( threshold >= paramtiming->VSAT ) {
    localthreshold = paramtiming->VSAT ;
  }
  else {
    localthreshold = threshold ;
  }

  if( paramtiming->NTHSHRK && !paramtiming->COMPAT30P3 ) {
  
    q      = 0.0 ;
    ltiabs = 0.0 ;
    lvout  = 0.0 ;
    
    backup_f           = paramtiming->F ;
    backup_dtpwl       = paramtiming->DTPWL ;
    backup_qinit       = paramtiming->QINIT ;

    for( i=0 ; i<paramtiming->NTHSHRK ; i++) {

      ptqsat = NULL ;

      if( V_BOOL_TAB[ __STM_QSAT_ENHANCED ].VALUE )
        te = paramtiming->PWTHSHRK[i].F * atanh( V_FLOAT_TAB[ __STM_QSAT_RATIO ].VALUE ) + paramtiming->PWTHSHRK[i].DT ;
      else
        te = FLT_MAX ;

      if( i==paramtiming->NTHSHRK-1 )
        tiabs = FLT_MAX*0.9 ; /* to be sure to get a good comparison with te */
      else
        tiabs = paramtiming->PWTHSHRK[i].T+paramtiming->PWTHSHRK[i].DT ;
     
      paramtiming->F     = paramtiming->PWTHSHRK[i].F ;
      paramtiming->DTPWL = paramtiming->PWTHSHRK[i].DT ;

      stm_estim_output( paramtiming, ltiabs, lvout );

      /* calcule de la charge initiale de l'intervalle courant. le modèle enhanced n'est activé 
      que sur l'intervalle courant, pas avant */
      if( i > 0 ) 
        q = q - stm_modscm_dual_calts_qbranch( paramtiming, NULL, lvout, paramtiming->PWTHSHRK[i-1].T+paramtiming->PWTHSHRK[i-1].DT-paramtiming->PWTHSHRK[i].DT ) ;
      paramtiming->QINIT = backup_qinit - q ;

      if( te < tiabs ) {
     
        /* détermination de la tension de sortie à partir de laquelle on utilise le modèle enhanced */
        if( i>0 && te < paramtiming->PWTHSHRK[i-1].T+paramtiming->PWTHSHRK[i-1].DT ) 
          te = paramtiming->PWTHSHRK[i-1].T+paramtiming->PWTHSHRK[i-1].DT ;
        vout_enh = stm_compute_vout( paramtiming, &te, &vqsat, NULL );
        
        if( localthreshold < vout_enh ) {
          break ;
        }
        else {
          /* il faut calculer les paramètres enhanced avant le calcul de la charge initiale car ce modèle
          peut se déclencher avant l'intervalle courant */
          paramtiming->QSAT_TE = te ;
          paramtiming->QSAT_VE = vout_enh ;
          paramtiming->QSAT_Q0 = vqsat ;
          stm_modscm_dual_calts_qsat_v( paramtiming, &qsat );
          ptqsat = &qsat ;

          /* détermination de la tension de sortie à la fin de l'intervalle courant */
          if( i==paramtiming->NTHSHRK-1 )
            vout = paramtiming->VDDMAX ;
          else
            vout = stm_compute_vout( paramtiming, &tiabs, NULL, ptqsat );
          if( localthreshold < vout ) {
            break ;
          }
        }
      }
      else {

        /* détermination de la tension de sortie à la fin de l'intervalle courant */
        if( i==paramtiming->NTHSHRK-1 )
          vout = paramtiming->VDDMAX ;
        else
          vout = stm_compute_vout( paramtiming, &tiabs, NULL, NULL );
        if( localthreshold < vout ) {
          break ;
        }
      }
   
      if( i==paramtiming->NTHSHRK-1 )
        avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_with_param_timing() : last iteration\n" );

      q = q + stm_modscm_dual_calts_qbranch( paramtiming, ptqsat, vout, paramtiming->PWTHSHRK[i].T ) ;
      lvout = vout ;
      ltiabs = tiabs ;
    }

    paramtiming->QINIT = backup_qinit - q ;
    tf = stm_modscm_dual_calts_with_param_timing_classic( paramtiming, threshold, ptqsat );
    tf = tf + paramtiming->PWTHSHRK[i].DT ;
    
    paramtiming->F     = backup_f ;
    paramtiming->DTPWL = backup_dtpwl ;
    paramtiming->QINIT = backup_qinit ;

  }
  else {

    if( V_BOOL_TAB[ __STM_QSAT_ENHANCED ].VALUE ) {
    
      if( paramtiming->QSAT_TE < 0.0 ) {
        // instant when the input reach x% of vddin.
        te = paramtiming->FIN * atanh( V_FLOAT_TAB[ __STM_QSAT_RATIO ].VALUE ) ;
        vout_enh = stm_compute_vout( paramtiming, &te, &vqsat, NULL );
        if( vout_enh < paramtiming->VSAT ) {
          paramtiming->QSAT_TE = te ;
          paramtiming->QSAT_VE = vout_enh ;
          paramtiming->QSAT_Q0 = vqsat ;
        }
      }
      
      if( paramtiming->QSAT_VE > 0.0 && threshold > paramtiming->QSAT_VE ) {
        stm_modscm_dual_calts_qsat_v( paramtiming, &qsat );
        tf = stm_modscm_dual_calts_with_param_timing_classic( paramtiming, threshold, &qsat );
        computed = 1 ;
      }
    }

    if( !computed )
      tf = stm_modscm_dual_calts_with_param_timing_classic( paramtiming, threshold, NULL );
  }

  return tf ;
}

/* Renvoie 0 pour le modèle enhanced. Le tsatmax n'est pas pris en compte. */
double stm_get_qsat_derivative( stm_qsat *qsat, double t )
{
  double dq ;

  if( qsat && t > 0.0 ) {
  
    if( qsat->QSATV ) {
      dq = 0.0 ;
    }
    else {
      switch( V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE ) {
    
      case 1 :
      case 2 :
        dq = 2.0 * qsat->QSAT2 * t ;
        break ;

      default :
        dq = 0.0 ;
      }
    }
  }
  else
    dq = 0.0 ;

  return dq ;
}

inline double stm_get_qsat( stm_qsat *qsat, double t )
{
  double q = 0.0 ;
  double vi ;
  double vf ;
  double a ;
  double b ;
  double c ;
  double t0 ;
  double ts ;
  double vdd ;
  double qsat0 ;
 
  if( qsat ) {
    if( qsat->QSATV ) {
    
      a     = qsat->QSATV->A ;
      b     = qsat->QSATV->B ;
      t0    = qsat->QSATV->T0 ;
      ts    = qsat->QSATV->TS ;
      qsat0 = qsat->QSATV->QSAT0 ;
      vdd   = qsat->VDD ;
      c     = qsat->C ;

      if( t > ts ) {
        vf = vdd-(exp(-a*(t-t0))*(vdd+b/a)-b/a) ;
        vi = vdd-(exp(-a*(ts-t0))*(vdd+b/a)-b/a) ;
        q = qsat->IMAX*(t-ts) - c*(vf-vi) + qsat0 ;
      }
      else
        q = qsat0 ;
    }
    else {
      if( t > 0.0 ) {
        switch( V_INT_TAB[ __STM_QSAT_LEVEL ].VALUE ) {
        
        case 0 :
          q = 0.0 ;
          break ;
          
        case 1 :
          if( t>qsat->TSATMAX )
            q = qsat->QSAT2*qsat->TSATMAX*qsat->TSATMAX ;
          else
            q = qsat->QSAT2*t*t ;
          break ;
        
        case 2 :
          if( t>qsat->TSATMAX )
            q = qsat->QSAT2*(qsat->TSATMAX-qsat->DT)*(qsat->TSATMAX-qsat->DT) ;
          else
            q = qsat->QSAT2*(t-qsat->DT)*(t-qsat->DT) ;
          break ;

        case 3 :
          q = qsat->QSAT2*t ;
        }
      }
    }
  }

  return q ;
}

/****************************************************************************/

double stm_modscm_dual_calculduts_threshold (dualparams *params, float slew, float load, double seuil, stm_pwl *pwl)
{
    double cconf;
    double capa, capa0;
    double pconf0    = (double)params->DP[STM_PCONF0];
    double pconf1    = (double)params->DP[STM_PCONF1];
    double capai     = (double)params->DP[STM_CAPAI];
    double irap      = (double)params->DP[STM_IRAP];
    double vddin     = (double)params->DP[STM_VDDIN];
    double vt        = (double)params->DP[STM_VT];
    double threshold = (double)params->DP[STM_THRESHOLD];
    double input_thr = (double)params->DP[STM_INPUT_THR];
    double imax      = (double)params->DP[STM_IMAX];
    double an        = (double)params->DP[STM_AN];
    double bn        = (double)params->DP[STM_BN];
    double delayrc   = (double)params->DP[STM_DRC];
    double rbr       = (double)params->DP[STM_RBR];
    double cbr       = (double)params->DP[STM_CBR];
    double rsat      = (double)params->DP[STM_RSAT];
    double rlin      = (double)params->DP[STM_RLIN];
    double rint      = (double)params->DP[STM_RINT];
    double vint      = (double)params->DP[STM_VINT];
    double vddmax    = (double)params->DP[STM_VDDMAX];
    double fout ;
    double ts;
    double deltat;
    double U, F, Ur, Fr, im, Fs, tm, Fm, k4Q, t0, Q, te, koshoot;
    char *str;
    /*double overshoot = (vt + (vddmax - vt) / 3.0)/vddmax ;*/

    if( pconf1*slew < 10.0*load ) 
      pconf1 = 1.2 * pconf1 ;

    if ((str = avt_gethashvar ("STM_TRACE_MODE")))
        if (!strcmp (str, "yes")) {
            fprintf (stdout, "pconf0 = %f\n", pconf0);
            fprintf (stdout, "pconf1 = %f\n", pconf1);
            fprintf (stdout, "capai = %f\n", capai);
            fprintf (stdout, "irap = %f\n", irap);
            fprintf (stdout, "vddin = %f\n", vddin);
            fprintf (stdout, "vt = %f\n", vt);
            fprintf (stdout, "threshold = %f\n", threshold);
            fprintf (stdout, "imax = %f\n", imax);
            fprintf (stdout, "an = %f\n", an);
            fprintf (stdout, "bn = %f\n", bn);
            fprintf (stdout, "\n");
        }


    /*     slew : front d'entrée réel original.
           Fr :   front d'entrée rapporté sans prendre en compte le cbr.
           Fs :   instant où on considère que le courant atteint imax.
           Fm :   front d'entrée réel prenant en compte le cbr.
           F :    front d'entrée rapporté prenant en compte cbr.

           Les équations de courant instantané sont :

           Le courant avec prise en compte explicite de rbr, cbr :
           
             stm_modscm_dual_get_ir( t, A, B, Vin, Fin, Ur, Fr, rbr, cbr, imax, Fs)    <--- eqt approchée
             
           Le courant équivalent :

             stm_modscm_dual_get_ia( t, A, B, Ua, Fa )
    */
    
    
    /* A décommenter pour revenir à l'ancienne version */
    /* rbr=-1.0 ; cbr = -1.0 ; */ 

    if((rbr > 0.0) && (cbr > 0.0)){
        /* Determine une approximation du courant dans la branche complète. Cela revient
           à la méthode Amjad : an et bn sont les paramètres équivalent à la branche vers
           la source, et U et Fr sont les paramètres rapportés équivalent à la branche vers
           le drain. 
        */
        stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, (double)slew, &Ur, &Fr);
        stm_modscm_dual_slopei(0.7, bn, Ur, imax, Fr, rsat, vddmax, vddmax, &im, &Fs);

        /* Calcule la modification du front d'entrée. */
        stm_modscm_dual_modifslope( vddin, vt, slew, vddmax, rbr, cbr, im, Fs, &tm, &Fm);
    }
    else {
        Fm = (double)slew;
    }

    /* Calcule le front d'entrée rapporté. */
    stm_modscm_dual_cal_UF (imax, an, bn, vddin, 0.0, Fm, &U, &F);

    koshoot = 0.0 ;
    if((capai > 0.00) && (capai/(load + (pconf0 + pconf1 * slew) * irap) > STM_OVERSHOOT)){
        deltat = stm_modscm_dual_calts (imax, an, bn, U, threshold, 0.0, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
        te = stm_modscm_dual_calte (vddin, vt, input_thr, (double)slew);
        koshoot = ( deltat/te ) * (deltat/te );
    }else{
        deltat = 0.0;
    }
    deltat += delayrc ;
    if( koshoot < 1.0 )
        cconf = pconf0 + pconf1 * slew *( 1.0 - koshoot );
    else
        cconf = pconf0;
    capa0 = cconf * irap + (double)load;
    t0 = stm_modscm_dual_calts (imax, an, bn, U, threshold, capa0, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
    capa = capa0 ;

   
    if((rbr > 0.0) && (cbr > 0.0)){
        // Calcule de la charge supplémentaire due à l'overshoot
        fout = stm_modscm_dual_calslew( imax, an, bn, U, vt, vddmax, capa, slew, F, t0);
        k4Q = stm_modscm_dual_get_k4Q( an, bn, vddin, slew, Ur, Fr, U, F, rbr, cbr, im, Fs, vddmax, rsat, rlin, rint, vint, vt );
        Q = stm_modscm_dual_get_Q( t0, k4Q, an, bn, vddin, slew, Ur, Fr, U, F, rbr, cbr, im, Fs, tm, vddmax, vt, rsat, rlin, rint, vint, fout, threshold );
        if( 2.0 * Q / vddmax < capa )
            capa -= 2.0 * Q / vddmax;
    }
    if( rsat <= 0.0 ) 
      ts = stm_modscm_dual_calts (imax, an, bn, U, seuil, capa, F, 0.0, vt, vddin, capai, NULL, NULL, slew, NULL, 0, 0.0, 0.0, 0.0, NULL );
    else {
      ts = stm_modscm_dual_calts_rsat_full_range(capai, imax, an, bn, U, vt, vddmax, threshold, seuil, rsat, rlin, capa, slew, F, stm_modscm_dual_calte (vddin, vt, vt, (double)slew), t0, vddin, pwl );
    }

    return (ts);
}

/****************************************************************************/

float stm_modscm_dual_slope (dualparams *params, float slew, float load)
{
    double cconf;
    double capa;
    double pconf0    = (double)params->DP[STM_PCONF0];
    double pconf1    = (double)params->DP[STM_PCONF1];
    double capai     = (double)params->DP[STM_CAPAI];
    double irap      = (double)params->DP[STM_IRAP];
    double vddin     = (double)params->DP[STM_VDDIN];
    double vt        = (double)params->DP[STM_VT];
    double threshold = (double)params->DP[STM_THRESHOLD];
    double imax      = (double)params->DP[STM_IMAX];
    double an        = (double)params->DP[STM_AN];
    double bn        = (double)params->DP[STM_BN];

    if( pconf1*slew < 10.0*load ) 
      pconf1 = 1.2 * pconf1 ;

    cconf = pconf0 + pconf1 * slew;
    capa  = capai + cconf * irap;
    capa += (double)load;

    return (float)stm_modscm_dual_calslope (imax, an, bn, vddin, vt, threshold, capa, (double)slew);
}

/****************************************************************************/

void  stm_modscm_dual_timing( dualparams *params, 
                              float       fin, 
                              stm_pwl    *pwlin, 
                              stm_driver *driver,
                              float       r,
                              float       c1,
                              float       c2,
                              float      *delay,
                              float      *fout,
                              stm_pwl   **pwlout,
                              char *modelname
                            )
{
  stm_dual_param_timing paramtiming ;
  float                 threshold = params->DP[STM_THRESHOLD] ;
  float                 vddin     = params->DP[STM_VDDIN] ;
  float                 vt        = params->DP[STM_VT] ;
  float                 input_thr = params->DP[STM_INPUT_THR] ;
  float                 delayrc   = params->DP[STM_DRC] ;
  float                 vdd       = params->DP[STM_VDDMAX];
  stm_pwth              pwthin[10] ;
  int                   nthin;
  stm_pwth             *ptthin ;
  float                 s1old ;
  float                 s1new ; 
  float                 s2old ;
  float                 s2new ;
  float                 seuil1 ;
  float                 seuil2 ;
  float                 ts;
  float                 ts1 ;
  float                 ts2 ;
  float                 te ;
  int                   oldmode=0 ;
  paramforslewpwl       parampwltanh;
  mbk_pwl              *mbkpwlout;
  static int            fordebug=0 ;
  int                   indextime ;
  int                   indexslope1 ;
  int                   indexslope2 ;
  float                 tabvout[5];
  float                 tabtin[5];
  float                 tabt[5];
  int                   nbval;
  int                   i;
  float                 load;
  float                 capa;
  float                 capa0;


  fordebug++;

  if( r>0 && c2>0.0 )
    load = stm_modscm_dual_capaeq (params, NULL, fin, r, c1, c2, threshold, modelname );
  else
    load = c1 ;

  stm_modscm_dual_fill_param( params, fin, pwlin, driver, load, &paramtiming ) ;
  nbval = 0 ;
  
  if( delay ) {
    indextime   = nbval ;
    tabvout[nbval] = threshold ;
    nbval++ ;
  }

  if( fout ) {
  
    if( !oldmode ) {
    
      s1old = vt ;
      s1new = 0.3*vdd ;

      s2old = 0.75 * (vdd-vt) + vt ;
      s2new = 0.8*vdd ;

      if( s1old > s1new ) seuil1 = s1old ; else seuil1 = s1new ;
      if( s2old > s2new ) seuil2 = s2old ; else seuil2 = s2new ;

      if( V_BOOL_TAB[ __STM_NEW_THRESHOLD ].VALUE ) {
        seuil1 = vdd * STM_DEFAULT_SMINR ;
        seuil2 = vdd * STM_DEFAULT_SMAXR ;
      }

      indexslope1 = nbval ;
      tabvout[nbval] = seuil1 ;
      nbval++ ;
      indexslope2 = nbval ;
      tabvout[nbval] = seuil2 ;
      nbval++ ;
    }
  }

  capa  = paramtiming.CAPA ;
  capa0 = paramtiming.CAPA0 ;

  if( paramtiming.INTEGNUMERIC ) {
    paramtiming.CAPA  = capa + load ;
    paramtiming.CAPA0 = capa0 + load ;
    stm_modscm_dual_calts_final_numeric( &paramtiming, tabvout, tabt, nbval );
  }
  else {
    if( V_BOOL_TAB[ __AVT_NUMSOL_FOR_PILOAD ].VALUE && c2>0 && r>0.0 ) {
      stm_modscm_dual_calts_final_numeric_pi(  &paramtiming, r, c1, c2, tabvout, tabt, nbval );
    }
    else {
      for( i=0 ; i<nbval ; i++ ) {
        if( r > 0.0 && c2 > 0.0 ) {
          paramtiming.CAPA = capa ;
          load = stm_modscm_dual_capaeq (params, &paramtiming, fin, r, c1, c2, tabvout[i], modelname );
        }
        paramtiming.CAPA  = capa + load ;
        paramtiming.CAPA0 = capa0 + load ;
        tabt[i] = stm_modscm_dual_calts_with_param_timing( &paramtiming, tabvout[i] ) ;
        if( paramtiming.NEWCTKMODEL )
          tabtin[i] = paramtiming.TIN ;
      }
    }
  }

  paramtiming.CAPA  = capa ;
  paramtiming.CAPA0 = capa0 ;

  if( delay ) {
  
    ts = tabt[ indextime ] ;
   
    ptthin = NULL ;
    if( pwlin ) {
      if( stm_pwl_to_tanh( pwlin, vt, vddin+vt, pwthin, &nthin, fin ) )
        ptthin = pwthin ;
    }

    if( ptthin )
      te = stm_get_t_pwth( vddin, vt, input_thr, ptthin );
    else {
      if( paramtiming.NEWCTKMODEL )
        te = tabtin[ indextime ] ;
      else
        te = stm_modscm_dual_calte( vddin, vt, input_thr, fin );
    }
      
    *delay = ts - te + delayrc ;
    
  }

  if( fout ) {
    if( oldmode ) {
      *fout = stm_modscm_dual_slew_old( params, fin, load );
    }
    else {

      ts1 = tabt[ indexslope1 ] ;
      ts2 = tabt[ indexslope2 ] ;

      if( ts2-ts1 > 0.0 ) {
        *fout = stm_thr2scm( ts2-ts1, 
                             seuil1/vdd, 
                             seuil2/vdd, 
                             vt, 
                             vdd, 
                             vdd, 
                             STM_UP 
                           );

        if( pwlout ) {

          parampwltanh.DUALSCM = &paramtiming;

          mbkpwlout = mbk_create_pwl_according_tanh( (char (*)(void*, float, float*))stm_modscm_dual_slew_for_pwl, 
                                                     (void*)&parampwltanh, 
                                                     vt, 0.95*vdd );
          *pwlout = mbk_pwl_to_stm_pwl( mbkpwlout );
          mbk_pwl_free_pwl( mbkpwlout );
        }
      }
      else {
        *fout = fin ;
      }
    }
  }
}

/****************************************************************************/
    
float stm_modscm_dual_delay (dualparams *params, float slew, stm_pwl *pwl, float load)
{
    double   vddin     = (double)params->DP[STM_VDDIN];
    double   vt        = (double)params->DP[STM_VT];
    double   input_thr = (double)params->DP[STM_INPUT_THR];
    double   delayrc   = (double)params->DP[STM_DRC];
    double   te;
    double   ts;
    stm_pwth pwth[10] ;
    int      nth ;

    if( pwl ) {
      if( stm_pwl_to_tanh( pwl, vt, vddin+vt, pwth, &nth, slew ) )
        te = stm_get_t_pwth( vddin, vt, input_thr, pwth );
      else
        te = stm_modscm_dual_calte (vddin, vt, input_thr, (double)slew);
    }
    else
      te = stm_modscm_dual_calte (vddin, vt, input_thr, (double)slew);
      
    ts = stm_modscm_dual_calculduts (params, slew, load, pwl);
    
    return (float)(ts - te + delayrc);
}

/****************************************************************************/

double stm_modscm_dual_calte (double vddin, double VT, double seuil, double fin)
{
  double te;
  
  if( seuil > VT ) 
    te = fin * atanh((seuil-VT)/(vddin)) ;
  else
    te = fin * (seuil-VT)/(vddin) ;
    
  return te;
}

/****************************************************************************/
double stm_modscm_dual_calts_old (double imax,
                                  double an,
                                  double bn,
                                  double U,
                                  double seuil,
                                  double c,
                                  double F)
{
    double an1, 
           bn1, 
           /* rc, */
           sf,
           b   = 0.0,
           z   = 0.0,
           tt  = 0.0,
           ttt = 0.0,
           /* k   = 0.0, */
           ti = 0.0,
           tmin = 0.0,
           tmax = 0.0,
           tint = 0.0,
           Qmin = 0.0,
           Qmax = 0.0,
           Qi   = 0.0,
           BU   = bn * U,
           BU2  = BU * BU,
           c1  = 0.0;
    long   i = 0, j = 0;
    float ltt;

    /* calcul de la tension rapportée à partir de :
    
              an.U²
       imax = ------
              1+bn.U
    */
    
    if((U <= 0.0)||(F <= 0.0))
        return 0.0;
    an1 = an / bn;
    bn1 = 1 / (U * bn);
    c1 = c;

    /* Dans ce qui suit, on a un changement de variable t = t/F 

    Résolution de :
    
          (         ln(2).b²u²-ln(1+b.u) )
     imax.( f.t - f.-------------------- )
          (              b²u²-b.u        )

             (    (     1-b.u          )                                )
             (  ln( 1 + -----.exp(-2t) )                                )
             (    (     1+b.u          )   (      1  )                  )
    + f.imax.(  ------------------------ + ( 1 + --- ).ln( 1+exp(-2t) ) )
             (        b²u² - b.u           (     b.u )                  )

    = c.seuil


    Solution initiale : on néglige la seconde ligne de l'expression

             ln(2).b²u²-ln(1+b.u)     c.seuil
         t - --------------------   = -------
                  b²u²-b.u            f.imax
                  
                     |                   |

                     sf                  tt
                     
    */
    z = ((seuil / 1000.0) * c1) / imax ; 
    tt = z/F ;
    sf = ((1.0 + bn1) * LOG_2) - (bn1 * bn1 * (log (0.5 * ((bn1 + 1.0) / bn1))) / (1.0 - bn1));
    tt += sf ;
    ti = tt * F;

    /* Puis on trouve par convergence :
                                            (     1-b.u          )
                                          ln( 1 + -----.exp(-2t) )
         c.seuil   ln(2).b²u²-ln(1+b.u)     (     1+b.u          )
    t =  ------- + -------------------- - ------------------------
         f.imax         b²u²-b.u                b²u² - b.u        

           (      1  )
         - ( 1 + --- ).ln( 1+exp(-2t) )
           (     b.u )
          
    */
    
    do {
        i++;
        ltt = tt;
        b = exp (- 2.0 * tt);
        // ttt correspond aux trois termes avec les ln.
        ttt = (1.0 + bn1) * log (2.0 / (1.0 + b))
            - ((bn1 * bn1) / (1.0 - bn1)) * log (1.0 + ((1.0 - bn1) * (1.0 - b) / (2.0 * bn1)));
        tt = z/F + ttt;
    }
    while( (fabs((ltt-tt)/tt) > STM_MCC_EPSILON) && (i < 20) );

    /* On défait le changement de variable */
    tt *= F;
    ti = tt;

    if (i >= 20){
        Qi = imax * (ti - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
             + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * ti / F))
             + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * ti / F))) - c1 * seuil / 1000.0;
        if(Qi > 0.0){
            tmax = ti;
            Qmax = Qi;
            tmin = ti - 0.1 * ti;
            Qmin = imax * (tmin - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
                 + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * tmin / F))
                 + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * tmin / F))) - c1 * seuil / 1000.0;
        }else if(Qi < 0.0){
            tmin = ti;
            Qmin = Qi;
            tmax = ti + 0.1 * ti;
            Qmax = imax * (tmax - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
                 + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * tmax / F))
                 + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * tmax / F))) - c1 * seuil / 1000.0;
        }else{
            return ti;
        }
        while((((Qmin > 0.0) && (Qmax > 0.0)) || ((Qmin < 0.0) && (Qmax < 0.0))) && (j < 100)){
            j++;
            Qmax = imax * (tmax - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
                 + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * tmax / F))
                 + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * tmax / F))) - c1 * seuil / 1000.0;
            Qmin = imax * (tmin - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
                 + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * tmin / F))
                 + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * tmin / F))) - c1 * seuil / 1000.0;
            if((Qmin > 0.0) && (Qmax > 0.0)){
                tint = tmin;
                tmin = tmin - (tmax - tmin);
                tmax = tint;
            }else  if((Qmin < 0.0) && (Qmax < 0.0)){
                tint = tmax;
                tmax = tmax + (tmax - tmin);
                tmin = tint;
            }
        }
        ti = tmin + (tmax - tmin) / 2.0;
        while((tmax - tmin) > (1000.0 * STM_MCC_EPSILON) && (j < 100)){
            j++;
            Qi = imax * (ti - F * (LOG_2 * BU2 - log(1.0 + BU))/(BU2 - BU))
               + F * imax * (1.0/(BU2 - BU) * log(1.0 + (1.0 - BU)/(1.0 + BU) * exp(-2.0 * ti / F))
               + (1.0 + 1.0/BU) * log(1.0 + exp(-2.0 * ti / F))) - c1 * seuil / 1000.0;
            if(Qi > 0.0){
                tmax = ti;
                ti = tmin + (tmax - tmin) / 2.0;
            }else if(Qi < 0.0){
                tmin = ti;
                ti = tmin + (tmax - tmin) / 2.0;
            }else{
                return ti;
            }
        }
        ti = (tmax + tmin)/2.0;
        tt = ti;
    }

    return tt;
}

void stm_debug( char   *fname, 
                double imax, 
                double F, 
                double BN, 
                double U, 
                stm_qsat *qsat,
                double c, 
                double threshold, 
                double dtin,
                double qinit,
                double ci,
                double vt,
                double vddin,
                double fin,
                double tmin,
                double tmax,
                double tref
              )
{
  double t ;
  double step ;
  double q ;
  double sol ;
  FILE   *file ;
  
  file = mbkfopen( fname, NULL, "w" ) ;
  if( !file ) {
    printf( "can't open file %s\n", fname ) ;
    return ;
  }
  
  step = (tmax-tmin)/1000.0 ;

  for( t=tmin ; t<=tmax ; t=t+step ) {
    q   = stm_get_q( imax, t, F, BN, U );
    sol = stm_get_qsat( qsat, t+dtin ) + (c/1000.0)*threshold + qinit + ci/1000.0 * (vt+vddin * tanh((t+dtin)/fin));
    fprintf( file, "%g %g %g %g %g\n", (t+tref)*1e-12, q*1e-12, sol*1e-12, (q-stm_get_qsat(qsat,t+dtin))*1e-12, stm_get_qsat(qsat, t+dtin)*1e-12 );
  }

  fclose( file );
}

double stm_modscm_dual_calts (double    imax,
                              double    an,
                              double    bn,
                              double    U,
                              double    seuil,
                              double    c,
                              double    F,
                              double    dt,
                              double    vt,
                              double    vddin,
                              double    ci,
                              double   *cie,
                              double   *vcap,
                              double    slew,
                              stm_pwth *pwth,
                              int       nth,
                              double    qinit0,
                              double    strans,
                              double    icf0,
                              stm_qsat *qsat
                             )
{
  double     ts ;
  int        i ;
  double     qcc, qprev, qinit, q, qsate ;
  double     vsmax ;
  double     vin ;
  int        k ;
  double     capai ;
  static int fordebug=0 ;
  double     qcapai ;
  double     vin0 ;
  double     t, qconf ;

  fordebug++;
  
  if( !nth )
    ts = stm_modscm_dual_calts_final( imax, an, bn, U, seuil, c, F, 
                                      vt, vddin, ci, slew, dt, qinit0, strans, icf0, qsat );
  else {
  
      q      = 0.0 ;
      qinit  = 0.0 ;
      qcapai = 0.0 ;
      capai  = ci ;
      k      = 0 ;
      vin0   = 0.0 ;
      qconf  = 0.0 ;
  
      for( i=0 ; i<nth-1 ; i++) {

        q = q + stm_get_q( imax, pwth[i].T, pwth[i].F, bn , U)
              - ( i>0 ? stm_get_q( imax, pwth[i-1].T+pwth[i-1].DT-pwth[i].DT, pwth[i].F, bn , U) : 0.0 );

        vin = vt + vddin * tanh( (pwth[i].T+pwth[i].DT) / slew );

        if( cie && vcap ) {
          while( k<3 && vin > vcap[k] ) {
            k++ ;
            qcapai = qcapai + vcap[k-1]*(cie[k-1]-cie[k])/1000.0;
          }
          capai  = cie[k];
        }
        
        if( strans>0.0 && icf0>0.0 ) {
          t = pwth[i].T+pwth[i].DT ;
          if( t>strans )
            t=strans ;
          qconf = -icf0*(t-t*t/(2.0*t ));
        }
        qcc = (capai/1000.0) * vin ;
        qsate = stm_get_qsat( qsat, pwth[i].T+pwth[i].DT );

        vsmax = (q-qconf-qcc-qinit0-qsate-qcapai)/(c/1000.0);

        if( vsmax > seuil )
          break ;

        qinit  = q ;
        vin0   = vin ;
      }

      if( i>0 )
        qprev = qinit0 + stm_get_q( imax, pwth[i-1].T+pwth[i-1].DT-pwth[i].DT, pwth[i].F, bn, U ) + qcapai - qinit;
      else
        qprev = qinit0 + qinit + qcapai ;
      
      ts = stm_modscm_dual_calts_final( imax, 
                                        an, 
                                        bn, 
                                        U, 
                                        seuil, 
                                        c, 
                                        pwth[i].F,
                                        vt, 
                                        vddin, 
                                        capai, 
                                        slew,
                                        pwth[i].DT,
                                        qprev,
                                        strans,
                                        icf0,
                                        qsat
                                      ) ;

      ts = ts + pwth[i].DT ;
  }
  return ts ;  
}

double stm_modscm_dual_calc_i( double imax,
                               double isat,
                               double a,
                               double b,
                               double rnt,
                               double rns,
                               double rni,
                               double vint,
                               double vdd,
                               double vt,
                               double vgs,
                               double vds
                             )
{
  double rlin ;
  double rint ;
  double rsat ;
  double vsat ;
  double vlin ;
  double i ;

  i = isat ;
  if( rnt > 0.0 && rns > 0.0 ) {
    stm_modscm_dual_calc_rsat( imax, isat, a, b, rnt, rns, rni, vint, vdd, vt, vgs, &rsat, &rlin, &rint, &vsat, &vlin );

    if( rsat > 0.0 ) {
      if( rint > 0.0 ) {
        if( vds < vlin )
          i = vds/rlin ;
        else {
          if( vds < vsat )
            i = vlin/rlin + (vds-vlin)/rint ;
          else
            i = isat - (vdd-vds)/rsat ;
        }
      }
      else {
        if( vds > vsat ) 
          i = isat - ( vdd - vds ) / rsat ;
        else 
          i = vds / rlin ;
      }
    }
  }

  return i ;
  
}

void stm_plot_final_i( char *filename, param_final_i *p )
{
  double  isat ;
  double  ids ;
  double  save_vout ;
  FILE   *ptf ;
  double  u ;

  ptf = fopen( filename, "w" );
  if( !ptf ) {
    perror( "can't open file " );
    return ;
  }
  
  u = p->VINR - p->VT ;
  isat = p->A*u*u / ( 1.0 + p->B*u );
  save_vout = p->VOUT ;
  
  for( p->VOUT=0.0 ; p->VOUT <= p->VDDOUT ; p->VOUT = p->VOUT + p->VDDOUT/100.0 ) {
    ids = stm_modscm_dual_calts_final_i( p );
    fprintf( ptf, "%g %g\n", p->VOUT, ids );
  }
  
  fclose( ptf );
  p->VOUT = save_vout ;
}

double stm_modscm_dual_calts_final_i( param_final_i *p )
{
  double i ;
  double isat ;
  double vds ;
  double u ;
  
  u = p->VINR - p->VT ;
  isat = p->A*u*u / ( 1.0 + p->B*u );
 
  vds = p->VDDOUT - p->VOUT ;

  i = stm_modscm_dual_calc_i( p->IMAX, isat, p->A, p->B, p->RNT, p->RNS, p->RINT, p->VINT, p->VDDOUT, p->VT, p->VIN, vds );
    
  return i ;
}
                                      
int stm_modscm_dual_calts_final_numeric_find_next_seuil( int index, float *tabv, int nbpoint )
{
  float seuil ;
  int   i ;
  float vmin ;
  int   imin ;

  if( index >= 0 )
    seuil = tabv[index] ;
  else
    seuil = -1.0 ;
    
  vmin = 1e10 ;
  imin = -1 ;

  for( i=0 ; i<nbpoint ; i++ ) {
    if( tabv[i] > seuil ) {
      if( tabv[i] < vmin ) {
        vmin = tabv[i] ;
        imin = i ;
      }
    }
  }

  return imin ;
}

void stm_modscm_dual_calts_final_numeric_pi( stm_dual_param_timing *paramtiming,
                                             float rl,
                                             float cl1,
                                             float cl2,
                                             float *tabv,
                                             float *tabt,
                                             int   nbpoint
                                           )
{
  int           index ;
  double        step ;
  double        r ;
  double        c1 ;
  double        c2 ;
  double        ci ;
  double        v1 ;
  double        v2 ;
  double        vi0 ;
  double        v10 ;
  double        v20 ;
  double        t ;
  double        dt ;
  double        lt ;
  double        uin ;
  double        uinr ;
  double        ts ;
  double        k0 ;
  double        k1 ;
  double        seuil ;
  double        i ;
  param_final_i param ;
  
  index = -1 ;

  step = 100.0 ;

  c1 = paramtiming->CAPA/1000.0 + cl1/1000.0 ;
  c2 = cl2/1000.0 ;
  ci = paramtiming->CAPAI/1000.0 ;
  r  = rl ;
  
  dt = (c1+c2)*paramtiming->VDDMAX/paramtiming->IMAX/step ;

  k1 = 1.0/( c1/dt + c2/(dt+r*c2) );

  v1  = 0.0 ; // to do : inclure valeur qinit et ci
  v2  = 0.0 ; // to do : inclure valeur qinit et ci
  v10 = 0.0 ;
  v20 = 0.0 ;
  t   = 0.0 ;

  param.IMAX   = paramtiming->IMAX ;
  param.A      = paramtiming->AN ;
  param.B      = paramtiming->BN ;
  param.U      = paramtiming->U ;
  param.RNT    = paramtiming->RLIN ;
  param.RNS    = paramtiming->RSAT ;
  param.RINT   = paramtiming->RINT ;
  param.VINT   = paramtiming->VINT ;
  param.VT     = paramtiming->VT ;
  param.VDDOUT = paramtiming->VDDMAX ;
  param.VIN    = paramtiming->VT ;
  param.VINR   = paramtiming->VT ;

  do {
      index = stm_modscm_dual_calts_final_numeric_find_next_seuil( index, tabv, nbpoint );
      if( index >=0 ) {
      
        seuil = tabv[ index ] ;

        if( seuil < v1 ) 
          ts = t ;

        do {

          v10 = v1 ;
          v20 = v2 ;
          vi0 = param.VIN ;
          lt  = t ;
         
          t = t + dt ;
          uin  = paramtiming->VDDIN * tanh( t / paramtiming->FIN ) ;
          uinr = paramtiming->U * tanh( t / paramtiming->F ) ;

          param.VOUT = v1 ;
          param.VIN  = paramtiming->VT + uin ;
          param.VINR = paramtiming->VT + uinr ;

          i = stm_modscm_dual_calts_final_i( &param );

          k0 = c1*v10/dt + c2*v20/(dt+r*c2) + ci*vi0/dt ;
          v1 = k1 * ( i - ci*param.VIN/dt + k0 ) ;
          v2 = v1 - r*c2*v1/(dt+r*c2) + r*c2*v20/(dt+r*c2) ;
          
        }
        while( v1 < seuil );
        ts = ( t-lt )/( v1-v10 )*( seuil-v10 ) + lt ;
        tabt[index] = ts ;
      }
  }
  while( index > 0 );
}

void stm_modscm_dual_calts_final_numeric( stm_dual_param_timing *paramtiming,
                                          float  *tabv,
                                          float  *tabt,
                                          int     nbpoint
                                        )
{
  double lt ;
  double li ;
  double lv ;
  double t ;
  double dq ;
  double q ;
  double v ;
  double vr ;
  double vrc ;
  double lvrc ;
  double k ;
  double dv ;
  double i ;
  double dt ;
  double ts ;
  double seuil ;
  double ci ;
  double c ;
  double chalf ;
  double uin ;
  double uinr ;
  double q0 ;
  double v0 ;
  double vin0 ;
  int    iter ;
  int    index ;
  int    half_set ;
  param_final_i param ;
  double step ;

  i  = 0.0 ;
  t  = 0.0 ;
  q  = -paramtiming->QINIT ;
  half_set = 0 ;

  ci    = paramtiming->CAPAI / 1000.0 ;
  c     = paramtiming->CAPA  / 1000.0 ;
  chalf = paramtiming->CHALF / 1000.0 ;
  iter  = 0 ;

  step = 100 ;
  dt = ((c+fabs(chalf)) * paramtiming->VDDMAX / (2.0 * paramtiming->IMAX) )/ step ;
  
  param.IMAX   = paramtiming->IMAX ;
  param.A      = paramtiming->AN ;
  param.B      = paramtiming->BN ;
  param.U      = paramtiming->U ;
  param.RNT    = paramtiming->RLIN ;
  param.RNS    = paramtiming->RSAT ;
  param.RINT   = paramtiming->RINT ;
  param.VINT   = paramtiming->VINT ;
  param.VT     = paramtiming->VT ;
  param.VDDOUT = paramtiming->VDDMAX ;
 
  index = -1 ;
 
  v    = ( q - ci * param.VT )/c ;  
  lv   = v ;
  li   = i ;
  lt   = t ;
  lvrc = 0.0 ;
  vrc  = 0.0 ;

  do {

    index = stm_modscm_dual_calts_final_numeric_find_next_seuil( index, tabv, nbpoint );

    if( index >= 0 ) {

      seuil = tabv[ index ] ;

      if( seuil < v ) {
        ts = t ;
      }
      else {
      
        if( v >= paramtiming->VDDMAX/2.0 && !half_set ) {
          half_set = 1 ;
          vin0 = param.VIN ;
          q0   = q ;
          v0   = v ;
        }
        
        do {
        
          iter++ ;

          lv   = v ;
          li   = i ;
          lt   = t ;
          lvrc = vrc ;
          t = t + dt ;

          uin  = paramtiming->VDDIN * tanh( t / paramtiming->FIN ) ;
          uinr = paramtiming->U * tanh( t / paramtiming->F ) ;

          param.VOUT = v ;
          param.VIN  = paramtiming->VT + uin ;
          param.VINR = paramtiming->VT + uinr ;

          i = stm_modscm_dual_calts_final_i( &param );

          if( paramtiming->RBR > 0.0 && paramtiming->CBR > 0.0 && t > lt ) {
          
            vr  = i*paramtiming->RBR ;
            k   = 1.0/( 1.0/paramtiming->RBR + (paramtiming->CBR/1000.0)/(t-lt) );
            vrc = k*( i + lvrc * (paramtiming->CBR/1000.0)/(t-lt) );
            dv = vr-vrc ;
            param.VIN  = paramtiming->VT + uin  + dv ;
            param.VINR = paramtiming->VT + uinr + dv * uinr/uin ;
            i = stm_modscm_dual_calts_final_i( &param );
          }

          dq = (i+li)*(t-lt)/2.0 ;
          q = q + dq ;

          if( v < paramtiming->VDDMAX/2.0 ) {
            half_set = 1 ;
            v = ( q - ci * param.VIN )/c ;  
            q0   = q ;
            v0   = v ;
            vin0 = param.VIN ;
          }
          else {
            v = v0 + ( (q-q0) - ci*(param.VIN-vin0) )/(c-chalf);
          }
          
        } 
        while( v < seuil && iter < 100*step );

        ts = ( t-lt )/( v-lv )*( seuil-lv ) + lt ;
      }
      tabt[index] = ts ;
    }
  }
  while( index >= 0 );
}

/* valide la solution t pour q(t)-qsat(t) */

int stm_modscm_dual_calts_valid( double    imax,
                                 double    bn,
                                 double    U,
                                 double    F,
                                 double    dtin,
                                 stm_qsat *qsat,
                                 double    t,
                                 char      checksol
                               )
{
  double bu ;
  double th ;
  double dq ;
  double dqsat ;

  if( !V_BOOL_TAB[ __STM_CHECK_SOLUTION ].VALUE || checksol==NO ) 
    return 1 ;

  bu = bn*U ;
  th = tanh(t/F);
  
  dq    = imax*(1.0+bu)*th*th/(1.0+bu*th);
  dqsat = stm_get_qsat_derivative( qsat, t );

  if( dq-dqsat <= 0.0 )
    return 0 ;

  dtin=0.0;
  return 1 ;
}

double stm_modscm_dual_calts_final (double    imax,
                                    double    an,
                                    double    bn,
                                    double    U,
                                    double    seuil,
                                    double    c,
                                    double    F,
                                    double    vt,
                                    double    vddin,
                                    double    ci,
                                    double    slew,
                                    double    dtin,
                                    double    qinit,
                                    double    strans,
                                    double    icf0,
                                    stm_qsat *qsat
                                   )
{
  double t ;
  double tamjad ;
  int status1 ;
  int status2 ;
  int status3 ;
  double backup_tsatmax ;
  static int fordebug = 0 ;

  fordebug++ ;
 
  if( V_BOOL_TAB[ __STM_CHECK_SOLUTION ].VALUE && qsat ) {
    backup_tsatmax = qsat->TSATMAX ;
    qsat->TSATMAX  = FLT_MAX ;
  }

  status1 = stm_modscm_dual_calts_amjad( imax, bn, U, seuil, c, F, vt, vddin, ci, slew, dtin, qinit, qsat, strans, icf0, &t );
  tamjad = t ;
  if( status1 == CALTS_AMJAD_OK ) {
    if( ! stm_modscm_dual_calts_valid( imax, bn, U, F, dtin, qsat, t, YES ) ) {
      status1 = CALTS_AMJAD_NC ;
    }
  }
  
  if( status1 == CALTS_AMJAD_NC ) {
  
    if( t < 0.0 ) 
      t = (c/1000.0*seuil)/imax ;
    status2 = stm_modscm_dual_calts_dichotomie( imax, bn, U, seuil, c, F, vt, vddin, ci, slew, dtin, qinit, qsat, strans, icf0, &t, YES ) ;

    switch( status2 ) {
    
    case CALTS_DICHO_NOSOL :
    
      if( V_BOOL_TAB[ __STM_CHECK_SOLUTION ].VALUE && qsat ) {
        avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_final(%d) : no solution found -> fix tsatmax\n", fordebug );
        qsat->TSATMAX = t ;
        status3 = stm_modscm_dual_calts_dichotomie( imax, bn, U, seuil, c, F, vt, vddin, ci, slew, dtin, qinit, qsat, strans, icf0, &t, NO ) ;
        if( status3 != CALTS_DICHO_OK ) {
          avt_log( LOGDEBUG, 1, "  -> failed !\n" );
          t = tamjad ;
        }
      }
      else {
        avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_final(%d) : no solution found -> very strange case !!!\n", fordebug );
      }
      break ;
      
    case CALTS_DICHO_NC :
    
      if( V_BOOL_TAB[ __STM_CHECK_SOLUTION ].VALUE && qsat ) {
        avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_final(%d) : no convergence -> fix tsatmax\n", fordebug );
        qsat->TSATMAX = t ;
        status3 = stm_modscm_dual_calts_dichotomie( imax, bn, U, seuil, c, F, vt, vddin, ci, slew, dtin, qinit, qsat, strans, icf0, &t, NO ) ;
        if( status3 != CALTS_DICHO_OK ) {
          avt_log( LOGDEBUG, 1, "  -> failed !\n" );
          t = tamjad ;
        }
      }
      else {
        avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_final(%g) : no convergence -> very strange case !!!\n", fordebug );
      }
    }
  }
  
  if( V_BOOL_TAB[ __STM_CHECK_SOLUTION ].VALUE && qsat ) {
    qsat->TSATMAX = backup_tsatmax ;
  }

  an=0.0 ;
  /* if status2 == CALTS_DICHO_KO, revenir à modèle ceqrsat */
  return t ; 
}

int    stm_modscm_dual_calts_amjad (double    imax,
                                    double    bn,
                                    double    U,
                                    double    seuil,
                                    double    c,
                                    double    F,
                                    double    vt,
                                    double    vddin,
                                    double    ci,
                                    double    slew,
                                    double    dtin,
                                    double    qinit,
                                    stm_qsat *qsat,
                                    double    strans,
                                    double    icf0,
                                    double   *t 
                                   )
{
    double bn1, 
           sf,
           b   = 0.0,
           z   = 0.0,
           tt  = 0.0,
           ttt = 0.0,
           c1  = 0.0 ;
    int    i = 0 ;
    int    ret ;
    double ltt;
    double tinit;
    double tin ;
    

    /* calcul de la tension rapportée à partir de :
    
              an.U²
       imax = ------
              1+bn.U
    */
    
    if((U <= 0.0)||(F <= 0.0))
        return 0.0;
        
    bn1 = 1 / (U * bn);
    c1 = c;

    /* Dans ce qui suit, on a un changement de variable t = t/F 

    Résolution de :
    
          (         ln(2).b²u²-ln(1+b.u) )
     imax.( f.t - f.-------------------- )
          (              b²u²-b.u        )

             (    (     1-b.u          )                                )
             (  ln( 1 + -----.exp(-2t) )                                )
             (    (     1+b.u          )   (      1  )                  )
    + f.imax.(  ------------------------ + ( 1 + --- ).ln( 1+exp(-2t) ) )
             (        b²u² - b.u           (     b.u )                  )

    = c.seuil + qsat2*t*t + ci * ( vt + vddin.th(t) ) + qinit + icf0*(t-t*t/(2*strans))


    Solution initiale : on néglige la seconde ligne de l'expression

             ln(2).b²u²-ln(1+b.u)     c.seuil   ci.vt    qinit
         t - --------------------   = ------- + ------ + ------
                  b²u²-b.u            f.imax    f.imax   f.imax
                  
                     |                   |

                     sf                  tt
                     
    */
    z = (seuil * c1/1000.0 + vt * ci/1000.0 + qinit ) / imax ; 
    tt = z/F ;
    sf = ((1.0 + bn1) * LOG_2) - (bn1 * bn1 * (log (0.5 * ((bn1 + 1.0) / bn1))) / (1.0 - bn1));
    tt += sf ;

    if((TESTIM > 0.0) &&  (TESTIM > dtin))
      tt = (TESTIM-dtin)/F ;
    TESTIM = -1.0 ;

    tinit = tt ;

    /* Puis on trouve par convergence :
                                            (     1-b.u          )
                                          ln( 1 + -----.exp(-2t) )
         c.seuil   ln(2).b²u²-ln(1+b.u)     (     1+b.u          )
    t =  ------- + -------------------- - ------------------------
         f.imax         b²u²-b.u                b²u² - b.u        

           (      1  )
         - ( 1 + --- ).ln( 1+exp(-2t) )
           (     b.u )
          
    */
   
    /* pas sur que le critère de Lipschitz soit bon suite à l'introduction de qsat2 */
    do {
        i++;
        ltt = tt;
        b = exp (- 2.0 * tt);
        // ttt correspond aux trois termes avec les ln et le ci.
        ttt = (1.0 + bn1) * log (2.0 / (1.0 + b))
            - ((bn1 * bn1) / (1.0 - bn1)) * log (1.0 + ((1.0 - bn1) * (1.0 - b) / (2.0 * bn1))) 
            + ci/1000.0 * vddin * tanh((tt*F+dtin)/slew)/F/imax + stm_get_qsat(qsat,tt*F)/F/imax ;
        if( strans>0.0 ) {
          tin = tt*F+dtin;
          if( tin > strans )
            tin = strans ;
          ttt = ttt - icf0*(tin-tin*tin/(2.0*strans))/F/imax ;
        }
        tt = z/F + ttt;
    }
    while( (fabs((F*ltt-F*tt)/(F*tt+dtin)) > STM_MCC_EPSILON) && tt < 10.0*tinit && (i < 20) );

    /* On défait le changement de variable */
    *t = tt * F;

    if( i>=20 || tt >= 10.0*tinit )
      ret = CALTS_AMJAD_NC ;
    else
      ret = CALTS_AMJAD_OK ;

    return ret ;
}

inline double stm_modscm_dual_calts_dicho_q( double imax, 
                                             double t, 
                                             double f, 
                                             double bu, 
                                             double bu2, 
                                             double c, 
                                             double seuil, 
                                             double ci, 
                                             double vt, 
                                             double vddin, 
                                             double dtin, 
                                             double fin, 
                                             double qinit,
                                             double strans,
                                             double icf0,
                                             stm_qsat *qsat 
                                           )
{
  double q ;
  double tin ;

  if (bu==1)
     q = imax * (t - f * (LOG_2))
         + f * imax * (2 * log(1.0 + exp(-2.0 * t / f)));
  else
     q = imax * (t - f * (LOG_2 * bu2 - log(1.0 + bu))/(bu2 - bu))
      + f * imax * (1.0/(bu2 - bu) * log(1.0 + (1.0 - bu)/(1.0 + bu) * exp(-2.0 * t / f))
      + (1.0 + 1.0/bu) * log(1.0 + exp(-2.0 * t / f)));
  
  q += - c * seuil / 1000.0 - ci/1000.0 * ( vt + vddin * tanh((t+dtin)/fin)) - qinit - stm_get_qsat(qsat,t);

  if( strans > 0.0 ) {
    tin = t ;
    if( tin>strans )
      tin = strans ;
    q = q + icf0*(tin-tin*tin/(2.0*strans));
  }
      
  return q ;
}

/* une solution estimée doit être donnée dans *t */
int    stm_modscm_dual_calts_dichotomie( double    imax,
                                         double    bn,
                                         double    U,
                                         double    seuil,
                                         double    c,
                                         double    F,
                                         double    vt,
                                         double    vddin,
                                         double    ci,
                                         double    fin,
                                         double    dtin,
                                         double    qinit,
                                         stm_qsat *qsat,
                                         double    strans,
                                         double    icf0,
                                         double   *t,
                                         char      checksol
                                       )
{
    double ti ;
    double tmin ;
    double tmax ;
    double Qmin ;
    double Qmax ;
    double BU ;
    double BU2 ;
    int    ret = CALTS_DICHO_OK ;
    int    iter ;
    double tvmin ;
    double tvmax ;
    double tm ;
    double qm ;
    int    validmin ;
    int    validmax ;
    int    validm ;

    ti = *t;
    BU   = bn * U,
    BU2  = BU * BU ;

    /* Etat initial : trouver qmin<0 valide et qmax>0 qui peut etre non valide */
    
    tmax = ti ;
    validmax = stm_modscm_dual_calts_valid( imax, bn, U, F, dtin, qsat, tmax, checksol ) ;
    if( validmax )
      Qmax = stm_modscm_dual_calts_dicho_q( imax, tmax, F, BU, BU2, c, seuil, ci, vt, vddin, dtin, fin, qinit, strans, icf0, qsat ) ;
    else
      Qmax = 0.0 ;

    if( validmax ) {
    
      if( Qmax < 0.0 ) {

        iter = 100;
        do {
        
          if( Qmax < 0 && validmax ) {
            tmin = tmax ;
            Qmin = Qmax ;
          }
          
          tmax  = 1.1 * tmax ;
          Qmax  = stm_modscm_dual_calts_dicho_q( imax, tmax, F, BU, BU2, c, seuil, ci, vt, vddin, dtin, fin, qinit, strans, icf0, qsat );
          validmax = stm_modscm_dual_calts_valid( imax, bn, U, F, dtin, qsat, tmax, checksol ) ;

          iter-- ;
        }
        while( Qmax < 0.0 && validmax && iter );

        if( !iter ) {
          avt_log( LOGDEBUG, 1, "stm_modscm_dual_calts_dichotomie() : no convergence\n" );
          ret = CALTS_DICHO_NOSOL ;
        }
      }
      else {
      
        tmin = 0.0 ;
        Qmin = stm_modscm_dual_calts_dicho_q( imax, tmin, F, BU, BU2, c, seuil, ci, vt, vddin, dtin, fin, qinit, strans, icf0, qsat );
        
      }
    }
    else {
    
      tmin = 0.0 ;
      Qmin = stm_modscm_dual_calts_dicho_q( imax, tmin, F, BU, BU2, c, seuil, ci, vt, vddin, dtin, fin, qinit, strans, icf0, qsat );
    
    }
   
    /* converge pour trouver qmax positif et valide */

    if( ret == CALTS_DICHO_OK && !validmax ) {
      tvmin    = tmin ;
      validmin = 1 ;
      tvmax    = tmax ;

      iter = 100 ;
      
      do {
      
        tm = ( tvmin + tvmax ) / 2.0 ;
        validm = stm_modscm_dual_calts_valid( imax, bn, U, F, dtin, qsat, tm, checksol ) ; 
        if( validm ) 
          qm = stm_modscm_dual_calts_dicho_q( imax, tm, F, BU, BU2, c, seuil, ci, vt, vddin, dtin, fin, qinit, strans, icf0, qsat );
        else
          qm = 0.0 ;
        
        if( validm && qm < 0.0 ) {
          tvmin = tm ;
          validmin = validm ;
        }
        else {
          tvmax = tm ;
          validmax = validm ;
        }

        iter-- ;
      }
      while( !(validmin && validmax) && iter && (tvmax-tvmin)>0.1 );
     
      tmax = tm ;
      Qmax = qm ;
      *t = tm ;

      if( !validmin || !validmax )
        ret = CALTS_DICHO_NOSOL ;
    }

    /* dichotomie classique d'une fonction croissante à une seule racine, Qmin et Qmax sont valides */

    if( ret == CALTS_DICHO_OK ) {

      iter = 100 ;
      
      do {
      
        tm = ( tmin + tmax )/2.0 ;
        qm = stm_modscm_dual_calts_dicho_q( imax, tm, F, BU, BU2, c, seuil, ci, vt, vddin, dtin, fin, qinit, strans, icf0, qsat );
        
        if( qm > 0.0 ) 
          tmax = tm ;
        else
          tmin = tm ;
      
        iter-- ;
      }
      while((tmax - tmin) > (1000.0 * STM_MCC_EPSILON) && iter );
  
      if( !iter )
        ret = CALTS_DICHO_NC ;
        
      *t = ( tmin + tmax )/2.0 ;
    }

    return ret ;
}

/****************************************************************************/

/* 
  Calcule rsat d'un transistor lorsqu'on a un VGS différent de VDD.
  Voir les explications dans mon classeur, chapitre rsat.
*/

void stm_modscm_dual_calc_rsat( double imax,
                                double isat,
                                double a,
                                double b,
                                double rnt,
                                double rns,
                                double rni,
                                double vint,
                                double vdd,
                                double vt,
                                double vgs,
                                double *rsat_r,
                                double *rlin_r,
                                double *rint_r,
                                double *vsat_r,
                                double *vlin_r
                              )
{
  double x, kres, ures, rsat, rsatmin, k, rlin, ires ;
  double vr, ir, gsep, ire, vre ;
  double iint, rint ; 

  rsatmin = vdd/imax ;
  
  if( rsat_r ) *rsat_r = -1.0 ;
  if( rlin_r ) *rlin_r = -1.0 ;
  if( rint_r ) *rint_r = -1.0 ;
  if( vsat_r ) *vsat_r = -1.0 ;
  if( vlin_r ) *vlin_r = -1.0 ;

  if( rni < 0.0 && rnt < rsatmin && rns > rsatmin ) {

    rsatmin = vdd/isat ;

    k     = imax*rnt / (vdd-vt) ;
    rlin  = k * ( 1.0 + b * (vgs-vt) ) /  ( a * (vgs-vt) ) ;

    if( rlin > rsatmin )
      return ;

    x    = ( imax*rnt - vdd*rnt/rns ) / ( 1.0 - rnt/rns ) ;
    kres = x / ( vdd - vt ) ;
    ures = kres * ( vgs - vt ) ;
    ires = ures / rlin ;
    rsat = ( vdd - ures ) / ( isat - ires ) ;

    if( rsat < rsatmin )
      return ;

    if( rsat_r ) *rsat_r = rsat ;
    if( rlin_r ) *rlin_r = rlin ;
    if( vsat_r ) *vsat_r = ures ;
  }
  else {

    if( rni > 0.0 ) {
    
      /* calcul point 1 vgs=vdd : (vint;iint) */
      iint = vint/rnt ;

      /* calcul point 2 vgs=vdd : (vr;ir) */
      vr   = ( imax - iint + vint/rni - vdd/rns ) * ( rns * rni ) / ( rns - rni ) ;
      ir   = imax - (vdd-vr)/rns ;

      /* calcul point 2 vgs<>vdd : (vre;ire) */
      ire  = ir*(vgs-vt)/(vdd-vt) ;
      if( ire > 0.95*isat )
        ire = 0.95*isat ;
      gsep = ir/(vdd-vr) ;
      vre  = vr + (ir-ire)/gsep ;

      /* calcul rlin vgs<>vdd */
      k    = ir*rnt/(vdd-vt);
      rlin = k*(vgs-vt)/ire ;
     
      /* calcul point 1 vgs<>vdd : (ures;ires) */
      kres = vint/(vdd-vt) ;
      ures = kres*(vgs-vt) ;
      ires = ures/rlin ;

      /* calcul rsat et rint */
      rsat = (vdd-vre)/(isat-ire) ;
      rint = (vre-ures)/(ire-ures/rlin) ;

      if( vr < vint  )
        avt_log( LOGSTM, 3, "warning : vr(%g)<vint(%g)\n", vr, vint );

      if( iint > ir  ) 
        avt_log( LOGSTM, 3, "warning : iint(%g)>ir(%g)\n", iint, ir );
        
      if( vre < ures ) 
        avt_log( LOGSTM, 3, "warning : vre(%g)<ures(%g)\n", vre, ures );
        
      if( ire < ires ) 
        avt_log( LOGSTM, 3, "warning : ire(%g)<ires(%g)\n", ire, ires );
        
      if( isat < ire ) 
        avt_log( LOGSTM, 3, "warning : isat(%g)<ire(%g)\n", isat, ire );
      
      if( rsat_r ) *rsat_r = rsat ;
      if( rlin_r ) *rlin_r = rlin ;
      if( rint_r ) *rint_r = rint ;
      if( vsat_r ) *vsat_r = vre ;
      if( vlin_r ) *vlin_r = ures ;
    }
    else {
      if( rnt > rsatmin && rns < rsatmin ) {
        vr   = (imax-vdd/rns)*(rns*rnt)/(rns-rnt) ;
        ir   = vr/rnt ;
        gsep = ir/(vdd-vr) ;
        ire  = ir*(vgs-vt)/(vdd-vt) ;
        vre  = vr + (ir-ire)/gsep ;
        rsat = (vdd-vre)/(isat-ire) ;
        rlin = vre/ire ;
        if( rsat_r ) *rsat_r = rsat ;
        if( rlin_r ) *rlin_r = rlin ;
        if( vsat_r ) *vsat_r = vre ;
      }
    }
  }
}

/****************************************************************************/

/* 
  Calcule le vgs du transistor lorsque la sortie commute.
  Pour l'instant, ne prend pas en compte le rbr/cbr.
*/

double stm_modscm_dual_calc_vin( double vddin,
                                 double vt,
                                 double slew,
                                 double ts
                               )
{
  double vgs ;
 
  vgs = vddin*tanh( ts/slew ) + vt ;

  if( vgs <=0.0 || vgs >= vddin+vt )
    EXIT(33);
  return vgs ;
}

/****************************************************************************/

double stm_modscm_dual_ceqrsat_numeric( stm_dual_param_timing *param, double seuil )
{
  double vt ;
  double vdd ;
  double f ;
  double fin ;
  double vddin ;
  double a ;
  double b ;
  double u ;
  double imax ;
  double capa ;
  double dt ;
  double rlin ;
  double rsat ;
  double qinit ;
  double capai ;
  double isat ;

  double q ;
  double t ;
  double vin ;
  double vout ;
  double qsat ;
  double ceqrsat ;
  double rs ;
  
  vt    = param->VT ;
  vddin = param->VDDIN ;
  fin   = param->FIN ;
  qinit = param->QINIT ;
  f     = param->F ;
  a     = param->AN ;
  b     = param->BN ;
  u     = param->U ;
  imax  = param->IMAX ;
  vdd   = param->VDDMAX ;
  rlin  = param->RLIN ;
  rsat  = param->RSAT ;
  capai = param->CAPAI / 1000.0 ;
  capa  = param->CAPA / 1000.0 ;
  dt    = param->T0 / 100.0 ;

  qsat = 0.0 ;

  t=dt ;

  do { 
    vin  = vt + vddin * tanh( t/fin ) ;
    q    = stm_get_q( imax, t, f, b, u ) ;
    vout = ( q - capai*vin - qinit )/capa ;
    isat = a*(vin-vt)*(vin-vt)/(1.0+b*(vin-vt));
    stm_modscm_dual_calc_rsat( imax, isat, a, b, rlin, rsat, -1.0, -1.0, vdd, vt, vin, &rs, NULL, NULL, NULL, NULL ) ;

    if( rsat < 0.0 )
      return 0.0 ;

    qsat = qsat + vout/rs ;
    t    = t + dt ;
  }
  while( vout < seuil );

  ceqrsat = qsat*dt/seuil*1000.0 ;

  return ceqrsat ;
}

/****************************************************************************/

/* 
  Calcule une capacité equivalente à la charge perdue dans rsat lors d'une 
  transition. La transition est modélisée par une tangente hyperbolique.
  
  vt, vmax, f : paramètres de la tangente hyperbolique de sortie.
  rsat :        valeur de la résistance.
  seuil :       tension où l'on réalise la mesure
  threshold :   seuil logique de la porte : vdd/2
  te :          instant de commutation de l'entrée minimum à partir duquel
                on compte rsat
  ts :          instant de commutation de la sortie à threshold
  fout :        estimation du front de sortie.

  retour :      capacité equivalente.

  ATTENTION : cette fonction ne doit pas s'utiliser si on a un modèle de
  switch utilisant RINT.
*/
  
double stm_modscm_dual_ceqrsat( double imax,
                                double vt,    // output slope
                                double vmax,
                                double f,
                                double vddin, // input slope normalized
                                double fin,
                                double rsat,
                                double rlin,
                                double an,
                                double bn,
                                double seuil,
                                double threshold,
                                double te,
                                double ts)
{
  double ta,     // Instant de début de la commutation.
         tb,     // Instant de fin de la commutation.
         qsat,   // Charge equivalente.
         ceq,    // Capacité equivalente.
         dt,     // Ecart entre les deux référentiels
         tee,    // Instant de commutation de la sortie
         tst;
  int    i, n ;
  double vea, veb ;
  double vgsa, vgsb, dve ;
  double rsatr ;
  double tsa, tsb ;
  static int fordebug = 0 ;
  double isat, vgs ;

  fordebug++ ;

  if( seuil > 0.97 * vmax )
    seuil = 0.97 * vmax ;

  /*
  modèle de front :
    t>=0 : v=vt+(vdd-vt)*tanh(t/f)
    t<0  : v=(vdd-vt)*t/f+vt
  */

  ta  = -f * vt / (vmax-vt);

  if( seuil > vt ) 
    tb  = f * atanh( (seuil-vt)/(vmax-vt) ) ;
  else
    tb = f * (seuil-vt)/(vmax-vt) ;

  if( threshold > vt )
    tst = f * atanh( (threshold-vt)/(vmax-vt) ) ;
  else 
    tst = f * (threshold-vt)/(vmax-vt) ;

  // On vérifie si on est bien après l'entrée.
  dt  = ts-tst;   // Ecart entre le référentiel de l'entrée et de la sortie
  tee = dt+ta;    // Début de commutation de la sortie dans le référentiel de l'entrée
  if( tee < te ) {
    ta = te-dt;   // Début de commutation de la sortie
  }
 
  if( ta < tb ) {
  
    n=3;
    
    vea = vddin - vddin*tanh( (ta+dt)/fin ) ;
    veb = vddin - vddin*tanh( (tb+dt)/fin ) ;
    dve = (veb-vea)/n ;

    qsat = 0.0 ;

    for( i=0; i < n ; i++ ) {
    
      vgsa  = vea + dve*((float)i) ;
      
      if( vgsa < 0.01*vmax ) {
      
        /* lorsque vgsb devient très faible, le calcul de tsb est impossible (atanh(1)=inf).
           On considère dans ce cas qu'on est sur la dernière borne qui va jusqu'à tb */
        i    = n ;
        vgsa = 0.0 ;
        vgsb = 0.0 ;
        tsa  = ta ;
        tsb  = tb ;
        
      }
      else {
      
        tsa  = fin * atanh( (vddin-vgsa)/vddin ) - dt ;
        
        vgsb  = vgsa + dve ;
      
        if( vgsb < 0.01*vmax ) {
          /* lorsque vgsb devient très faible, le calcul de tsb est impossible (atanh(1)=inf).
             On considère dans ce cas qu'on est sur la dernière borne qui va jusqu'à tb */
          i = n ;
          vgsb = 0.0 ;
          tsb   = tb ;
        }
        else
          tsb  = fin * atanh( (vddin-vgsb)/vddin ) - dt ;
      }
     
      vgs  = (vddin+vt)-vgsb ;
      isat = an*(vgs-vt)*(vgs-vt)/(1.0+bn*(vgs-vt));
      stm_modscm_dual_calc_rsat( imax, isat, an, bn, rlin, rsat, -1.0, -1.0, vmax, vt, vgs, &rsatr, NULL, NULL , NULL, NULL );

      if( rsatr < 0.0 ) {
        qsat = 0.0 ;
        break ;
      }

      qsat = qsat + ( f*(vmax-vt) * log(cosh(tsb/f)/cosh(tsa/f)) + vt*(tsb-tsa) ) / rsatr ;
    }    
    
    ceq  = qsat / seuil * 1000.0 ;
  }
  else
    ceq = 0.0;

  return ceq;
}

/****************************************************************************/

double stm_modscm_dual_calts_rsat_full_range (double capai,
                                              double imax,
                                              double an,
                                              double bn,
                                              double U,
                                              double VT,
                                              double vddmax,
                                              double threshold,
                                              double seuil,
                                              double rsat,
                                              double rlin,
                                              double c,
                                              double fin,
                                              double F,
                                              double te,
                                              double t0,
                                              double vddin,
                                              stm_pwl *pwl
                                             )
{
  double    vsat ;
  double    fout ;
  double    capa ;
  double    ceqrsat ;
  double    tf ;
  double    tsat ;
  double    rsatmin ;
  stm_pwth  pwth[10] ;
  int       npwth;
  stm_pwth *ptpwth ;
 
  ptpwth = NULL ;
  npwth  = 0 ;
  if( pwl ) {
    if( stm_pwl_to_tanh( pwl, VT, vddin+VT, pwth, &npwth, fin ) )
      ptpwth = pwth ;
  }
  
  fout    = stm_modscm_dual_calslew( imax, an, bn, U, VT, vddmax, c, fin, F, t0);
  
  ceqrsat = stm_modscm_dual_ceqrsat( imax, VT, vddmax, fout, U, F, rsat, rlin, an, bn, seuil, threshold, te, t0 );
  capa    = c + ceqrsat ;

  rsatmin = vddmax/imax ;

  if( rsat > rsatmin && rsatmin > rlin ) {
    vsat  = vddmax - ( imax - vddmax / rsat ) * rlin * rsat / ( rsat - rlin ) ;
    if( seuil < vsat ) 
      tf = stm_modscm_dual_calts( imax, an, bn, U, seuil, capa, F, 0.0, VT, vddin, capai, NULL, NULL, fin, ptpwth, npwth, 0.0, 0.0, 0.0, NULL );
    else {
      tsat = stm_modscm_dual_calts( imax, an, bn, U, vsat, capa, F, 0.0, VT, vddin, capai, NULL, NULL, fin, ptpwth, npwth, 0.0, 0.0, 0.0, NULL );
      /* bonne relation, mise en commentaire en attendant que soit résulu le pb de modélisation des slopes
         qui rend certains bench d'ifx defectueux 
      tf = tsat - rlin * c/1000.0 * log( 1.0 - ( seuil - vsat ) / ( vddmax - vsat ) );
      */
      tf = tsat - rlin * (capa+capai)/1000.0 * log( 1.0 - ( seuil - vsat ) / ( vddmax - vsat ) );
    }
  }
  else
    tf = stm_modscm_dual_calts( imax, an, bn, U, seuil, capa, F, 0.0, VT, vddin, capai, NULL, NULL, fin, ptpwth, npwth, 0.0, 0.0, 0.0, NULL );
    
  return tf ;  
}

/****************************************************************************/

double stm_modscm_dual_calslew (double imax,
                                double an,
                                double bn,
                                double U,
                                double VT,
                                double vddmax,
                                double c,
                                double fin,
                                double F,
                                double ts)
{
    double pente,
           fout;

    if(c < 0.001)
        return fin;

    /* pente en vdd/2 et front de sortie */
    pente = imax * ((1 + bn * U) * pow (tanh (ts / F), 2)) / (c * (1 + bn * U * tanh (ts / F)));

    if( VT < vddmax/2.0 )
      fout = ((3 * vddmax - 4 * VT) * vddmax) / (4 * pente * (vddmax - VT)) / 1000;
    else
      fout = (vddmax-VT)/pente/1000.0 ;

    an=0.0;
    return fout;
}

/****************************************************************************/

long stm_modscm_dual_calslope (double imax,
                               double an,
                               double bn,
                               double vddin,
                               double VT,
                               double seuil,
                               double c,
                               double fin)
{
    double an1, 
           bn1, 
           rc,
           sf,
           b   = 0.0,
           z   = 0.0,
           tt  = 0.0,
           ttt = 0.0,
           k   = 0.0,
           c1  = 0.0,
           U,
           F,
           slope;
    long   i = 0;
    float  ltt;

    /* calcul de la tension rapportée */
    U = (imax * bn + sqrt (pow ((imax * bn), 2) + 4 * an * imax)) / (2 * an);

    /* calcul du front rapporté */
    F = fin * U / vddin;

    an1 = an / bn;
    bn1 = 1 / (U * bn);
    c1 = c;
    
    /* calcul des deux premiers termes de l'équation de charge Qn(t) */
    z = ((seuil / 1000) * c1) / (F * an1 * U); 
    tt = (1.0 + bn1) * z;
    rc = tt ;
    sf = ((1.0 + bn1) * LOG_2) - (bn1 * bn1 * (log (0.5 * ((bn1 + 1.0) / bn1))) / (1.0 - bn1));
    tt += sf ;

    /* convergence du troisieme terme */
    do
    {
        i++;
        ltt = tt;
        b = exp (-2.0 * tt);
        ttt = (1.0 + bn1) * log (2.0 / (1.0 + b))
            - ((bn1 * bn1) / (1.0 - bn1)) * log (1.0 + ((1.0 - bn1) * (1.0 - b) / (2.0 * bn1)));
        tt = (1.0 + bn1) * z + ttt;
    }
    while( (fabs((ltt-tt)/tt) > STM_MCC_EPSILON) && (i < 100) );

    k = (seuil - VT) / vddin;
    sf = (tt / (rc + sf)) * sf ; 
    sf -= 0.5 * log ((1.0 + k) / (1.0 - k)) ;
    slope = (long)(1000.0 * sf) ;

    return slope;
}

/****************************************************************************/

float stm_modscm_dual_capaeq ( dualparams *dual, stm_dual_param_timing *paramtiming, float slew, float r, float c1, float c2, float vth, char *signame )
{
  stm_param_isat param ;
  float          cconf ;
  float          load ;
  stm_qsat       qsat ;
  float          localthreshold ;
  float          capaadd ;
  double         ceqrsat ;

  cconf = stm_modscm_dual_cconf ( dual, slew );
  ceqrsat = 0.0 ;
  
  if( paramtiming && V_BOOL_TAB[ __AVT_CEQRSAT_FOR_PILOAD ].VALUE ) {
    if( vth >= paramtiming->VSAT )
      localthreshold = paramtiming->VSAT ;
    else
      localthreshold = vth ;
    stm_modscm_dual_calts_qsat( paramtiming, localthreshold, NULL, &qsat, &ceqrsat, NULL );
  }
  
  if( paramtiming && V_BOOL_TAB[ __STM_ENHANCED_CAPAEQ_FOR_PILOAD ].VALUE ) {
    param.IMAX = paramtiming->IMAX ;
    param.A    = paramtiming->AN ;
    param.B    = paramtiming->BN ;
    param.VT   = paramtiming->VT ;
    param.U    = paramtiming->U ;
    param.F    = paramtiming->F ;
    param.NTH  = paramtiming->NTHSHRK ;
    param.PWTH = paramtiming->PWTHSHRK ;
    cconf      = 2.0*paramtiming->CAPAI + paramtiming->CAPA ;
  }
  else {

    param.IMAX = dual->DP[STM_IMAX] ;
    param.B    = dual->DP[STM_BN] ;
    param.A    = -1.0 ;
    param.VT   = -1.0 ;
    stm_modscm_dual_cal_UF( dual->DP[STM_IMAX],
                            dual->DP[STM_AN],
                            dual->DP[STM_BN],
                            dual->DP[STM_VDDIN],
                            0.0,
                            slew,
                            &(param.U),
                            &(param.F)
                          );
    param.NTH  = 0;
    param.PWTH = NULL;
  }
 
  capaadd = cconf + ceqrsat ;
  if( paramtiming && vth > dual->DP[STM_THRESHOLD] && V_BOOL_TAB[ __STM_PILOAD_SATURATION ].VALUE )
    load = stm_capaeq_fn( (char (*)(void*,double,double*)) stm_isat, (void*) &param, 10.0*slew, r, c1+capaadd, c2, vth, paramtiming->VSAT, paramtiming->RLIN, paramtiming->VDDMAX, signame );
  else
    load = stm_capaeq_fn( (char (*)(void*,double,double*)) stm_isat, (void*) &param, 10.0*slew, r, c1+capaadd, c2, vth, -1.0, -1.0, -1.0, signame );
  load = load - capaadd ;

  return load ;
}

/****************************************************************************/

char stm_isat( stm_param_isat *param, double t, double *i )
{
  double uin ;

  if( param->NTH ) {
    if( t<0.0 )
      *i=0.0 ;
    else {
      uin = stm_get_v_pwth( t, param->VT, param->U+param->VT, STM_UP, param->PWTH )-param->VT;
      *i = param->A*uin*uin/(1.0+param->B*uin);
      if( *i > param->IMAX ) {
        *i = param->IMAX ;
      }
    }
  }
  else {
    *i = stm_get_ic_imax( t, param->U, param->IMAX, param->F, param->B );
  }

  return 1;
}

/****************************************************************************/

int stmtanhfn( stmtanhdata *data, double t, double *v )
{
  if( t-data->T0>0.0 )
    *v = data->VT+(data->VDD-data->VT)*tanh( (t-data->T0)/data->F );
  else {
    *v = (data->VDD-data->VT)*(t-data->T0)/data->F+data->VT ;
  }
  if( *v<0.0 )
    *v = 0.0 ;

  return 1 ;
}

void stm_plot_tanh( char *name, float f, float vt, float vdd, float t0, float vth, char sens )
{
  float te ;
  float dt ;
  float t ;
  float vu ;
  float v ;
  FILE *ptf ;

  ptf = mbkfopen( name, NULL, "w" );
  if( !ptf ) {
    printf( "can't open file for writing\n" );
    return ;
  }
  
  if( vt > vth ) 
    te = f * (vth-vt)/(vdd-vt);
  else
    te = f * atanh((vth-vt)/(vdd-vt));

  dt = t0-te ;

  t = -f*vt/(vdd-vt);
  vu = 0.0 ;
  if( sens=='U' || sens=='u' || sens=='R' || sens=='r' )
    v = vu ;
  else 
    v = vdd-vu ;
  fprintf( ptf, "%g %g\n", (t+dt)*1e-12, v );
  
  for( t=0.0 ; t<=3*f ; t++ ) {
    vu = vt+(vdd-vt)*tanh(t/f);
    if( sens=='U' || sens=='u' || sens=='R' || sens=='r' )
      v = vu ;
    else 
      v = vdd-vu ;
    fprintf( ptf, "%g %g\n", (t+dt)*1e-12, v );
  }

  fclose(ptf);
}
