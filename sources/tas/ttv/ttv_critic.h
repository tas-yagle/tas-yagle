/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_critic.h                                                */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#ifndef __TTV_CRITIC_H__
#define __TTV_CRITIC_H__

typedef struct infopara
    {
     struct ttvfig       *FIG ;
     struct ttvfig       *INS ;
     struct ttvevent     *ROOT ;
     struct ttvevent     *NODE ;
     struct ttvevent     *CURNODE ;
     struct ttvevent     *LATCH ;
     chain_list          *MASK ;
     union
     {
      chain_list         *CHAIN ;
      ttvpath_list       *PATHS;
     } LIST;
     chain_list          *CHAINFIG ;
     long                 MAX ;
     long                 MIN ;
     long                 LEVEL ;
     long                 TYPE ;
     int                  NBPATH ;
     int                  NBMAX ;
     int                  CLOCK ;
     ht                  *CLOCKTAB ;
     int                  SAVEALL;
    }
ttvinfopara ;

extern chain_list  *TTV_FIFO_HEAD ;
extern chain_list  *TTV_FIFO_UP ;
extern chain_list  *TTV_FIFO_B ;
extern chain_list  *TTV_FIFO_SAV ;
extern char         TTV_FIFO_NUM ;

//extern long            ttv_getinittime           __P(( ttvfig_list *, ttvevent_list *, char, long, char)) ;
extern inline int      ttv_transfertnodefindinfo __P(( ttvfig_list *,
                                                       ttvevent_list *,
                                                       ttvevent_list *,
                                                       ttvline_list *,
                                                       long,
                                                       long 
                                                    )) ;
extern inline long     ttv_connectorsearchtype __P(( ttvfig_list *,
                                                     ttvevent_list *,
                                                     long 
                                                  )) ;
extern inline long     ttv_signalsearchtype __P(( ttvfig_list *,
                                                  ttvevent_list *,
                                                  long 
                                               )) ;
extern long            ttv_getnodeslope   __P(( ttvfig_list *,
                                                ttvfig_list *,
                                                ttvevent_list *,
                                                long *, 
                                                long 
                                             )) ;
extern int             ttv_islocononlyend __P(( ttvfig_list *,
                                                ttvevent_list *,
                                                long 
                                             )) ;
extern int             ttv_islineonlyend  __P(( ttvfig_list *,
                                                ttvline_list *,
                                                long 
                                             )) ;
extern void            ttv_monolatcherror __P((ttvfig_list *,
                                                ttvsig_list *
                                             )) ;
extern void            ttv_addhtpath      __P(( ht **,
                                                ttvevent_list*,
                                                ttvevent_list*
                                              ));
extern ptype_list*     ttv_gethtpath      __P(( ht *,
                                                ttvevent_list*,
                                                ttvevent_list*
                                              ));
extern void            ttv_delhtpath      __P(( ht *)) ;
//extern ttvpath_list*   ttv_filterpathhz   __P(( ttvpath_list*,int *)) ;
extern ttvpath_list*   ttv_filterpath     __P(( ttvpath_list*,
                                                long,
                                                int *
                                             )) ;
extern int             ttv_pathcompar     __P(( ttvpath_list**,
                                                ttvpath_list**
                                             ));
extern ttvpath_list*   ttv_classpath      __P(( ttvpath_list*, long ));
extern void            ttv_fifopush       __P(( ttvevent_list* ));
extern ttvevent_list*  ttv_fifopop        __P(( void ));
extern void            ttv_fifoclean      __P(( void ));
extern void            ttv_fifodelete     __P(( void ));
extern void            ttv_fifosave       __P(( void ));
extern void            ttv_fiforestore    __P(( void ));
extern ptype_list*     ttv_getlatchaccess __P(( ttvfig_list*,
                                                ttvevent_list*,
                                                long
                                             ));
extern ptype_list*     ttv_getcommandaccess __P(( ttvfig_list*,
                                                  ttvevent_list*,
                                                  long
                                               ));
extern chain_list*     ttv_findaccess     __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long
                                              ));
extern chain_list*     ttv_findclockpath  __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                ttvevent_list *
                                              ));
extern int             ttv_iscrosslatch   __P(( ttvfig_list*,
                                                ttvevent_list*,
                                                long
                                              ));
