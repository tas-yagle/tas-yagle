#include AVT_H
#include "rcn.h"

char              RCN_DEBUG = 0;

#define RCN_MAXCACHE 16
mbkcache*         RCN_POOLCACHE[RCN_MAXCACHE];
int               RCN_NBCACHE=0;

/******************************************************************************\
Fonction d'accès aux ensembles de cache.
\******************************************************************************/
mbkcache* rcn_getpoolcache( int pool )
{
  return RCN_POOLCACHE[pool];
}

void rcn_setpoolcache( int pool, mbkcache *cache )
{
  RCN_POOLCACHE[pool] = cache ;
}

int rcn_getnewpoolcache( void )
{
  if( RCN_NBCACHE == RCN_MAXCACHE ) {
    avt_errmsg( RCN_ERRMSG, "001", AVT_FATAL );
  }
  return RCN_NBCACHE++;
}

/******************************************************************************\

Active le cache pour la lofig. Aucuns réseaux RC ne doit être déjà présent. Si 
un réseau RC est déjà présent sur un signal, le cache n'est pas actif pour ce 
signal. Cette fonction est principalement destinée à être utilisée dans les
parser.

\******************************************************************************/

void rcn_enable_cache( lofig_list *lofig,
                       unsigned long int (*fn_load)(lofig_list*, losig_list*),
                       void (*fn_free)(lofig_list*),
                       int pool
                     )
{
  mbkcache       *cache;

// rcnenv();
  cache = rcn_getlofigcache( lofig );
  if( cache || RCN_CACHE_SIZE == 0ul ) return;

  cache = rcn_getpoolcache( pool );
  
  if( !cache ) {
    cache = mbk_cache_create( 
                           (char(*)(void*,void*))NULL,
                           (unsigned long int(*)(void*,void*))fn_load,
                           (unsigned long int(*)(void*,void*))rcn_cache_release,
                           RCN_CACHE_SIZE
                                );
    rcn_setpoolcache( pool, cache );
  }

  rcn_alloclofigcache( lofig, cache, fn_free );
}

/******************************************************************************\

Libération du cache

\******************************************************************************/

void rcn_disable_cache( lofig_list *lofig )
{
  mbkcache       *cache;
  void          (*fn_free)(lofig_list*) ;
  ptype_list     *ptl;
  losig_list     *scanlosig;
  
  cache = rcn_getlofigcache( lofig );
  if( !cache )
    return;

  for( scanlosig = lofig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT )
    rcn_flush_signal( lofig, scanlosig );

  ptl = getptype( lofig->USER, RCN_LOFIGCACHE_FREE );
  if( ptl ) {
    fn_free = (void(*)(lofig_list*)) ptl->DATA;
    fn_free( lofig );
    lofig->USER = delptype( lofig->USER, RCN_LOFIGCACHE_FREE );
  }

  lofig->USER = delptype( lofig->USER, RCN_LOFIGCACHE );
}

/******************************************************************************\

Récupère les informations relative au cache pour une lofig. Si cette fonction
renvoie NULL, c'est que le cache n'est pas actif pour cette lofig.

\******************************************************************************/

mbkcache* rcn_getlofigcache( lofig_list *lofig )
{
  ptype_list *ptl;

  ptl = getptype( lofig->USER, RCN_LOFIGCACHE );
  if( ptl )
    return (mbkcache*)(ptl->DATA);
  return NULL;
}

/******************************************************************************\

Alloue les informations de cache pour une lofig.

\******************************************************************************/

void rcn_alloclofigcache( lofig_list *lofig, 
                          mbkcache *cache, 
                          void (*fn_free)(lofig_list*) 
                        )
{
  lofig->USER = addptype( lofig->USER, RCN_LOFIGCACHE, cache );
  if( fn_free ) 
    lofig->USER = addptype( lofig->USER, RCN_LOFIGCACHE_FREE, fn_free );
}

/******************************************************************************\

Met à jour un signal. Sans effet si le cache n'est pas actif.

\******************************************************************************/

