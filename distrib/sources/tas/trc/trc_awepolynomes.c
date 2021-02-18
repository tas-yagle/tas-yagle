/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : RCX - AWE support.                                          */
/*    Fichier : trc_awepolynomes.c                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.7 $
Author   : $Author: gregoire $
Date     : $Date: 2002/10/29 11:10:28 $

*/

#include "trc.h"
#include "trc_awepolynomes.h"

#define POLYMAXITER 40

#ifndef MAX
  #define MAX(a,b) ((a)>(b)?(a):(b))
#endif

RCXFLOAT poly_gauss( RCXFLOAT*, int, RCXFLOAT, char* );
void  poly_display( RCXFLOAT*, int );

/******************************************************************************\
* Affiche un polynome de degrès n.                                             *
\******************************************************************************/

void poly_display( RCXFLOAT *poly, int n )
{
  int i;

  for( i=n ; i>= 0 ; i-- )
  {
    printf( " %+e", poly[i] );
    switch( i )
    {
    case 0:
      printf( "\n" );
      break;
    case 1:
      printf( "*x" );
      break;
    default:
      printf( "*x**%d", i );
    }
  }
}

/******************************************************************************\
* Récupère les n racines réelles d'un polynome à coéfficients réel de degrès   *
* n. poly[i] = coefficient de degrè n.                                         *
* Utilise l'algorithme de déflation combiné avec un algorithme de Gauss.       *
* Le tableau root contiendra les racines. Il doit faire n éléments             *
\******************************************************************************/

int poly_findroot( RCXFLOAT *poly, int n, RCXFLOAT *root )
{
  RCXFLOAT t[2] ;
  int   i, j, pt;
  RCXFLOAT r;
  char status;
  
  // printf( "    Recherche des racines pour le polynôme :\n" );
  for( i=n ; i>0 ; i-- )
  {
    // printf( "    Polynôme de degrè %d :\n", i );
    // poly_display( poly, i );

    r = poly_gauss( poly, i, 1e-6, &status );
    // printf( "    racine = %e\n", r );

    if( status == 'n' )
      return(0);

    root[i-1]=r;

    t[0]    = poly[i];
    poly[i] = 0.0;
    pt      = 1;
    for( j=i-1 ; j>=0 ; j-- )
    {
      t[pt]=poly[j];
      pt=(pt==0?1:0);
      poly[j] = t[pt]+r*poly[j+1];
    }
  }

  return(1);
}


/******************************************************************************\
* Récupère une racine réelle du polynome de degrès n. Ce polynome doit avoir   *
* n racines réelle. Il ne faut pas utiliser cet algorithme si le polynome      *
* ne comporte pas n racines réelles.                                           *
\******************************************************************************/

RCXFLOAT poly_gauss( RCXFLOAT *poly, int n, RCXFLOAT epsilon, char *status )
{
  RCXFLOAT  f0, f1;
  RCXFLOAT  x0, xs;
  int    i;
  int    r;
  RCXFLOAT  delta;

  // Cas triviaux

  if( n==1 ) {
    RCXFLOAT r;
    r = -poly[0]/poly[1];
    if( finite(r) ) {
      *status='y';
      return( r );
    } else {
      *status='n';
      return( 0.0 );
    }
  } else 
  if( n==2 ) {
    RCXFLOAT delta, a, b, c, r;
    a=poly[2];
    b=poly[1];
    c=poly[0];
    delta = b*b-4*a*c;
    r = - (b+sqrt(delta))/2/a;
    if( finite(r) ) {
      *status='y';
      return( r );
    } else {
      *status='n';
      return(0.0);
    }
  }
  
  xs = 0.0 ;/* solution estimée */
  r  = 1;

  do
  {
    x0 = xs ;
    f0=poly[0];
    for( i=1 ; i<=n ; i++ )
      f0 = f0 + poly[i]*pow(x0,i);

    f1=poly[1];
    for( i=2 ; i<=n ; i++ )
      f1 = f1 + i*poly[i]*pow(x0,i-1);

    xs = (f1*x0-f0)/f1;
    delta = fabs(xs-x0);
    r++;
  }
  while( fabs(xs-x0) > fabs(epsilon * xs)
         && r < POLYMAXITER                      );
  
  if( r >= POLYMAXITER )
    *status = 'n';
  else
    *status = 'y';
  return( xs );
}
