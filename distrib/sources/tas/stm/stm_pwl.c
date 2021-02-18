/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_pwl.c                                                   */
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

int STM_NBALLOCATED=0 ;
chain_list *CHAIN_ALLOCATED=NULL;
char STM_PWL_DEBUG_ALLOC=NO;

void stm_pwl_finish(void)
{
  chain_list *chain ;
  stm_pwl    *pwl;

  if( STM_PWL_DEBUG_ALLOC == NO )
    return ;

  for( chain = CHAIN_ALLOCATED ; chain ; chain = chain->NEXT ) {
    pwl = (stm_pwl*)chain->DATA ;
    if( pwl->TAB )
      printf( "stm_pwl %08X\n", (unsigned int)(long)pwl );
  }
  
  printf( "STM_NBALLOCATED=%d\n",STM_NBALLOCATED );
}

stm_pwl *stm_pwl_create (float *tab, int n)
{
    stm_pwl *pwl = NULL;
    int i;

    if (n > 0) {
        pwl = (stm_pwl*)mbkalloc (sizeof (struct stm_pwl)); 
        pwl->TAB = (float*)mbkalloc (2 * n * sizeof (float));
        for (i = 0; i < 2 * n; i++) 
            pwl->TAB[i] = tab[i]; 
        pwl->N = n;

        if( STM_PWL_DEBUG_ALLOC == YES ) {
          STM_NBALLOCATED++;
          CHAIN_ALLOCATED = addchain( CHAIN_ALLOCATED, pwl );
        }
    }

    return pwl;
}

stm_pwl *stm_pwl_dup (stm_pwl *pwl)
{
    stm_pwl *pwl_dup = NULL;
    int i;

    if (pwl->N > 0 && pwl->TAB) {
        pwl_dup = (stm_pwl*)mbkalloc (sizeof (struct stm_pwl)); 
        pwl_dup->TAB = (float*)mbkalloc (2 * pwl->N * sizeof (float));
        for (i = 0; i < 2 * pwl->N; i++) 
            pwl_dup->TAB[i] = pwl->TAB[i]; 
        pwl_dup->N = pwl->N;

        if( STM_PWL_DEBUG_ALLOC == YES ) {
          STM_NBALLOCATED++;
          CHAIN_ALLOCATED = addchain( CHAIN_ALLOCATED, pwl );
        }
    }

    return pwl_dup;
}

void stm_pwl_destroy (stm_pwl *pwl)
{
    if (pwl) {
        if (pwl->TAB) mbkfree (pwl->TAB);
        pwl->TAB = NULL;
        if( STM_PWL_DEBUG_ALLOC == NO )
          mbkfree (pwl);
        else
          STM_NBALLOCATED--;
    }
}

double stm_get_t_pwl( stm_pwl *pwl, float v )
{
  int i ;
  double t1, t2, v1, v2 ;
  double a, b, t ;
  
  for( i=0 ; i<pwl->N-1 ; i++ ) {
    if( ( pwl->TAB[2*i+1] < v && pwl->TAB[2*i+3] > v ) ||
        ( pwl->TAB[2*i+1] > v && pwl->TAB[2*i+3] < v )    )
      break ;
  }

  if( i >= pwl->N-1 ) {
    /* extrapole */
    
    if( v < pwl->TAB[3] )
      i = 0 ;
    else
      i = pwl->N-2 ;
  }

  t1 = pwl->TAB[2*i+0];
  v1 = pwl->TAB[2*i+1];
  t2 = pwl->TAB[2*i+2];
  v2 = pwl->TAB[2*i+3];

  /* calcule la droite v=at+b */
  a = (v1-v2)/(t1-t2) ;
  b = v1-a*t1 ;

  t = (v-b)/a ;
  return t ;
}

/* Shrink le front d'entrée pwl suivant la transformation qui a été faite 
   pour la tanh originale. */
stm_pwl* stm_shrink_pwl( stm_pwl *pwin, float u0, float f0, float u1, float f1, float vt )
{
  stm_pwl *shrinked ;
  float   *tab ;
  float    t, v ;
  float    s1, s2 ;
  float    dt0, dt1 ;
  float    k ;
  float    t0 ;
  int      i;
  int      debug = 0 ;

  if( !pwin )
    return NULL ;

  s1 = vt ;
  s2 = 0.75*u0 + vt ;
  dt0 = f0 * ( atanh((s2-vt)/u0) - atanh((s1-vt)/u0) );

  s1 = vt ;
  s2 = 0.75*u1 + vt ;
  dt1 = f1 * ( atanh((s2-vt)/u1) - atanh((s1-vt)/u1) );

  k  = dt1/dt0 ;
  t0 = stm_get_t_pwl( pwin, s1 );
 
  tab = (float*)alloca( 2 * pwin->N * sizeof(float) );

  for( i=0 ; i<pwin->N ; i++ ) {
    
    t = pwin->TAB[ 2*i   ] ;
    v = pwin->TAB[ 2*i+1 ] ;

    v = (v-vt)*u1/u0 + vt ;
    t = k*( t - t0 ) + t0 ;

    tab[ 2*i   ] = t ;
    tab[ 2*i+1 ] = v ;
  }
  
  shrinked = stm_pwl_create( tab, pwin->N );
  
  if( debug ) {
    FILE *ptf ;

    ptf = fopen( "debugshrink_tanh.dat", "w" );
    for( t=0.0 ; t<2.0*f0 ; t=t+1.0 )
      fprintf( ptf, "%g %g %g\n", t, vt+u0*tanh(t/f0), vt+u1*tanh(t/f1) );
    fclose( ptf );
    ptf = fopen( "debugshrink_pwl0.dat", "w" );
    for( i=0 ; i < pwin->N ; i++ )
      fprintf( ptf, "%g %g\n", pwin->TAB[2*i], pwin->TAB[2*i+1] );
    fclose( ptf );
    ptf = fopen( "debugshrink_pwl1.dat", "w" );
    for( i=0 ; i < shrinked->N ; i++ )
      fprintf( ptf, "%g %g\n", shrinked->TAB[2*i], shrinked->TAB[2*i+1] );
    fclose( ptf );
    ptf = fopen( "debugshrink.gpl", "w" );
    fprintf( ptf, "plot 'debugshrink_tanh.dat' using 1:2 title \"f0\" with lines, 'debugshrink_tanh.dat' using 1:3 title \"f1\" with lines, 'debugshrink_pwl0.dat' title \"pwl0\" with linespoints, 'debugshrink_pwl1.dat' title \"pwl1\" with linespoints\n" );
    fprintf( ptf, "pause -1\n" );
    fclose(ptf);
  }
  return shrinked ;
}

