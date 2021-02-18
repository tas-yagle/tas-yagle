/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Verison 5                                               */
/*    Fichier : tas_builthfig.h                                             */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* declaration de variable */

/* declaration de fonction */
extern float          tas_getcapa            __P(( lofig_list*, losig_list* ));
extern void           tas_routelosig         __P(( lofig_list*, 
                                                   losig_list*,
                                                   float, 
                                                   float 
                                                ));
extern void           tas_addcapalosig       __P(( lofig_list*, 
                                                   losig_list*,
                                                   float, 
                                                   float 
                                                ));
extern int            tas_deleteflatemptyfig __P(( ttvfig_list*, long, char ));
extern void           tas_detectloop         __P(( ttvfig_list*, long ));
extern void           tas_calcfigdelay       __P(( ttvfig_list*, 
                                                   lofig_list* ,
                                                   long,
                                                   long
                                                ));
extern void           tas_lofigchain         __P(( lofig_list* ));
extern long           tas_gettypesig         __P(( losig_list* ));
extern void           tas_flattenhfig        __P(( lofig_list* ));
extern ttvfig_list*   tas_builtfig           __P(( lofig_list* ));
extern void           tas_cleanfig           __P(( lofig_list*, ttvfig_list* ));
extern void           tas_loconorient        __P(( lofig_list*, ttvfig_list* ));
extern void           tas_detecloconsig      __P(( lofig_list*, ttvfig_list* ));
extern void           tas_builtrcxview       __P(( lofig_list*, ttvfig_list* ));
extern long           tas_calcullinetime     __P(( ttvline_list*, long ));
extern long           tas_calcullineslope    __P(( ttvline_list*, long ));
extern long           tas_gettypeline        __P(( ttvline_list* ));
extern ttvevent_list* tas_getnodesig         __P(( ttvfig_list*,
                                                   ttvevent_list* 
                                                ));
extern ttvevent_list* tas_getnodesighz       __P(( ttvfig_list*,
                                                   ttvevent_list* 
                                                ));
extern void           tas_calcaracline       __P(( ttvline_list*,
                                                   ttvline_list*,
                                                   int,
                                                   long
                                                ));
extern void           tas_freercxfig         __P(( ttvfig_list* ));
extern void           tas_loadrcxfig         __P(( ttvfig_list* ));
extern void           tas_addsigrcxload      __P(( ttvsig_list* ));
extern void           tas_builtline          __P(( ttvfig_list*, long ));
extern int            tas_calcrcnline        __P(( ttvfig_list*,
                                                   ttvevent_list*,
                                                   ttvevent_list*,
                                                   long
                                                ));
extern int            tas_findrcnline        __P(( ttvfig_list*,
                                                   losig_list*,
                                                   ttvevent_list*,
                                                   ttvevent_list*,
                                                   ttvevent_list*,
                                                   long
                                                ));
extern void           tas_calcrcxdelay       __P(( lofig_list*,
                                                   ttvfig_list*,
                                                   long
                                                ));
extern ttvfig_list*   tas_builtttvfig        __P(( lofig_list* ));
