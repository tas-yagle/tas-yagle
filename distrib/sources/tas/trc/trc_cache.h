// USER des lofig

#define RCX_FILEPOS     0x5243580F
// Gestion des offsets dans les fichiers pour le cache.

#define RCX_CACHE_SET 0x52435810
// Indique le la lofig est lue avec le cache actif.

/* Active le cache de la description des RC. Ce chache ne doit être activé que 
sur des lofig qu'on lit à partir d'un fichier. L'appel doit se faire avant
l'appel à la fonction rcx_getlofig(). Chaque lofig sait si elle a été lue avec
ou sans cache. */
extern void rcx_cache_enable( void );

unsigned long int rcx_cache_parse( lofig_list *lofig, losig_list* );
void rcx_set_cache_lofig( lofig_list *lofig, char *filename, char *extname, unsigned long int (*fn_load)(lofig_list*, losig_list*), void (*fn_free)(lofig_list*) );
int rcx_isset_cache_lofig( lofig_list *lofig );
FILE* rcx_cache_getlofigfile( lofig_list *lofig );
void rcx_cache_setlofigfile( lofig_list *lofig, FILE*, char* );
void rcx_cache_clearlofigfile( lofig_list *lofig );

struct rcxcache {
  struct rcxcache *NEXT;
  struct rcxcache *PREV;
  losig_list      *LOSIG;
};

struct rcxfilepos {
  FILE            *PTF;
  long             POS;
  char             COMPRESSED;
};

/* La configuration du cache */
extern char RCX_USECACHE;
