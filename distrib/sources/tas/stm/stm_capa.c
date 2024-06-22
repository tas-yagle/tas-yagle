/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_capa.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gregoire Avot                                             */
/*                                                                          */
/****************************************************************************/

#include "stm.h"

/******************************************************************************\
stm_solvepi()

 Détermine le temps de (dé)charge d'une cellule en PI alimentée par un
 générateur de courant constant.

 Arguments : 

 vth : tension de seuil, typiquement vdd/2.
 i   : courant dans le générateur de courant.
 r   : résistance de la cellule en PI.
 c1  : capacité de la cellule en PI.
 c2  : idem

 Retour : Temps de (de)charge.

\******************************************************************************/

float stm_solvepi (float i, float r, float c1, float c2, float vth)
{
	float lt, t;
	float a, b, c;
	int n;
	float delta;

	/* Resolution (t) de l'équation :
	   *   v = vi + i * ( t/(c1+c2) + r*c2^2/(c1+c2)^2*(1-e(-(c1+c2)/r/c1/c2*t)) )
	   *   cohérence :
	   *   i doit être positif pour v > vi
	   *   i doit être négatif pour v < vi
	 */

	a = i * r * c2 * c2 / (c1 + c2) / (c1 + c2);
	b = i / (c1 + c2);
	c = (c1 + c2) / (r * c1 * c2);

	/* Valeur initiale : le temps moyen d'une porte */
	t = 20.0;

	/* Ensuite on résoud en utilisant comme solution l'équation de droite
	   * correspondant à la dérivée */

	n = 1;
	do {
		lt = t;
		t = -(a * (1 - (1 + c * t) * exp(-c * t)) - vth) / (c * a * exp(-c * t) + b);
		n++;
		if (n > 100)
            avt_errmsg (STM_ERRMSG,"002", AVT_ERROR);

		delta = lt - t;
		if (delta < 0.0)
			delta = -delta;
	}
	while (delta > 1.0);		/* On demande une précision de 1pS */

	return t;
}

/******************************************************************************\
stm_capaeq()

Renvoie la capacité équivalente vue par un générateur de courant constant 
lorsque celui-ci attaque une charge en pi.

Arguments :

i               Valeur du courant constant.
r, c1, c2       Charge en pi.
vth             Tension à laquelle est calculée cette capacité équivalente.
\******************************************************************************/

float stm_capaeq (float i, float r, float c1, float c2, float vth, char *signame)
{
	float t, c;

	t = stm_solvepi (fabs (i), r, c1, c2, vth);

	c = i * t / vth;

	if (c < 0.0) {
          avt_errmsg (STM_ERRMSG,"003", AVT_ERROR, signame?signame:"?");
          c = fabs(c1+c2);
        }

	return c;

}

/******************************************************************************\
stm_calc_pi_pwl_plot
Fonction de debuggage : affiche la réponse d'une charge en pi.
\******************************************************************************/
void stm_calc_pi_pwl_plot( mbk_laplace *laplace,
                           mbk_pwl     *lines,
                           char (*fn)( void*, double, double* ),
                           void *data,
                           double r,
                           double c1,
                           double c2,
                           double c,
                           double tmax,
                           char *filename
                         )
{
  FILE *ptf;
  double t;
  double ve;
  double vs;
  double va;
  double vc;
  double v2;
  
  ptf = mbkfopen( filename, "dat", "w" );

  for( t=0.0 ; t < tmax ; t = t + tmax/1000.0 ) {
    if( !fn( data, t, &ve ) )
      ve=0.0;
    if( mbk_pwl_get_value( lines, t, &va ) == MBK_PWL_ERROR )
      va=0.0;
    if( r > 0.0 && c1 > 0.0 && c2 > 0.0 ) {
      vs = stm_calc_pi_pwl( laplace, t, r, c1, c2 );
      v2 = stm_calc_pi_pwl_2( laplace, t, r, c1, c2 );
    }
    if( c > 0.0 )
      vc = stm_calc_c_pwl( laplace, t, c );
    fprintf( ptf, "%g %g %g %g %g %g\n", t, ve, va, vs, v2, vc );
  }

  fclose( ptf );

  ptf = mbkfopen( filename, "plt", "w" );
  
  fprintf( ptf, "set key top left\n" );
  fprintf( ptf, "plot '%s.dat' using 1:2 title 'i(t)', ", filename ) ;
  fprintf( ptf, "'%s.dat' using 1:3 title 'i(t) approx'\n", filename ) ;
  fprintf( ptf, "pause -1\n" );
  fprintf( ptf, "plot '%s.dat' using 1:4 title 'vs pi', ", filename ) ;
  fprintf( ptf, "'%s.dat' using 1:5 title 'vs c'\n", filename ) ;
  fprintf( ptf, "pause -1\n" );
  fprintf( ptf, "quit\n" );

  fclose( ptf );
}
                           
