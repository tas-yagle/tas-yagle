/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Verison 5                                               */
/*    Fichier : tas_class.h                                                 */
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
extern int            tas_isnowire        __P(( losig_list * )) ;
extern int            tas_isnorcdelay     __P(( losig_list * )) ;
extern void           tas_movectc         __P((lofig_list *,
                                               losig_list *,
                                               losig_list * 
                                             )) ;
extern void           tas_getfcritic      __P(( cone_list*,
                                                front_list *,
                                                front_list *
                                             )) ;
extern void           tas_calcrcslope     __P(( cnsfig_list*)) ;
extern void           tas_comdelay        __P(( delay_list*,
                                                delay_list*,
                                                long
                                             )) ;
extern void           tas_cleanloconin    __P(( cnsfig_list*)) ;
extern void           tas_mergercdelay    __P(( cnsfig_list*)) ;
extern losig_list*    tas_getlosigcone    __P(( cone_list* ));
extern void           tas_setelmlimits    __P(( 
                                                losig_list*,
                                                long,
                                                long
                                             ));
extern void           tas_tprc            __P(( cnsfig_list* ));
extern long           tas_tprcincone      __P(( inffig_list *ifl,
                                                cone_list*,
                                                edge_list *,
                                                char,
                                                char,
                                                long  
                                             ));
extern void           tas_flatrcx         __P(( lofig_list*,
                                                cnsfig_list*,
                                                ttvfig_list * 
                                             ));
extern int            visu_branche        __P(( branch_list* ));
extern void           tas_caparabl        __P(( cone_list* ));
extern int            res_branche         __P(( cone_list* ));
extern branch_list*   clas_liste_branche  __P(( branch_list* ));
extern void           clas_branche        __P(( cone_list* ));
extern long           valfupfdown         __P(( branch_list*, char ));
extern branch_list*   decision            __P(( char,
                                                char,
                                                branch_list*,
                                                branch_list*
                                             ));
extern long           front               __P(( cone_list* ));
extern int            interface           __P(( cnsfig_list* ));
extern cone_list*     tas_findbufdelay    __P(( cone_list*,
                                                long*,
                                                long*,
                                                char
                                             ));
extern int            tas_buftoinv        __P(( cone_list* ));
extern void           tas_detectbuf       __P(( cnsfig_list* ));
extern int            tas_timing          __P(( cnsfig_list*,
                                                lofig_list * ,
                                                ttvfig_list *
                                              ));
extern chain_list*    tas_reducechaincon  __P(( chain_list*));
extern int            tas_get_output_carac __P(( cone_list *cone, 
                                                 char type, 
                                                 char trans,
                                                 output_carac_trans *infos
                                              ));
extern void           tas_set_output_carac __P(( output_carac **ptcarac,
                                                 timing_model  *tmodel,
                                                 char           transition,
                                                 char           lmodel,
                                                 double         r,
                                                 double         c1,
                                                 double         c2,
                                                 double         slope
                                              ));
extern void           tas_clean_carac_cone   __P(( cone_list *cone ));
extern output_carac*  tas_alloc_output_carac __P(()) ;
extern void tas_rcxsetdriver __P(( cone_list *cone ));
void tas_rcxsetnodebytransition( cone_list *cone );

