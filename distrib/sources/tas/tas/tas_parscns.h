/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_parscns.h                                               */
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


extern char*          tas_getconename        __P(( cone_list* ));
extern char*          tas_getinconename      __P(( edge_list*,
                                                   chain_list*
                                                ));
extern char*          tas_getsigname         __P(( cone_list*,
                                                   locon_list*,
                                                   edge_list*
                                                ));  
extern ttvevent_list* tas_getcmd             __P(( cone_list*,
                                                   cone_list*,
                                                   link_list*
                                                ));
extern void           tas_detectsig          __P(( cnsfig_list* ));
extern void           tas_printloop          __P(( int, 
                                                   chain_list*, 
                                                   ttvfig_list* 
                                                ));  
extern void           tas_loconpathmarque    __P(( cone_list*, 
                                                   long 
                                                ));
extern chain_list*    tas_getlatchcmdlist    __P(( cone_list* ));
extern chain_list*    tas_getlatchcmd        __P(( cone_list*, 
                                                   edge_list*, 
                                                   long 
                                                ));
extern void           tas_linklocontcone     __P(( ttvfig_list*,
                                                   locon_list*,
                                                   edge_list *
                                                ));
extern void           tas_linkconelocon      __P(( ttvfig_list*,
                                                   locon_list*
                                                ));
extern void           tas_linkconenode       __P(( ttvfig_list*,
                                                ttvsig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                cone_list*,
                                                edge_list*,
                                                long
                                             ));
extern ttvfig_list*   tas_cns2ttv            __P(( cnsfig_list* ,
                                                   ttvfig_list*,
                                                   lofig_list * 
                                                ));
