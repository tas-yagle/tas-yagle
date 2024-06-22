#include MUT_H
#include AVT_H
#include "mbk_cache.h"

mbkcache  *FILE_CACHE = NULL ;
int        FILE_CACHE_INDEX = 0 ;
HeapAlloc  FILE_CACHE_HEAP ;
ht        *FILE_CACHE_HT = NULL ;
int        MBK_MAX_CACHE = 128 ;

/*****************************************************************************\
mbk_cache_create()
Fonction utilisateur.
Crée un nouveau cache.
\*****************************************************************************/

mbkcache* mbk_cache_create( char              (*isactive)( void *root, void *elem ),
                            unsigned long int (*load    )( void *root, void *elem ),
                            unsigned long int (*release )( void *root, void *elem ),
                            unsigned long int cachesize
                          )
{
  mbkcache *cache;

  cache = mbk_cache_alloc();

  cache->CACHESIZE = cachesize;
  cache->INFOS     = addht( CACHE_ALLOC_BLOCLIST );
  cache->FN_ISACTIVE  = isactive;
  cache->FN_LOAD      = load;
  cache->FN_RELEASE   = release;

  return cache;
}

/*****************************************************************************\
mbk_cache_refresh()
Fonction utilisateur. 
Charge en mémoire un élément si il n'est pas déjà présent. Libère d'autres 
éléments si c'est nécessaire.
\*****************************************************************************/

void mbk_cache_refresh( mbkcache *cache, void *root, void *elem )
{
  mbkcachelist *incache;
  
  if( !cache || mbk_cache_call_iscative( cache, root, elem ) == 0 ) 
    return;

  avt_logenterfunction(LOGMBKCACHE,2, "mbk_cache_refresh()" );
  incache =  mbk_cache_getmbkcachelist( cache, elem );
  
  if( incache ) {
    avt_log(LOGMBKCACHE,2,"element already loaded\n");
    // L'élément est présent dans le cache, on le met en tête des éléments les
    // derniers accédés.
    if( incache != cache->FIRST )
      mbk_cache_makeitfirst( cache, incache );
  }
  else {
  
    // L'élément n'est pas dans le cache. On fait du ménage si necéssaire, puis
    // on lit le nouvel élément.
 
    // si le cache est limité en nb d'éléments, on réserve un éléments pour
    // celui qui va être lu.
    if( cache->MAXELEM )
      cache->CURELEM++;

    mbk_cache_update_memory( cache, root );

    if( cache->MAXELEM )
      cache->CURELEM--;

    if( elem )
      mbk_cache_add( cache, root, elem );
  }

  avt_logexitfunction(LOGMBKCACHE,2);
}

/*****************************************************************************\
mbk_cache_release()
Fonction utilisateur. 
Force la libération d'un élément du cache.
\*****************************************************************************/

void mbk_cache_release( mbkcache *cache, void *root, void *elem )
{
  mbkcachelist *incache;
  
  if( !cache || mbk_cache_call_iscative( cache, root, elem ) == 0 )
    return;

  incache =  mbk_cache_getmbkcachelist( cache, elem );
  if( incache == NULL ) return;

  if( incache->LOCKED == 0 )
    mbk_cache_remove( cache, incache, root );
}

/*****************************************************************************\
mbk_cache_delete()
Fonction utilisateur. 
Efface le cache. Appel mbk_cache_release() pour tous les éléments encore 
présents dans le cache.
\*****************************************************************************/

extern void mbk_cache_delete( mbkcache *cache, void *root )
{
  while( cache->FIRST )
    mbk_cache_remove( cache, cache->FIRST, root );
  mbk_cache_free( cache );
}

/*****************************************************************************\
mbk_cache_lock()
Fonction utilisateur. 
Verrouille un élément en mémoire
\*****************************************************************************/

extern void mbk_cache_lock( mbkcache *cache, void *elem )
{
  mbkcachelist *incache;

  incache =  mbk_cache_getmbkcachelist( cache, elem );
  if( incache == NULL ) return;

  incache->LOCKED++;
}

/*****************************************************************************\
mbk_cache_unlock()
Fonction utilisateur. 
Deverrouille un élément en mémoire
\*****************************************************************************/

extern void mbk_cache_unlock( mbkcache *cache, void *elem )
{
  mbkcachelist *incache;

  incache =  mbk_cache_getmbkcachelist( cache, elem );
  if( incache == NULL ) return;

  if( incache->LOCKED > 0 )
    incache->LOCKED--;
}

