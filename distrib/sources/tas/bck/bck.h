#ifndef BCK
#define BCK

/******************************************************************************/
/* includes                                                                   */
/******************************************************************************/

#include MUT_H
#include MLO_H
#define API_HIDE_TOKENS
#include STM_H
#include TTV_H
#include MGL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/******************************************************************************/
/* defines                                                                    */
/******************************************************************************/

/* PROCESS */
#define BEST         0x1
#define WORST        0x2
#define TYPICAL      0x3

/* PTYPE */
#define PTYPE_BCK_RC  (long)0x60000001
#define PTYPE_BCK_INS (long)0x60000002

/* VALUES */ 
#define NO_VALUE     (-LONG_MAX)

/* TYPE field of bck_delay */
#define ABSOLUTE     0x1 
#define INCREMENT    0x2
#define IOPATH       0x4  /* gate */
#define INTERCONNECT 0x8  /* rc   */

/* TYPE field of bck_checklist */
#define SETUP        0x1
#define HOLD         0x2
#define RECOVERY     0x3
#define REMOVAL      0x4
#define SKEW         0x5
/* #define WIDTH        0x6 */
#define BCK_PERIOD       0x7

/* EVENT1 and EVENT2 fields of bck_trans */
#define EVNO         0x0
#define EV01         0x1 
#define EV10         0x2
#define EV0Z         0x3
#define EVZ0         0x4
#define EV1Z         0x5
#define EVZ1         0x6
#define EV0X         0x7
#define EVX0         0x8
#define EV1X         0x9
#define EVX1         0xa
#define EVZX         0xb
#define EVXZ         0xc
#define EV_Z         0xd           /* n'importe quoi vers HZ                  */
#define EV__         0xe           /* indifferent                             */
#define POSEDGE      0xf           /* etat haut                               */
#define NEGEDGE      0x10          /* etat bas                                */

/******************************************************************************/
/* globals                                                                    */
/******************************************************************************/

extern int PROCESS ;               /* WORST, TYPICAL or BEST                  */

/******************************************************************************/
/* structures                                                                 */
/******************************************************************************/
/* delays & timing checks */
/* driver dtx */

typedef struct bck_trans {         /* transition                              */               
	struct bck_trans *NEXT ;       /* transition suivante                     */
	unsigned short    EVENT1 ;     /* evenement sur START ou DATA             */
	unsigned short    EVENT2 ;     /* evenement sur END ou COMMAND            */
	long              VALUE ;      /* delai associe a la transition           */
} bck_translist ;

typedef struct bck_cond {
} bck_cond ;

typedef struct bck_delay {
	struct bck_delay *NEXT ;      /* delai suivant                            */
	long              TYPE ;      /* ABSOLUTE INCREMENT / IOPATH INTERCONNECT */
	locon_list       *START ;     /* debut du chemin                          */
	locon_list       *END ;       /* fin du chemin                            */
	bck_cond         *COND ;      /* NULL pour l'instant                      */
	struct bck_trans *TRANSLIST ; /* liste des transitions possibles entre    */
								  /* les evenements sur LOCON1 et LOCON2      */
} bck_delaylist ;

typedef struct bck_check {         /* timing check (contrainte)               */
	struct bck_check *NEXT ;       /* timing check suivant                    */
	long              TYPE ;       /* type de contrainte (setup, hold ...)    */
	locon_list       *COMMAND ;    /* commande                                */
	locon_list       *DATA ;       /* data                                    */
	bck_cond         *COND ;       /* condition, NULL pour l'instant          */
	struct bck_trans *TRANSLIST ;
} bck_checklist ;

/* timing environment */
/* driver stb */

typedef struct bck_env {
	struct bck_env   *NEXT ;
	long              TYPE ;
} bck_envlist ;

/* annotation structure */

typedef struct bck_annot {         /* structure d'annotation associee a une   */
								   /* LOINS                                   */
	bck_delaylist  *DELAYS ;       /* delais pin to pin                       */
	bck_checklist  *CHECKS ;       /* contraintes                             */
	bck_envlist    *ENVIRONMENT ;  /* environnement                           */
} bck_annot ;

/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/

extern lofig_list    *bck_sdfparse    __P((char *sdfname, lofig_list *lofig)) ;
extern void           bck_expandtrans __P((lofig_list *lofig)) ;
extern bck_translist *bck_addtrans    __P((bck_translist *head)) ;
extern void           bck_freetrans   __P((bck_translist *head)) ;
extern bck_delaylist *bck_adddelay    __P((bck_delaylist *head, long type)) ;
extern bck_checklist *bck_addcheck    __P((bck_checklist *head, long type)) ;
extern bck_annot     *bck_addannot    __P(()) ;
void                  bck_freetrans   __P((bck_translist *trans)) ;
void                  bck_freedelays  __P((bck_delaylist *delays)) ;
void                  bck_freechecks  __P((bck_checklist *checks)) ;
void                  bck_freeannot   __P((bck_annot     *annot)) ;
extern void           bck_view        __P((bck_annot     *annot)) ;
extern void           bck_viewtrans   __P((bck_translist *trans)) ;
extern void           bck_viewdelay   __P((bck_delaylist *delay)) ;
extern void           bck_viewcheck   __P((bck_checklist *check)) ;
extern ttvfig_list   *bck_bck2ttv     __P(( lofig_list*, char*)) ;

#endif
