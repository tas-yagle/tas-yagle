/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_ctl.h                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                Grégoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* Constantes pour les fonction stb_markagr() et stb_getchainevent() */
#define STB_CTK_WORST_AGR   ((char)(0x2))
#define STB_CTK_BEST_AGR    ((char)(0x1))

/* Constantes pour la définition d'une agression */
/* mode meilleur cas : */
#define STB_CTK_UP_WRST      ((char)(0x01))
#define STB_CTK_UP_BEST      ((char)(0x02))
#define STB_CTK_DW_WRST      ((char)(0x04))
#define STB_CTK_DW_BEST      ((char)(0x08))
#define STB_CTK_UP_MTX_BEST  ((char)(0x10))
#define STB_CTK_UP_MTX_WORST ((char)(0x20))
#define STB_CTK_DW_MTX_BEST  ((char)(0x40))
#define STB_CTK_DW_MTX_WORST ((char)(0x80))
/* mode pire cas : */
#define STB_CTK_UP_NOWRST   ((char)(0x1))
#define STB_CTK_UP_NOBEST   ((char)(0x2))
#define STB_CTK_DW_NOWRST   ((char)(0x4))
#define STB_CTK_DW_NOBEST   ((char)(0x8))

/* Ptype des rcxparam */
#define STB_CTK_RCXTTVSIG   ((long) 0x43544B15 ) /* Pointeur vers le ttvsig */

/* Ptype des event */
#define STB_CTK_RCXPARAM      ((long) 0x43544B14 ) /* pointeur vers rcxparam */
#define STB_CTK_OLDMINSLOPE   ((long) 0x43544B17 ) /* slope                  */
#define STB_CTK_OLDMAXSLOPE   ((long) 0x43544B18 ) /* slope                  */
#define STB_CTK_MARKREALACTIF ((long) 0x43544B19)
/* Ptype des ttvsig */
#define STB_CTK_RCXAGRPT    ((long) 0x43544B15 ) /* Agresseurs stockés sous la
                                                    forme d'une ptype list    */
#define STB_CTK_RCXAGRHT    ((long) 0x43544B16 ) /* Agresseurs stockés sous la
                                                    forme d'une table de hash */
#define STB_CTK_EXCLUDED    ((long) 0x43544B1A )
/* Ptype des ttvfig */
#define STB_CTK_TTVTABSIG   ((long) 0x43544B20 ) /* ht des signaux           */

/* Ptype des stbfig */
#define STB_CTK_STAT        ((long) 0x43544B11 ) /* tableau résultat crosstalk */

/* Ptype temporaire des ttvsig */
#define STB_CTK_DRIVER      ((long) 0x43544B21)

/* Nombre d'élément maximum dans la ptype_list                          */
#define STB_CTK_MAXCHAIN ((int)10)

/* Valeur de retour pour stb_hasslopechanged */
#define STB_NO  0       // Non
#define STB_YES 1       // Oui
#define STB_UNK 2       // L'info n'est pas disponible

// Filtres pour les reports.
extern long  CTK_REPORT_DELTA_DELAY_MIN;
extern long  CTK_REPORT_DELTA_SLOPE_MIN;
extern float CTK_REPORT_CTK_MIN;
extern long  CTK_REPORT_NOISE_MIN;
extern long  STB_CTK_MARGIN;
extern int   STB_CTK_NOINFO_ACTIF;

typedef struct {
  ttvline_list  *LINE ;
  float          VALUE ;
} sortdelaycmp ;

extern void (*CTK_PROGRESSBAR)(int, int, int, char*);

extern void stb_ctk_env           __P(( void ));
chain_list* stb_createchainevent  __P(( chain_list *headagr, long type ));
void stb_progressbar              __P(( int, int ));
rcxparam* stb_getrcxparamfromevent __P(( ttvevent_list* ));
void stb_addrcxparamfromevent     __P(( ttvevent_list*, rcxparam* ));
void stb_delrcxparamfromevent     __P(( ttvevent_list* ));
char stb_getagressiontype         __P(( ttvsig_list*, ttvsig_list* ));
void stb_setagressiontype         __P(( ttvsig_list*, ttvsig_list*, char ));
void stb_marqactiffromgap         __P(( ttvevent_list*, stbgap_list* ));
chain_list* stb_creatchainevent   __P(( chain_list*, long ));
chain_list *stb_getchainevent     __P(( ttvevent_list*, char, chain_list* ));
void stb_freechainevent           __P(( chain_list* ));
void stb_detectactiveagressorworst __P(( stbfig_list*,
                                        long,
                                        long,
                                        ttvevent_list*, 
                                        chain_list* 
                                     ));
void stb_detectactiveagressorbest  __P(( stbfig_list*,
                                        long,
                                        long,
                                        ttvevent_list*, 
                                        chain_list* 
                                     ));
void stb_fillttvsigrcxparam       __P(( ttvfig_list*, 
                                        long, 
                                        long, 
                                        chain_list* 
                                     ));
long stb_getminoldslope           __P(( ttvevent_list* ));
long stb_getmaxoldslope           __P(( ttvevent_list* ));
void stb_setminoldslope           __P(( ttvevent_list*, long ));
void stb_setmaxoldslope           __P(( ttvevent_list*, long ));
void stb_cleanoldslope            __P(( ttvevent_list* ));
void stb_ctk_clean_oldslope       __P(( stbfig_list* ));
int  stb_hasslopechanged          __P(( ttvevent_list*,
                                        ttvfig_list*,
                                        long,
                                        long
                                     ));
