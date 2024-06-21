

extern void rcn_enable_cache( lofig_list *lofig, unsigned long int (*fn_load)(lofig_list*, losig_list*), void (*fn_free)(lofig_list*), int pool );
extern void rcn_disable_cache( lofig_list *lofig );

extern mbkcache* rcn_getlofigcache( lofig_list *lofig );
void rcn_alloclofigcache( lofig_list *lofig, mbkcache *cache, void(*)(lofig_list*) );

unsigned long int rcn_cache_release( lofig_list *lofig, losig_list *losig );

extern void rcn_refresh_signal(  lofig_list *lofig, losig_list *losig );
extern void rcn_flush_signal( lofig_list *lofig, losig_list *losig );

void rcn_setsignal_loaded( losig_list *losig );
void rcn_clearsignal_loaded( losig_list *losig );
extern char rcn_issignal_loaded( losig_list *losig );
extern void rcn_setsignal_disablecache( losig_list *losig );
extern char rcn_issignal_disablecache( losig_list *losig );

extern char rcn_cache_removable_ctc( loctc_list *ctc );
extern char rcn_cache_addable_ctc( losig_list *sig1, losig_list *sig2 );

extern void rcnenv(void);

extern int rcn_getnewpoolcache( void );
void rcn_setpoolcache( int pool, mbkcache *cache );
mbkcache* rcn_getpoolcache( int pool );

extern void rcn_lock_signal( lofig_list *lofig, losig_list *losig );
extern void rcn_unlock_signal( lofig_list *lofig, losig_list *losig );
extern char rcn_islock_signal( lofig_list *lofig, losig_list *losig );

// --- Debug function
void rcn_enablewritelofig( lofig_list *lofig );
void rcn_disablewritelofig( lofig_list *lofig );

void rcn_disablewritesignal( losig_list* );
void rcn_enblewritesignal( losig_list* );
char rcn_iswritesignal( losig_list* );
void rcn_checkwritesignal( losig_list *losig );
// --- End of debug function.
//
extern int rcn_hascache( lofig_list *lofig);

