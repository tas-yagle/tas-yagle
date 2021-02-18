/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_pre.h                                                   */
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

/* declaration de fonction */
extern void           tas_setsigname        __P(( lofig_list *));
extern int            tas_compelmdelay      __P(( losig_list**, losig_list**));
extern void           tas_testelmore        __P(( lofig_list* ));
extern void           tas_parafactor        __P(( lofig_list *,
                                                  cnsfig_list *,
                                                  float,
                                                  float 
                                               )) ;
extern void           tas_capasig           __P(( lofig_list* ));
extern char           *tas_devect           __P(( char* ));
extern int            tas_testbleeder       __P(( cone_list*, long, char ));
extern edge_list      *tas_GetInLink        __P(( cone_list*, link_list* ));
extern short          tas_test_p            __P(( char* ));
extern int            tas_CorrectHZType     __P(( cone_list* ));
extern int            tas_TreatInfSig       __P(( inffig_list *ifl, cone_list* ));
extern int            init_timing           __P(( cnsfig_list* ));
extern void           tas_detectinout       __P(( cnsfig_list* ));
extern int            tas_capara            __P(( cnsfig_list* ));
extern int            tas_DetectConeConnect __P(( cnsfig_list* ));
extern long           tas_prelink           __P(( cnsfig_list* ));
extern int            delpath               __P(( cone_list*, branch_list* ));
extern long           tas_get_width_equiv_lotrs __P(( lotrs_list *lotrs1, lotrs_list *lotrs2, long w1, long l1, long w2, long l2 ));
extern int            del_para              __P(( cone_list*,
                                                  chain_list*,
                                                  chain_list*
                                               ));
extern long           number_maillon        __P(( branch_list* ));
extern int            tas_para              __P(( cone_list* ));
extern int            tas_switch_cmos       __P(( cone_list* ));
extern void           tas_addptype_switch_cmos __P(( cone_list* ));
extern void           tas_AddCapaDiff       __P(( lofig_list* ));
extern void           tas_AddCapaOut        __P(( cnsfig_list* ));
extern int            tas_NewConeAlim       __P(( cone_list*, long ));
extern int            tas_StuckAt           __P(( cnsfig_list* ));
extern list_list      *tas_AddList          __P(( list_list*, long, char*, void* ));
//extern list_list      *tas_traiteinout      __P(( list_list* ));
extern void           tas_traiteinout       __P((inffig_list *));
extern list_list      *tas_GetAliasSig      __P(( lofig_list*, list_list* ));
extern int            tas_DifLatch          __P(( cnsfig_list* ));
extern int            test_if_connec        __P(( lotrs_list* ));
extern void           tas_AffectConnectorsInputSlew __P((inffig_list *ifl,cnsfig_list*));
void tas_AffectConnectorsOutputCapacitance (inffig_list *ifl, cnsfig_list *cnsfig);