void rcn_refresh_signal( lofig_list *lofig, losig_list *losig )
{
  mbkcache *cache;
  static lofig_list *lastlofig = NULL;
  static losig_list *lastlosig = NULL;

  if( lastlofig == lofig && lastlosig == losig )
    return;

  if( rcn_issignal_disablecache( losig ) ) {
//    rcn_error( 34, AVT_WARNING );
    return ;
  }
  
  avt_logenterfunction(LOGMBKCACHE,2,"rcn_refresh_signal()" );
  cache = rcn_getlofigcache( lofig );
  if( cache ) { 
    rcn_enablewritelofig( lofig );
    mbk_cache_refresh( cache, lofig, losig );
    rcn_disablewritelofig( lofig );
    rcn_setsignal_loaded( losig );
    lastlofig = lofig;
    lastlosig = losig;

    rcn_synccapa( lofig, losig );
  }
  avt_logexitfunction(LOGMBKCACHE,2);

  // rcn_cache_debug( lofig );

}

/******************************************************************************\

Fonctions de verrouillage d'un signal.

\******************************************************************************/

void rcn_lock_signal( lofig_list *lofig, losig_list *losig )
{
  mbkcache *cache;
  
  cache = rcn_getlofigcache( lofig );
  if( !cache )
    return;
    
  rcn_refresh_signal( lofig, losig );
  mbk_cache_lock( cache, losig );
}

void rcn_unlock_signal( lofig_list *lofig, losig_list *losig )
{
  mbkcache *cache;
  
  cache = rcn_getlofigcache( lofig );
  if( !cache )
    return;
    
  mbk_cache_unlock( cache, losig );
}

/* Renvoie YES ou NO */
char rcn_islock_signal( lofig_list *lofig, losig_list *losig )
{
  mbkcache *cache;
  
  cache = rcn_getlofigcache( lofig );
  if( !cache )
    return NO;
    
  return mbk_cache_islock( cache, losig );
}

/******************************************************************************\

Force la libération des RC d'un signal

\******************************************************************************/

void rcn_flush_signal( lofig_list *lofig, losig_list *losig )
{
  mbkcache *cache;
  
  cache = rcn_getlofigcache( lofig );
  if( !cache )
    return;

  mbk_cache_release( cache, lofig, losig );
}

/******************************************************************************\

Autorise ou interdit les fonctions de modification des vues RCN de la lofig

\******************************************************************************/

void rcn_enablewritelofig( lofig_list *lofig )
{
  losig_list *losig;
  if( RCN_DEBUG ) {
    for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT )
      rcn_enblewritesignal( losig );
  }
}

void rcn_disablewritelofig( lofig_list *lofig )
{
  losig_list *losig;
  if( RCN_DEBUG ) {
    for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT )
      rcn_disablewritesignal( losig );
  }
}

/******************************************************************************\

Autorise ou interdit les fonctions de modification des losigs

\******************************************************************************/

void rcn_disablewritesignal( losig_list *losig )
{
  if( RCN_DEBUG ) {
    RCN_SETFLAG( losig->RCNCACHE, RCNCACHEFLAG_WF );
  }
}

void rcn_enblewritesignal( losig_list *losig )
{
  if( RCN_DEBUG ) {
    RCN_CLEARFLAG( losig->RCNCACHE, RCNCACHEFLAG_WF );
  }
}

char rcn_iswritesignal( losig_list *losig )
{
  if( RCN_GETFLAG( losig->RCNCACHE, RCNCACHEFLAG_WF ) )
    return 0;
  return 1;
}

void rcn_checkwritesignal( losig_list *losig )
{
  if( RCN_DEBUG ) {
    if( ! rcn_iswritesignal( losig ) ) {
      rcn_error( 35, AVT_WARNING );
      return ;
    }
  }
}

/******************************************************************************\

Indicateurs de l'état d'un signal.

\******************************************************************************/

void rcn_setsignal_loaded( losig_list *losig )
{
  RCN_SETFLAG( losig->RCNCACHE, RCNCACHEFLAG_LOAD );
}

void rcn_clearsignal_loaded( losig_list *losig )
{
  RCN_CLEARFLAG( losig->RCNCACHE, RCNCACHEFLAG_LOAD );
}

char rcn_issignal_loaded( losig_list *losig )
{
  if( RCN_GETFLAG( losig->RCNCACHE, RCNCACHEFLAG_LOAD ) )
    return 1;
  return 0;
}

void rcn_setsignal_disablecache( losig_list *losig )
{
  RCN_SETFLAG( losig->RCNCACHE, RCNCACHEFLAG_DISABLE );
}

