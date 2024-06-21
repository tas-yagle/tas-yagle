/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv_fig.h                                                   */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#ifndef __TTV_FIG_H__
#define __TTV_FIG_H__

#define TTV_NODE_LOOP      ((long) 0x01000000 ) /* noeud dans une loop       */
#define TTV_NODE_USED      ((long) 0x02000000 ) /* noeud deja parcouru       */
#define TTV_NODE_USEDDUAL  ((long) 0x04000000 ) /* noeud deja parcouru       */
#define TTV_NODE_LINEEXT   ((long) 0x08000000 ) /* noeud deja parcouru       */
#define TTV_NODE_LOOPNODE  ((long) 0x00100000 ) /* noeud deja parcouru       */
#define TTV_LINE_LOOP      ((long) 0x01000000 ) /* lien sur une boucle       */
#define TTV_LINE_EXT       ((long) 0x02000000 ) /* lien sur un chemin ext    */
#define TTV_LINE_USED      ((long) 0x04000000 ) /* lien sur une boucle       */
#define TTV_LINE_USEDDUAL  ((long) 0x08000000 ) /* lien sur un chemin ext    */

#define TTV_PROP_MIN       ((int)  0x00000001 )
#define TTV_PROP_MAX       ((int)  0x00000002 )

extern ttvsbloc_list *TTV_HEAD_REFSIG ;
extern long TTV_NUMB_REFSIG ;
extern chain_list *TTV_FREE_REFSIG ;
extern ht *TTV_HT_MODEL ;
extern int TTV_MAX_PATH_PERIOD ;

extern void               ttv_addhtmodel      __P(( ttvfig_list*));
extern void               ttv_delhtmodel      __P(( ttvfig_list*));
extern ttvfig_list*       ttv_gethtmodel      __P(( char *));
extern ttvsbloc_list **   ttv_addsigtab       __P(( ttvfig_list*));
extern void               ttv_delsigtab       __P(( ttvfig_list*));
extern ttvsig_list*       ttv_addsig          __P(( ttvfig_list*,
                                                    char*,
                                                    char*,
                                                    float,
                                                    long
                                                 ));
extern chain_list*        ttv_addrefsig       __P(( ttvfig_list*,
                                                    char*,
                                                    char*,
                                                    float,
                                                    long,
                                                    chain_list*
                                                 ));
extern long               ttv_gettypesig       __P(( ttvsig_list* ));
extern void               ttv_delrefsig       __P(( ttvsig_list* ));
extern ttvline_list*      ttv_addline         __P(( ttvfig_list*,
                                                    ttvevent_list*,
                                                    ttvevent_list*,
                                                    long,
                                                    long,
                                                    long,
                                                    long,
                                                    long
                                                 ));
extern void               ttv_addcaracline    __P(( ttvline_list*,
                                                     char *,
                                                     char *,
                                                     char *,
                                                     char *
                                                 ));
extern int                ttv_calcaracline    __P(( ttvline_list*,
                                                    ttvline_list*,
                                                    long,
                                                    int
                                                 ));
extern void               ttv_addconttype     __P(( ttvline_list*,
                                                     long
                                                 ));
extern long               ttv_getconttype     __P(( ttvline_list* ));
extern void               ttv_delconttype     __P(( ttvline_list* ));
extern void               ttv_addmodelline    __P(( ttvline_list*,
                                                     char *,
                                                     char *
                                                 ));
extern void               ttv_getmodelline    __P(( ttvline_list*,
                                                     char **,
                                                     char **
                                                  ));
extern void               ttv_delmodelline    __P(( ttvline_list* ));
extern int                ttv_delline         __P(( ttvline_list* ));
extern long               ttv_getlinetype     __P(( ttvline_list* )); 
extern long               ttv_getnewlinetype  __P(( ttvline_list*, 
                                                    ttvsig_list*,
                                                    ttvsig_list*
                                                 ));
extern void               ttv_delcmd          __P(( ttvfig_list*,
                                                    ttvsig_list*
                                                 ));
extern ttvevent_list*     ttv_getcmd          __P(( ttvfig_list*,
                                                    ttvevent_list*
                                                 ));
extern ttvevent_list*     ttv_getlinecmd      __P(( ttvfig_list*,
                                                    ttvline_list*,
                                                    long
                                                 ));
extern ptype_list*        ttv_addcmd          __P(( ttvline_list*,
                                                    long,
                                                    ttvevent_list*
                                                 ));
extern chain_list*        ttv_getlrcmd        __P(( ttvfig_list*,
                                                    ttvsig_list*
                                                 ));
extern void               ttv_addprevline     __P(( ttvline_list* ));
extern void               ttv_delprevline     __P(( ttvline_list* ));
extern int                ttv_islinelevel     __P(( ttvfig_list*,
                                                    ttvline_list*,
                                                    long
                                                 ));
extern ttvfig_list*       ttv_getttvfig       __P(( char*,
                                                     long
                                                  ));
extern ttvfig_list*       ttv_getttvins       __P(( ttvfig_list*,
                                                    char*,
                                                    char*
                                                 ));
