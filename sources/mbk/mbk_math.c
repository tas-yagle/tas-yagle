#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#ifdef Linux
#include <values.h>
#else
#include <float.h>
#endif
#ifdef Solaris
#include <ieeefp.h>
#endif

#include AVT_H
#include MUT_H
#include "mbk_math.h"

/******************************************************************************\

Bibliothèque de manipulation des objets mathématiques MBK.

\******************************************************************************/

/******************************************************************************\
mbk_double2long
\******************************************************************************/
long mbk_double2long (double val, long factor)
{
  return (long)( val*factor );
}

/******************************************************************************\
mbk_pwl_alloc_point()
Alloue une structure mbk_pwl_point_list.
\******************************************************************************/
mbk_pwl_point_list* mbk_pwl_alloc_point( void )
{
  return (mbk_pwl_point_list*)mbkalloc( sizeof( mbk_pwl_point_list ) );
}

/******************************************************************************\
mbk_pwl_free_point()
Libère une structure mbk_pwl_point_list.
\******************************************************************************/
void mbk_pwl_free_point( mbk_pwl_point_list *pt )
{
  mbkfree( pt );
}

/******************************************************************************\
mbk_pwl_free_point_list()
Libère une liste de structures.
\******************************************************************************/
void mbk_pwl_free_point_list( mbk_pwl_point_list *head )
{
  mbk_pwl_point_list *scan ;
  mbk_pwl_point_list *next ;

  for( scan = head ; scan ; scan = next ) {
    next = scan->NEXT ;
    mbk_pwl_free_point( scan );
  }
}

/******************************************************************************\
mbk_pwl_alloc_pwl()
\******************************************************************************/
mbk_pwl* mbk_pwl_alloc_pwl( int n )
{
  mbk_pwl *p;

  p = (mbk_pwl*)mbkalloc( sizeof( mbk_pwl ) );
  if( n )
    p->DATA = (mbk_pwl_data*)mbkalloc( sizeof( mbk_pwl_data ) * n );
  else
    p->DATA = NULL;
  p->N = n;

  return p;
}

/******************************************************************************\
mbk_pwl_free_pwl()
\******************************************************************************/
void mbk_pwl_free_pwl( mbk_pwl *pt )
{
  if( pt ) {
    mbkfree( pt->DATA );
    mbkfree( pt );
  }
}

/******************************************************************************\
mbk_pwl_alloc_param()
Alloue une structure de paramètre
\******************************************************************************/
mbk_pwl_param* mbk_pwl_alloc_param( void )
{
  mbk_pwl_param *param;
  param = (mbk_pwl_param*)mbkalloc(sizeof(mbk_pwl_param));
  return param;
}

/******************************************************************************\
mbk_pwl_free_param()
Libère une structure de paramètre
\******************************************************************************/
void mbk_pwl_free_param( mbk_pwl_param *param )
{
  mbk_pwl_free_param_point_list( param->POINTS );
  mbkfree( param );
}

/******************************************************************************\
mbk_pwl_get_default_param()
Crée des paramètres par défaut.
\******************************************************************************/
mbk_pwl_param* mbk_pwl_get_default_param( char (*fn)(void*, double, double*),
                                          void *data,
                                          double x0,
                                          double x1
                                        )
{
  mbk_pwl_param *defparam ;
  double y0 ;
  double y1 ;

  defparam = mbk_pwl_alloc_param();
  
  defparam->MODEL  = MBK_PWL_DEFAULT ;
  defparam->POINTS = NULL;

  if( !fn( data, x0, &y0 ) ) return NULL ;
  if( !fn( data, x1, &y1 ) ) return NULL ;
  
  defparam->PARAM.DEFAULT.DELTAYMAX = fabs( y1-y0 ) / 50.0 ;
  defparam->PARAM.DEFAULT.DELTAXMIN = ( x1 - x0 ) / 100.0 ;
  defparam->PARAM.DEFAULT.SDERIVE   = ( x1 - x0 ) / 10000.0;

  return defparam ;
}

/******************************************************************************\
mbk_pwl_get_surface_param()
Crée des paramètres par défaut.
\******************************************************************************/
mbk_pwl_param* mbk_pwl_get_surface_param( char (*fn)(void*, double, double*),
                                          void *data,
                                          double x0,
                                          double x1
                                        )
{
  mbk_pwl_param *defparam ;
  double y0 ;
  double y1 ;

  defparam = mbk_pwl_alloc_param();
  
  defparam->MODEL  = MBK_PWL_SURFACE ;
  defparam->POINTS = NULL;

  if( !fn( data, x0, &y0 ) ) return NULL ;
  if( !fn( data, x1, &y1 ) ) return NULL ;
  
  defparam->PARAM.SURFACE.DELTAXMIN = ( x1 - x0 ) / 100.0 ;
  defparam->PARAM.SURFACE.SDERIVE   = ( x1 - x0 ) / 10000.0;

  return defparam ;
}

/******************************************************************************\
mbk_pwl_add_param_point()
Ajoute un point de passage obligé sur la courbe
\******************************************************************************/
void mbk_pwl_add_param_point( mbk_pwl_param *param, double x )
{
  mbk_pwl_param_point_list *p;
  p = (mbk_pwl_param_point_list*)mbkalloc( sizeof(mbk_pwl_param_point_list) );
  p->NEXT = param->POINTS ;
  p->X = x ;
  param->POINTS = p;
}