stm_pwl* mbk_pwl_to_stm_pwl( mbk_pwl *mbkpwl )
{
  stm_pwl *stmpwl ;
  float   *tab ;
  int      i;
  
  if( !mbkpwl ) return NULL ;
 
  tab = (float*)alloca( mbkpwl->N * 2 * sizeof( float ) );

  for( i=0 ; i<=mbkpwl->N ; i++ ) {
    if( i<mbkpwl->N ) {
      tab[ 2*i     ] = mbkpwl->DATA[i].X0 ;
      tab[ 2*i + 1 ] = mbkpwl->DATA[i].X0 * mbkpwl->DATA[i].A + mbkpwl->DATA[i].B ;
    }
    else {
      tab[ 2*i     ] = mbkpwl->X1 ,
      tab[ 2*i + 1 ] = mbkpwl->X1 * mbkpwl->DATA[i-1].A + mbkpwl->DATA[i-1].B ;
    }
  }

  stmpwl = stm_pwl_create( tab, mbkpwl->N+1 );

  return stmpwl ;
}

mbk_pwl* stm_pwl_to_mbk_pwl( stm_pwl *stmpwl, float vt, float vmax )
{
  mbk_pwl            *mbkpwl ;
  stm_pwth_fn         data ;
  stm_pwth            stmpwth[128] ;
  int                 nth ;
  int                 r ;
  double              tmin ;
  double              tmax ;
  mbk_pwl_param      *param;

  if( !stmpwl )
    return NULL ;
  r = stm_pwl_to_tanh( stmpwl, vt, vmax, stmpwth, &nth, 0.0 );
  if( !r ) 
    return NULL ;

  data.PWTH = stmpwth ;
  data.NTH  = nth ;
  data.VT   = vt ;
  data.VDD  = vmax ;

  tmin = stmpwth[0].F * atanh((-vt)/(vmax-vt)) ;
  tmax = stmpwth[nth-1].DT + stmpwth[nth-1].F * atanh( (0.99*vmax-vt)/(vmax-vt) ) ;

  param = mbk_pwl_get_default_param( (char(*)(void*,double,double*))stm_get_v_pwth_fn,
                                     (void*)&data,
                                     tmin,
                                     tmax
                                   );

  mbk_pwl_add_param_point( param, stm_get_t_pwl( stmpwl, vmax/2.0 ) );
  
  if( STM_DEFAULT_SMINR > 0.0 )
    mbk_pwl_add_param_point( param, stm_get_t_pwl( stmpwl, vmax*STM_DEFAULT_SMINR ) );
    
  if( STM_DEFAULT_SMAXR > 0.0 )
    mbk_pwl_add_param_point( param, stm_get_t_pwl( stmpwl, vmax*STM_DEFAULT_SMAXR ) );
    
  mbkpwl = mbk_pwl_create( (char(*)(void*,double,double*))stm_get_v_pwth_fn,
                           (void*)&data,
                           tmin,
                           tmax,
                           param
                         );

  mbk_pwl_free_param( param );

  return mbkpwl;
}

void stm_pwl_plot( char *name, stm_pwl *pwl, float t0, float threshold, float vdd, char trans ) 
{
  FILE  *ptf ;
  float  te ;
  float  dt ;
  float  v ;
  int    n ;

  ptf = mbkfopen( name, NULL, "w" );
  if( !ptf ) {
    printf( "can't open file \"%s\"\n.", name );
    return ;
  }
  
  te = 1e-12*stm_get_t_pwl( pwl, threshold );
  dt = t0-te ;
  for( n=0 ; n<pwl->N ; n++ ) {
    if( trans=='D' || trans=='d' || trans=='F' || trans=='f' )
      v = vdd-pwl->TAB[2*n+1] ;
    else
      v = pwl->TAB[2*n+1] ;
    fprintf( ptf, "%g %g\n", pwl->TAB[2*n+0]*1e-12+dt, v );
  }
  fclose( ptf );
}
