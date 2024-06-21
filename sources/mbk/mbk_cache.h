typedef struct {
  int           IFILE ;
  FILE         *PFILE ;
  char         *BASENAME ;
  char         *EXTENSION ;
  char          SIZE ;
} cache_file ;

char              mbk_cache_call_iscative( mbkcache *cache, void *root, void *elem );
unsigned long int mbk_cache_call_load( mbkcache *cache, void *root, void *elem );
unsigned long int mbk_cache_call_release( mbkcache *cache, void *root, void *elem );

void mbk_cache_makeitfirst( mbkcache *cache, mbkcachelist *incache );
void mbk_cache_remove( mbkcache *cache, mbkcachelist *incache, void *root );
void mbk_cache_add( mbkcache *cache, void *root, void *elem );

void mbk_cache_update_memory( mbkcache *cache, void *root );

mbkcachelist* mbk_cache_getmbkcachelist( mbkcache *cache, void *data );
void mbk_cache_setmbkcachelist( mbkcache *cache, void *data, mbkcachelist *cachelist );
void mbk_cache_delmbkcachelist( mbkcache *cache, void *data );

mbkcache*     mbk_cache_alloc( void );
void          mbk_cache_free( mbkcache *cache );
mbkcachelist* mbk_cache_alloccachelist( mbkcache *cache );
void          mbk_cache_freecachelist( mbkcache *cache, mbkcachelist *incache );

unsigned long int mbk_cache_file_close( void *root, cache_file *elem );
unsigned long int mbk_cache_file_open( void *root, cache_file *elem );

#define CACHE_ALLOC_BLOCLIST 128
