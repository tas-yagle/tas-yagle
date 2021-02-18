/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_pwl.c                                                  */
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

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/* Transforme une structure stm_pwl en un ensemble de tangentes hyperbolique
   le tableau pwth doit contenir (n-1) éléments.
   Renvoie 1 si OK, 0 si un pb est survenu.
   ntanh contient au retour le nb d'éléments dans pwth.
*/

int stm_pwl_to_tanh( stm_pwl  *pwl, 
                     double    vt, 
                     double    vdd, 
                     stm_pwth *pwth, 
                     int      *ntanh,
                     float     oldslew
                   )
{
  int i, n ;
  double v1, v2, t1, t2 ;
  double k1, k2 ;
  double dt, f ;
  double dt0 ;
  double vm ;

  if( pwl->N < 2 ) 
    return 0;
  
  vm     = 2*vt - vdd ;
  n      = pwl->N-1 ;
  *ntanh = 0;
  
  for( i=0 ; i<n ; i++) {

    t1 = pwl->TAB[ 2*i+0 ] ;
    v1 = pwl->TAB[ 2*i+1 ] ;
    t2 = pwl->TAB[ 2*i+2 ] ;
    v2 = pwl->TAB[ 2*i+3 ] ;

    if( v1 < vm || fabs((v2-v1)/v1) < 1.0e-6 )
      continue ;

    if( v1 > 0.99*vdd || v2 > 0.99*vdd ) {
      /**ntanh = 0;*/
      break;
    }

    k1 = atanh( (v1-vt)/(vdd-vt) );
    k2 = atanh( (v2-vt)/(vdd-vt) );

    dt = ( t2*k1 - t1*k2 ) / (k1-k2) ;
    f  = ( t1-t2) / ( k1-k2 );

    if( i==0 )
      dt0 = dt ;

    pwth[*ntanh].F = f ;
    pwth[*ntanh].DT = dt-dt0 ;
    pwth[*ntanh].T = t2-pwth[*ntanh].DT ;
    (*ntanh)++;
  }
 
  if( *ntanh < 1 )
    return 0 ;

  pwth[(*ntanh)-1].T = FLT_MAX ;
   
  return 1 ;
}

void stm_pwl_debug_pwl_q( double      vt,
                          double      vdd,
                          double      fin,
                          stm_pwl    *pwlin,
                          dualparams *params,
                          char       *filename,
                          double      tp 
                        )
{
  char      name[256] ;
  stm_pwth  tabthin[32] ;
  int       nthin ;
  FILE     *ptf ;
  double    imax, an, bn, u, f ;
  int       i ;
  double    q ;
  double    t ;

  if( !pwlin )
    return ;

  sprintf( name, "%s_q.dat", filename );

  if( !stm_pwl_to_tanh( pwlin, vt, vdd, tabthin, &nthin, fin ) ) {
    printf( "error in stm_pwl_to_tanh()\n" );
    return ;
  }

  ptf = fopen( name, "w" );
  if( !ptf )
    return ;

  imax = params->DP[STM_IMAX];
  an   = params->DP[STM_AN];
  bn   = params->DP[STM_BN];
  stm_modscm_dual_cal_UF( imax, an, bn, (vdd-vt), 0.0, fin, &u, &f );

  for( t=0.0 ; t<=tp ; t=t+0.1 ) {
 
    q = 0.0 ;
    
    for( i=0 ; i<nthin && tabthin[i].T+tabthin[i].DT < t ; i++) {

      q = q + stm_get_q( imax, tabthin[i].T, tabthin[i].F, bn , u)
            - ( i>0 ? stm_get_q( imax, tabthin[i-1].T+tabthin[i-1].DT-tabthin[i].DT, tabthin[i].F, bn , u) : 0.0 );
    }

    q = q + stm_get_q( imax, t - tabthin[i].DT, tabthin[i].F, bn , u)
          - ( i>0 ? stm_get_q( imax, tabthin[i-1].T+tabthin[i-1].DT-tabthin[i].DT, tabthin[i].F, bn , u) : 0.0 );

    fprintf( ptf, "%g %g %g\n", t, q, stm_get_q( imax, t, fin, bn, u ) );
  }

  fclose( ptf );
}

