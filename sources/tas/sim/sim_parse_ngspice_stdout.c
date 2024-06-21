#include SIM_H
#include "sim.h"

#define NGSPICE_MAXLENGTH 1024

int ngspice_checkinput( char *test_string, char **label, int nblabel )
{
  char buffer[ NGSPICE_MAXLENGTH ] ;
  char *pt ;
  int i, l ;
  
  strcpy( buffer, test_string );

  pt = strtok( buffer, " " ) ;
  if( !pt ) return -1 ;
  if( strcasecmp( pt, "index" ) ) return -1 ;

  pt = strtok( NULL, " " ) ;
  if( !pt ) return -1 ;
  if( strcasecmp( pt, "time" )    &&
      strcasecmp( pt, "v-sweep" )    ) return -1 ;

  pt = strtok( NULL, " " ) ;
  if( !pt ) return -1 ;

  /* node voltage: v(label) */
  if( pt[0] == 'v' &&
      pt[1] == '('    ) {
    for( i=0 ; i<nblabel ; i++ ) {
      l = strlen( label[i] );
      if( strncasecmp( pt+2, label[i], l )== 0 ) {
        if( strcmp( pt+2+l, ")" ) == 0 )
          return i ;
      }
    }
  }
 
  /* current branch i(vmeas) -> vmeas#branch */
  for( i=0 ; i<nblabel ; i++ ) {
    l = strlen( label[i] );
    if( strncasecmp( pt, label[i], l ) == 0 &&
        strcasecmp( pt+l, "#branch" ) == 0     )
      return i ;
  }

  /* internal parameter : @tr[param] */
  if( pt[0] == '@' ) {
    for( i=0 ; i<nblabel ; i++ ) {
      if( strcasecmp( pt, label[i] ) == 0 )
        return i ;
    }
  }
  
  return -1 ;
}

int ngspice_readvalue( char *test_string, double *t, double *v )
{
  int nbread ;
  int index ;
  float ft, fv ;
  
  nbread = sscanf( test_string, "%d %g %g\n", &index, &ft, &fv );
  if( nbread != 3 )
    return 0 ;

  *t = ft ;
  *v = fv ;
  return 1 ;
}

void ngspice_store( double *store_tab, 
                    int     store_size, 
                    double  limit, 
                    double *tabtime, 
                    double *tabvoltage, 
                    int     tabsize 
                  )
{
  int i;
  for( i=0 ; i<tabsize && tabtime[i]<=limit ; i++ ) ;
  mbk_make_tab_equi( tabtime, tabvoltage, i, store_tab, store_size );
}

void sim_parse_ngspice_stdout( char *fileout,
                               char **argv,
                               int nbx,
                               int nby,
                               double **tab,
                               double limit
                             )
{
  FILE   *file ;
  char    buffer[ NGSPICE_MAXLENGTH ] ;
  int     id_reading ;
  int     id ;
  double *tab_time ;
  double *tab_voltage ;
  int     tab_size ;
  double  t ;
  double  v ;
  int     nbvalue ;
  int     ret ;
  int     line=0 ;
  char   *flag ;
  char    error ;
  
  file = mbkfopen( fileout, NULL, "r" );
  if( !file ) {
    avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, fileout);
    EXIT(1);
  }

  id_reading  = -1 ;
  tab_size    = nby ;
  tab_time    = mbkalloc( sizeof(double) * tab_size );
  tab_voltage = mbkalloc( sizeof(double) * tab_size );
  nbvalue     = 0 ;
  flag        = mbkalloc( sizeof( char ) * nbx );
  for( id=0 ; id<nbx ; id++ ) flag[id]=0 ;
  
  
  while( fgets( buffer, NGSPICE_MAXLENGTH, file ) ) {
    line++ ;
    
    id = ngspice_checkinput( buffer, argv, nbx );
    if( id>=0 ) {
      if( id != id_reading ) {
        if( id_reading >= 0 ) {
          ngspice_store( tab[ id_reading ], nby, limit, tab_time, tab_voltage, nbvalue );
          flag[ id_reading ] = 1 ;
        }
        nbvalue = 0 ;
      }
      id_reading = id ;
      continue ;
    }

    if( id_reading >= 0 ) {
      ret = ngspice_readvalue( buffer, &t, &v );
      if( ret ) {
        if( nbvalue >= tab_size ) {
          tab_size    = tab_size<<1 ;
          tab_time    = realloc( tab_time, sizeof( double ) * tab_size );
          tab_voltage = realloc( tab_voltage, sizeof( double ) * tab_size );
        }
        tab_time[nbvalue] = t ;
        tab_voltage[nbvalue] = v ;
        nbvalue++ ;
      }
    }
  }

  if( id_reading >= 0 ) {
    ngspice_store( tab[ id_reading ], nby, limit, tab_time, tab_voltage, nbvalue );
    flag[ id_reading ] = 1 ;
  }

  error = 0 ;
  for( id=0 ; id<nbx ; id++ ) {
    if( flag[id] != 1 ) {
      avt_errmsg( SIM_ERRMSG, "004", AVT_ERROR, argv[id], fileout );
      error = 1 ;
    }
  }
  if( error ) {
    avt_errmsg( SIM_ERRMSG, "005", AVT_FATAL );
  }
  mbkfree( tab_time );
  mbkfree( tab_voltage );
  mbkfree( flag );
  fclose( file );
}