/******************************************************************************\
stm_calc_c_pwl
Calcule la tension sur le premier noeud de la capacité à l'instant t.
\******************************************************************************/
double stm_calc_c_pwl( mbk_laplace *laplace,
                       double t,
                       double c
                     )
{
  double v ;
  int i ;
  double tr ;
  double a ;

  v = laplace->B * t;
  
  for( i=0 ; i<laplace->N && ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T < t ; i++ ) {
    a = ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.A ;
    tr = t - ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T ;
    v = v + 0.5 * a * tr * tr  ;
  }

  v = v / c;

  return v ;
}

/******************************************************************************\
stm_calc_pi_pwl
Calcule la tension sur le premier noeud de la charge en pi à l'instant t.
\******************************************************************************/
double stm_calc_pi_pwl( mbk_laplace *laplace,
                        double t,
                        double r,
                        double c1,
                        double c2
                      )
{
  int i ;
  double v ;
  double tr ;
  double k1, k2, k3, x ;
  double ct ;

  ct = c1+c2;
  x = ct/(r*c1*c2);

  v = 0 ;
  
  for( i=0 ; i<laplace->N && ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T < t ; i++ ) {
  
    tr = t - ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T ;
    k1 = ((stm_laplace_capaeq*)laplace->DATA)[i].k1 ;
    k2 = ((stm_laplace_capaeq*)laplace->DATA)[i].k2 ;
    k3 = ((stm_laplace_capaeq*)laplace->DATA)[i].k3 ;
    v = v + k3*tr*tr + k2*tr - k1 + k1*exp( -x*tr ) ;
  }

  return v;
}

double stm_calc_pi_pwl_2( mbk_laplace *laplace, double t, double r, double c1, double c2 )
{
  int i ;
  double v ;
  double tr ;
  double k3, k4, k5, k6, k7, x, k ;
  double ct ;

  ct = c1+c2;
  x = ct/(r*c1*c2);
  k = 1.0/(r*c2);

  v = 0 ;
  
  for( i=0 ; i<laplace->N && ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T < t ; i++ ) {
  
    tr = t - ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T ;
    k3 = ((stm_laplace_capaeq*)laplace->DATA)[i].k3 ;
    k4 = ((stm_laplace_capaeq*)laplace->DATA)[i].k4 ;
    k5 = ((stm_laplace_capaeq*)laplace->DATA)[i].k5 ;
    k6 = ((stm_laplace_capaeq*)laplace->DATA)[i].k6 ;
    k7 = ((stm_laplace_capaeq*)laplace->DATA)[i].k7 ;
    v = v + k3*tr*tr + k4*tr + k5 + k6*exp(-k*tr) + k7*exp(-x*tr);
  }

  return v;
}

double stm_calc_pi_rlin( double rl, double vdd, double r, double c1, double c2, double v1, double v2, double vth )
{
  double a ;
  double b ;
  double c ;
  double delta ;
  double p1 ;
  double p2 ;
  double tmin ;
  double tmax ;
  double t ;
  double v ;
  int    found ;
  
  a     = r*rl*c1*c2 ;
  b     = r*c2 + rl*(c1+c2) ;
  c     = 1.0 ;
  delta = b*b-4.0*a*c ;
  p1    = (-b+sqrt(delta))/(2.0*a) ;
  p2    = (-b-sqrt(delta))/(2.0*a) ;

  tmin  = 0 ;
  tmax  = r*(c1+c2) ;
  found = 0 ;
  
  do {
  
    t = tmax ;
    v = ( c1*v1 + c2*v2 + vdd*r*c2/rl ) / (r*c1*c2) * ( exp(p1*t)-exp(p2*t) ) / (p1-p2) 
        + v1 * ( -p1*exp(p1*t) + p2*exp(p2*t) ) / (p2-p1) 
        - vdd/(r*rl*c1*c2) * ( p1*exp(p2*t) - p2*exp(p1*t) + (p2-p1) ) / (p1*p2*(p1-p2)) ;
    if( v < vth ) {
      tmin = tmax ;
      tmax = tmax * 2.0 ;
    }
    else
      found = 1 ;
  }
  while( !found );

  do {
    t = (tmax+tmin)/2.0 ;
    v = ( c1*v1 + c2*v2 + vdd*r*c2/rl ) / (r*c1*c2) * ( exp(p1*t)-exp(p2*t) ) / (p1-p2) 
        + v1 * ( -p1*exp(p1*t) + p2*exp(p2*t) ) / (p2-p1) 
        - vdd/(r*rl*c1*c2) * ( p1*exp(p2*t) - p2*exp(p1*t) + (p2-p1) ) / (p1*p2*(p1-p2)) ;
    if( v >= vth )
      tmax = t ;
    if( v <= vth )
      tmin = t ;
  }
  while( tmax-tmin>0.1 );

  t = (tmax+tmin)/2.0 ;

  return t ;
}

