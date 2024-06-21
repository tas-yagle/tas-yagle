
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

#include "stb_ctk.h"
#include "stb_ctk_agr.h"
#include "stb_parse.h"

void stb_ctk_drive_agression( stbfig_list *stbfig )
{
  chain_list    *chain ;
  int            pb_max ;
  agrstbfile    *file ;
  
  file = stb_ctk_open_agr_file( stbfig, "w" );
  if( !file )
    return ;
  
  for( chain = stbfig->NODE, pb_max=0; chain ; chain = chain->NEXT, pb_max++ );
  file->ACCESS.DRIVE.PB_MAX = pb_max ;

  stb_ctk_agr_nbsignals( file );
  stb_ctk_agr_headers( file );
  stb_ctk_agr_signals( file );
  stb_ctk_agr_drive( file );
  stb_ctk_agr_clean( file );

  stb_agr_info( file, "EOF\n" );

  stb_ctk_close_agr_file( file );
}

void stb_ctk_agr_headers( agrstbfile *file )
{
  char *ctkmode ;
  
  if( ( file->STBFIG->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST ) 
    ctkmode = "WORST" ;
  else
    ctkmode = "BEST" ;

  stb_agr_info( file, "%s N=%d\n", ctkmode, file->ACCESS.DRIVE.NBSIG );
}

void stb_ctk_agr_drive( agrstbfile *file )
{
  chain_list     *chain ;
  int             pb_cur ;
  ttvevent_list  *event ;
  char            transition ;
  int             idx ;
  losig_list     *sigvic ;
  char           *insname ;
  chain_list     *chainfig ;
  lofig_list     *figvic ;
  chain_list     *headagr ;
  long            type ;
  long            level ;
  int             idxagr ;
  rcxparam       *param ;
  ttvsig_list    *ttvagr ;
  chain_list     *scanagr ;
  
  type = TTV_FIND_LINE | TTV_FIND_MIN | TTV_FIND_MAX ;
  if((file->STBFIG->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = file->STBFIG->FIG->INFO->LEVEL ;
  else
    level = 0 ;
  
  CTK_PROGRESSBAR( 0, 1, 0, "driving aggressions" );
  for( chain = file->STBFIG->NODE, pb_cur=0; chain ; chain = chain->NEXT, pb_cur++ ) {
 
    CTK_PROGRESSBAR( 0, file->ACCESS.DRIVE.PB_MAX, pb_cur, NULL );
    
    event = (ttvevent_list*)chain->DATA ;
    sigvic = ttv_getlosigfromevent( file->STBFIG->FIG, event->ROOT, &insname, &chainfig, &figvic );

    if( sigvic ) {

      headagr = rcx_getagrlist( figvic, sigvic, insname, chainfig );
      stb_fillttvsigrcxparam( file->STBFIG->FIG, level, type, headagr );
      if( ( file->STBFIG->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST )
        stb_fillinactifrcxparam( file->STBFIG->FIG, event, headagr, 'Y' );
      else
        stb_fillactifrcxparam( file->STBFIG->FIG, event, headagr, 'Y' );

      idx = stb_ctk_agr_get_index( event->ROOT );

      if( ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
        transition = 'u' ;
      else
        transition = 'd' ;

      stb_agr_info( file, "%c %d\n", transition, idx );

      for( scanagr = headagr ; scanagr ; scanagr = scanagr->NEXT ) {
      
        param = (rcxparam*)scanagr->DATA;
        ttvagr = stb_getttvsigrcxparam(param);
        
        if( ttvagr ) {
      
          idxagr = stb_ctk_agr_get_index( ttvagr );
          stb_agr_info( file, "%d", idxagr );
        
          if( ( param->ACTIF & RCX_AGRBEST   ) == RCX_AGRBEST   ) stb_agr_info( file, " B" );
          if( ( param->ACTIF & RCX_MTX_BEST  ) == RCX_MTX_BEST  ) stb_agr_info( file, " b" );
          if( ( param->ACTIF & RCX_AGRWORST  ) == RCX_AGRWORST  ) stb_agr_info( file, " W" );
          if( ( param->ACTIF & RCX_MTX_WORST ) == RCX_MTX_WORST ) stb_agr_info( file, " w" );

          stb_agr_info( file, "\n" );
        }
      }
      
      stb_delttvsigrcxparam( headagr );
      rcx_freeagrlist( figvic, sigvic, headagr );
      freechain( chainfig );
      stb_agr_info( file, "-\n" );
    }
  }
  CTK_PROGRESSBAR( 0, file->ACCESS.DRIVE.PB_MAX, file->ACCESS.DRIVE.PB_MAX, NULL );
}

void stb_ctk_agr_clean( agrstbfile *file )
{
  chain_list    *chain ;
  int            pb_cur ;
  ttvevent_list *event ;

  for( chain = file->STBFIG->NODE, pb_cur=0; chain ; chain = chain->NEXT, pb_cur++ ) {
 
    event = (ttvevent_list*)chain->DATA ;
    stb_ctk_clear_agr_index( event->ROOT );
  }

  if( file->ACCESS.DRIVE.TABSIG )
    mbkfree( file->ACCESS.DRIVE.TABSIG );
}

void stb_ctk_agr_signals( agrstbfile *file )
{
  char           ttvname[1024] ;
  int            nbsig ;

  nbsig = 0 ;

  for( nbsig = 1 ; nbsig <= file->ACCESS.DRIVE.NBSIG ; nbsig ++ ) {
    ttv_getsigname( file->STBFIG->FIG, ttvname, file->ACCESS.DRIVE.TABSIG[nbsig-1] ) ;
    stb_agr_info( file, "%d %s\n", stb_ctk_agr_get_index( file->ACCESS.DRIVE.TABSIG[nbsig-1] ), ttvname );
  }
  stb_agr_info( file, "-\n" );
}

void stb_ctk_agr_nbsignals( agrstbfile *file )
{
  chain_list    *chain ;
  ttvevent_list *event ;
  ttvsig_list   *ttvsig ;
  int            nbsig ;

  nbsig = 0 ;

  for( chain = file->STBFIG->NODE ; chain ; chain = chain->NEXT ) {
 
    event = (ttvevent_list*)chain->DATA ;
    ttvsig = event->ROOT ;
    
    if( stb_ctk_agr_get_index( ttvsig ) == -1 ) {
      nbsig++;
      stb_ctk_agr_set_index( ttvsig, nbsig ) ;
    }
  }

  file->ACCESS.DRIVE.NBSIG  = nbsig ;
  file->ACCESS.DRIVE.TABSIG = (ttvsig_list**)mbkalloc( sizeof( ttvsig_list* ) * nbsig );

  for( chain = file->STBFIG->NODE ; chain ; chain = chain->NEXT ) {
 
    event = (ttvevent_list*)chain->DATA ;
    ttvsig = event->ROOT ;
  
    file->ACCESS.DRIVE.TABSIG[ stb_ctk_agr_get_index( ttvsig ) - 1 ] = ttvsig ;
  }
}

void stb_ctk_agr_set_index( ttvsig_list *ttvsig, int index )
{
  ptype_list *ptl ;

  ptl = getptype( ttvsig->USER, CTK_AGR_IDX );
  if( ptl )
    ptl->DATA = (void*)(long)index ;
  else
    ttvsig->USER = addptype( ttvsig->USER, CTK_AGR_IDX, (void*)(long)index );
}

int stb_ctk_agr_get_index( ttvsig_list *ttvsig )
{
  ptype_list *ptl ;
  int         index ;

  ptl = getptype( ttvsig->USER, CTK_AGR_IDX );
  if( ptl )
    index = (int)(long)ptl->DATA ;
  else
    index = -1 ;

  return index ;
}

void stb_ctk_clear_agr_index( ttvsig_list *ttvsig )
{
  ptype_list *ptl ;

  ptl = getptype( ttvsig->USER, CTK_AGR_IDX );
  if( ptl )
    ttvsig->USER = delptype( ttvsig->USER, CTK_AGR_IDX );
}


void stb_ctk_close_agr_file( agrstbfile *file )
{
  fclose( file->FD );
}

agrstbfile* stb_ctk_open_agr_file( stbfig_list *stbfig, char *mode )
{
  static agrstbfile     file ;
  char                  buffer[1024];
  
  file.FD = mbkfopen( stbfig->FIG->INFO->FIGNAME, "agr", mode ) ;
  if( ! file.FD )
    return NULL ;
  file.STBFIG = stbfig ;
 
  sprintf( buffer, "crosstalk aggression for circuit %s\n", stbfig->FIG->INFO->FIGNAME );

  avt_printExecInfoCustom( (void*)&file, "#", buffer, "", (void(*)(void*,...))stb_agr_info );

  return &file ;
}

void stb_agr_info( agrstbfile *file, ... )
{
  va_list       index;
  char         *fmt;

  if( !file ) return;

  va_start( index, file );
  fmt = va_arg( index, char* );

  vfprintf( file->FD, fmt, index );
}

void stb_ctk_parse_agression( stbfig_list *stbfig )
{
  agrstbfile *file ;
  char        line[CTK_AGR_BUF] ;
  int         parse ;
  int         lastparse ;
  int         onemoretime ;
 
  avt_logenterfunction( LOGCTK, 2, "stb_ctk_parse_agression()" );

  file = stb_ctk_open_agr_file( stbfig, "r" );

  if( !file ) {
    avt_logexitfunction( LOGCTK, 2 ) ;
    return ;
  }

  lastparse = -1 ;
  parse = 0 ;
  onemoretime = 1 ;
 
  file->ACCESS.PARSE.ERROR  = 0 ;
  file->ACCESS.PARSE.NBLINE = 0 ;
  file->ACCESS.PARSE.EVENT = NULL ;
  
  do {
  
    fgets( line, 1024, file->FD );
    file->ACCESS.PARSE.NBLINE++ ;

    if( line[0] != '#' ) {

      switch( parse ) {
      
      case 0 : /* global information, one line */
        if( lastparse != parse ) { printf( "  - reading header\n" ); lastparse = parse ; }
        if( stb_ctk_parse_header( file, line ) )
          parse = 1 ;
        break ;
        
      case 1 : /* list of signals */
        if( lastparse != parse ) { printf( "  - reading list of signals\n" ); lastparse = parse ; }
        if( stb_ctk_parse_signals( file, line ) )
          parse = 2 ;
        break ;
        
      case 2 : /* list of aggression */
        if( lastparse != parse ) { printf( "  - reading list of agressions\n" ); lastparse = parse ; }
        if( stb_ctk_parse_agr( file, line ) )
          parse = 3 ;
        break;
        
      case 3 : /* the end of the file */
        onemoretime = 0 ;
        break ;
      }

      if( file->ACCESS.PARSE.ERROR ) {
        printf( "an error occured line %d\n", file->ACCESS.PARSE.NBLINE );
        EXIT(0);
      }
    }
  }
  while( onemoretime );

  cleanStartForNode_HT();

  if( file->ACCESS.PARSE.TABSIG )
    mbkfree( file->ACCESS.PARSE.TABSIG );
  stb_ctk_close_agr_file( file );

  avt_logexitfunction( LOGCTK, 2 ) ;
}

int stb_ctk_parse_header( agrstbfile *file, char *line )
{
  char *token ;
  int   ret = 0 ;
  int   idx ;
  char *endptr ;
 
  avt_logenterfunction( LOGCTK, 2, "stb_ctk_parse_header()" );

  token = strtok( line, CTKAGRSEPAR );
  
  while( token ) {

    avt_log( LOGCTK, 2, "token read : \"%s\"\n", token ? token : "no token" );

    if( strcmp( token, CTKAGRSEPAR ) == 0 )
      break ;

    ret = 1 ;
    file->ACCESS.PARSE.ERROR = 1 ;
    
    if( strcmp( token, "BEST"  ) == 0 ) {
      file->STBFIG->CTKMODE = file->STBFIG->CTKMODE & ~STB_CTK_WORST  ;
      file->ACCESS.PARSE.ERROR = 0 ;
      avt_log( LOGCTK, 2, "file contains aggressions (best mode)\n" );
    }

    if( strcmp( token, "WORST" ) == 0 ) {
      file->STBFIG->CTKMODE = file->STBFIG->CTKMODE | STB_CTK_WORST ;
      file->ACCESS.PARSE.ERROR = 0 ;
      avt_log( LOGCTK, 2, "file contains non-aggressions (worst mode)\n" );
    }

    if( strncmp( token, "N=", 2 ) == 0 ) {
      idx = strtol( token+2, &endptr, 10 );
      if( *endptr == '\0' ) {
        file->ACCESS.PARSE.ERROR = 0 ;
        file->ACCESS.PARSE.NBSIG = idx ;
        file->ACCESS.PARSE.TABSIG = (ttvsig_list**)mbkalloc( sizeof( ttvsig_list* ) * idx ) ;
        avt_log( LOGCTK, 2, "there is %d timing signal with crosstalk information in database\n", idx );
      }
    }

    if( file->ACCESS.PARSE.ERROR ) {
      avt_log( LOGCTK, 2, "there is a syntax error\n" );
      ret = 0 ;
      break ;
    }

    token = strtok( NULL, CTKAGRSEPAR );
  }

  avt_log( LOGCTK, 2, "header parsed : return code is %d\n", ret );
  avt_logexitfunction( LOGCTK, 2 );
  return ret ;
}

int stb_ctk_parse_signals( agrstbfile *file, char *line )
{
  char        *token ;
  char        *endptr ;
  int          ret = 0 ;
  int          idx ;
  ttvsig_list *ttvsig ;
  chain_list  *chain ;
 
  avt_logenterfunction( LOGCTK, 2, "stb_ctk_parse_signals()" );
  
  token = strtok( line, CTKAGRSEPAR );

  if( strcmp( token, "-" ) == 0 ) {
 
    ret = 1 ;
    avt_log( LOGCTK, 2, "end of signals\n" );
    avt_logexitfunction( LOGCTK, 2 );
    return ret ;
    
  }
  else {
  
    idx = strtol( token, &endptr, 10 );
    if( *endptr != '\0' ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "bad index\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }
    
    token = strtok( NULL, CTKAGRSEPAR );
    if( !token ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "missing name\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }
    
    // ttvsig = ttv_getsig( file->STBFIG->FIG, token );
    chain = getStartForNode_HT( file->STBFIG, token, 0 );
    
    if( !chain || !chain->DATA ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "can't find timing signal \"%s\"\n", token );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }
    ttvsig = (ttvsig_list*)chain->DATA ;
    freechain(chain);

    token = strtok( NULL, CTKAGRSEPAR );
    if( token ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "extra argument\n", token );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }

    file->ACCESS.PARSE.TABSIG[idx-1] = ttvsig ;
    avt_log( LOGCTK, 2, "timing signal %s correctly parsed\n", ttvsig->NAME );
  }

  avt_logexitfunction( LOGCTK, 2 );
  return ret ;
}

int stb_ctk_parse_agr( agrstbfile *file, char *line )
{
  char        *token ;
  int          ret = 0 ;
  long         transition ;
  ttvsig_list *ttvsig ;
  int          idx ;
  char        *endptr ;
  int          flag ;

  avt_logenterfunction( LOGCTK, 2, "stb_ctk_parse_agr()" );

  token = strtok( line, CTKAGRSEPAR );
  if( !token ) {
    avt_logexitfunction( LOGCTK, 2 );
    return ret ;
  }

  if( ! file->ACCESS.PARSE.EVENT ) {
  
    if( strcmp( token, "EOF" ) == 0 ) {
      ret = 1 ;
      avt_log( LOGCTK, 2, "EOF meet\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }
    
    transition = 0 ;
    if( strcmp( token, "d"   ) == 0 ) transition = TTV_NODE_DOWN ;
    if( strcmp( token, "u"   ) == 0 ) transition = TTV_NODE_UP ;

    if( !transition ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "bad transition\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }
      
    token = strtok( NULL, CTKAGRSEPAR );
    if( !token ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "missing signal number\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }
    
    idx = strtol( token, &endptr, 10 );
    if( *endptr != '\0' ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "bad number\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }

    ttvsig = file->ACCESS.PARSE.TABSIG[idx-1];

    if( !ttvsig ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "can't find signal #%d\n", idx );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }

    switch( transition ) {
    case TTV_NODE_DOWN : file->ACCESS.PARSE.EVENT = & ttvsig->NODE[0] ; break ;
    case TTV_NODE_UP   : file->ACCESS.PARSE.EVENT = & ttvsig->NODE[1] ; break ;
    }

    token = strtok( NULL, CTKAGRSEPAR );
    if( token ) {
      file->ACCESS.PARSE.ERROR = 1 ;
      avt_log( LOGCTK, 2, "extra parameter\n" );
      avt_logexitfunction( LOGCTK, 2 );
      return ret ;
    }

    avt_log( LOGCTK, 2, "setting aggression for timing signal %s %s\n", transition==TTV_NODE_UP ? "up" : "dw", ttvsig->NAME );
  }
  else {
    if( strcmp( token, "-" ) == 0 ) {
    
      file->ACCESS.PARSE.EVENT = NULL ;

      token = strtok( NULL, CTKAGRSEPAR );
      if( token ) {
        file->ACCESS.PARSE.ERROR = 1 ;
        avt_log( LOGCTK, 2, "extra parameter\n" );
        avt_logexitfunction( LOGCTK, 2 );
        return ret ;
      }
    }
    else {
      
      idx = strtol( token, &endptr, 10 );
      if( *endptr != '\0' ) {
        file->ACCESS.PARSE.ERROR = 1 ;
        avt_logexitfunction( LOGCTK, 2 );
        avt_log( LOGCTK, 2, "bad number\n", idx );
        return ret ;
      }
    
      ttvsig = file->ACCESS.PARSE.TABSIG[idx-1];

      if( !ttvsig ) {
        file->ACCESS.PARSE.ERROR = 1 ;
        avt_log( LOGCTK, 2, "can't find signal #%d\n", idx );
        avt_logexitfunction( LOGCTK, 2 );
        return ret ;
      }

      flag = 0 ;

      do {
        token = strtok( NULL, CTKAGRSEPAR );

        if( token ) {

          if( strcmp( token, "B" ) == 0 ) 
            flag = flag | RCX_AGRBEST ;
            
          if( strcmp( token, "W" ) == 0 ) 
            flag = flag | RCX_AGRWORST ;

          if( strcmp( token, "b" ) == 0 ) 
            flag = flag | RCX_MTX_BEST ;

          if( strcmp( token, "w" ) == 0 )
            flag = flag | RCX_MTX_WORST ;
        }
      }
      while( token );

      if( ( file->STBFIG->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST )
        stb_saveinactifcoupling( file->ACCESS.PARSE.EVENT, ttvsig, flag );
      else
        stb_saveactifcoupling( file->ACCESS.PARSE.EVENT, ttvsig, flag );
    }
  }

  avt_logexitfunction( LOGCTK, 2 );
  return ret ;
}