/******************************************************************************\
mbk_pwl_free_param_point()
Libère une liste de point de passage obligé sur la courbe
\******************************************************************************/
void mbk_pwl_free_param_point_list( mbk_pwl_param_point_list *head )
{
  mbk_pwl_param_point_list *scan, *next ;
  for( scan = head ; scan ; scan = next ) {
    next = scan->NEXT ;
    mbkfree( scan );
  }
}

/******************************************************************************\
mbk_pwl_calc_deriv()
Calcul la dérivée de y=fn(x) entre deux points proches x0 et x1.
\******************************************************************************/
char mbk_pwl_calc_deriv( char (*fn)(void*, double, double*), 
                         void *data,
                         double x0, 
                         double x1, 
                         double *p 
                       )
{
  double y0, y1 ;
  
  if( !fn( data, x0, &y0 ) ) return 0;
  if( !fn( data, x1, &y1 ) ) return 0;

  *p = (y1-y0)/(x1-x0);
  if( !isfinite( *p ) )
    return 0;

  return 1;
}

char mbk_pwl_calc_deriv2( char (*fn)(void*, double, double*), 
                         void *data,
                         double x0,  double y0,
                         double x1, 
                         double *p 
                       )
{
  double y1 ;
  
  if( !fn( data, x1, &y1 ) ) return 0;

  *p = (y1-y0)/(x1-x0);
  if( !isfinite( *p ) )
    return 0;

  return 1;
}

/******************************************************************************\
mbk_pwl_duplicate()
\******************************************************************************/
mbk_pwl* mbk_pwl_duplicate( mbk_pwl *pwl )
{
  mbk_pwl *newpwl ;
  int      i ;
  
  newpwl = mbk_pwl_alloc_pwl( pwl->N ) ;
  newpwl->X1 = pwl->X1 ;
  for( i=0 ; i<pwl->N ; i++ ) {
    newpwl->DATA[i].X0 = pwl->DATA[i].X0 ;
    newpwl->DATA[i].A = pwl->DATA[i].A ;
    newpwl->DATA[i].B = pwl->DATA[i].B ;
  }

  return newpwl ;
}
/******************************************************************************\
mbk_pwl_create()
Renvoie un ensemble de droites permettant d'approximer une fonction. L'argument
param est optionnel. Renvoie NULL en cas d'echec.
\******************************************************************************/
mbk_pwl* mbk_pwl_create( char (*fn)(void*, double, double*),
                         void *data,
                         double x0,
                         double x1,
                         mbk_pwl_param *param
                       )
{
  mbk_pwl *list = NULL ;
  char     myparam = 0 ;
  
  if( !param ) {
    param = mbk_pwl_get_default_param( fn, data, x0, x1 ) ;
    myparam = 1 ;
  }

  if( !param || x1 <= x0 ) 
    return NULL ;

  switch( param->MODEL ) {
  case MBK_PWL_DEFAULT:
    list = mbk_pwl_create_default( fn, data, x0, x1, param );
    break;
  case MBK_PWL_SURFACE:
    list = mbk_pwl_create_surface( fn, data, x0, x1, param );
    break;
  }
 
  if( myparam )
    mbk_pwl_free_param( param );

  return list;
}

/******************************************************************************\
mbk_pwl_fill_point()
Remplis les champs numérique d'un point.
x      : abscisse du point
x1, x2 : intervalle sur lequel calculer la dérivée.
renvoie 1 si ok, 0 si ko.
\******************************************************************************/
char mbk_pwl_fill_point( char (*fn)(void*, double, double*),
                         void *data,
                         double x,
                         double x1,
                         double x2,
                         mbk_pwl_point_list *point
                       )
{
  point->X = x;
  
#ifndef OPTIM13
  if( !mbk_pwl_calc_deriv( fn, data, x1, x2, &(point->P) ) )
    return 0;
#endif
  if( !fn( data, x, &(point->Y) ) ) 
    return 0;

#ifndef OPTIM13
  point->O = point->Y - point->P * point->X ;
#endif

  return 1;
}

/******************************************************************************\
mbk_qsort_dbl_cmp()
\******************************************************************************/
int mbk_qsort_dbl_cmp( double *x1, double *x2 )
{
  if( *x1 < *x2 )
    return -1 ;
  if( *x1 > *x2 )
    return 1 ;
  return 0 ;
}

