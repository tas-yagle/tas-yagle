extern FILE* CTK_LOGFILE;
extern int CTK_LOGLEVEL;
void stb_log_chain( stbfig_list *stbfig, chain_list *headagr, char *title );
void stb_ctkprint( int, char*, ... );
void stb_debug_stab( stbfig_list *stbfig, ttvevent_list *evtvic, chain_list *headagr );
/****** infos conso mémoire *******/
#define STBMEMPASS 0x6000001A
void stb_display_mem( stbfig_list *stbfig );
void stb_mem_hash( ptype_list *USER, long pthash, long ptptype, unsigned int *util, unsigned int *real, unsigned int *lost);
int stb_init_debug_node( stbfig_list *stbfig, int level, long type );
void stb_debug_mark_aggressor( stbfig_list *stbfig, int level, long type, ttvevent_list *event );
void stb_debug_mark_ttvsig( ttvsig_list *ttvsig );
