/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

extern chain_list *TTV_NAME_IN ;
extern chain_list *TTV_NAME_OUT ;
extern char TTV_LANG ;

extern char*  ttv_getstmmodelname   __P(( ttvfig_list*, 
                                          ttvline_list *, 
                                          int,
                                          char
                                       ));
extern char*  ttv_revect            __P(( char* ));
extern char*  ttv_devect            __P(( char* ));
extern char*  ttv_motifinname       __P(( char*, char* ));
extern void   ttv_subelt            __P(( char*, char*, char**, char ));
extern int    ttv_jokersubst        __P(( char*, char*, char ));
extern int    ttv_testmask          __P(( ttvfig_list*,
                                          ttvsig_list*,
                                          chain_list*
                                       ));
extern int ttv_testnetnamemask(ttvfig_list* ttvfig, ttvsig_list* ptsig, chain_list* mask);

extern void   ttv_setttvlevel       __P(( ttvfig_list* ));
extern void   ttv_setsigttvfiglevel __P(( ttvfig_list* ));
extern void   ttv_setttvdate        __P(( ttvfig_list*, long ));
extern char*  ttv_checkfigname      __P((char *)) ;
extern int    ttv_checkdate         __P(( ttvinfo_list*, ttvinfo_list* ));
extern int    ttv_checktechno       __P(( ttvinfo_list*, ttvinfo_list* ));
extern int    ttv_checktool         __P(( ttvinfo_list*, ttvinfo_list* ));
extern void   ttv_checkfigins       __P(( ttvfig_list*, ttvfig_list* ));
extern void   ttv_checkallinstool   __P(( ttvfig_list* ));
extern void   ttv_setcachesize      __P(( double, double ));
extern double ttv_getsigcachesize   __P(( void ));
extern double ttv_getlinecachesize  __P(( void ));
extern double ttv_getsigmemorysize  __P(( void ));
extern double ttv_getlinememorysize __P(( void ));
extern void   ttv_getinffile        __P(( ttvfig_list* ));
extern void   ttv_addaxis           __P(( inffig_list *ifl, ttvfig_list* ttvfig ));

extern chain_list*   ttv_getclocklist        __P(( ttvfig_list* ));
extern chain_list*   ttv_getclocksiglist     __P(( ttvfig_list* ));
void ttv_post_traitment(ttvfig_list *ttvfig);
void ttv_init_stm(ttvfig_list *ttvfig);
ttvpath_list *ttv_mergepathlists(ttvpath_list *pathx, int nbx, ttvpath_list *path, int nb, int max, long type, int *nbtot);
ttvpath_list *ttv_keep_critic_paths(ttvpath_list *path, int *nb, long type, int multiplecmd);
void ttv_set_UTD_slope(ttvevent_list *tve, long slope);
void ttv_set_UTD_outputcapa(ttvsig_list *tvs, float capa);
int ttv_get_path_margins_info(ttvfig_list *tvf, ttvpath_list *path, float *factor, long *delta);
void ttv_disablecache(ttvfig_list *ttvfig);
ht *ttv_buildquickaccessht(ttvfig_list *tvf, NameAllocator *NA);