extern chain_list*     ttv_findpath       __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long
                                              ));
extern ttvcritic_list* ttv_getcritic      __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long
                                             ));
extern ttvpath_list*   ttv_savpath        __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvpath_list *,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long
                                             ));
extern ttvcritic_list* ttv_savcritic      __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                int
                                             ));
/*extern ttvpath_list*   ttv_keepnbpath     __P(( ttvpath_list *,
                                                int,
                                                int*,
                                                long
                                             ));*/
extern ttvpath_list*   ttv_savpathlist    __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvpath_list *,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                chain_list*,
                                                long,
                                                long,
                                                int,
                                                int,
                                                int *
                                             ));
extern ttvpath_list*   ttv_savallpath     __P(( ttvpath_list *,
                                                int,
                                                int*,
                                                long,
                                                ttvpath_list *,
                                                int
                                             ));
extern ttvpath_list*   ttv_savcriticpath  __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvpath_list *,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                int,
                                                int*,
                                                long,
                                                long,
                                                chain_list*,
                                                int
                                             ));
extern ttvpath_list*   ttv_findsigpath    __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                long,
                                                long,
                                                ttvpath_list *,
                                                char,
                                                int,
                                                int*,
                                                int,
                                                char*
                                             ));
extern ttvpath_list*   ttv_getcriticpath  __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                chain_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                int,
                                                long
                                             ));
extern ttvpath_list*   ttv_getcriticaccess __P(( ttvfig_list*,
                                                 ttvfig_list*,
                                                 chain_list*,
                                                 chain_list*,
                                                 chain_list*,
                                                 long,
                                                 long,
                                                 int,
                                                 long
                                              ));
extern ttvpath_list*   ttv_getpath        __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern ttvpath_list*   ttv_getpathnocross __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern ttvpath_list*   ttv_getaccessnocross __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern ttvpath_list*   ttv_getpathsig     __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern ttvpath_list*   ttv_getaccess      __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern ttvpath_list*   ttv_getsigaccess   __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvsig_list*,
                                                chain_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern ttvpath_list*   ttv_getallpath     __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                chain_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                int,
                                                long
                                             ));
extern ttvpath_list*   ttv_getallaccess   __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                chain_list*,
                                                chain_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                int,
                                                long
                                             ));
extern int             ttv_testparamask   __P(( ttvinfopara* ));
extern void*           ttv_findpara       __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                long,
                                                long,
                                                chain_list*,
                                                int,
                                                int
                                             ));
extern ttvpath_list *  ttv_reduceparapath __P(( ttvinfopara*, int ));
extern void            ttv_savparapath    __P(( ttvevent_list*, ttvinfopara* ));
extern void            ttv_findpclock     __P(( ttvevent_list*, ttvinfopara* ));
extern void            ttv_findp          __P(( ttvevent_list*, ttvinfopara* ));
extern chain_list*     ttv_getcriticpara  __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                chain_list*,
                                                long
                                             ));
extern int             ttv_freecriticpara __P(( chain_list* ));
extern ttvpath_list*   ttv_getpara        __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long,
                                                long,
                                                long,
                                                chain_list*,
                                                int
                                             ));
extern chain_list*     ttv_getdelay       __P(( ttvfig_list*,
                                                ttvfig_list*,
                                                chain_list*,
                                                chain_list*,
                                                long,
                                                long,
                                                long
                                             ));
extern int             ttv_freedelaylist  __P(( chain_list* ));
extern long            ttv_getnodeslew    __P(( ttvevent_list*,
                                                long
                                             ));
extern void            ttv_allocpathmodel __P(( ttvfig_list*,
                                                ttvpath_list*,
                                                long
                                             ));
extern void            ttv_allocnewmodel  __P(( ttvfig_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                ttvline_list*,
                                                ttvline_list*,
                                                ttvline_list*,
                                                ttvline_list*,
                                                long
                                             ));
extern void            ttv_addpath        __P(( ttvfig_list*,
                                                chain_list*,
                                                ttvevent_list*,
                                                ttvevent_list*,
                                                long
                                             ));
extern void            ttv_builtpath      __P(( ttvfig_list*, long ));

#endif //__TTV_CRITIC_H__