/******************************************************************************\
mbk_pwl_param_point_to_point()
transforme un liste de points de passage obligés configurés dans le param en une
liste de points pour la détermination des intervales.
Renvoie au moins les 2 points extrèmes, et NULL en cas d'erreur.
\******************************************************************************/
mbk_pwl_point_list* mbk_pwl_param_point_to_point( 
                                        char (*fn)(void*, double, double* ),
                                        void *data,
                                        double x0,
                                        double x1,
                                        mbk_pwl_param *param
                                                )
{
  mbk_pwl_param_point_list  *scan;
  double                    *x;
  double                     xd1, xd2, dx, dx2 ;
  int                        n, i;
  mbk_pwl_point_list        *head;
  mbk_pwl_point_list        *pnew;
  mbk_pwl_point_list        *last;

  switch( param->MODEL ) {
  case MBK_PWL_DEFAULT:
    dx2 = param->PARAM.DEFAULT.SDERIVE / 2.0 ;
    dx  = param->PARAM.DEFAULT.SDERIVE ;
    break;
  case MBK_PWL_SURFACE:
    dx2 = param->PARAM.SURFACE.SDERIVE / 2.0 ;
    dx  = param->PARAM.SURFACE.SDERIVE ;
    break;
  default:
    return NULL;
  }

  // Crée un tableau des points de passage obligés et le trie
  n=2;
  for( scan = param->POINTS ; scan ; scan = scan->NEXT ) {
    // Cette comparaison permet de ne garder que les points compris entre
    // x0 et x1, et dont le calcul de la dérivée est également compris dans
    // cette zone.
    if( scan->X - dx2 > x0 && scan->X + dx2 < x1 )
      n++;
  }
  x=(double*)mbkalloc( n*sizeof(double) );
  x[0] = x0 ;
  x[n-1] = x1 ;
  i = 1 ;
  for( scan = param->POINTS ; scan ; scan = scan->NEXT ) {
    if( scan->X - dx2 > x0 && scan->X + dx2 < x1 ) {
      x[i] = scan->X;
      i++;
    }
  }
  qsort( x, 
         n, 
         sizeof(double), 
         (int (*)(const void*, const void*))mbk_qsort_dbl_cmp 
       );
 
  // Transforme ces points en une liste
  head = NULL;
  
  for( i=n-1 ; i>=0 ; i-- ) {
  
    pnew = mbk_pwl_alloc_point();
    pnew->NEXT = head;
    if( head )
      head->PREV = pnew;
    else
      last = pnew;
    pnew->PREV = NULL;
    head = pnew ;

    if( i == (n-1) ) {
      xd1 = x[i] - dx ;
      xd2 = x[i] ;
    }
    else {
      if( i == 0 ) {
        xd1 = x[i];
        xd2 = x[i] + dx ;
      }
      else {
        xd1 = x[i] - dx2;
        xd2 = x[i] + dx2;
      }
    }

    if( !mbk_pwl_fill_point( fn, data, x[i], xd1, xd2, pnew ) )
      break;
  }

  mbkfree( x );
  if( i >= 0 ) {
    mbk_pwl_free_point_list( head );
    return NULL;
  }
 
  return head ;
}

/******************************************************************************\
mbk_pwl_point_to_pwl()
convertie une liste de points en une liste d'intervales.
\******************************************************************************/
mbk_pwl* mbk_pwl_point_to_pwl( mbk_pwl_point_list *head )
{
  mbk_pwl_point_list        *last;
  mbk_pwl_point_list        *p1;
  mbk_pwl                   *list=NULL;
  int                        n;

  for( p1 = head, n=0 ; p1->NEXT ; p1 = p1->NEXT, n++ );
  last = p1;

  list = mbk_pwl_alloc_pwl( n );

  list->X1 = last->X ;
  for( p1 = head, n=0 ; p1 != last ; p1 = p1->NEXT, n++ ) {
    list->DATA[n].X0 = p1->X ;
    list->DATA[n].A  = ( p1->NEXT->Y - p1->Y ) / ( p1->NEXT->X - p1->X ) ;
    list->DATA[n].B  = p1->Y - list->DATA[n].A * p1->X ;
  }

  return list;
}

/******************************************************************************\
mbk_pwl_create_defaut()
\******************************************************************************/
mbk_pwl* mbk_pwl_create_surface( char (*fn)(void*, double, double*),
                                 void *data,
                                 double x0,
                                 double x1,
                                 mbk_pwl_param *param
                               )
{
  fn=NULL;
  data=NULL;
  x0=0.0;
  x1=0.0;
  param=NULL;
  return NULL;
}


