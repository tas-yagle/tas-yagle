/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_alloc.h                                                 */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#ifndef __TTV_ALLOC_H__
#define __TTV_ALLOC_H__

extern ttvfig_list *TTV_LIST_TTVFIG ;
extern chain_list *TTV_HEAD_TTVFIG ;
extern ttvsbloc_list *TTV_FREE_SBLOC ;
extern ttvlbloc_list *TTV_FREE_LBLOC ;
extern ttvcritic_list *TTV_FREE_CRITIC ;
extern ttvfind_list *TTV_FREE_FIND ;
extern ttvpath_list *TTV_FREE_PATH ;
extern long TTV_NUMB_SIG ;
extern long TTV_MAX_SIG ;
extern long TTV_NUMB_LINE ;
extern long TTV_MAX_LINE ;

extern ttvfig_list*       ttv_allocttvfig      __P(( char*,
                                                     char*,
                                                     ttvfig_list*
                                                  ));
extern void               ttv_allocinfottvfig  __P(( ttvfig_list* ));
extern void               ttv_lockttvfig       __P(( ttvfig_list* ));
extern void               ttv_unlockttvfig     __P(( ttvfig_list* ));
extern int                ttv_freettvfig       __P(( ttvfig_list* ));
extern int                ttv_freettvfiglist   __P(( chain_list* ));
extern int                ttv_freettvfigtree   __P(( ttvfig_list* ));
extern void               ttv_freeall          __P(( ));
extern int                ttv_freeallttvfig    __P(( ttvfig_list* ));
extern ttvsbloc_list*     ttv_allocsbloc       __P(( ttvfig_list*,
                                                     ttvsbloc_list*
                                                  ));
int ttv_freesbloclist(ttvsbloc_list *pthead);
extern int                ttv_getsbloclist     __P(( ttvfig_list* ));
extern long               ttv_cleansbloclist   __P(( ttvsbloc_list*, long ));
extern ttvlbloc_list*     ttv_alloclbloc       __P(( ttvfig_list*,
                                                     ttvlbloc_list*,
                                                     long
                                                  ));
extern int                ttv_freelbloclist    __P(( ttvlbloc_list* ));
extern int                ttv_getlbloclist     __P(( ttvfig_list*, long ));
extern ttvpath_list*      ttv_allocpath        __P(( ttvpath_list*,
                                                     ttvfig_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ttvevent_list*,
                                                     ptype_list *,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     long,
                                                     timing_model*,
                                                     timing_model*,
                                                     long,
                                                     char,
                                                     long
                                                  ));
extern int                ttv_freepathlist     __P(( ttvpath_list* ));

ttvcritic_list *ttv_alloccritic(ttvcritic_list *headcritic,ttvfig_list *ttvfigh,ttvfig_list *ttvfig,ttvevent_list *node,long type,long data,long delay,long slope,long newdelay,long newslope, char nodeflags, char *linemodelname, ttvline_list *line);


extern int                ttv_freecriticlist   __P(( ttvcritic_list* ));
extern ttvfind_list*      ttv_allocfind        __P(( ttvevent_list* ));
extern int                ttv_freefindlist     __P(( ttvfind_list* ));
extern ttvsig_list**      ttv_allocreflist     __P(( chain_list*, long ));
extern int                ttv_freereflist      __P(( ttvfig_list*,
                                                     ttvsig_list**,
                                                     long 
                                                  ));
extern chain_list*        ttv_chainreflist     __P(( ttvfig_list*,
                                                     ttvsig_list**,
                                                     long 
                                                  ));
extern void               ttv_tagttvfigfree    __P(( ttvfig_list*,
                                                     ttvfig_list*,
                                                     ttvfig_list*,
                                                     long
                                                  ));
extern void               ttv_cleantagttvfig   __P(( long ));
extern void               ttv_addinfreelist    __P(( ttvfig_list*, long ));
extern int                ttv_delinfreelist    __P(( ptype_list *, ttvfig_list* ));
extern int                ttv_freememoryifmax  __P(( ttvfig_list*, long ));
extern int                ttv_freememoryiffull __P(( ttvfig_list*, long ));
extern int                ttv_freettvfigmemory __P(( ttvfig_list*, long ));
extern void               ttv_allocdualline    __P(( ttvfig_list*, long ));
extern void               ttv_freedualline     __P(( ttvfig_list*, long ));

extern ttvdelay_list     *ttv_alloclinedelay   __P(( ttvline_list *));
extern ttvdelay_list     *ttv_allocdelayline   __P(( ttvfig_list*,int,int,int));
extern void               ttv_freettvfigdelay  __P(( ttvfig_list *));
extern void               ttv_alloclineindex   __P(( ttvfig_list*, long ));
extern ttvsig_capas      *ttv_allocsigcapas    __P(( float cu, float cumin, float cumax,
                                                     float cd, float cdmin, float cdmax));

void ttv_freenodeuserdata(ptype_list *ptype);

ttvfind_stb_stuff *ttv_allocfindstb();
void ttv_freefindstblist(ttvfind_stb_stuff *head);
void ttv_initfindstb(ttvfind_stb_stuff *sfs);

ttvpath_stb_stuff *ttv_allocpath_stb_stuff();
void ttv_freepathstblist(ttvpath_stb_stuff *head);

#endif // __TTV_ALLOC_H__
