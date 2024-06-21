/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_tp.h                                                    */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* declaration de variable */
extern long FHH ;
extern long FLL ;
extern long CCUSED ;
extern char TAS_PATH_TYPE ;

/* declaration de fonction */
//extern void         tas_addrcdelaylink   __P(( link_list*,
//                                               long
//                                            )) ;
//extern void         tas_delrcdelaylink   __P(( link_list*)) ;
//extern long         tas_getrcdelaylink   __P(( link_list*)) ;
//extern void         tas_addrcresislink   __P(( link_list*,
//                                               long
//                                            )) ;
//extern void         tas_delrcresislink   __P(( link_list*)) ;
//extern long         tas_getrcresislink   __P(( link_list*)) ;
extern double       tas_get_rlin_br      __P(( link_list*)); 
extern double       tas_get_rlin_br_krt  __P(( link_list *link ));
extern float        tas_delayrc          __P(( link_list*,
                                               link_list*
                                            ));
extern float        tas_delayrcchemin    __P(( branch_list*, link_list *));
extern front_list*  tas_getslope         __P(( cone_list*, 
                                               cone_list*
                                            ));
extern double       tas_getcapabl        __P(( link_list* ));
extern short        tas_PathFonc         __P(( cone_list*,
                                               branch_list*
                                            ));
extern double       tas_GetRint          __P(( cone_list*,
                                               edge_list*,
                                               long,
                                               char
                                            ));
extern int          affiche_maillon      __P(( link_list* ));
extern int          affiche_time         __P(( cone_list*,
                                               cone_list*,
                                               char*,
                                               long
                                            ));
extern link_list*   existe_tpd_maillon   __P(( branch_list*,
                                               link_list*,
                                               cone_list*,
                                               char
                                            ));
extern branch_list* existe_tpd           __P(( cone_list*,
                                               cone_list*,
                                               long,
                                               char,
                                               link_list**
                                            ));
extern branch_list*   existe_conflit       __P(( cone_list*,
                                               cone_list*,
                                               long,
                                               char,
                                               int,
                                               int,
                                               link_list**
                                            ));
extern double       i_brmcc              __P(( link_list*, link_list*, float ));
extern timing_scm*  tpd_dualmodel        __P(( cone_list *,
                                               branch_list*,
                                               link_list*,
                                               double,
                                               double,
                                               branch_list*,
                                               double,
                                               float,
                                               float,
                                               float,
                                               float,
                                               float,
                                               float,
                                               double,
                                               double,
                                               double,
                                               double*,
                                               char,
                                               double
                                            ));
extern delay_list*  val_dual             __P(( cone_list*,
                                               cone_list*,
                                               delay_list*,
                                               long,
                                               double

                                            ));
extern timing_scm*  tpd_pathmodel        __P(( cone_list*,
                                               link_list*,
                                               char,
                                               double,
                                               double,
                                               double,
                                               double,
                                               double,
                                               float,
                                               float
                                            ));
//extern int          affiche_conec        __P(( cone_list*,
//                                               locon_list*,
//                                               char*,
//                                               long
//                                            ));
extern branch_list* tas_GiveActivExtPath __P(( cone_list*, locon_list*, char ));
extern delay_list*  val_con              __P(( cone_list*,
                                               locon_list*,
                                               delay_list*,
                                               long,
                                               double
                                            ));
extern double       f6                   __P(( double, 
                                               double 
                                            ));
extern timing_scm*  tpd_trans_goodmodel  __P(( cone_list *,
                                               branch_list*,
                                               link_list*,
                                               double,
                                               double,
                                               double,
                                               double,
                                               double,
                                               double,
                                               float,
                                               float,
                                               float,
                                               float,
                                               double
                                            ));
extern timing_scm*  tpd_trans_falsemodel __P(( cone_list*,
                                               link_list*,
                                               link_list*,
                                               double,
                                               double,
                                               double,
                                               double,
                                               double,
                                               double,
                                               float,
                                               float,
                                               float,
                                               float
                                            ));
extern double       calcul_FB            __P(( cone_list*,
                                               branch_list*,
                                               char
                                            ));
extern delay_list*  val_non_dual         __P(( cone_list*,
                                               cone_list*,
                                               delay_list*,
                                               long,
                                               double

                                            ));
