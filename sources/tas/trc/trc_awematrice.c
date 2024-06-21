/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : RCX - AWE support.                                          */
/*    Fichier : trc_awematrice.c                                            */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.9 $
Author   : $Author: gregoire $
Date     : $Date: 2005/06/01 16:52:56 $

*/

# include "trc.h"

void      mat_display( matrice* );

matrice *headmatrice = NULL;

matrice* mat_create( int ligne, int colonne )
{
  matrice *m;

  if( headmatrice )
  {
    m           = headmatrice;
    headmatrice = headmatrice->NEXT ;
    m->lin = ligne ;
    m->col = colonne ;
    return( m );
  }
  
  m = (matrice*) mbkalloc( sizeof( matrice ) );
  m->data = (RCXFLOAT*) mbkalloc( sizeof( RCXFLOAT ) * MAT_ALLOCLINE * MAT_ALLOCCOL );
  m->lin = ligne ;
  m->col = colonne ;

  return( m ); 
}

void mat_free( matrice *a )
{
  a->NEXT = headmatrice;
  headmatrice = a;
}


matrice* mat_dup( matrice *a )
{
  matrice *new;
  
  new = mat_create( a->lin, a->col );
  memcpy( new->data, a->data, a->lin * a->col * sizeof( RCXFLOAT ) );
  return( new );
}

void mat_sub( matrice *a, matrice *b, matrice *c )
{
  int lin, col ;
  for( col=0 ; col < a->col ; col++ )
  {
    for( lin=0 ; lin < a->lin ; lin++ )
      MATELEM( c,lin,col ) = MATELEM( a,lin,col ) - MATELEM( b,lin,col );
  }
}

void mat_mult(matrice *a, matrice *b, matrice *c )
{
  int lin, col, t ;

  for( col = 0 ; col < b->col ; col++ )
  {
    for( lin = 0 ; lin < a->lin ; lin++ )
    {
      MATELEM( c, lin, col ) = 0.0;
      for( t=0 ; t < a->col ; t++ )
        MATELEM( c, lin, col ) = MATELEM( c, lin, col ) + 
                                 MATELEM( a, lin, t ) * MATELEM( b, t, col );
    }
  }
}

void mat_display( matrice *a )
{
  int l, c;
  fflush( stdout );

  for( l = 0 ; l < a->lin ; l++ )
  {
    for( c = 0 ; c < a->col ; c++ )
      printf( " %+.10e", MATELEM( a, l, c ) );
    printf( "\n" );
  }
}

RCXFLOAT mat_sq( matrice *a )
{
  RCXFLOAT z=0.0;
  int col, lin;
  for( col=0 ; col < a->col ; col++ )
    for( lin=0 ; lin < a->lin ; lin++ )
      z = z + MATELEM( a, lin, col ) * MATELEM( a, lin, col );
  return( z );
}

int mat_solve( matrice *a, matrice *c, matrice *res )
{
  int n,l, k, x ;
  RCXFLOAT  m ;

  if( ( a->lin != a->col ) || ( c->col != 1 ) || c->lin != a->lin )
  {
    awe_error( 11, AVT_FATAL );
  }
  
  n = a->lin;
  
  /* diagonalisation du système d'équations */
  for( l = 0 ; l < n-1 ; l++ )
  {
    for( k = l+1 ; k < n ; k++ )
    {
      m = MATELEM( a, k, l ) / MATELEM( a, l, l );
      if( !isfinite( m ) )
        return(0);
      for( x = 0 ; x < n ; x++ )
      {
        MATELEM( a, k, x ) = MATELEM( a, k, x ) - m * MATELEM( a, l, x ); 
      }
      MATELEM( c, k, 0 ) = MATELEM( c, k, 0 ) - m * MATELEM( c, l, 0 );
    }
  }

  /* calcul du résultat */
  for( l = n-1 ; l >= 0 ; l-- )
  {
    m = MATELEM( c, l, 0 ) ;
    for( k = l+1 ; k < n ; k++ )
      m = m - MATELEM( a, l, k ) * MATELEM( res, k, 0 );
    MATELEM( res, l, 0 ) = m / MATELEM( a, l, l );
    if( !isfinite(  MATELEM( res, l, 0 ) ) )
      return(0);
  }

  return(1);
}
