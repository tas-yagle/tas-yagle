/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* declaration de variable */
extern tas_context_list *TAS_CONTEXT;
extern info_list      TAS_INFO ;

/* declaration de fonction */
extern int            tas_version      __P(( void ));
extern int            tas_GenPwl       __P(( FILE*,
                                             double,
                                             double,
                                             double,
                                             char*
                                          ));
extern int            tas_PwlFile      __P(( void ));
extern int            tas_PwlFileInput __P(( FILE*,
                                             char,
                                             char* 
                                          ));
extern int            tas_setenv       __P(( void ));
extern void           tas_restore      __P(( void ));
extern void           perfpath         __P(( char* ));
extern short          retkey           __P(( char* ));
extern int            optionword       __P(( char* ));
extern void           tas_init         __P(( void )) ;
extern int            tas_option       __P(( int, char**, char ));
extern int            tas_CloseTerm    __P(( void ));
extern int            tas_RestoreTerm  __P(( void ));
extern void           tas_GetKill      __P(( int ));
extern void           tas_handler_core __P(( void ));
extern info_list      tas_InitInfo     __P(( struct information ));
extern int            tas_PrintTime    __P(( struct rusage,
                                             struct rusage,
                                             long
                                          ));
extern void           tas_yaginit      __P(( void ));
extern void tas_addcapalink(link_list *link, float capa);
extern float tas_gettotalcapa(lofig_list *lofig, losig_list *losig, int type);
extern float tas_getcapalink(lofig_list *lofig, link_list *link, char event);
extern int tas_get_lotrs_current(lotrs_list *lotrs, long width, long length, float vgs, float vds, float vbs, float *ids);
extern float tas_get_current_rate(link_list *link);

extern void  tas_set_vpol    __P(( link_list*, float ));
extern float tas_get_vpol    __P(( link_list* ));
extern void  tas_clean_vpol  __P(( link_list* ));
extern long  tas_get_length_active __P(( lotrs_list* ));
extern long  tas_get_width_active __P(( lotrs_list*, long ));
void tas_SetPinSlew (locon_list *locon, float rise, float fall);
extern long  tas_get_pinslew __P((locon_list*, char));
extern void  tas_update_mcctemp __P(( inffig_list *ifl ));
extern int   tas_check_prop (link_list *link, double vdd, double vddin, double vt, double gate_th);
extern void tas_log_multivoltage (lofig_list *lofig);
extern chain_list* tas_find_corresp_link_from_lotrs( cone_list *cone, lotrs_list *lotrs, int where );
int tas_checkcnsfig(cnsfig_list *cf);


int tas_disable_gate_delay(char *input, cone_list *cn, int dirin, int dirout);
int tas_is_degraded_memsym(branch_list *branch, cone_list *cone, char type_tran);
void tas_remove_disable_gate_delay(cnsfig_list *cf);
void tas_add_disable_gate_delay(cnsfig_list *cf);
void tas_RemoveRC(lofig_list *lf);

#define TAS_SCALE_FLOAT ((int)1500000000)