void stm_pwl_debug_pwl_to_tanh( stm_pwl *pwl, 
                                double fold,
                                dualparams *params,
                                char *filename, 
                                double vt, 
                                double vdd,
                                double fin,
                                stm_pwl *pwlin,
                                double load,
                                double tp,
                                char eventin,
                                char eventout
                              ) 
{
  char      filenamepwldat[256] ;
  char      filenametanhdat[256] ;
  char      filenamemccdat[256] ;
  char      filenamegpl[256] ;
  FILE     *filepwldat, 
           *filetanhdat, 
           *filemccdat, 
           *filegpl ;
  stm_pwth  tabth[32], tabthin[32];
  int       nth, nthin;
  int       i ;
  double    t, 
            tmax,
            v ;
  double    tref_th, tref_pw, tabs=100.0, dt_th=0.0, trefthout ;
  char      *env;
  stm_dual_param_timing paramtimingpwl ;
  stm_dual_param_timing paramtimingtanh ;
  float     tspwl ;
  float     tstanh ;

  env = getenv( "DEBUG_PWL_TE" );
  if( env ) {
    tabs = atof(env );
  }

  if( !pwl || !pwlin )
    return ;
  sprintf( filenamepwldat, "%s_pwl.dat", filename );
  sprintf( filenametanhdat, "%s_tanh.dat", filename );
  
  if( params )
    sprintf( filenamemccdat, "%s_mcc.dat", filename );

  sprintf( filenamegpl, "%s.gpl", filename );

  filepwldat = fopen( filenamepwldat, "w" );
  if( !filepwldat ) {
    printf( "can't open file %s for writting\n", filenamepwldat );
    return ;
  }
  
  filetanhdat = fopen( filenametanhdat, "w" );
  if( !filetanhdat ) {
    printf( "can't open file %s for writting\n", filenametanhdat );
    return ;
  }
  
  if( params ) {
    filemccdat = fopen( filenamemccdat, "w" );
    if( !filemccdat ) {
      printf( "can't open file %s for writting\n", filenamemccdat );
      return ;
    }
  }
  
  filegpl = fopen( filenamegpl, "w" );
  if( !filegpl ) {
    printf( "can't open file %s for writting\n", filenamegpl );
    return ;
  }
 
  if( !stm_pwl_to_tanh( pwl, vt, vdd, tabth, &nth, fold ) ||
      !stm_pwl_to_tanh( pwlin, vt, vdd, tabthin, &nthin, fin ) ) {
    printf( "error in stm_pwl_to_tanh()\n" );
    fclose( filetanhdat ) ;
    fclose( filepwldat ) ;
    fclose( filegpl ) ;
    return ;
  }
 
  tref_th = tabs - stm_get_t(vdd/2.0, vt, 0.0, vdd, fin) ;
  dt_th   = stm_get_t(vt, vt, 0.0, vdd, fin) ;
  tref_pw = tabs - stm_get_t_pwth(vdd-vt, vt, vdd/2.0, tabthin ) ;
 
  trefthout = stm_modscm_dual_calculduts_threshold( params, 
                                                     fin, 
                                                     load, 
                                                     vdd/2.0, 
                                                     pwlin 
                                                   )+tref_pw
              - stm_get_t_pwth(vdd-vt, vt, vdd/2.0, tabth ) ;

  for( i=0 ; i<pwl->N ; i++ ) {
    if( eventout =='U' )
      fprintf( filepwldat, "%g %g\n", (pwl->TAB[2*i]+tref_pw)*1e-12, pwl->TAB[2*i+1] );
    else
      fprintf( filepwldat, "%g %g\n", vdd - (pwl->TAB[2*i]+tref_pw)*1e-12, pwl->TAB[2*i+1] );
    tmax = 2.0*pwl->TAB[2*i]+tref_pw ;
  }

  for( t=0.0 ; t<=tmax ; t++ ) {
    fprintf( filetanhdat, "%g ", t*1e-12 );
    
    if( eventout=='U' )
      fprintf( filetanhdat, "%g ", stm_get_v_pwth( t-trefthout, vt, vdd, STM_UP, tabth ) );
    else
      fprintf( filetanhdat, "%g ", vdd-stm_get_v_pwth( t-trefthout, vt, vdd, STM_UP, tabth ) );
    
    if( eventout=='U' )
      fprintf( filetanhdat, "%g ", stm_get_v( t-tref_pw-tp, vt, 0.0, vdd, fold ) );
    else
      fprintf( filetanhdat, "%g ", vdd-stm_get_v( t-tref_pw-tp, vt, 0.0, vdd, fold ) );

    if( eventin=='U' )
      fprintf( filetanhdat, "%g ", stm_get_v_pwth( t-tref_pw, vt, vdd, STM_UP, tabthin ) );
    else
      fprintf( filetanhdat, "%g ", vdd-stm_get_v_pwth( t-tref_pw, vt, vdd, STM_UP, tabthin ) );

    if( eventin=='U' )
      fprintf( filetanhdat, "%g\n", stm_get_v( t-tref_th, vt, 0.0, vdd, fin ) );
    else
      fprintf( filetanhdat, "%g\n", vdd-stm_get_v( t-tref_th, vt, 0.0, vdd, fin ) );
  }

  if( params ) {
    stm_modscm_dual_fill_param( params, fin, pwlin, NULL, load, &paramtimingpwl  );
    stm_modscm_dual_fill_param( params, fin, NULL, NULL, load, &paramtimingtanh );
    
    for( v=0.01*vdd ; v<=0.99*vdd ; v=v+0.01*vdd ) {
      tspwl  = stm_modscm_dual_calts_with_param_timing( &paramtimingpwl, v );
      tstanh = stm_modscm_dual_calts_with_param_timing( &paramtimingtanh, v );
      
      fprintf( filemccdat, "%g %g %g\n", ( tspwl + tref_pw ) * 1e-12,
                                         ( tstanh + tref_th + dt_th ) * 1e-12,
                                         eventout=='U' ? v : vdd-v 
             );
    }
  }
  
  fprintf( filegpl,
           "set key bottom right\n"
         );
  if( params ) 
    fprintf( filegpl, 
             "plot [0:%g] '%s' title \"pwl\" with linespoints, '%s' using 1:2 title \"pwtanh\" with lines, '%s' using 1:3 title \"old tanh\" with lines, '%s' using 1:3 title \"mcc (in=pwl)\" with lines, '%s' using 2:3 title \"mcc (in=th)\" with lines, '%s' using 1:5 title \"input old tanh\" with lines, '%s' using 1:4 title \"input pwl\" with lines\n",
             tmax*1e-12,
             filenamepwldat,
             filenametanhdat,
             filenametanhdat,
             filenamemccdat,
             filenamemccdat,
             filenametanhdat,
             filenametanhdat
           );
  else
    fprintf( filegpl, 
             "plot [0:%g] '%s' title \"pwl\" with linespoints, '%s' using 1:2 title \"pwtanh\" with lines, '%s' using 1:3 title \"old tanh\" with lines\n",
             tmax*1e-12,
             filenamepwldat,
             filenametanhdat,
             filenametanhdat
           );
  fprintf( filegpl, "pause -1\n" );

  fclose( filegpl );
  fclose( filepwldat );
  fclose( filetanhdat );
  if( params )
    fclose( filemccdat );
}