/*****************************************************************************\
mbk_cache_islock()
Fonction utilisateur. 
Informe si l'élément est vérouillé
\*****************************************************************************/

extern char mbk_cache_islock( mbkcache *cache, void *elem )
{
  mbkcachelist *incache;

  incache =  mbk_cache_getmbkcachelist( cache, elem );
  if( incache == NULL ) return NO;

  if( incache->LOCKED > 0 )
    return YES;
  return NO;
}

/*****************************************************************************\
mbk_cache_update_size()
Fonction utilisateur. 
Informe le cache que la taille d'un ou plusieurs de ses éléments a changée.
Libère des éléments si la taille maximum du cache a été dépassée.
\*****************************************************************************/
void mbk_cache_update_size( mbkcache *cache, void *root, long int size )
{
  cache->CURSIZE = cache->CURSIZE + size ;
  mbk_cache_update_memory( cache, root );
}

/*****************************************************************************\
mbk_cache_list_content()
Fonction utilisateur. 
renvoie la liste chainée des éléments présents dans le cache
\*****************************************************************************/
chain_list* mbk_cache_list_content( mbkcache *cache )
{
  mbkcachelist *scan ;
  chain_list   *list ;

  list = NULL ;
  
  for( scan = cache->LAST ; scan ; scan = scan->PREV ) {
    list = addchain( list, scan->DATA );
  }

  return list ;
}

/*****************************************************************************\
mbk_cache_set_limit_element()
Fonction utilisateur
Limite le nombre d'éléments pouvant être présent à chaque instant dans le
cache, en plus du critère de taille de cache.
Si 0, le nombre d'élément est illimité, le nombre d'éléments présents dans le
cache n'est limité que par la taille du cache.
Appelle automatiquement mbk_cache_release() pour tous les éléments en excès dans
le cache.
\*****************************************************************************/
void mbk_cache_set_limit_element( mbkcache *cache, 
                                  void *root, 
                                  unsigned int nbelem 
                                )
{
  cache->MAXELEM = nbelem ;
  mbk_cache_update_memory( cache, root );
}

/*****************************************************************************\
mbk_cache_update_memory()
Fonction interne.
Si la taille maximum du cache est atteinte ou dépassée, libère les éléments de
la mémoire
\*****************************************************************************/
void mbk_cache_update_memory( mbkcache *cache, void *root )
{
  mbkcachelist *lastlock;
  mbkcachelist *testremove;

  lastlock = NULL;
  while( cache->CURSIZE >= cache->CACHESIZE ||
         ( cache->MAXELEM > 0 && cache->CURELEM >= cache->MAXELEM ) ) {
  
    if( lastlock )
      testremove = lastlock->PREV ;
    else
      testremove = cache->LAST ;
    
    if( !testremove )
      break ;

    if( mbk_cache_islock( cache, testremove->DATA )==NO )
      mbk_cache_remove( cache, testremove, root );
    else
      lastlock = testremove ;

    if( cache->FIRST == lastlock ) {
      /* Le cache est saturé d'éléments vérrouillés */
      if( cache->MAXELEM > 0 && cache->CURELEM > cache->MAXELEM ) {
        avt_log(LOGMBKCACHE,2,"maximum number of element in cache is excedeed : max=%u current=%u\n", cache->MAXELEM, cache->CURELEM );
      }
      break;
    }
  }
}

/*****************************************************************************\
mbk_cache_add()
Fonction interne.
Ajoute un élément dans le cache.
\*****************************************************************************/
void mbk_cache_add( mbkcache *cache, void *root, void *elem )
{
  mbkcachelist *incache;
 
  avt_logenterfunction(LOGMBKCACHE,2,"mbk_cache_add()" );
  
  incache = mbk_cache_alloccachelist( cache );
  cache->CURSIZE = cache->CURSIZE + mbk_cache_call_load( cache, root, elem );
  
  incache->DATA   = elem;
  incache->PREV   = NULL;
  incache->NEXT   = cache->FIRST;
  incache->LOCKED = 0 ;
  cache->FIRST  = incache;
  if( !incache->NEXT )
    cache->LAST = incache;
  else
    incache->NEXT->PREV = incache;

  mbk_cache_setmbkcachelist( cache, elem, incache );
  cache->CURELEM++;
  avt_logexitfunction(LOGMBKCACHE,2);
}

