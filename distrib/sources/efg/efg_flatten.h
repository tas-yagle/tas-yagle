
/******************************************************************************\
DEFINES    
\******************************************************************************/

/******************************************************************************\
VARIABBLES 
\******************************************************************************/
extern efg_global *EFGCONTEXT;

/******************************************************************************\
FONCTIONS  
\******************************************************************************/

//=> Allocations
extern efg_fullsiglist *efg_addchainfullsig ( efg_fullsiglist *head );
extern void             efg_freechainfullsig ( efg_fullsiglist *head );
extern efg_srcsiglist  *efg_addchainsrcsig ( efg_srcsiglist* head );
extern void             efg_freechainsrcsig ( efg_srcsiglist* head );
extern efg_ctxlist*     efg_getcontext( char *context );
extern void             efg_createcontext ( void );
extern void             efg_freecontext ( void );
extern void             efg_setsrcfig2context ( lofig_list *fig);
extern void             efg_setdestfig2context ( lofig_list *fig);
extern void             efg_setspisig2context ( spisig_list *spisig);
extern void             efg_setdestlocon( char *context, locon_list *locon, locon_list *newlocon );
extern locon_list*      efg_getdestlocon( char *context, locon_list *locon );

//=> Gestion du contexte
extern void             efg_setfullsig ( char *context,
                                         lofig_list *lofig, 
                                         losig_list *losig, 
                                         int level,
                                         efg_fullsiglist *fullsig 
                                       );
extern void             efg_clearfullsig( char *context, losig_list *losig );
extern int              efg_isloinscopied( char *context, char *insname );
extern void             efg_setloinscopied( char *context, char *insname );
extern void             efg_set_sig2srcsig ( char *context, losig_list *losig, efg_srcsiglist *srcsig );
extern efg_srcsiglist*  efg_getsrcsig( char *context, losig_list *losig );

//=> A travers la hierarchie
extern char            *efg_getsigname ( efg_fullsiglist *fullsig );
extern char             efg_getdir ( efg_fullsiglist *fullsig );
extern locon_list*      efg_getexternallocon( efg_fullsiglist *fullsig );
extern efg_srcsiglist*  efg_gettopsig( efg_fullsiglist *fullsig );
//=>
extern chain_list      *efg_getsigtobecopied ( char *context );
extern int              efg_isloinsbasic ( char *context );
extern chain_list      *efg_getloinstobetreated ( char *context );

//=> 
extern efg_fullsiglist *efg_buildfullsig( char *context,
                                          char withexternal,
                                          efg_fullsiglist *head,
                                          int level
                                          );
extern void            efg_recgetmerge ( efg_fullsiglist *head,
                                         lofig_list      *lofig,
                                         char            *context,
                                         losig_list      *losig,
                                         int              level
                                       );
extern efg_fullsiglist *efg_getmerge ( efg_fullsiglist *head,
                                       char            *context,
                                       losig_list      *currentsig,
                                       int level 
                                     );
extern efg_fullsiglist *efg_buildhierfullsig ( char            *context,
                                               char             withexternal,
                                               efg_fullsiglist *head,
                                               int              level
                                             );
extern void             efg_createalllosig ( efg_fullsiglist *siglist );
extern void             efg_set_loins_rc_node ( loins_list *loins );
extern void             efg_createloins ( char *context, loins_list *loins );
extern void             efg_createallloins ( efg_fullsiglist *siglist );
extern void             efg_addtabconv( efg_srcsiglist *siglist, int decalage );
extern int              efg_getequivnode( efg_srcsiglist *sig, int index );
extern int              efg_buildtabconv( efg_fullsiglist *fullsig, 
                                          efg_srcsiglist  *topsig,
                                          int              decalage
                                        );
extern void             efg_cleantabconv( efg_fullsiglist *fullsig );
extern void             efg_copyrc( efg_srcsiglist *srcsig, losig_list *dstsig, losig_list *dstvss );
extern void             efg_createrc ( efg_fullsiglist *siglist );
extern void             efg_createallrc ( efg_fullsiglist *siglist );
extern void             efg_set_path_info ( efg_fullsiglist *siglist );
extern chain_list      *efg_get_loins_on_path ( void );
extern void             efg_buildlofig( efg_fullsiglist *siglist );
extern void             efg_exit( int index );