extern int          tas_tpd              __P(( cone_list* ));

extern double       tas_get_an           __P((double,
                                              double, 
                                              double, 
                                              double
                                            ));
extern double       tas_get_bn           __P((double, 
                                              double, 
                                              double, 
                                              double
                                            ));
extern float        tas_getcapaparams    __P((cone_list*,
                                              branch_list*, 
                                              link_list*, 
                                              double, 
                                              double, 
                                              double, 
                                              double*,
                                              double*,
                                              double*,
                                              char,
                                              int
                                            ));
extern double      tas_get_cgd           __P((double,
                                              double,
                                              double,
                                              double,
                                              double,
                                              double,
                                              double,
                                              double,
                                              double
                                            ));
extern void        tas_getcapaparamsgood __P((branch_list*,
                                              link_list*, 
                                              link_list*, 
                                              double, 
                                              double, 
                                              double*,
                                              double*
                                            ));
extern void         tas_getmcc           __P((link_list*,
                                              link_list*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*, 
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*
                                            ));
extern double       tas_getfin           __P((link_list*,
                                              cone_list*, 
                                              float,
                                              stm_pwl**,
                                              float*,
                                              float*,
                                              float*
                                            ));
extern void         tas_getRCbranch      __P((link_list*,
                                              double*,
                                              double*
                                            ));
extern void         tas_getresforconflit __P((link_list *,
                                              link_list *, 
                                              double*,
                                              double*
                                            ));
extern void         tas_getwlr2effdual   __P((link_list*, 
                                              double,
                                              double*,
                                              double*
                                            ));
extern void         tas_getwlr2effgood   __P((link_list*, 
                                              double,
                                              double*,
                                              double*
                                            ));
extern double       tas_getfindown       __P((cone_list*, 
                                              locon_list*,
                                              float
                                            ));
extern double       tas_getfinup         __P((cone_list*, 
                                              locon_list*,
                                              float
                                            ));
extern int         tas_getfalseparam    __P((link_list*,
                                              double*,
                                              double*
                                            ));
extern link_list*   tas_splitbranch      __P((link_list*, 
                                              link_list*
                                            )); 
extern link_list*   tas_getpathlink      __P((link_list*, 
                                              link_list*
                                            )); 
extern link_list*   tas_getfirstdual     __P((link_list*, 
                                              link_list*
                                            )); 
extern double       tas_get_vdd_path     __P((link_list* 
                                            ));
extern double       tas_get_vdd_input    __P((link_list* 
                                            ));
extern int         tpd_pathparams       __P((cone_list*,
                                              link_list*, 
                                              char,
                                              float*,
                                              float*,
                                              float*
                                            ));
extern void         capa_eqparampath     __P((link_list*,
                                              double,
                                              double,
                                              double,
                                              double,
                                              float*,
                                              float*,
                                              char*,
                                              char*
                                            ));
extern void         capa_eqparam         __P((link_list*,
                                              double,
                                              double,
                                              double,
                                              double,
                                              float*,
                                              float*,
                                              float*,
                                              char*,
                                              char*,
                                              char*
                                            ));
extern void         tas_getconflictparam __P((cone_list*,
                                              cone_list*,
                                              long,
                                              char,
                                              link_list*,
                                              link_list*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              double*,
                                              branch_list**,
                                              char
                                            ));
extern void         UpdateDelayMax       __P((delay_list*,
                                              delay_list*
                                            ));
extern delay_list*  tas_extract          __P((cone_list*,
                                              edge_list*,
                                              double*
                                            ));
extern double       tas_thr2scm          __P((double,
                                              link_list*,
                                              char
                                            ));
extern void       tas_DupTabFloat        __P((float*,
                                              double*,
                                              int
                                            ));
extern void       tas_evalmodel1Dloadfix __P((timing_model*,
                                              timing_model*,
                                              double*,
                                              double*,
                                              int,
                                              char*,
                                              char*,
                                              double
                                            ));
extern void       tas_evaldelay1Dloadfix __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              double*,
                                              double*,
                                              int,
                                              char,
                                              double
                                            ));
extern void       tas_eval1Dloadfix      __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              delay_list*,
                                              double*,
                                              double*,
                                              int,
                                              double
                                            ));