/*****************************************************************************\
mbk_cache_remove()
Fonction interne.
Libère un élément présent du cache.
\*****************************************************************************/
void mbk_cache_remove( mbkcache *cache, mbkcachelist *incache, void *root )
{
  avt_logenterfunction(LOGMBKCACHE,2,"mbk_cache_remove()");
  cache->CURSIZE = cache->CURSIZE - mbk_cache_call_release( cache, root, incache->DATA );
  mbk_cache_delmbkcachelist( cache, incache->DATA );

  if( incache->PREV ) 
    incache->PREV->NEXT = incache->NEXT;
  else
    cache->FIRST = incache->NEXT;

  if( incache->NEXT )
    incache->NEXT->PREV = incache->PREV;
  else
    cache->LAST = incache->PREV;

  mbk_cache_freecachelist( cache, incache );
  cache->CURELEM--;
  avt_logexitfunction(LOGMBKCACHE,2);
}

/*****************************************************************************\
mbk_cache_makeitfirst()
Fonction interne.
Fait de l'élément incache le premier élément de la liste.
\*****************************************************************************/

void mbk_cache_makeitfirst( mbkcache *cache, mbkcachelist *incache )
{
  if( incache->PREV ) incache->PREV->NEXT = incache->NEXT ;
          
  if( incache->NEXT ) 
    incache->NEXT->PREV = incache->PREV;
  else 
    cache->LAST = incache->PREV;
  
  cache->FIRST->PREV = incache;
  incache->NEXT = cache->FIRST;
  incache->PREV = NULL;
      
  cache->FIRST = incache;
}

/*****************************************************************************\
Appels des fonctions utilisateur.
Fonctions internes.
\*****************************************************************************/

char mbk_cache_call_iscative( mbkcache *cache, void *root, void *elem )
{
  if( !cache->FN_ISACTIVE )
    return 1;

  return (cache->FN_ISACTIVE)(root, elem);
}

unsigned long int mbk_cache_call_load( mbkcache *cache, void *root, void *elem )
{
  return (cache->FN_LOAD)(root, elem);
}

unsigned long int mbk_cache_call_release( mbkcache *cache, void *root, void *elem )
{
  return (cache->FN_RELEASE)(root, elem);
}

/*****************************************************************************\
Accède à un mbkcachelist à partir du data.
Fonctions internes.
\*****************************************************************************/

mbkcachelist* mbk_cache_getmbkcachelist( mbkcache *cache, void *data )
{
  mbkcachelist *cachelist;

  cachelist = (mbkcachelist*)gethtitem( cache->INFOS, data );
  if( cachelist == (mbkcachelist*)EMPTYHT || cachelist == (mbkcachelist*)DELETEHT )
    return NULL;
  return cachelist;
}

void mbk_cache_setmbkcachelist( mbkcache *cache, void *data, mbkcachelist *cachelist )
{
  addhtitem( cache->INFOS, data, (long int)cachelist );
}

void mbk_cache_delmbkcachelist( mbkcache *cache, void *data )
{
  delhtitem( cache->INFOS, data );
}

/*****************************************************************************\
Fonctions d'allocation.
Fonctions internes.
\*****************************************************************************/

void mbk_cache_freecachelist( mbkcache *cache, mbkcachelist *incache )
{
  DelHeapItem( &(cache->HEAPCACHELIST), incache );
}

mbkcachelist* mbk_cache_alloccachelist( mbkcache *cache )
{
  mbkcachelist *elem;
  
  elem = (mbkcachelist*) AddHeapItem( & (cache->HEAPCACHELIST) );

  elem->NEXT   = NULL;
  elem->PREV   = NULL;
  elem->DATA   = NULL;
  elem->LOCKED = 0;

  return elem;
}

void mbk_cache_free( mbkcache *cache )
{
  DeleteHeap( &( cache->HEAPCACHELIST ) );
  delht( cache->INFOS );
  mbkfree( cache );
}

mbkcache* mbk_cache_alloc( void )
{
  mbkcache *cache;

  cache = (mbkcache*)mbkalloc( sizeof( mbkcache ) );

  cache->FIRST         = NULL;
  cache->LAST          = NULL;
  cache->CACHESIZE     = 0ul;
  cache->CURSIZE       = 0ul;
  cache->INFOS         = NULL;
  cache->FN_ISACTIVE   = NULL;
  cache->FN_LOAD       = NULL;
  cache->FN_RELEASE    = NULL;
  cache->MAXELEM       = 0;
  cache->CURELEM       = 0;
  CreateHeap( sizeof( mbkcachelist ), 
              CACHE_ALLOC_BLOCLIST, 
              &(cache->HEAPCACHELIST) 
            );

  return cache;
}