/******************************************************************************\
mbk_pwl_create_defaut_rec()
\******************************************************************************/
#ifdef OPTIM13
static char mbk_pwl_create_default_rec( char (*fn)(void*, double, double*), 
                                 void *data,
                                 mbk_pwl_point_list *p1, 
                                 mbk_pwl_point_list *p2,
                                 mbk_pwl_param *param,
                                 double mid
                               )
{
  double xm, y, yd, dec ;
  mbk_pwl_point_list *pm;
  
  if( ( p2->X - p1->X ) >= (param->PARAM.DEFAULT.DELTAXMIN/OPTIM13) ) {

    // Test d'intersection des deux dérivées.

    xm = (p2->X + p1->X)/2;

    // Test de l'ecart entre l'intersection des deux dérivées et la fonction.
    if( !fn( data, xm, &y ) )
      return 0;

    yd = ( p2->Y - p1->Y ) / ( p2->X - p1->X ) * ( xm - p1->X ) + p1->Y ;
    
    if( !isfinite(yd) || fabs( y - yd ) < (param->PARAM.DEFAULT.DELTAYMAX/OPTIM13) )
      return 1;

    pm = mbk_pwl_alloc_point();
    p1->NEXT = pm ;
    pm->NEXT = p2 ;
    p2->PREV = pm ;
    pm->PREV = p1 ;
    
    pm->X = xm ;
    pm->Y = y ;

    if( !mbk_pwl_create_default_rec( fn, data, p1, pm, param, mid ) )
      return 0;
    if( !mbk_pwl_create_default_rec( fn, data, pm, p2, param, mid ) )
      return 0;
  }
  else {
  
    // On vérifie si on est pas sur une droite entre les deux points.

    xm = ( p1->X + p2->X ) / 2.0 ;
    if( !fn( data, xm, &y ) )
      return 0;

    yd = ( p2->Y - p1->Y ) / ( p2->X - p1->X ) * ( xm - p1->X ) + p1->Y ;
    if( !isfinite(yd) || fabs( y - yd ) < (param->PARAM.DEFAULT.DELTAYMAX/OPTIM13) )
      return 1;
      
    pm = mbk_pwl_alloc_point();
    p1->NEXT = pm ;
    pm->NEXT = p2 ;
    p2->PREV = pm ;
    pm->PREV = p1 ;
    pm->X = xm ;
    pm->Y = y ;
    if( !mbk_pwl_create_default_rec( fn, data, p1, pm, param, mid ) )
      return 0;
    if( !mbk_pwl_create_default_rec( fn, data, pm, p2, param, mid ) )
      return 0;
  }

  return 1;
}
#else
static char mbk_pwl_create_default_rec( char (*fn)(void*, double, double*), 
                                 void *data,
                                 mbk_pwl_point_list *p1, 
                                 mbk_pwl_point_list *p2,
                                 mbk_pwl_param *param,
                                 double mid
                               )
{
  double xm, y, yd, dec ;
  mbk_pwl_point_list *pm;
  
  if( ( p2->X - p1->X ) >= param->PARAM.DEFAULT.DELTAXMIN ) {

    // Test d'intersection des deux dérivées.

    xm = ( p2->O - p1->O ) / ( p1->P - p2->P ) ;

    if( isfinite( xm ) && xm > p1->X && xm < p2->X ) {
     
      // Test de l'ecart entre l'intersection des deux dérivées et la fonction.
      if( !fn( data, xm, &y ) )
        return 0;

      yd = ( p2->Y - p1->Y ) / ( p2->X - p1->X ) * ( xm - p1->X ) + p1->Y ;
      
      if( fabs( y - yd ) < param->PARAM.DEFAULT.DELTAYMAX )
        return 1;

      pm = mbk_pwl_alloc_point();
      p1->NEXT = pm ;
      pm->NEXT = p2 ;
      p2->PREV = pm ;
      pm->PREV = p1 ;
      
      pm->X = xm ;
      pm->Y = y ;

#ifndef OPTIM12
      if( ! mbk_pwl_calc_deriv( fn, 
                                data,
                                pm->X - param->PARAM.DEFAULT.SDERIVE / 2.0,
                                pm->X + param->PARAM.DEFAULT.SDERIVE / 2.0,
                                &(pm->P)
                              ) ) 
        return 0;
#else
      dec=param->PARAM.DEFAULT.SDERIVE / 2.0;
      if (pm->X>mid) dec=-dec;
      if( ! mbk_pwl_calc_deriv2( fn, 
                                data,
                                pm->X, pm->Y,
                                pm->X + dec,
                                &(pm->P)
                              ) ) 
        return 0;
#endif
      pm->O = y - pm->X * pm->P ;

      if( !mbk_pwl_create_default_rec( fn, data, p1, pm, param, mid ) )
        return 0;
      if( !mbk_pwl_create_default_rec( fn, data, pm, p2, param, mid ) )
        return 0;
    }
    else {
    
      // On vérifie si on est pas sur une droite entre les deux points.

      xm = ( p1->X + p2->X ) / 2.0 ;
      if( !fn( data, xm, &y ) )
        return 0;

      if( fabs( p1->P * xm + p1->O - y ) < param->PARAM.DEFAULT.DELTAYMAX &&
          fabs( p2->P * xm + p2->O - y ) < param->PARAM.DEFAULT.DELTAYMAX
        )
        return 1;
        
      pm = mbk_pwl_alloc_point();
      p1->NEXT = pm ;
      pm->NEXT = p2 ;
      p2->PREV = pm ;
      pm->PREV = p1 ;
      if( ! mbk_pwl_fill_point( *fn,
                                data,
                                xm,
                                xm - param->PARAM.DEFAULT.SDERIVE / 2.0,
                                xm + param->PARAM.DEFAULT.SDERIVE / 2.0,
                                pm
                              ) )
        return 0;
      if( !mbk_pwl_create_default_rec( fn, data, p1, pm, param, mid ) )
        return 0;
      if( !mbk_pwl_create_default_rec( fn, data, pm, p2, param, mid ) )
        return 0;
    }
  }

  return 1;
}
#endif
/******************************************************************************\
mbk_pwl_create_defaut()
\******************************************************************************/
mbk_pwl* mbk_pwl_create_default( char (*fn)(void*, double, double*),
                                 void *data,
                                 double x0,
                                 double x1,
                                 mbk_pwl_param *param
                               )
{
  mbk_pwl_point_list        *head;
  mbk_pwl_point_list        *last;
  mbk_pwl_point_list        *p1;
  mbk_pwl_point_list        *p2;
  mbk_pwl                   *list=NULL;
 
  head = mbk_pwl_param_point_to_point( fn, data, x0, x1, param );
  if( !head )
    return NULL;

  for( p1 = head ; p1->NEXT ; p1 = p1->NEXT );
  last = p1;

  // Subdivise les intervalles
  for( p1 = head ; p1 != last ; p1 = p2 ) {
    p2 = p1->NEXT ;
      if( ! mbk_pwl_create_default_rec( fn, data, p1, p2, param, (x0+x1)/2 ) )
        break;
  }

  // Crée la structure finale
  if( p1 == last ) 
    list = mbk_pwl_point_to_pwl( head );
  else
    list = NULL ;

  mbk_pwl_free_point_list( head );

  return list; 
}

/******************************************************************************\
mbk_pwl_get_value()
Calcule la valeur y d'une fonction représentée sous la forme d'une structure 
mbk_pwl.
Renvoie : MBK_PWL_ERROR, MBK_PWL_FOUND ou MBK_PWL_EXTPL
\******************************************************************************/

