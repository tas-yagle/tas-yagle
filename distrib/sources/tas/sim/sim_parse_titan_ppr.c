/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_parse_titan_ppr.c                                        */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire Avot                                              */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"
#define SIMBUFSIZE 10240

/*

Un fichier Titan se décrit sous la forme :

      | infos non interprétées
      | ....
      | infos non interprétées
      | TIME\n                    <---- Repéré comme le début des infos à parser
      | V(print1)\n               <---- Les mesures, une par ligne
      | V(print2)\n
      | V(print3)\n
      | 0.000 0.000 0.000 0.000\n <---- Les datas, présentées par colonnes.
      | ...
      | 9.999 9.999 9.999 9.999\n <---- La dernière ligne du fichier.


Pb : le STEP sur fichier ne correspond pas forcement à celui qui a été
demandé. Par exemple, on a dans le fichier de commande :

.tran 1e-12s 1e-07s

Et dans le fichier data on a :

      | *** SIM LIBRARY ***    15Oct03     17:33:23
      |           TR                 0   70.0000
      | C        OUTPUT FORMATS:     (1P, E13.5)         (1P, E13.05)
      | C        XRANGE:              0.0000000E+00  1.0000000E-07
      |     3
      |     0
      | TIME
      | V(S1.XTEMPLATE)
      | V(S1IN.XTEMPLATE)
      |   0.00000E+00  2.03940E-07  1.35000E+00
      |   2.00000E-12  2.03171E-07  1.35000E+00
      |   4.00000E-12  2.04340E-07  1.35000E+00

*/

#define NBALLOC 16

void sim_parse_titan_ppr( char *filename, 
                          char *printname[], 
                          int   nbprint, 
                          int   nbdata, 
                          double **tabprint, 
                          double tmax, 
                          double step
                        )
{
  FILE    *file ;
  char     buffer[SIMBUFSIZE] ;
  char     ilabel[SIMBUFSIZE] ;
  char     vlabel[SIMBUFSIZE] ;
  char     olabel[SIMBUFSIZE] ;
  int     *id_spicetosim ;
  int     *id_simtospice ;
  double  *bufdata ;
  int      dataread ;
  char     intitle ;
  char     indata ;
  int      nlabel ;
  int      n ;
  char     breakonerror;
  int      sizebloc;
  double **datas;
  int      idxdatas;

  file = fopen( filename, "r" );
  if( !file ) {
    avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, filename);
//    fprintf( stderr, "\nSIM error : can't open file %s\n", filename );
    EXIT(1);
  }
 
  intitle      = NO ;
  indata       = NO ;
  breakonerror = NO ;
  nlabel   = 0 ; 
  dataread = 0 ;
  
  /* les tableaux id_spicetosim et id_simtospice permettent de faire la 
  correspondance entre la n-ieme serie de donnée lue dans le fichier spice et
  la m-ieme serie de donnée de sim. Dans le fichier résultat de Titan, il y
  a une série de données en plus : le temps.
  */
  
  id_spicetosim = (int*)mbkalloc( sizeof( int ) * ( 1 + nbprint) );
  id_simtospice = (int*)mbkalloc( sizeof( int ) * nbprint  );
  bufdata = NULL;
  datas = NULL;

  do
  {
    if( fgets( buffer, SIMBUFSIZE, file )==NULL ) {
      printf( "file parsed\n" );
      sim_fclose (file,filename);
      break;
    }
     
    if( intitle == NO && indata == NO ) {
      /* Cherche le début des résultats */
      if( strcmp( buffer, "TIME\n" )==0 || strcmp( buffer, "VAR\n" )==0  ) {
        intitle = YES ;
        id_spicetosim[nlabel]=999999999;
        nlabel++;
        continue ;
      }
    }

    if( intitle == YES ) {
    
      /* Retrouve le label */
      
      for( n=0 ; n < nbprint ; n++ ) {
        sprintf( vlabel, "v(%s)\n", printname[n] );
        sprintf( ilabel, "i(%s)\n", printname[n] );
        sprintf( olabel, "%s\n", printname[n] );
        if( strcasecmp( vlabel, buffer ) == 0 || 
            strcasecmp( ilabel, buffer ) == 0 ||
            strcasecmp( olabel, buffer ) == 0
            )  {
          id_simtospice[n]=nlabel;
          id_spicetosim[nlabel]=n;
          nlabel++;
          break;
        }
      }
      if( n < nbprint )
        continue;
     
      if( nlabel != nbprint+1 ) {
        fprintf( stderr, 
                 "\nSIM error : %d labels found vs %d labels requested.\n",
                 nlabel,
                 nbprint
               );
        breakonerror = YES ;
        break;
      }

      /* Fin des labels, début des séries de données */
      intitle = NO;
      indata  = YES;
      /* buffer de récupéraiton d'une ligne */
      bufdata = (double*)mbkalloc( sizeof( double ) * nlabel );
      /* tableau des tableaux des valeurs lues */
      datas   = (double**)mbkalloc( sizeof( double* ) * nlabel );
      sizebloc = NBALLOC;
      for( n=0 ; n < nlabel ; n++ )
        datas[n] = (double*)mbkalloc( sizeof( double ) * sizebloc );
      idxdatas = 0;
    }

    if( indata == YES ) {
      if( sim_get_ppr_data( buffer, nlabel, bufdata ) ) {
        if( idxdatas >= NBALLOC ) {
          sizebloc+=NBALLOC;
          for( n=0 ; n < nlabel ; n++ )
            datas[n] = (double*)mbkrealloc( datas[n],
                                            sizeof( double ) * sizebloc 
                                          );
          idxdatas = 0;
        }
          
        for( n=0 ; n < nlabel ; n++ )
          ( datas[n] )[ dataread ] = bufdata[ n ] ;
        dataread++;
        idxdatas++;
      }
    }
  }
  while( 1 );

  if( dataread > 0 ) {
    for( n = 1 ; n < nlabel ; n++ )
      mbk_make_tab_equi( datas[0], 
                         datas[n], 
                         dataread, 
                         tabprint[id_spicetosim[n]], 
                         nbdata 
                       );
  }
  else
    breakonerror = YES;

  if( bufdata )       mbkfree( bufdata );
  if( id_spicetosim ) mbkfree( id_spicetosim );
  if( id_simtospice ) mbkfree( id_simtospice );
  if( datas ) {
    for( n=0 ; n < nlabel ; n++ ) mbkfree( datas[n] );
    mbkfree( datas );
  }

  if( breakonerror == YES ) {
    EXIT(1);
  }
}

/* Lecture de n nombre réels dans la chaine string. Renvoie 1 si OK, 0 si non */

char sim_get_ppr_data( char *string, int n, double *data ) 
{
  char *pt, *e ;
  int  i;

  pt = string ;
  for( i=0 ; i< n ; i++ ) {
    data[i] = strtod( pt, &e );
    if( e == pt ) return 0;
    pt = e;
  }
  if( *pt !='\n' ) return 0;
  return 1;
}