/******************************************************************************\
stm_calc_pi_pwl_instant
Calcule l'instant où la tension sur le premier noeud de la charge en pi
atteind vth.
La réponse doit être strictement croissante.
\******************************************************************************/
double stm_calc_pi_pwl_instant( mbk_laplace *laplace,
                                double vth,
                                double r,
                                double c1,
                                double c2,
                                double testim
                              )
{
  char encore;
  double tmin, tmax, tm ;
  double vmin, vmax, vm ;
  int n;

  // Recherche l'intervalle qui va bien
  
  tmin = 0.0;
  vmin = stm_calc_pi_pwl( laplace, tmin, r, c1, c2 );
  tmax = testim;

  n=0;
  do {
    encore = 0;
    vmax = stm_calc_pi_pwl( laplace, tmax, r, c1, c2 );
    
    if( vmax < vth ) {
      tmin = tmax ;
      vmin = vmax ;
      tmax = tmax * 2.0 ;
      encore = 1 ;
    }
    n++;
  }
  while( encore && n < 100 );

  if( n >= 100 ) return -1.0;

  // Résolution par dichotomie
 
  n=0;
  do {
    n++;
    tm = (vth-vmin)/(vmax-vmin)*(tmax-tmin)+tmin;
    vm = stm_calc_pi_pwl( laplace, tm, r, c1, c2 );

    if( fabs(vm-vth) < 1.0e-6 ) { 
      tmax = tm ;
      vmax = vm ;
      tmin = tm ;
      vmin = vm ;
    }
    else {
      if( vm > vth ) {
        tmax = tm ;
        vmax = vm ;
      }
      else {
        tmin = tm ;
        vmin = vm ;
      }
    }
  }
  while( (tmax-tmin) > 0.3 && n < 100 );

  if( n >= 100 ) return -1.0;

  return (tmax+tmin)/2.0;
}


/******************************************************************************\
stm_capaeq_laplace()

Renvoie la capacité équivalente qui permet d'obtenir le temps tpi lorque le
générateur est décrit sous la forme d'un ensemble de droites.
\******************************************************************************/

double stm_capaeq_laplace( mbk_laplace *laplace, double tpi, double vth )
{
  int i, n;
  double x;
  double c;
  // Retrouve l'intervalle

  for( n=1 ; 
       n < laplace->N && tpi > ((stm_laplace_capaeq*)laplace->DATA)[n].DATA.T ; 
       n++ 
     );
  n-- ;
 
  // Calcule le C

  x = 0.0;
  for( i=0 ; i<= n ; i++ ) 
    x = x + ( tpi - ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T ) * 
            ( tpi - ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.T ) * 
            ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.A ;
  
  c = ( 0.5 * x + laplace->B * tpi ) / vth ;

  return c ;
}

/******************************************************************************\
stm_capaeq_fn()

Renvoie la capacité équivalente vue par un générateur de courant décrit par une
fonction lorsque celui-ci attaque une charge en pi.

Arguments :

fn              Fonction à appeler pour déterminer i=f(t).
data            Paramètre passé à cette fonction.
tmax            Temps à partir duquel on considère que le générateur est 
                constant.
r, c1, c2       Charge en pi.
vth             Tension à laquelle est calculée cette capacité équivalente.
\******************************************************************************/