int mbk_pwl_get_value( mbk_pwl *pwl, double x, double *y )
{
  int i;
  char ret = MBK_PWL_ERROR ;
  
  if( pwl->N == 0 )
    return MBK_PWL_ERROR ;

  /* Vérification extrapolation */
  if( x < pwl->DATA[0].X0 ) {
    ret = MBK_PWL_EXTPL ;
    i = 0;
  }
  if( x > pwl->X1 ) {
    ret = MBK_PWL_EXTPL ;
    i = pwl->N - 1 ;
  }
 
  /* Recherche dans les intervales */
  if( ret == MBK_PWL_ERROR ) {
    for( i = 0 ; i < (pwl->N - 1) ; i++ ) {
      if( x >= pwl->DATA[i].X0 && x <= pwl->DATA[i+1].X0 ) {
        ret = MBK_PWL_FOUND ;
        break;
      }
    }
  }

  if( ret == MBK_PWL_ERROR ) {
    i = pwl->N - 1 ;
    ret = MBK_PWL_FOUND ;
  }

  /* Calcul de la valeur */
  *y = pwl->DATA[i].A * x + pwl->DATA[i].B ;
  return ret ;
}

/******************************************************************************\
mbk_pwl_get_inv_value()
Calcule la première valeur x d'une fonction représentée sous la forme d'une 
structure mbk_pwl.
\******************************************************************************/

char mbk_pwl_get_inv_value( mbk_pwl *pwl, double y, double *x )
{
  int i;
  double y0, y1 ;

  if( pwl->N == 0 )
    return 0;
    
  for( i = 0 ; i < pwl->N ; i++ ) {
  
    if( i == 0)
      y0 = pwl->DATA[i].X0 * pwl->DATA[i].A + pwl->DATA[i].B ;
    else
      y0 = y1 ;
    if( i != ( pwl->N - 1 ) )
      y1 = pwl->DATA[i+1].X0 * pwl->DATA[i].A + pwl->DATA[i].B ;
    else
      y1 = pwl->X1 * pwl->DATA[i].A + pwl->DATA[i].B ;

    if( y >= y0 && y <= y1 ||
        y <= y0 && y >= y1    ) {
      *x = ( y - pwl->DATA[i].B ) / pwl->DATA[i].A ;
      if( isfinite( *x ) )
        return 1;
    }
  }

  return 0;
}

/******************************************************************************\
mbk_pwl_gnuplot()
crée un fichier au format gnuplot d'une structure pwl.
\******************************************************************************/
void mbk_pwl_gnuplot( mbk_pwl *lines, double tstart, double tstop, char *fname )
{
  double  t ;
  double  step ;
  double  ve ;
  FILE   *file ;

  file = mbkfopen( fname, "dat", "w" );
  if( !file ) {
    printf( "Can't open file.\n" );
    return;
  }

  step = (tstop-tstart)/1000.0 ;
  for( t=tstart ; t<=tstop ; t=t+step ) {
    if( mbk_pwl_get_value( lines, t, &ve ) != MBK_PWL_ERROR ) {
      fprintf( file, "%g %g\n", t, ve );
    }
  }
  fclose( file );
}

/******************************************************************************\
mbk_create_pwl_with_tanh()
Calcul la structure pwl qui pourra être transformée en un ensemble de tanh.
\******************************************************************************/
mbk_pwl* mbk_create_pwl_according_tanh( char (*fn)(void*, float, float*), 
                                        void *data,
                                        float v1,
                                        float v2
                                      )
{
  mbk_pwl *pwl ;
  int      n ;
  int      i ;
  float    tprev ;
  float    vprev ;
  float    t ;
  float    v ;

  n = 4 ;

  vprev = v1 ;
  if( !fn( data, v1, &tprev ) ) 
    return NULL ;
    
  pwl = mbk_pwl_alloc_pwl( n-1 );

  for( i=1 ; i<n ; i++ ) {
  
    v = ((float)i)*(v2-v1)/(n-1.0)+v1 ;
    
    if( !fn( data, v, &t )) {
      mbk_pwl_free_pwl( pwl );
      return NULL;
    }

    pwl->DATA[i-1].X0 = tprev ;
    pwl->DATA[i-1].A  = (v-vprev) / (t-tprev) ;
    pwl->DATA[i-1].B  = v - pwl->DATA[i-1].A * t ;
    pwl->X1 = t ;

    tprev = t ;
    vprev = v ;
  }
  
  return pwl;
}

/******************************************************************************\
mbk_alloc_laplace
Allocation d'une structure laplace.
\******************************************************************************/
mbk_laplace* mbk_alloc_laplace( int n, int size )
{
  mbk_laplace *pt;

  pt = (mbk_laplace*)mbkalloc( sizeof( mbk_laplace ) );
  pt->N = n;
  pt->SIZE = size;
  pt->B = 0.0;
  if( n )
    pt->DATA = mbkalloc( size * n );
  else
    pt->DATA = NULL;
  return pt;
}

/******************************************************************************\
mbk_free_laplace
Libération d'une structure laplace.
\******************************************************************************/
void mbk_free_laplace( mbk_laplace *pt )
{
  if( pt->DATA )
    mbkfree( pt->DATA );
  mbkfree( pt );
}

/******************************************************************************\
mbk_pwl_to_laplace
Calcule une structure appropriée pour manipuler des transformées de laplace de
fonctions approximées par un ensemble de droites. La taille la structure 
utilisateur pour chaque segment de droite doit être fournie par 'size'.
Le premier champs de la structure utilsateur DOIT être :
  mbk_laplace_data  unnomquivabien;
\******************************************************************************/