char rcn_issignal_disablecache( losig_list *losig )
{
  if( RCN_GETFLAG( losig->RCNCACHE, RCNCACHEFLAG_DISABLE ) )
    return 1;
  return 0;
}

/******************************************************************************\

Fonction appellée par mbk_cache qui libère le signal.

\******************************************************************************/

unsigned long int rcn_cache_release( lofig_list *lofig, losig_list *losig )
{
  chain_list *scanctc;
  chain_list *nextctc;
  loctc_list *ptctc;
  unsigned long int removed=0;

  if (rcn_issignal_disablecache(losig)) return 0;

  rcn_clearsignal_loaded( losig );

  freetable(losig);
  while( losig->PRCN->PWIRE ) {
    dellowire( losig,
               losig->PRCN->PWIRE->NODE1,
               losig->PRCN->PWIRE->NODE2
             );
    removed = removed + RCN_SIZEOFLOWIRE;
  }

  for( scanctc = losig->PRCN->PCTC ; scanctc ; scanctc = nextctc ) {
    nextctc = scanctc->NEXT; 
    ptctc = (loctc_list*)(scanctc->DATA);
    if( rcn_cache_removable_ctc( ptctc ) ) {
      delloctc( ptctc );
      removed = removed + RCN_SIZEOFLOCTC;
    }
  }

  lofig = NULL; //avoid a warning

  return removed;
}

/******************************************************************************\

Lors de l'effacement d'une ctc en raison d'un flush du cache, renvoie 1 si la 
capacité passée en paramètre peut être retirée, et 0 sinon.

\******************************************************************************/

char rcn_cache_removable_ctc( loctc_list *ctc )
{
  if(  !rcn_issignal_loaded( ctc->SIG1 ) &&
       !rcn_issignal_loaded( ctc->SIG2 )    )
    return 1;
  return 0;
}

/******************************************************************************\

Lors de l'ajout d'une capacité lors d'un refresh d'un signal, renvoie 1 si il
faut ajouter une capacité placée entre les 2 signaux.

\******************************************************************************/

char rcn_cache_addable_ctc( losig_list *sig1, losig_list *sig2 )
{
  if(   rcn_issignal_loaded( sig1 )  ||
        rcn_issignal_loaded( sig2 )     )
    return 0;
  return 1;
}

/******************************************************************************\

L'environnement

\******************************************************************************/

void rcnenv( void )
{
  char *str, *ptend ;

  str = getenv("RCN_DEBUG");

  if( str ) {
    if( !strcmp( str, "yes" ) ) {
      RCN_DEBUG=1;
      fflush( stdout );
      fprintf( stderr, "\n*** RCN DEBUG MODE enabled ***.\n" );
    }
    else {
      if( !strcmp( str, "no" ) )
        RCN_DEBUG=0;
      else {
        fflush( stdout );
        fprintf( stderr, "\n*** warning *** bad value for RCN_DEBUG.\n" );
      }
    }
  }
}

void rcn_cache_debug( lofig_list *lofig )
{
  mbkcache          *cache ;
  losig_list        *losig ;
  unsigned long int  size ;
  chain_list        *chain ;
  lowire_list       *wire ;
  loctc_list        *ctc ;
  mbkcachelist      *x;
  int                y ;
  
  cache = rcn_getlofigcache( lofig );
  if( !cache )
    return;

  /* check the cache size */

  size = 0 ;
  
  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
  
    if( losig->PRCN ) {
    
      for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT )
        size += RCN_SIZEOFLOWIRE ;

      for( chain = losig->PRCN->PCTC ; chain ; chain = chain->NEXT ) {
        ctc = (loctc_list*)chain->DATA ;
        if( ctc->SIG1 == losig )
          size += RCN_SIZEOFLOCTC ;
      }
    }

    x = mbk_cache_getmbkcachelist( cache, losig ) ;
    y = rcn_issignal_loaded( losig ) ;

    if( (x && !y) || (!x && y) )
      printf( "error : mismatch cache status for signal %p\n", losig );
        
  }

  if( cache->CURSIZE != size ) {
    printf( "error : cache->CURSIZE=%lu size=%lu\n", cache->CURSIZE, size );
  }
}

int rcn_hascache( lofig_list *lofig)
{
  return rcn_getlofigcache( lofig )!=NULL;
}