extern ttvfig_list*       ttv_givehead        __P(( char*,
                                                    char*,
                                                    ttvfig_list*
                                                 ));
extern ttvfig_list*       ttv_givettvfig      __P(( char*,
                                                    char*,
                                                    ttvfig_list*
                                                 ));
extern chain_list*        ttv_getttvfiglist   __P(( ttvfig_list* ));
extern char*              ttv_getinsname      __P(( ttvfig_list*,
                                                    char*,
                                                    ttvfig_list*
                                                 ));
extern char*              ttv_getsigname      __P(( ttvfig_list*,
                                                    char*,
                                                    ttvsig_list*
                                                 ));
extern char*              ttv_getnetname      __P(( ttvfig_list*,
                                                    char*,
                                                    ttvsig_list*
                                                 ));
extern ttvsig_list*       ttv_getsig          __P(( ttvfig_list*,
                                                    char*
                                                 ));
extern ttvsig_list*       ttv_getsigbyhash    __P(( ttvfig_list*,
                                                    char*
                                                 ));
extern ttvsig_list*       ttv_getsigbyname    __P(( ttvfig_list*,
                                                    char*,
                                                    long
                                                 ));
extern ttvsig_list*       ttv_getsigbyinsname __P(( ttvfig_list*,
                                                    char*,
                                                    long
                                                 ));
extern ttvsig_list*       ttv_getsigbyindex   __P(( ttvfig_list*,
                                                    long,
                                                    long
                                                 ));
extern chain_list*        ttv_getsigbytype    __P(( ttvfig_list*,
                                                    ttvfig_list*,
                                                    long,
                                                    chain_list*
                                                 ));
extern chain_list*        ttv_getsignamelist  __P(( ttvfig_list*,
                                                    ttvfig_list*,
                                                    chain_list*
                                                 ));
extern void               ttv_freenamelist    __P(( chain_list* ));
extern chain_list*        ttv_getallsigbytype __P(( ttvfig_list*,
                                                    long,
                                                    chain_list*
                                                 ));
extern void               ttv_setsiglevel     __P(( ttvsig_list*, long ));
extern long               ttv_getsiglevel     __P(( ttvsig_list* ));
extern void               ttv_delsiglevel     __P(( ttvfig_list*,
                                                    ttvsig_list*
                                                 ));
extern void               ttv_expfigsig       __P(( ttvfig_list*,
                                                    ttvsig_list*,
                                                    long,
                                                    long,
                                                    long,
                                                    long
                                                 ));
extern FILE *              ttv_openfile       __P((ttvfig_list *,
                                                   long,
                                                   char *
                                                 ));
extern long                ttv_existefile     __P((ttvfig_list *,
                                                   long
                                                 ));
extern chain_list*         ttv_addlooplist    __P(( ttvfig_list*, 
                                                    chain_list*
                                                 ));
extern chain_list*         ttv_addloop        __P(( ttvfig_list*,
                                                    ttvevent_list*
                                                 ));
extern void                ttv_marknode       __P(( ttvfig_list*,
                                                    ttvevent_list*,
                                                    long,
                                                    long
                                                 ));
extern chain_list*         ttv_depthmark      __P(( ttvfig_list*,
                                                    ttvevent_list*,
                                                    long,
                                                    long
                                                 ));
extern chain_list*         ttv_detectloop     __P(( ttvfig_list*,
                                                    long
                                                 ));
extern ttvfig_list*        ttv_builtrefsig    __P(( ttvfig_list* ));
extern void                ttv_printloop      __P(( int,
                                                    chain_list*,
                                                    char*
                                                 ));
extern int                 ttv_existeline     __P(( ttvfig_list*,
                                                    ttvevent_list*,
                                                    ttvevent_list*,
                                                    long
                                                 ));
extern ttvline_list       *ttv_getline        __P(( ttvfig_list*,
                                                    ttvfig_list*,
                                                    ttvevent_list*,
                                                    ttvevent_list*,
                                                    ttvevent_list*,
                                                    long,
                                                    long,
                                                    int,
                                                    int
                                                 ));
extern lofig_list         *ttv_getrcxlofig    __P(( ttvfig_list*));
extern void                ttv_delrcxlofig    __P(( ttvfig_list*));
extern int                 ttv_getdelaypos    __P(( ttvfig_list*,long,int*));
extern ttvdelay_list      *ttv_getlinedelay   __P(( ttvline_list*));
extern ttvdelay_list      *ttv_addlinedelay   __P(( ttvline_list*,
                                                    long,
                                                    long,
                                                    long,
                                                    long,
                                                    float,
                                                    float
                                                 ));
extern void                ttv_delnodedelay   __P(( ttvevent_list*));
extern ttvdelay_list      *ttv_getnodedelay   __P(( ttvevent_list*));
extern ttvdelay_list      *ttv_addnodedelay   __P(( ttvevent_list*,
                                                    long,
                                                    long,
                                                    long,
                                                    long
                                                 ));