extern void       tas_evalmodel1Dslewfix __P((timing_model*,
                                              timing_model*,
                                              double*,
                                              int,
                                              char*,
                                              char*,
                                              double,
                                              double
                                            ));
extern void       tas_evaldelay1Dslewfix __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              double*,
                                              int,
                                              char,
                                              double
                                            ));
extern void       tas_eval1Dslewfix      __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              delay_list*,
                                              double*,
                                              int,
                                              double
                                            ));
extern void       tas_evalmodel2D        __P((timing_model*,
                                              timing_model*,
                                              double*,
                                              double*,
                                              int,
                                              double*,
                                              int,
                                              char*,
                                              char*,
                                              double
                                            ));
extern void       tas_evaldelay2D        __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              double*,
                                              double*,
                                              int,
                                              double*,
                                              int,
                                              char,
                                              double
                                            ));
extern void       tas_eval2D             __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              delay_list*,
                                              double*,
                                              double*,
                                              int,
                                              double*,
                                              int,
                                              double
                                            ));
extern void       tas_newname            __P((char*,
                                              int
                                            ));
extern float      tas_getcapai           __P((cone_list*));
extern void       tas_store_scm          __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              char
                                            ));
extern void     tas_store_scm_minandmax  __P((cone_list*,
                                              edge_list*,
                                              delay_list*,
                                              delay_list*
                                            ));
extern void       tas_destroymodscm      __P((delay_list*,
                                              delay_list*
                                            ));
extern void       tas_compute_delays     __P((timing_model*,
                                              cone_list*,
                                              float,
                                              float,
                                              float,
                                              float,
                                              stm_pwl*,
                                              stm_pwl**,
                                              long*,
                                              long*,
                                              output_carac**,
                                              link_list*,
                                              char*,
                                              char,
                                              char
                                            ));
extern void    tas_getparamsforslope     __P((cone_list*,
                                              link_list*,
                                              char,
                                              float*,
                                              float*,
                                              float*,
                                              float*
                                            ));
extern void tas_getparamsforinputslope __P(( cone_list *cone, 
                                             char type, 
                                             float *vt, 
                                             float *vth, 
                                             float *vend,
                                             float *vdd
                                          ));
extern int tas_is_switchdegraded_branch __P((branch_list *br));
double tas_threshold                   __P(( double an, 
                                             double bn, 
                                             double vtn, 
                                             double ap, 
                                             double bp, 
                                             double vtp, 
                                             double vdd 
                                          ));
extern link_list* tas_get_last_active_link __P(( link_list *active ));
double tas_get_cgd_for_conflict_branch __P(( link_list*, link_list* ));
void tas_getraccess __P(( link_list*, double*, double* ));
double tas_get_rlin __P(( link_list *link ));
double tas_get_cgd_from_link( cone_list *cone, link_list *link, double vdd, double vds, double vt, double vgsi, double vgsf, double vsi, double vsf, char side, char addswitch ) ;
void tas_getcdeltaswitch( cone_list *cone, link_list *link0, link_list *activelink, link_list *linkconf0, double c, float fin, char sensactif, double *pconf0, double *pconf1, double *chalf, double *qinit );
void tas_getinverterforswitch( cone_list *cone, link_list *activelink, lotrs_list *trspair, link_list **retlink, long *retdelay );
float tas_getvgsdeg( link_list *pairlink, float delta, float fdeg, float t );
float tas_getvgsdeg_old( cone_list *cone, link_list *pairlink, float delta, float t );
float tas_get_current( branch_list *branch, link_list *head, float level, char savepol );
float tas_get_current_leakage( branch_list *branch, link_list *head, link_list *activelink );
float tas_get_current_leakage_2( branch_list *branch, link_list *head, link_list *activelink );
void tas_get_point_rsat_br( branch_list *branch, double vddmax, double imax, double *isat, double *vsat, double *ilin, double *vlin );
double tas_get_a(double e1, double imax, double b);
double tas_get_b(double e1, double e2, double imax, double i2);
double tas_get_i(double a, double b, double r, double v);
double tas_getcdeltaswitch_static( link_list *bractive, link_list *brconf, link_list *activelink, link_list *pairlink, double vgactive, double vgpair, double vdd, float *imax );
void tasdebuglotrs();
float tas_getrintercone( cone_list *cone, cone_list *cone_avant );
void tas_Leakage_Calculation(cone_list *cone);

