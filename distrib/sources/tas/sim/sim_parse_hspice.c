#include <alloca.h>
#include SIM_H
#include "sim.h"
#define SIMBUFSIZE 10240


#define SIM_SEARCH_START 1
#define SIM_SEARCH_LABEL 2
#define SIM_SEARCH_NAME  3
#define SIM_SEARCH_DATA  4

/* format de fichier

argv="qgi(mn0)"
.print dc qgi(mn0)

        volt    qgi        
                    mn0    
    0.          5.215e-16  
    0.10000     5.542e-16  
    0.20000     5.854e-16  

argv="vdnsat"
.print dc i(vdnsat) i(vdnres) i(vdnvgs) i(vdpsat)

        volt    current      current      current      current    
                  vdnsat       vdnres       vdnvgs       vdpsat   
    0.          1.000e-11      0.           0.        -1.000e-11  
    0.10000     1.004e-11    4.169e-05    2.398e-05   -1.000e-11  

argv="s0"
.print tran v(s0) v(s1) v(s2) v(s3)
        time    voltage      voltage      voltage      voltage    
                    s0           s1           s2           s3     
    0.            0.           5.0000       0.           5.0000   
    1.00000p     23.9840m      5.0001     479.0898n      5.0000   
    2.00000p     47.9680m      5.0002       1.0967u      5.0000   

argv="qdint_0"
.print dc qdint_0=PAR(`qd(mn0)-(vds(mn0)-vgs(mn0))*COVLGD(mn0)')
        volt      qdint_0  
                           
    0.            0.       
    0.10000       0.       
    0.20000    -1.134e-16  

Hypothèse : l'ordre d'apparition des résultat est le meme que celui des .print.
*/

int sim_get_number( char *s, double *n )
{
  char *e ;
  double m ;
  
  *n = strtod( s, &e );

  if( *e ) {
    m = 0.0 ;
    if( strcasecmp( e, "a" )==0 ) m=1e-18 ;
    if( strcasecmp( e, "f" )==0 ) m=1e-15 ;
    if( strcasecmp( e, "p" )==0 ) m=1e-12 ;
    if( strcasecmp( e, "n" )==0 ) m=1e-9 ;
    if( strcasecmp( e, "u" )==0 ) m=1e-6 ;
    if( strcasecmp( e, "m" )==0 ) m=1e-3 ;
    if( strcasecmp( e, "k" )==0 ) m=1e+3 ;
    if( strcasecmp( e, "meg" )==0 ) m=1e+6 ;
    if( strcasecmp( e, "x" )==0 ) m=1e+6 ;
    if( strcasecmp( e, "mi" )==0 ) m=25.4e+6 ;
    if( strcasecmp( e, "g" )==0 ) m=1e+9 ;

    if( m==0.0 )
      return 0 ;

    *n = *n * m ;
  }

  return 1 ;
}

void sim_parse_hspice(char *filename, char *argv[], int nbx, int nby, double **tab)
{
  char   buffer[ SIMBUFSIZE ];
  FILE  *file ;
  int    state ;
  char  *separ = " \n\r\t" ;
  char **label ;
  char **name ;
  int    i ;
  int    n ;
  int    ncol ;
  int    idparse ;
  char  *pt ;
  int    lgn ;
  
  file = mbkfopen( filename, NULL, "r" );
  if( !file ) {
    avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, filename);
    EXIT(1);
  }

  /* reparti les argv en couple label/name :
     s0       -> NULL/s0
     qgi(mn0) -> qgi/mn0
     le parser valid d'abord la ligne des label, puis la ligne des name.
  */
  
  label = (char**)alloca( sizeof(char*)*nbx );
  name  = (char**)alloca( sizeof(char*)*nbx );

  for( i=0 ; i<nbx ; i++ ) {
    if( strchr( argv[i], '(' ) ) {
      strcpy( buffer, argv[i] );
      pt = strtok( buffer, "(" );
      label[i] = alloca( (strlen( pt )+1)*sizeof(char) );
      strcpy( label[i], pt );
      pt = strtok( NULL, ")" );
      name[i] = alloca( (strlen(pt)+1)*sizeof(char) );
      strcpy( name[i], pt );
    }
    else {
      label[i] = NULL ;
      name[i] = alloca( (strlen(argv[i])+1)*sizeof(char) );
      strcpy( name[i], argv[i] );
    }
  }
  
  state = SIM_SEARCH_START ;
  idparse = 0 ;

  do {
    pt = fgets( buffer, SIMBUFSIZE, file );
    if( !pt )
      break ;
      
    pt = strtok( buffer, separ );

    if( pt ) {

      switch( state ) {
      
      case SIM_SEARCH_START :
        if( strcmp( buffer, "x" )==0 ) 
          state = SIM_SEARCH_LABEL ;
        break ;
        
      case SIM_SEARCH_LABEL :
        if( strcmp( pt, "volt" )==0 ||
            strcmp( pt, "time" )==0    ) {
          ncol = 0;
          while( (pt = strtok( NULL, separ )) && ncol < nbx ) {
            if( label[idparse+ncol] ) {
              if( strcasecmp( label[idparse+ncol], pt ) ) {
                printf( "label %s not recognized !\n", pt );
                EXIT(1);
              }
              state = SIM_SEARCH_NAME ;
            }
            else {
              if( strcmp( pt, "voltage" )==0 ||
                  strcmp( pt, "current" )==0    )
                state = SIM_SEARCH_NAME ;
              else {
                if( strcasecmp( name[idparse+ncol], pt )==0 )
                  state = SIM_SEARCH_DATA ;
                else {
                  if( strcasecmp( label[idparse+ncol], pt ) ) {
                    printf( "label %s not recognized !\n", pt );
                    EXIT(1);
                  }
                }
              }
            }
            ncol++ ;
          }
          lgn = 0 ;
        }
        break ;

      case SIM_SEARCH_NAME :
        n = 0 ;
        do {
          n++ ;
        }
        while( (pt = strtok( NULL, separ )) && n<nbx );
        if( n != ncol ) {
          printf( "parameter label and name mismatch\n" );
          EXIT(1);
        }
        state = SIM_SEARCH_DATA ;
        break ;

      case SIM_SEARCH_DATA :
        if( strcmp( pt, "y" )==0 ) {
          idparse = idparse + ncol ;
          state = SIM_SEARCH_START ;
        }
        else {
          n=0 ;
          while( (pt = strtok( NULL, separ )) && n<nbx )  {
            if( lgn < nby ) {
              if( !sim_get_number( pt, &(tab[idparse+n][lgn]) ) ) {
                printf( "an error occured when reading data\n" );
                EXIT(1);
              }
            }
            n++ ;
          }
          lgn++ ;
          if( n != ncol ) {
            printf( "number of title column and data column missmatch\n" );
            EXIT(1 );
          }
        }
        break ;
      }
    }
  }
  while( !feof( file ) );

  fclose( file );
    
}