/*****************************************************************************\
Fonctions permettant de limiter le nombre maximum de fichiers ouverts.
\*****************************************************************************/

/*****************************************************************************\
mbk_cache_set_file()
Fonctions utilisateur.
Renvoie un index associé au fichier passé en paramètre.
\*****************************************************************************/
int mbk_cache_set_file( FILE *fd, char *filename, char *extension )
{
  cache_file    *elem ;
  
  if( !FILE_CACHE ) {
    FILE_CACHE = mbk_cache_create( NULL,
                                (unsigned long int (*)(void*,void*))mbk_cache_file_open,
                                (unsigned long int (*)(void*,void*))mbk_cache_file_close,
                                   MBK_MAX_CACHE
                                  );
    CreateHeap( sizeof( cache_file ), 16, &FILE_CACHE_HEAP );
    FILE_CACHE_HT = addht( 16 );
  }

  elem            = (cache_file*) AddHeapItem( &FILE_CACHE_HEAP );
  elem->IFILE     = ++FILE_CACHE_INDEX ;
  elem->PFILE     = fd ;
  elem->BASENAME  = mbkstrdup( filename );
  elem->EXTENSION = mbkstrdup( extension );
  elem->SIZE      = mbk_getfileacces( fd );
  addhtitem( FILE_CACHE_HT, (void*)((long)elem->IFILE), (long)elem );

  mbk_cache_refresh( FILE_CACHE, NULL, elem );

  return elem->IFILE ;
}

/*****************************************************************************\
mbk_cache_get_file()
Fonctions utilisateur.
Renvoie le fichier associé à l'index passé en paramètre
\*****************************************************************************/
FILE* mbk_cache_get_file( int id )
{
  cache_file    *elem ;

  if( !FILE_CACHE )
    return NULL ;

  elem = (cache_file*)gethtitem( FILE_CACHE_HT, (void*)((long)id));
  if( (long)elem == EMPTYHT || (long)elem == DELETEHT )
    return NULL ;
  
  mbk_cache_refresh( FILE_CACHE, NULL, elem );

  return elem->PFILE ;
}

/*****************************************************************************\
mbk_cache_clear_file()
Fonctions utilisateur.
Libère les informations associées à l'index passé en paramètre. Si le fichier 
est ouvert, il est fermé.
\*****************************************************************************/
void mbk_cache_clear_file( int id )
{
  cache_file    *elem ;
  
  if( !FILE_CACHE )
    return ;

  elem = (cache_file*)gethtitem( FILE_CACHE_HT, (void*)((long)id));
  if( (long)elem == EMPTYHT || (long)elem == DELETEHT )
    return ;
  
  mbk_cache_release( FILE_CACHE, NULL, elem );
  delhtitem( FILE_CACHE_HT, (void*)elem );
  mbkfree( elem->BASENAME );
  mbkfree( elem->EXTENSION );
  DelHeapItem( &FILE_CACHE_HEAP, elem );
}

/*****************************************************************************\
mbk_cache_file_open()
Fonction interne.
Fonction de refresh : elle est appellée pour ouvrir un fichier.
\*****************************************************************************/
unsigned long int mbk_cache_file_open( void *root, cache_file *elem )
{
  if( !elem->PFILE ) {
    elem->PFILE = mbkfopen_ext( elem->BASENAME,
                                elem->EXTENSION,
                                "r",
                                elem->SIZE,
                                1
                              );
    if( !elem->PFILE ) {
        avt_errmsg (MBK_ERRMSG, "019", AVT_FATAL,elem->BASENAME );
    }
  }
  root = NULL ;
  return 1 ;
}

/*****************************************************************************\
mbk_cache_file_close()
Fonction interne.
Fonction de libération : elle est appellée pour fermer un fichier.
\*****************************************************************************/
unsigned long int mbk_cache_file_close( void *root, cache_file *elem )
{
  if( elem->PFILE ) {
    fclose( elem->PFILE );
    elem->PFILE = NULL ;
  }
  root = NULL ;
  return 1 ;
}