mbk_laplace* mbk_pwl_to_laplace( mbk_pwl *lines, int size )
{
  mbk_laplace  *laplace ;
  int           i;

  laplace = mbk_alloc_laplace( lines->N + 1, size );
  
  for( i = 0 ; i < lines->N ; i++) {
  
    if( i == 0 )
      LAPLACEDATA( laplace, i )->A = lines->DATA[i].A ;
    else
      LAPLACEDATA( laplace, i )->A = lines->DATA[i].A - lines->DATA[i-1].A ;

    LAPLACEDATA( laplace, i )->T = lines->DATA[i].X0 ;
  }
  LAPLACEDATA( laplace, i )->A = - lines->DATA[i-1].A ;
  LAPLACEDATA( laplace, i )->T = lines->X1 ;

  laplace->B = lines->DATA[0].A * lines->DATA[0].X0 + lines->DATA[0].B ;

  return laplace ;
}

/******************************************************************************\
mbk_pwl_get_value_bytabindex ()
Calcule la valeur y d'une fonction représentée sous la forme d'une structure 
mbk_pwl.
Renvoie : MBK_PWL_ERROR, MBK_PWL_FOUND ou MBK_PWL_EXTPL
\******************************************************************************/

int mbk_pwl_get_value_bytabindex( mbk_pwl *pwl, double x, double *y , int *tabindex)
{
  int ret = MBK_PWL_ERROR ;
  double x1 ;
  int dir ;

  if( pwl->N == 0 )
    return MBK_PWL_ERROR ;

  if( *tabindex >= pwl->N )
    return MBK_PWL_ERROR ;

  /* On utilise le dernier intervalle connu */
  if( *tabindex >= 0 ) {
    
    /* Détermination de la borne sup de cet intervalle */
    if( *tabindex == pwl->N - 1 )
      x1 = pwl->X1 ;
    else
      x1 = pwl->DATA[ *tabindex + 1 ].X0 ;

    /* On vérifie si on est dans cet intervalle */
    if( x >= pwl->DATA[ *tabindex ].X0 && x <= x1 ) {
      ret = MBK_PWL_FOUND ;
    }
    else {
      /* On détermine le sens de la recherche et l'intervalle de départ */
      if( x < pwl->DATA[ *tabindex ].X0 ) {
        if( *tabindex > 0 ) 
          (*tabindex)--;
        dir = -1 ;
      }
      else {
        if( *tabindex < pwl->N-1 ) 
          (*tabindex)++;
        dir = +1 ;
      }
    }
  }
  else {
    /* Sinon on se positionne sur le premier intervalle */
    *tabindex = 0 ;
    dir = +1 ;
  }

  /* Teste si la solution recherchée est en dehors des intervalles */
  if( ret == MBK_PWL_ERROR ) {
  
    if( x < pwl->DATA[0].X0 ) {
      *tabindex = 0 ;
      ret = MBK_PWL_EXTPL ;
    }
    
    if( x > pwl->X1 ) {
      *tabindex = pwl->N-1 ;
      ret = MBK_PWL_EXTPL ;
    }
  
  }

  /* Recherche de l'intervale */
  if( ret == MBK_PWL_ERROR ) {
  
    while( 1 ) {
      /* Détermination de la borne sup de cet intervalle */
      if( *tabindex == pwl->N - 1 )
        x1 = pwl->X1 ;
      else
        x1 = pwl->DATA[ *tabindex + 1 ].X0 ;

      if( x >= pwl->DATA[ *tabindex ].X0 && x <= x1 )
        break;

      *tabindex = *tabindex + dir ;
    }
    ret = MBK_PWL_FOUND ;
  }

  *y = pwl->DATA[ *tabindex ].A * x + pwl->DATA[ *tabindex ].B ;
  return ret;
}

