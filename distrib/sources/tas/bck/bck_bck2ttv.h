/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SDF Version 1.00                                            */
/*    Fichier : bck_bck2ttv.h                                               */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim Dioury                                              */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

/* inclusion mbk */
#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include INF_H
#include STM_H
#include TTV_H
#include BCK_H

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

#define BCK_ALIM 'A'
#define BCK_LOCON_SIG    (long)0x70000001
#define BCK_LOCON_BREAK  (long)0x70000002
#define BCK_LOCON_CLOCK  (long)0x70000003
#define BCK_LOCON_EXT    (long)0x70000004
#define BCK_LOCON_IN     (long)0x70000006
#define BCK_LOCON_MARQUE (long)0x70000006
#define BCK_LOSIG_SIG    (long)0x70000007
#define BCK_LOSIG_BREAK  (long)0x70000008
#define BCK_LOSIG_CLOCK  (long)0x70000009
#define BCK_LOSIG_EXT    (long)0x7000000a

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern void bck_addlocontype            __P(( locon_list*,
                                              long
                                           )) ;
extern void bck_addlosigtype            __P(( losig_list*,
                                              long
                                           )) ;
extern void bck_setloconin              __P(( locon_list*)) ;
extern void bck_setloconout             __P(( locon_list*)) ;
extern void bck_detectalim              __P(( lofig_list*)) ;
extern void bck_delptype                __P(( lofig_list*)) ;
extern void bck_detectbreakinf          __P(( lofig_list*)) ;
extern void bck_detectbreak             __P(( lofig_list*)) ;
extern void bck_detectaccess            __P(( lofig_list*)) ;
extern void bck_detectext               __P(( lofig_list*)) ;
extern long bck_getlocontype            __P(( locon_list*)) ;
extern ttvsig_list *bck_getextlocon     __P(( locon_list*)) ;
extern void bck_addttvsig               __P(( lofig_list*,
                                              ttvfig_list*
                                           )) ;
extern ttvsig_list *bck_getsiglocon     __P(( locon_list*)) ;
extern ttvevent_list *bck_geteventlocon __P(( locon_list*,
                                              short
                                           )) ;
extern long bck_getlinetype             __P(( locon_list*,
                                              short,
                                              locon_list*,
                                              short,
                                              long,
                                              char
                                           )) ;
extern void bck_detectunconnect         __P(( losig_list*)) ;
extern int bck_ifexistetrans            __P(( loins_list *,
                                              ttvevent_list *,
                                              char *,
                                              ttvevent_list *,
                                              char *
                                           )) ;
extern void bck_addttvline              __P(( lofig_list*,
                                              ttvfig_list*
                                           )) ;
extern ttvfig_list *bck_bck2ttv         __P(( lofig_list*,
                                              char*
                                           )) ;