int stm_get_v_pwth_fn( stm_pwth_fn *args, double t, double *v )
{
  *v = stm_get_v_pwth( t,
                       args->VT,
                       args->VDD,
                       STM_UP,
                       args->PWTH
                     );
  return 1 ;
}

/* Renvoie la tension à l'instant t */
double stm_get_v_pwth (float t, float vt, float vdd, char sens, stm_pwth *pwth )
{
  int i ;
  double v ;

  i=0;
 
  if( t<0.0 ) {
    v = vt + (vdd-vt)*t/pwth[0].F ;
    if( v<0.0 )
      v=0.0 ;
  }
  else {

    while( pwth[i].DT + pwth[i].T < t )
      i++;

    v = vt + (vdd-vt)*tanh( (t - pwth[i].DT ) / pwth[i].F ) ;
    
  }

  if( sens == STM_DN ) {
    v = vdd-v ;
  }
  
  return v ;
}

/* Renvoie l'instant t où le signal passe à v */
double stm_get_t_pwth (float vddin, float vt, float seuil, stm_pwth *pwth )
{
  double vemax, tmax, dt, f ;
  int    i=0 ;
  double te ;
 
  if( seuil < vt ) {
    te = pwth[0].F*(seuil-vt)/vddin ;
  }
  else {
    do {
      tmax  = pwth[i].T ;
      dt    = pwth[i].DT ;
      f     = pwth[i].F ;
      
      vemax = vt + vddin*tanh(tmax/f);
      if( vemax < seuil ) {
        i++ ;
      }
    }
    while( vemax < seuil );

    te = stm_modscm_dual_calte( vddin, vt, seuil, f ) + dt ;
  }

  return te ;
}

void stm_plot_pwth( stm_pwth *pwth, char *filename, float t0, float vddin, float vt, float vth, char sens )
{
  FILE *ptf ;
  float te ;
  float dt ;
  float v ;
  float vr ;
  float t ;

  ptf = mbkfopen( filename, NULL, "w" ) ;
  if(!ptf) {
    printf( "can't open file \"%s\" for writting\n", filename ) ;
    return ;
  }

  te = stm_get_t_pwth( vddin, vt, vth, pwth );
  dt = t0-te ;

  for( v=0.02*vddin ; v<=0.98*(vddin+vt) ; v=v+vddin/100 ) {
    t = stm_get_t_pwth( vddin, vt, v, pwth );
    if( tolower(sens)=='d' || tolower(sens)=='f' )
      vr = 2.0*vth-v ;
    else
      vr = v ;
    fprintf( ptf, "%g %g\n", (t+dt)*1e-12, vr );
  }

  fclose( ptf );
}
