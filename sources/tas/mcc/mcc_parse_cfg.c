
#include MCC_H
#include "mcc_parse_cfg.h"

#define MAXLENGTHLINE 256
#define MAXWORDS      200

moddeflist* mcc_allocmoddeflist( moddeflist *head, char *name, double value )
{
  moddeflist *n ;

  n = (moddeflist*)mbkalloc( sizeof( moddeflist ) );
  n->NEXT   = head ;
  n->NAME   = strdup( name );
  n->VALUE  = value ;

  return n ;
}

modcfglist* mcc_allocmodcfglist( modcfglist *head, char *name )
{
  modcfglist *n ;

  n = (modcfglist*)mbkalloc( sizeof( modcfglist ) );
  n->NEXT    = head ;
  n->NAME    = strdup(name) ;
  n->MODLIST = NULL ;

  return n ;
}

modmodellist* mcc_allocmodmodellist( modmodellist *head, int mcctype )
{
  modmodellist *n ;

  n = (modmodellist*)mbkalloc( sizeof( modmodellist ) );
  n->NEXT       = head ;
  n->MCCTYPE    = mcctype ;
  n->CRITERION  = 0 ;
  n->DEFAULT    = NULL ;
  n->EXTNAME       = NULL ;
  
  return n ;
}

void mcc_decompline( char *line, char **tabwords, int *nbwords )
{
  int            max ;
  static char    buf[MAXLENGTHLINE];
  char          *pt ;

  max = *nbwords ;
  *nbwords = 0 ;

  strcpy( buf, line );
  pt = strtok( buf, " \t\n" );

  while( pt ) {
    tabwords[*nbwords]=pt ;
    (*nbwords)++ ;
    pt = strtok( NULL, " \t\n" );
  }
}

modcfglist* mcc_parsecfg( char *fname )
{
  FILE         *f ;
  char          line[MAXLENGTHLINE];
  modcfglist   *headmodcfg ;
  char         *tabwords[MAXWORDS] ;
  int           nbwords ;
  int           ln ;
  int           nbtech ;
  int           i ;
  int           error ;
  
  struct techno {
    char *TECNAME ;
    int   MCCLEVEL ;
  } ;
  struct techno techlist[]={ { "MOS2",    MCC_MOS2    }, \
                             { "MM9",     MCC_MM9     }, \
                             { "BSIM3V3", MCC_BSIM3V3 }, \
                             { "BSIM4",   MCC_BSIM4   }, \
                             { "PSP",     MCC_MPSP    }, \
                             { "PSPB",    MCC_MPSPB   }, \
                             { "EXT",     MCC_EXTMOD   }  \
                           } ;

  f = fopen( fname, "r" );
  if( !f ) {
    return NULL ;
  }
    
  headmodcfg = NULL ;
  ln         = 0 ;
  error      = 0 ;
  nbtech     = sizeof( techlist ) / sizeof( struct techno ) ;

  while( !feof( f ) ) {
 
    if( !fgets( line, MAXLENGTHLINE-1, f ) )
      continue ;

    ln++ ;

    nbwords = MAXWORDS ;
    mcc_decompline( line, tabwords, &nbwords );

    if( nbwords == 0 || *(tabwords[0])=='#' )
      continue ;

    if( strcmp( tabwords[0], "MODEL" )==0 ) {
    
      if( nbwords != 2 ) {
        error=1 ;
        break ;
      }
      
      headmodcfg = mcc_allocmodcfglist( headmodcfg, tabwords[1] ) ;
      continue ;
    }

    for( i = 0 ; i < nbtech ; i++ ) 
      if( strcmp( tabwords[0], techlist[i].TECNAME ) == 0 ) 
        break ;

    if( i >= nbtech ) {
      error = 1 ;
      break ;
    }

    headmodcfg->MODLIST = mcc_allocmodmodellist( headmodcfg->MODLIST, 
                                                 techlist[i].MCCLEVEL 
                                               );

    if( nbwords < 2 ) {
      error = 1 ;
      break ;
    }
    
    if( strcmp( tabwords[1], "param" ) == 0 ) {
      if( nbwords < 4 ) {
        error = 1 ;
        break ;
      }
      headmodcfg->MODLIST->CRITERION = MCC_CRIT_PARAM ;
      headmodcfg->MODLIST->VALUE.PARAM.PARAM = strdup( tabwords[2] );
      headmodcfg->MODLIST->VALUE.PARAM.VALUE = atof( tabwords[3] );
      i = 4 ;
    }
    else {
      if( strcmp( tabwords[1], "model" ) == 0 ) {
        if( nbwords < 3 ) {
          error = 1 ;
          break ;
        }
        headmodcfg->MODLIST->CRITERION = MCC_CRIT_MODEL ;
        headmodcfg->MODLIST->VALUE.MODEL = strdup( tabwords[2] ) ;
        i = 3 ;
      }
      else {
        error = 1 ;
        break ;
      }
    }

    if( i < nbwords ) {
      if( strcmp( tabwords[i], "name" ) == 0 ) {
        i++ ;
        if( i >= nbwords ) {
          error = 1 ;
          break ;
        }
        headmodcfg->MODLIST->EXTNAME = namealloc(tabwords[i]);
        i++;
      }
    }

    if( i < nbwords ) {
      if( strcmp( tabwords[i], "setdefault" ) == 0 ) {
        i++ ;
        do {
          if( i+1 >= nbwords ) {
            error = 1 ;
            break ;
          }
          headmodcfg->MODLIST->DEFAULT = mcc_allocmoddeflist( headmodcfg->MODLIST->DEFAULT,
                                                              tabwords[i],
                                                              atof(tabwords[i+1])
                                                            );
          i+=2 ;
        }
        while( i != nbwords );
        if( error )
          break ;
      }
      else {
        error = 1 ;
        break ;
      }
    }
  }

  if( error ) {
    avt_errmsg( MCC_ERRMSG, "044", AVT_FATAL, ln, fname );
    headmodcfg = NULL ;
  }

  headmodcfg = (modcfglist *)reverse( (chain_list *)headmodcfg );
  return headmodcfg ;
}

modcfglist* mcc_getmodcfg( void )
{
  static modcfglist     *modcfg = NULL ;
  static int             parsed = 0 ;
  char                  *env ;
  char                   buffer1[1024];
  char                   buffer2[1024];

  if( parsed )
    return modcfg ;

  sprintf( buffer1, "<null>" );
  sprintf( buffer2, "<null>" );

  parsed = 1 ;

  env = getenv("AVT_TOOLS_DIR");
  if( env ) {
    sprintf( buffer1, "%s/etc/trmodel.cfg", env );
    modcfg = mcc_parsecfg( buffer1 ) ;
  }
  if( !modcfg ) {
    env = getenv( "AVERTEC_TOP" );
    if( env ) {
      sprintf( buffer2, "%s/etc/trmodel.cfg", env );
      modcfg = mcc_parsecfg( buffer2 ) ;
    }
  }

  if( !modcfg ) {
    avt_errmsg( MCC_ERRMSG, "045", AVT_FATAL, buffer1, buffer2 );
  }
  return modcfg ;
}
