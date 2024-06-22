#include AVT_H
#include API_H
#include MUT_H
#include MLO_H
#include RCN_H
#include MSL_H

#define API_USE_REAL_TYPES
#include "spi_API.h"


void drive_capacitance( FILE *ptf, int n1, int n2, float capa, int *idx )
{
  spi_print( ptf, 
           "C%d %d %d %g\n", 
           (*idx)++, 
           n1, 
           n2, 
           (capa < CAPAMINI ? CAPAMINI : capa)*1e-12
         );
}

void drive_resistance( FILE *ptf, int n1, int n2, float resi, int *idx )
{
  spi_print( ptf, 
           "R%d %d %d %g\n", 
           (*idx)++, 
           n1, 
           n2, 
           resi < RESIMINI ? RESIMINI : resi
         );
}

int spi_DriveSignal( losig_list *losig )
{
  char         *signame;
  FILE         *ptf;
  char          filename[1024];
  ptype_list   *ptl;
  chain_list   *chain;
  locon_list   *locon;
  num_list     *node;
  lowire_list  *scanwire;
  loctc_list   *loctc;
  int           idxresi=1, idxcapa=1;
  char         *tabnode;
  int           n;
  
  if( !((losig_list*)losig) ) return 0;
  if( !((losig_list*)losig)->PRCN ) return 0;

  signame = getsigname( ((losig_list*)losig) );

  sprintf( filename, "%s.spi", signame );

  ptf = mbkfopen( filename, NULL, "w" );
  if( !ptf ) return 0;

  spi_print( ptf, "* Description for signal %s\n\n", signame );

  /* Drive information about interface */

  spi_print( ptf, "* This signal is connected to :\n" );

  ptl = getptype( ((losig_list*)losig)->USER, LOFIGCHAIN );
  if( ptl ) {
    for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)chain->DATA;
      spi_print( ptf, "* - Connector " );
      switch( locon->TYPE ) {
      case 'I':
        spi_print( ptf, 
                 "on instance %s : %s\n", 
                 ((loins_list*)locon->ROOT)->INSNAME, 
                 locon->NAME 
               );
        break;
      case 'T':
        spi_print( ptf, 
                 "on transistor %s : %s\n", 
                 ((lotrs_list*)locon->ROOT)->TRNAME, 
                 locon->NAME 
               );
        break;
      case 'E':
        spi_print( ptf, 
                 "on figure : %s\n", 
                 locon->NAME 
               );
        break;
      default :
        spi_print( ptf, 
                 " : %s\n", 
                 locon->NAME 
               );
        break;
      }
      
      if( locon->PNODE ) {
        spi_print( ptf, "*    nodes : " ) ;
        for( node = locon->PNODE ; node ; node = node->NEXT )
          spi_print( ptf, "%ld ", node->DATA ) ;
        spi_print( ptf, "\n" );
      }
      else {
      }
    }
  }
  spi_print( ptf, "\n\n" );
  
  /* Drive de l'interface */
  
  tabnode = (char*)mbkalloc( sizeof(char)*((losig_list*)losig)->PRCN->NBNODE) ;
  for( n=0 ; n<((losig_list*)losig)->PRCN->NBNODE ; n++ ) tabnode[n]=0;
  
  spi_print( ptf, ".subckt %s ", signame );

  ptl = getptype( ((losig_list*)losig)->USER, LOFIGCHAIN );
  if( ptl ) {
    for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)chain->DATA;
      if( locon->PNODE ) {
        for( node = locon->PNODE ; node ; node = node->NEXT ) {
          if( tabnode[ node->DATA - 1 ] == 0 ) {
            spi_print( ptf, "%ld ", node->DATA ) ;
            tabnode[ node->DATA - 1 ] = 1;
          }
        }
      }
    }
  }
  spi_print( ptf, "\n\n" );

  /* Drive des résistances */
  
  for( scanwire = ((losig_list*)losig)->PRCN->PWIRE ; 
       scanwire ; 
       scanwire = scanwire->NEXT
     ) {
    drive_resistance( ptf, 
                      scanwire->NODE1, 
                      scanwire->NODE2, 
                      scanwire->RESI, 
                      &idxresi 
                    );
    if( scanwire->CAPA > 0.0 ) {
      drive_capacitance( ptf, 
                         scanwire->NODE1, 
                         0,
                         scanwire->CAPA/2.0, 
                         &idxcapa 
                       );
      drive_capacitance( ptf, 
                         scanwire->NODE2, 
                         0,
                         scanwire->CAPA/2.0, 
                         &idxcapa 
                       );
    }
  }
  spi_print( ptf, "\n" );

  /* Drive des capacités */

  for( chain = ((losig_list*)losig)->PRCN->PCTC ;
       chain ; 
       chain = chain->NEXT
     ) {
    loctc = (loctc_list*)chain->DATA;
      drive_capacitance( ptf,
                         rcn_ctcnode( loctc, losig ),
                         0,
                         loctc->CAPA, 
                         &idxcapa 
                       );
  }
  spi_print( ptf, "\n" );

  spi_print( ptf, ".ends\n" );
  fclose( ptf );

  mbkfree( tabnode );
  return 1;
}

void drive_spice (lofig_list *fig, char *filename)
{
    FILE *f = fopen (filename, "w+");
    spicesavelofigsinfile (addchain (NULL, fig), f);
}