/******************************************************************************\
mbk_dichotomie()

Résolution d'une équation par dichotomie f(x)=0.

fn :       Fonction à résoudre. Code de retour : 1 ok, 0, ko.
fnd :      Sa dérivée, utilisée uniquement si MBK_DICHO_DERIV.
data :     Les paramètres utilisés par fn.
*x0, *x1 : Les bornes de l'interval initial. A la fin de l'execution, elles
           contiennent l'interval final.
mode :     MBK_DICHO_EQUAL
           MBK_DICHO_PROP
           MBK_DICHO_NR
           MBK_DICHO_DERIV  (fnd requiered).
*itmax :   Nombre maximum d'itérations, 0=illimité. A la fin de l'execution,
           contient le nb d'itérations effectivement réalisées.
epsilonx : largeur minimale de l'interval x (DBL_MAX si pas ce critère).
epsilony : ecart absolue de la solution (DBL_MAX si pas ce critère).
*x :       Solution.

l'algo s'arrète dès que la condition sur epsilonx OU epsilony est rencontrée.

Code de retour.

MBK_DICHO_OK    : La solution est trouvée.
MBK_DICHO_ERRFN : La fonction fn ou fnd a renvoyée 0.
MBK_DICHO_MAXIT : Le nombre maximum d'itération est atteind.
MBK_DICHO_ERR :   Autres erreurs.
\******************************************************************************/
int mbk_dichotomie( int   (*fn)(void*, double, double*),
                    int   (*fnd)(void*, double, double*),
                    void   *data,
                    double  *x0,
                    double  *x1,
                    char    mode,
                    int    *itmax,
                    double  epsilonx,
                    double  epsilony,
                    double *x
                  )
{
  int nbit = 0 ;
  double y0, y1, y ;
  char good ;
  int why = MBK_DICHO_ERR ;
  double lx, ly, tx ;
 
  fnd = NULL;
  good = 1 ;
  
  if( good ) {
    if( !fn( data, *x0, &y0 ) ) {
      good = 0 ;
      why = MBK_DICHO_ERRFN;
    }
  }
  
  if( good ) {
    if( !fn( data, *x1, &y1 ) ) {
      good = 0 ;
      why = MBK_DICHO_ERRFN;
    }
  }

  if( good ) {
    if( ( y0 > 0.0 && y1 > 0.0 ) || ( y0 < 0.0 && y1 < 0.0 ) ) {
      good = 0 ;
      why = MBK_DICHO_ERR ;
    }
  }

  while( good )
  {
    nbit++ ;

    switch( mode ) {
    
    case MBK_DICHO_EQUAL :
      *x = ( *x1 + *x0 ) / 2.0 ;
      break ;
      
    case MBK_DICHO_PROP :
      *x = *x0 - y0 * ( *x1 - *x0 ) / ( y1-y0 );
      break ;
      
    case MBK_DICHO_DERIV :
      /* not implemented */
      why = MBK_DICHO_ERR;
      good = 0;
      
    case MBK_DICHO_NR :
    
      if( nbit > 1 ) {
      
        if( lx < *x0 ) 
          tx = *x0 - y0 * ( *x0 - lx ) / ( y0 - ly ) ;
        else 
          tx = *x1 - y1 * ( *x1 - lx ) / ( y1 - ly ) ;

        if( tx > *x0 && tx < *x1 )
          *x = tx ;
        else
        *x = ( *x1 + *x0 ) / 2.0 ;
      }
      else 
        *x = ( *x1 + *x0 ) / 2.0 ;

      break ;

    default :
      return MBK_DICHO_ERR;
    }

    if( good ) {
      if( !fn( data, *x, &y ) ) {
        good = 0 ;
        why = MBK_DICHO_ERRFN;
      }
    }
   
    if( good ) {
      if( y > 0.0 && y0 > 0.0 ) { lx = *x0 ; ly = y0 ; *x0=*x ; y0 = y ; }
      if( y < 0.0 && y0 < 0.0 ) { lx = *x0 ; ly = y0 ; *x0=*x ; y0 = y ; }
      if( y > 0.0 && y1 > 0.0 ) { lx = *x1 ; ly = y1 ; *x1=*x ; y1 = y ; }
      if( y < 0.0 && y1 < 0.0 ) { lx = *x1 ; ly = y1 ; *x1=*x ; y1 = y ; }

      if( epsilonx != DBL_MAX && ( *x1 - *x0 ) < epsilonx ) {
        why = MBK_DICHO_OK ;
        good = 0 ;
      }
      
      if( epsilony != DBL_MAX ) {
        if( y < epsilony && y > -epsilony ) {
          why = MBK_DICHO_OK ;
          good = 0 ;
        }
      }
    }
   
    if( good ) {
      if( *itmax && nbit >= *itmax ) {
        why = MBK_DICHO_MAXIT ;
        good = 0 ;
      }
    }
  }

  /* la solution suivante est fausse si l'écart x1-x0 est
  grand mais que le x choisi tombe juste */
  // *x = ( *x0 + *x1 )/ 2.0 ;
  
  *itmax = nbit ;
  return why ;
}

/******************************************************************************\
mbk_make_tab_equi
Converti un tableau de valeur (x,y) à pas variable en x en un tableau dont les 
x sont répartis de manière equidistants.
\******************************************************************************/
void mbk_make_tab_equi( double *tabinx, 
                        double *tabiny, 
                        int nbin, 
                        double *tabout, 
                        int nbout 
                      )
{
  double pas;
  double x0;
  int    nin;
  int    nout;
  double x;
  
  pas = ( tabinx[nbin-1] - tabinx[0] ) / ( ((double)nbout) - 1.0 );
  x0  = tabinx[0] ;
  nin = 0;
  
  for( nout=0 ; nout < nbout ; nout++ ) {
  
    if( nout ) {
      x = ((double)nout)*pas + x0 ;
    }
    else {
      /* pour éviter des erreurs d'arrondis qui nous ferait rater le 1° point */
      x = x0;
    }
    
    /* On se positionne sur le premier "bon" intervale */
    while( nin < nbin && tabinx[ nin ] < x ) 
      nin++ ;

    if( nin == 0 )
      tabout[ nout ] = tabiny[nin];
    else
      tabout[ nout ] = ( x - tabinx[nin-1] ) / ( tabinx[nin] - tabinx[nin-1] ) *
                       ( tabiny[nin] - tabiny[nin-1] ) +
                       tabiny[nin-1] ;
  }
}

/******************************************************************************\
Fonctions d'approximation de nombres par des intervalles de précision controlée.

  precis : precision demandée. ex 5% -> precis = 0.05.

mbk_float_approx_interval()
  Renvoie un entier représentant l'intervalle d'un nombre x strictement positif.
  L'intervale renvoyé peut être positif (x>1) ou négatif(x<1).
  Renvoie INT_MAX (limits.h) si une erreur est survenue.

mbk_float_approx_limits()
  Calcule dans ll et lh les limites de l'intervalle correspondant à n.

exemple :

mbk_float_approx_interval( 5.32, 0.05 ) = 36
mbk_float_approx_limits( 36, 0.05 ) = [ 5.2533 ; 5.5160 ]
\******************************************************************************/

int mbk_float_approx_interval( float x, float precis )
{
  float r ;
  int   n ;
 
  if( x <= 0.0 ) {
    return INT_MAX ;
  }
  
  r = 1.0 + log( x ) / log( 1.0 + precis ) ;
  n = ceil( r ) ;
 
  return n ;
}