extern void                ttv_dellinedelay   __P(( ttvline_list*));
extern long                ttv_getdelaymax    __P(( ttvline_list*));
extern long                ttv_getdelaymin    __P(( ttvline_list*));
extern long                ttv_getslopemax    __P(( ttvline_list*));
extern long                ttv_getslopemin    __P(( ttvline_list*));
extern float               ttv_getcapamax     __P(( ttvline_list*));
extern float               ttv_getcapamin     __P(( ttvline_list*));
extern float               ttv_getslopeforload __P(( ttvline_list*, long ));
extern long                ttv_calclinedelayslope   __P(( ttvline_list*,
                                                          long,
                                                          long,
                                                          stm_pwl*,
                                                          stm_pwl*,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          float,
                                                          long*,
                                                          long*,
                                                          stm_pwl**,
                                                          long*,
                                                          long*,
                                                          stm_pwl**,
                                                          char,
                                                          float,
                                                          float,
                                                          double*,
                                                          double*,
                                                          double*,
                                                          float,
                                                          float,
                                                          double*,
                                                          double*,
                                                          double*,
                                                          float*,
                                                          float*
                                                       ));
extern void                ttv_calcgatercdelayslope __P(( ttvline_list*,
                                                          ttvline_list*,
                                                          long,
                                                          long,
                                                          float,
                                                          long*,
                                                          long*,
                                                          long*,
                                                          long*,
                                                          char
                                                       ));
extern long                ttv_calcnodedelayslope   __P(( ttvfig_list*,
                                                          long,
                                                          ttvevent_list *,
                                                          long,
                                                          char 
                                                       ));
extern long                ttv_getslopenode         __P(( ttvfig_list*,
                                                          long,
                                                          ttvevent_list *,
                                                          long,
                                                          long
                                                       ));
extern long                ttv_getdelaynode         __P(( ttvfig_list*,
                                                          long,
                                                          ttvevent_list *,
                                                          long
                                                       ));
extern chain_list         *ttv_levelise             __P(( ttvfig_list *,
                                                          long,
                                                          long 
                                                        ));
extern void                ttv_movedelayline        __P(( ttvfig_list *,
                                                          long
                                                        ));
extern void                ttv_cleanfigmodel        __P(( ttvfig_list *,
                                                          long,
                                                          long
                                                        ));
extern void                ttv_freefigdelay         __P(( chain_list * ));
extern void                ttv_readcell             __P(( char * ));
extern void                ttvenv                   __P(( ));
extern int                 ttv_isemptyttvins        __P(( ttvfig_list *,
                                                          long 
                                                       ));
extern chain_list         *ttv_copyttvinsinttvfig   __P(( ttvfig_list *,
                                                          ttvfig_list *,
                                                          long 
                                                        ));
extern chain_list         *ttv_flatttvfigrec        __P(( ttvfig_list *,
                                                          chain_list *,
                                                          long 
                                                        ));
extern void                ttv_flatttvfigfromlist   __P(( ttvfig_list *,
                                                          chain_list *,
                                                          long 
                                                        ));
extern void                ttv_flatttvfig           __P(( ttvfig_list *,
                                                          chain_list *,
                                                          long 
                                                        ));
extern losig_list*         ttv_getlosigfromevent    __P(( ttvfig_list *,
                                                          ttvsig_list *,
                                                          char **,
                                                          chain_list **,
                                                          lofig_list **
                                                       ));
extern ttvfig_list*        ttv_getttvinsbyhiername  __P(( ttvfig_list *,
                                                          char *
                                                       ));
extern int                 ttv_isttvsigdriver       __P(( ttvfig_list *, 
                                                          long,
                                                          long,
                                                          ttvsig_list*   
                                                       ));
extern void                ttv_addttvsigfast        __P(( ttvfig_list *,
                                                          char *,
                                                          losig_list *,
                                                          ttvsig_list *
                                                       ));
extern ttvsig_list*        ttv_getttvsigfast        __P(( ttvfig_list *,
                                                          char *,
                                                          losig_list *
                                                       ));
extern ttvsig_list*        ttv_getttvsig            __P(( ttvfig_list *,
                                                          long,
                                                          long,
                                                          char *,
                                                          losig_list *,
                                                          char
                                                       ));
extern void                ttv_addsigcapas          __P(( ttvsig_list *ttvsig,
                                                          float cu,
                                                          float cumin,
                                                          float cumax,
                                                          float cd,
                                                          float cdmin,
                                                          float cdmax
                                                       ));
extern ttvsig_capas*       ttv_getsigcapas          __P(( ttvsig_list *ttvsig,
                                                          float *cu,
                                                          float *cumin,
                                                          float *cumax,
                                                          float *cd,
                                                          float *cdmin,
                                                          float *cdmax
                                                       ));
extern void                ttv_delsigcapas          __P(( ttvsig_list *ttvsig
                                                       ));
extern void                ttv_setallsigcapas       __P(( lofig_list *lofig,
                                                          losig_list *losig,
                                                          ttvsig_list *ttvsig
                                                       ));
int ttv_addsig_addrcxpnode(int mode);

#endif // __TTV_FIG_H__