float stm_capaeq_fn( char (*fn)( void*, double, double* ),
                     void *data,
                     double tmax,
                     float r, 
                     float c1, 
                     float c2, 
                     float vth,
                     float vsat,
                     float rlin,
                     float vdd,
                     char *signame
                   )
{
  mbk_pwl     *lines ;
  mbk_laplace *laplace ;
  double       tpi, tplin ;
  double       c ;
  double       testim ;
  double       iestim ;
  double       a, b, ct ;
  double       isat, rt, rl ;
  int          i ;
  double       m=1000.0 ;
  double       k1, k2, k3, k4, k5, k6, k7, k, x, vi, v2 ;

  c1=c1/m ;
  c2=c2/m ;

  if( vsat>0 && vth>vsat ) {
    c = c1+c2 ;
  }
  else {

    // Création de l'approximation par segments de droites
    // du générateur.
    lines = mbk_pwl_create( fn, data, 0.0, tmax, NULL ) ;
    if( !lines ) {
      avt_errmsg (STM_ERRMSG,"003", AVT_ERROR, signame?signame:"?");
      return fabs(c1+c2) ;
    }
   
    // Calculs des paramètres constants pour les transformées de Laplace

    laplace = mbk_pwl_to_laplace( lines, sizeof( stm_laplace_capaeq ) );
    ct = c1 + c2 ;
    k  = 1.0/(r*c2) ;
    x = ct/(r*c1*c2);

    for( i=0 ; i<laplace->N  ; i++ ) {
    
      a = ((stm_laplace_capaeq*)laplace->DATA)[i].DATA.A; 
      
      if( i==0 ) {
        b = laplace->B;
        k1 = r*c2*c2*( a*r*c1*c2 - b*ct )/ct/ct/ct ;
        k2 = ( a*r*c2*c2 + b*ct )/ct/ct ;
        k3 = 0.5*a/ct ;
      }
      else {
        k1 = a*r*r*c1*c2*c2*c2/ct/ct/ct ;
        k2 = a*r*c2*c2/ct/ct ;
        k3 = 0.5*a/ct ;
      }
      k4 = -k3/k + k2 ;
      k5 = k3/(k*k) - k2/k - k1 ;
      k6 = -k3/(k*k) + k2/k + k1 + k1*k/(x-k) ;
      k7 = k*k1/(k-x);
      ((stm_laplace_capaeq*)laplace->DATA)[i].k1 = k1 ;
      ((stm_laplace_capaeq*)laplace->DATA)[i].k2 = k2 ;
      ((stm_laplace_capaeq*)laplace->DATA)[i].k3 = k3 ;
      ((stm_laplace_capaeq*)laplace->DATA)[i].k4 = k4 ;
      ((stm_laplace_capaeq*)laplace->DATA)[i].k5 = k5 ;
      ((stm_laplace_capaeq*)laplace->DATA)[i].k6 = k6 ;
      ((stm_laplace_capaeq*)laplace->DATA)[i].k7 = k7 ;
    }
   
    // Estimation de la solution recherchée (pour accélérer les convergences).
    
    if( !fn( data, tmax, &iestim ) ) {
      mbk_free_laplace( laplace );
      mbk_pwl_free_pwl( lines );
      avt_errmsg (STM_ERRMSG,"003", AVT_ERROR, signame?signame:"?");
      return m*fabs(c1+c2) ;
    }
   
    if( vsat>0.0 && vth>vsat )
      vi = vsat ;
    else
      vi = vth ;
    testim = ( c1 + c2 ) * vi / iestim ;
    tpi = stm_calc_pi_pwl_instant( laplace, vi, r, c1, c2, testim );

    if( tpi < 0.0 ) {
                          
      mbk_free_laplace( laplace );
      mbk_pwl_free_pwl( lines );

      return m*(c1+c2) ;
    }

    if( vsat>0.0 && vth>vsat ) {
      mbk_pwl_get_value( lines, tpi, &isat );
      rt = (vdd-vi)/isat ;
      if( rt > rlin )
        rl = rt ;
      else
        rl = rlin ;
      v2 = stm_calc_pi_pwl_2( laplace, tpi, r, c1, c2 );
      tplin = stm_calc_pi_rlin( rl, vdd, r, c1, c2, vi, v2, vth );
      tpi = tpi + tplin ;
    }
    
    // Détermination de la capacité équivalente.
    
    c = stm_capaeq_laplace( laplace, tpi, vth );

    // stm_calc_pi_pwl_plot( *laplace, *lines, *fn, *data, r, c1, c2, c, tmax, *filename );
    mbk_free_laplace( laplace );
    mbk_pwl_free_pwl( lines );

    if (c < 0.0) {
      avt_errmsg (STM_ERRMSG,"003", AVT_ERROR, signame?signame:"?");
      return m*fabs(c1+c2) ;
    }
  }
   
  return m*c ;
}
