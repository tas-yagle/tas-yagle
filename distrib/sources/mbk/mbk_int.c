/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*******************************************************************************
*                                                                              *
*  Tool        : Spice parser / driver v 7.00                                  *
*  Author(s)   : Gregoire AVOT                                                 *
*  Updates     : March, 18th 1998                                              *
*                                                                              *
*******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mut_lib.h"
#include "mbk_int.h"

#ifdef ENABLE_STATS
long ___intcnt=0;
#endif

mbk_tableint*       creatmbk_tableint()
{
  mbk_tableint	*new;
  int		 i;

  new = (mbk_tableint*) mbkalloc( sizeof( mbk_tableint ) * SPI_MBK_TABLEINTMAX );
#ifdef ENABLE_STATS
  ___intcnt+=sizeof( mbk_tableint ) * SPI_MBK_TABLEINTMAX;
#endif
  for( i = 0 ; i < SPI_MBK_TABLEINTMAX ; i++ )
  {
    new[ i ].index = 0;
    new[ i ].data  = -1;
    new[ i ].down  = NULL;
  }  

  return( new );
}

int             setmbk_tableint( table, value, data )
mbk_tableint *table;
int value;
int data;
{
  int           p;
  int           niveau;

  niveau = 3;

  while( 1 )
  {
    p = ( value >> ( niveau * 8 ) ) & ( SPI_MBK_TABLEINTMASK );

    if( table[ p ].index == value )
    {
      /* L'element existe déjà : on le remplace */
      table[ p ].data = data;
      return( 0 );
    }

    if( table[ p ].index == 0 )
    {
      /* Nouvel élément */
      table[ p ].index = value;
      table[ p ].data  = data;
      return( 1 );
    }

    if( table[ p ].down == NULL )
      table[ p ].down = creatmbk_tableint();

    niveau--;
 
    table = table[p].down;
  }
}

void             removembk_tableint( table, value )
mbk_tableint *table;
int value;
{
  int           p;
  int           niveau;

  niveau = 3;

  while( 1 )
  {
    p = ( value >> ( niveau * 8 ) ) & ( SPI_MBK_TABLEINTMASK );

    if( table[ p ].index == value )
    {
      /* L'element existe déjà : on le vire */
      table[ p ].index = 0;
      table[ p ].data = -1;
      return;
    }

    if( table[ p ].down != NULL )
      table = table[ p ].down ;
    else
      return;

    niveau--;
  }
}
int           tstmbk_tableint( table, value )
mbk_tableint        *table;
int              value;
{
  int       p;
  int       niveau;

  niveau = 3;

  while( 1 )
  {
    p = ( value >> ( niveau * 8 ) ) & (SPI_MBK_TABLEINTMASK);

    if( table[ p ].index == value ) 
      return( table[ p ].data );

    if( table[ p ].down == NULL )
      return( -1 );

    niveau--;
 
    table = table[ p ].down;
  }
}

void            freembk_tableint( table )
mbk_tableint        *table;
{
  int           i;

  for( i = 0 ; i < SPI_MBK_TABLEINTMAX ; i++ )
  {
    if( table[ i ].down )
      freembk_tableint( table[ i ].down );
  }
  mbkfree( table );
#ifdef ENABLE_STATS
  ___intcnt-=sizeof( mbk_tableint ) * SPI_MBK_TABLEINTMAX;
#endif
}

int             mbk_scanint( table, n )
mbk_tableint        *table;
int             n;
{
  int           i=0, l=0 ;
  int           n3, n2, n1, n0 ;
  mbk_tableint      *t=NULL, *t3, *t2, *t1, *t0 ;

  if( n == 0 )
  {
    for( i = 0 ; i < SPI_MBK_TABLEINTMAX ; i++ )
    {
      if( table[i].index )
        return( table[i].index );
    }
    return( 0 );
  }

  n0 =   n         & SPI_MBK_TABLEINTMASK;
  n1 = ( n >> 8  ) & SPI_MBK_TABLEINTMASK;
  n2 = ( n >> 16 ) & SPI_MBK_TABLEINTMASK;
  n3 = ( n >> 24 ) & SPI_MBK_TABLEINTMASK;

  /* recherche de l'element n */

  t3 = NULL;
  t2 = NULL;
  t1 = NULL;
  t0 = NULL;

  if( table[ n3 ].index == n )
  {
    t  = table;
    l  = n3;
    t3 = t;
  }
  else
  if( table[ n3 ].down[ n2 ].index == n )
  {
    t =  table[ n3 ].down ;
    l =  n2;
    t3 = table;
    t2 = t;
  }
  else
  if( table[ n3 ].down[ n2 ].down[ n1 ].index == n)
  {
    t  = table[ n3 ].down[ n2 ].down;
    l  = n1;
    t3 = table;
    t2 = table[ n3 ].down;
    t1 = t;
  }
  else
  if( table[ n3 ].down[ n2 ].down[ n1 ].down[ n0 ].index == n )
  {
    t  = table[ n3 ].down[ n2 ].down[ n1 ].down;
    l  = n0;
    t3 = table;
    t2 = table[ n3 ].down;
    t1 = table[ n3 ].down[ n2 ].down;
    t0 = t;
  }

  /* Descent d'un niveau */

  if( t[ l ].down )
  {
    t = t[ l ].down;
    
    for( i = 0 ; i < SPI_MBK_TABLEINTMAX ; i++ )
    {
      if( t[ i ].index )
        return( t[ i ].index );
    }
    /* jamais executé */
  }

  while( 1 )
  {
    /* Parcour un niveau horizontalement */

    for( i = l + 1 ; i < SPI_MBK_TABLEINTMAX ; i++ )
    {
      if( t[ i ].index )
        return( t[ i ].index );
    }

    /* Remonte les niveaux */

    if( t == t0 )
    {
      t = t1;
      l = n1;
    }
    else
    if( t == t1 )
    {
      t = t2;
      l = n2;
    }
    else
    if( t == t2 )
    {
      t = t3;
      l = n3;
    }
    else
    if( t == t3 )
      return( 0 );
  }
  /* Cette boucle se finie en interne par un return */
}
