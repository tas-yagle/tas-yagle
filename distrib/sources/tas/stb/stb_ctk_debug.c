/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_ctk.c                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                Grégoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

#include "stb_util.h"
#include "stb_init.h"
#include "stb_error.h"
#include "stb_transfer.h"
#include "stb_relaxation.h"
#include "stb_overlap.h"
#include "stb_ctk.h"
#include "stb_ctk_noise.h"
#include "stb_ctk_debug.h"

// Le fichier de trace
FILE *CTK_LOGFILE=NULL;

// Le niveau de log
int CTK_LOGLEVEL=10000;

#define REALMALLOC(t) (8+8*((t)/8+((t)%8!=0?1:0)))

void stb_log_chain( stbfig_list *stbfig, chain_list *headagr, char *title )
{
  chain_list    *chain;
  ttvevent_list *evt;

  if( !CTK_LOGFILE )
    return ;

  stb_ctkprint( 0, "%s\n", title );
  for( chain = headagr ; chain ; chain = chain->NEXT ) {
    evt=(ttvevent_list*)chain->DATA;
    stb_ctkprint( 1, "      %50s\n", evt->ROOT->NAME );
  }
}

void stb_debug_stab( stbfig_list   *stbfig, 
                     ttvevent_list *evtvic, 
                     chain_list    *headagr )
{
  chain_list    *scan;
  ttvevent_list *evt;
 
  if( !CTK_LOGFILE ) return;

  stb_ctkprint( 1, "      %50s : ", "victime" );
  ctk_display_event( stbfig, evtvic );
  for( scan = headagr ; scan ; scan = scan->NEXT ) {
    evt=(ttvevent_list*)scan->DATA;
    stb_ctkprint( 1, "      %50s : ", evt->ROOT->NAME );
    ctk_display_event( stbfig, evt );
  }
}

void stb_ctkprint( int level, char *fmt, ... )
{
  va_list    index;

  va_start( index, fmt );

  if( CTK_LOGFILE && level <= CTK_LOGLEVEL )
    vfprintf( CTK_LOGFILE, fmt, index );
}

/*** infos conso mémoire ******************************************************/

void stb_display_mem( stbfig_list *stbfig )
{
  chain_list            *scan;
  ttvevent_list         *node;
  ttvsig_list           *ttvsig;
  losig_list            *losig;
  unsigned int           util, real, lost;
  unsigned int           ttv_util=0, ttv_real=0, ttv_lost=0;
  unsigned int           lo_util=0, lo_real=0, lo_lost=0;
  char                  *insname;
  lofig_list            *figvic;
  chain_list            *chainfig;

  printf( "Mémoire :\n" );
  for( scan = stbfig->NODE ; scan ; scan = scan->NEXT ) {
    node = (ttvevent_list*)scan->DATA;
    ttvsig = node->ROOT;
    if( !getptype( ttvsig->USER, STBMEMPASS ) ) {
      ttvsig->USER = addptype( ttvsig->USER, STBMEMPASS, NULL );
      losig = ttv_getlosigfromevent( stbfig->FIG, 
                                     node->ROOT, 
                                    &insname, 
                                    &chainfig, 
                                    &figvic 
                                   );
      stb_mem_hash( ttvsig->USER, 0x60000016, 0x60000015, &util, &real, &lost );
      ttv_util+=util;
      ttv_real+=real;
      ttv_lost+=lost;
      if( losig ) {
        stb_mem_hash( losig->USER, 0x42435813, 0x52435812, &util, &real, &lost );
        lo_util+=util;
        lo_real+=real;
        lo_lost+=lost;
        freechain( chainfig );
      }
    }
  }

  printf( "      %9s %9s %9s\n", "util", "real", "lost" );
  printf( "TTV : %9u %9u %9u\n", ttv_util, ttv_real, ttv_lost );
  printf( "ALC : %9u %9u %9u\n", lo_util, lo_real, lo_lost );
}