void mbk_float_approx_limits( int n, float precis, float *ll, float *lh )
{
  if( n == INT_MAX ) {
    if( ll )
      *ll = 0.0 ;
    if( lh )
      *lh = 0.0 ;
  }
  else {
    if( ll )
      *ll = pow( 1.0 + precis, ((float)n)-2.0 );
    if( lh )
      *lh = pow( 1.0 + precis, ((float)n)-1.0 );
  }
}

float mbk_float_approx_middle( float x, float precis )
{
    int n;
    float ll,lh;
    if(( n = mbk_float_approx_interval( x, precis )) == INT_MAX){
        return x;
    }else{
        mbk_float_approx_limits( n, precis, &ll, &lh );
        return ((ll+lh)/2.0);
    }
}

long mbk_long_round(double val)
{
  long low;
  low=floor(val);
  if (val-low>0.5) return low+1;
  else return low;
}

/* compute the error between curve a (the reference) and the curbe b (the
test curve). Error is S(b-a)/S(a) */

double mbk_compute_curve_error( int na, double *xa, double *ya,
                                int nb, double *xb, double *yb )
{
  int pa ;
  int pb ;
  double x1, x2 ;
  double ya1, ya2 ;
  double yb1, yb2 ;
  double deltas, refs ;
  double err ;
  char start, end ;

  if( na<2 || nb<2 || !xa || !ya || !xb || !yb )
    return -1.0 ;

  pa      = 0 ;
  pb      = 0 ;
  deltas  = 0.0 ;
  refs    = 0.0 ;

  while( pa < na && pb < nb ) {
  
    /* get the first x */
    if( xa[pa] < xb[pb] ) {
      x1 = xa[pa] ; 
      start = 'a' ;
    }
    else {
      x1 = xb[pb] ;
      start = 'b' ;
    }

    /* check x against the origin of the curves */
    if( x1 < xa[0] ) {
      pb++ ;
      continue ;
    }

    if( x1 < xb[0] ) {
      pa++ ;
      continue ;
    }
   
    /* get the second x */
    if( start=='a' ) {

      if( pa == na-1 )
        break ;
        
      if( xa[pa+1] < xb[pb] ) {
        x2 = xa[pa+1] ;
        end = 'a' ;
      }
      else {
        x2 = xb[pb] ;
        end = 'b' ;
      }
    }
    else {

      if( pb == nb-1 )
        break ;

      if( xb[pb+1] < xa[pa] ) {
        x2 = xb[pb+1] ;
        end = 'b' ;
      }
      else {
        x2 = xa[pa] ;
        end = 'a' ;
      }
    }

    /* possible case : x1=x2 */
    if( x2 > x1 ) {

      /* compute the first y for the two curves */
      if( start == 'a' ) {
        ya1 = ya[pa] ;
        yb1 = (x1-xb[pb-1])/(xb[pb]-xb[pb-1])*(yb[pb]-yb[pb-1])+yb[pb-1] ;
      }
      else {
        yb1 = yb[pb] ;
        ya1 = (x1-xa[pa-1])/(xa[pa]-xa[pa-1])*(ya[pa]-ya[pa-1])+ya[pa-1] ;
      }

      /* compute the second y for the two curves */
      if( end == 'a' ) {
        ya2 = ya[pa] ;
        yb2 = (x2-xb[pb])/(xb[pb+1]-xb[pb])*(yb[pb+1]-yb[pb])+yb[pb] ;
      }
      else {
        yb2 = yb[pb] ;
        ya2 = (x2-xa[pa])/(xa[pa+1]-xa[pa])*(ya[pa+1]-ya[pa])+ya[pa] ;
      }

      /* compute the surface between the two curves */
      
      deltas = deltas + /* 0.5* */ fabs( ((yb1-ya1)+(yb2-ya2))*(x2-x1) );
      refs   = refs   + /* 0.5* */ fabs( (ya1+ya2)*(x2-x1) );
    }

    /* move to the next point */
    if( xa[pa] < xb[pb] )
      pa++ ;
    else
      pb++ ;
  }

  if( refs > 0.0 )
    err = deltas/refs ;
  else {
    if( deltas > 0.0 )
      err = 1.0 ;
    else
      err = 0.0 ;
  }

  return err ;
}

int mbk_cmpdouble(double a, double b, double precision)
{
  int expa, expb;
  double mta, mtb;
  long ia, ib;

  if (a<0 && b>0) return -1;
  if (a>0 && b<0) return 1;

  mta=frexp(a, &expa);
  mtb=frexp(b, &expb);
//  printf("%d %d\n", expa, expb);
  if (expa<expb) return -1;
  if (expa>expb) return 1;
//  printf("%.20g %.20g\n", mta, mtb);

//  printf("%.20g\n",mta*pow(2, expa));
//  printf("%.20g\n",mtb*pow(2, expb));
  ia=(long)(mta*precision+0.5);
  ib=(long)(mtb*precision+0.5);
//  printf("%ld %ld\n",ia,ib);
  if (ia<ib) return -1;
  if (ia>ib) return 1;
  return 0;
}

double mbk_rounddouble(double a, double precision)
{
  int expa;
  double entier, fraction, mul;
  double ia;
  
  if (a==0) return a;
  fraction=modf(a, &entier);
//  printf("f=%.20g\n",fraction);
  if (entier==0)
  {
    expa=-(int)log10(fabs(fraction));
    mul=pow(10, expa);
//    printf("mul=%.20g , e=%d\n",mul,expa);
    precision*=mul;
  }
  ia=floor(fraction*precision+0.5)/precision;
  return entier+ia;
}