void stb_saveslope                __P(( ttvevent_list*,
                                        ttvfig_list*, 
                                        long, 
                                        long 
                                     ));
int stb_needeval                  __P(( ttvfig_list*,
                                        long,
                                        ttvevent_list*,
                                        long,
                                        char,
                                        chain_list*
                                     ));
void stb_delttvsigrcxparam        __P(( chain_list* ));
void stb_fillactifrcxparam        __P(( ttvfig_list*, 
                                        ttvevent_list*, 
                                        chain_list*,
                                        char
                                     ));
void stb_fillinactifrcxparam      __P(( ttvfig_list*, 
                                        ttvevent_list*, 
                                        chain_list*,
                                        char
                                     ));
ttvsig_list* stb_getttvsigrcxparam __P(( rcxparam* ));
int stb_saveactifrcxparam         __P(( ttvfig_list*, 
                                        ttvevent_list*, 
                                        chain_list* 
                                     ));
int stb_saveinactifrcxparam       __P(( ttvfig_list*, 
                                        ttvevent_list*, 
                                        chain_list* 
                                     ));
int stb_calcctkdelaynode          __P(( stbfig_list*, 
                                        ttvevent_list*,
                                        long,
                                        long,
                                        char,
                                        long*,
                                        int,
                                        int
                                     ));
int  stb_calcctkdelay             __P(( stbfig_list*, long*, int ));
void stb_ctk_clean                __P(( stbfig_list* ));
int  stb_ctk                      __P(( stbfig_list* ));
void stb_ctk_exit                 __P(( int, char*, ... ));
void ctk_setprogressbar           __P(( void (*callfn)( int, int, int, char* ) ));
void stb_info                   __P(( stbfile*, ... ));
extern stbfile* stb_info_open          __P(( stbfig_list *stbfig ));
extern void stb_info_close             __P(( stbfile *file ));
void stb_ctk_error                __P(( int, char*, ... ));
char* stb_display_ttvevent( ttvfig_list *fig, ttvevent_list *node );
char* stb_display_ttvsig( ttvfig_list *fig, ttvsig_list *ttvsig );
char* stb_display_ttvevent_noise( ttvfig_list *ttvfig, ttvevent_list *node );
void ctk_display_event( stbfig_list *stbfig, ttvevent_list *refevent );
void stb_drive_line               __P(( stbfile*, 
                                        ttvfig_list*, 
                                        ttvline_list*
                                     ));
void stb_built_ttv_htab           __P(( ttvfig_list* ));
void stb_resync( stbfig_list *stbfig );
stb_ctk_detail* stb_ctk_get_detail( stbfig_list *stbfig, ttvevent_list *ptevent );
void stb_ctk_free_detail( stbfig_list *stbfig, stb_ctk_detail *detail );
stb_ctk_detail_agr_list* stb_ctk_get_node_detail( ttvsig_list *ttvsig );
void ctk_calc_constraint( stbfig_list *stbfig );
void stb_clean_ttvfig( stbfig_list *stbfig );
int stb_drive_crosstalk_start( stbfile *report );
int stb_drive_crosstalk_detail( stbfile *report, stb_ctk_detail *detail, stb_ctk_tab_stat *stat );
int stb_drive_crosstalk_end( stbfile *report );
void stb_ctk_stat_event( stbfig_list *stbfig, stb_ctk_detail *ctkdetail, stb_ctk_tab_stat *stat );
stb_ctk_stat* stb_ctk_fill_stat( stbfig_list *stbfig, int fast );
void stb_ctk_set_stat( stbfig_list *stbfig, stb_ctk_stat *stat );
void stb_ctk_free_stat( stb_ctk_stat *stat );
void stb_ctk_handle_mutex( ttvfig_list *ttvfig, ttvevent_list *event, chain_list* headagr, char actif );
void stb_clean_mark_input( chain_list *tocleanmark );
int stb_is_mark_input( ttvsig_list *ttvsig ) ;
chain_list* stb_mark_input( ttvfig_list *ttvfig, ttvsig_list *victime, long level );
void stb_remove_input( ttvfig_list *ttvfig, ttvsig_list *ttvsig, long level, chain_list *headagr );
void stb_ctk_fill_stat_tab( stb_ctk_stat *stat, stbfig_list *stbfig );
void stb_ctk_fill_stat_line( stb_ctk_stat *stat, stbfig_list *stbfig );
void stb_ctk_fill_stat_line_one( stb_ctk_tab_delay *delay, ttvline_list *line );
char calc_pct( long vi, long vf );
extern void stb_ctk_drive_stat( stbfile *report, stbfig_list *stbfig, stb_ctk_stat *stat );
int stb_ctk_sort_delay_cmp( sortdelaycmp *delay1, sortdelaycmp *delay2 );
extern void stb_ctk_sort_delay( stb_ctk_stat *stat, char criterion, float delta );
void stb_progress( int iteration, int max, int pos, char *msg );
void stb_print( char *msg );
void ctk_setprint( void (*print)( char* ) );
inline void filltabdelayvalue( sortdelaycmp *elem, char criterion );
int sort_drive_detail( stb_ctk_detail_agr_list **ag1, stb_ctk_detail_agr_list**ag2 );
int stb_saveinactifcoupling( ttvevent_list *victime, ttvsig_list *aggressor, int rcxagression );
int stb_saveactifcoupling( ttvevent_list *victime, ttvsig_list *aggressor, int rcxagression );
stb_ctk_gap* stb_fill_gap( stbfig_list *stbfig, stb_ctk_detail *detail, char transition );