void stb_mem_hash( ptype_list *USER, 
                   long pthash, 
                   long ptptype,
                   unsigned int *util, // Ce qui est réellement utilisé
                   unsigned int *real, // Ce qui est réellement alloué
                   unsigned int *lost  // Hash : la taille libre dans table
                 )
{
  ptype_list    *ptl;
  ht            *table;
  int            i;
  
  *util=0u;
  *real=0u;
  *lost=0u;
  
  ptl = getptype( USER, pthash );
  if( ptl ) {
    table=(ht*)ptl->DATA;
    *util = table->count * 8u;
    *real = REALMALLOC( sizeof(ht) ) + 
            REALMALLOC( sizeof(htitem)*table->length ) +
            sizeof( ptype_list );
    *lost = (table->length-table->count)*sizeof(htitem);
  }

  ptl = getptype( USER, ptptype );
  if( ptl ) {
    for( i=0, ptl=(ptype_list*)ptl->DATA ; ptl ; ptl=ptl->NEXT, i++ );
    *util = 8u * i;
    *real = (1+i)*sizeof( ptype_list );
    *lost = 0u;
  }
}

void stb_debug_mark_ttvsig( ttvsig_list *ttvsig ) 
{
  ptype_list *ptype ;
  ptype = getptype( ttvsig->USER, STB_MARK_DEBUG ) ;
  if( !ptype )
    ttvsig->USER = addptype( ttvsig->USER, STB_MARK_DEBUG, NULL );
}

void stb_debug_mark_aggressor( stbfig_list *stbfig, int level, long type, ttvevent_list *event )
{
  losig_list *sigvic ;
  lofig_list *figvic ;
  char       *insname ;
  chain_list *chainfig ;
  chain_list *headagr ;
  chain_list *chain ;
  ttvsig_list *ttvsig ;

  sigvic = ttv_getlosigfromevent( stbfig->FIG, 
                                   event->ROOT, 
                                   &insname, 
                                   &chainfig, 
                                   &figvic 
                                 );
                                 
  if( sigvic ) {                 
  
    headagr = rcx_getagrlist( figvic, sigvic, insname, chainfig );
    freechain( chainfig );
    
    if( headagr ) {
    
      stb_fillttvsigrcxparam( stbfig->FIG, level, type, headagr );

      for( chain = headagr ; chain ; chain = chain->NEXT ) {
        ttvsig = stb_getttvsigrcxparam( (rcxparam*)chain->DATA );
        if( ttvsig ) {
          stb_debug_mark_ttvsig( ttvsig );
        }
      }
      
      stb_delttvsigrcxparam( headagr );
      rcx_freeagrlist( figvic, sigvic, headagr );
    }
  }
}

int stb_init_debug_node( stbfig_list *stbfig, int level, long type )
{
  char *env ;
  char *nodename ;
  ttvsig_list *ttvsig ;
  int i ;
  ttvline_list *line ;

  env = getenv( "STB_DEBUG_NODE" );
  if( !env )
    return 0 ;
    
  nodename = namefind( env );
  if( !nodename )
    return 0;
  
  ttvsig = ttv_getsig( stbfig->FIG, nodename );
  if( !ttvsig ) {
    printf( "STB_DEBUG_MODE : signal %s doesn't exist\n", nodename );
    return 0 ;
  }
  
  printf( "STB_DEBUG_MODE : evaluating only for neigbough of signal %s\n", nodename );

  stb_debug_mark_ttvsig( ttvsig );
  stb_debug_mark_aggressor( stbfig, level, type, & ttvsig->NODE[0] );
  stb_debug_mark_aggressor( stbfig, level, type, & ttvsig->NODE[1] );

  for( i=0 ; i<=1 ; i++ ) {
    for( line = ttvsig->NODE[i].INLINE ; line ; line = line->NEXT ) {
      ttvsig = line->NODE->ROOT ;
      stb_debug_mark_ttvsig( ttvsig );
      stb_debug_mark_aggressor( stbfig, level, type, line->NODE );
    }
  }

  return 1 ;
}
