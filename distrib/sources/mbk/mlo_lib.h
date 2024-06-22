/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* 
 * Purpose : constantes, externs, and data-structures
 * Date    : 05/08/93
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 * $Id: mlo_lib.h,v 1.49 2008/07/20 11:44:58 fabrice Exp $
 */

#ifndef _MLO_H_
#define _MLO_H_

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif
/* transistor type */
#define TRANSN      4
#define TRANSP      8

#define MLO_IS_TRANSN(x) ((x&TRANSN)==TRANSN)
#define MLO_IS_TRANSP(x) ((x&TRANSP)==TRANSP)

/* connector & signal direction */
#define INTERNAL    'I' /* instance connector or signal on instances only */
#define EXTERNAL    'E' /* figure connector or signal on figure and instances */

/* connector direction */
#define IN          'I'
#define OUT         'O'
#define INOUT       'B'
#define UNKNOWN     'X'
#define TRISTATE    'Z'
#define TRANSCV     'T'

/* USER field of lofig */
#define LOFIG_LOCK 0x4D424B00
#define LOFIG_QUICK_SIG_HT 0x4D424B01
#define LOFIG_QUICK_INS_HT 0x4D424B02
#define LOFIG_QUICK_TRS_HT 0x4D424B04
#define LOFIG_INFO         0x4D424B03
#define PARAM_CONTEXT 0x18062003
#define MBK_RC_ARE_INTERNAL 0x20040602

/* USER field of losig */
#define LOSIGALIM  0x13082003
#define LOSIGALIM_EXPR  0x13082004

#define MBK_SIGNAL_ALIM_DONE_FLAG 0x01
#define MBK_ALIM_VDD_FLAG         0x02
#define MBK_ALIM_VSS_FLAG         0x04
#define MBK_HAS_GLOBAL_NODE_FLAG  0x08

/* USER field of lotrs or loins lowire or loctc */
#define OPT_PARAMS 0x11062003
#define TRANS_FIGURE 0x24052004
#define PARAM_CONTEXT 0x18062003
#define MIN_PARAM_CONTEXT 0x25052004
#define MCC_COMPUTED_KEY 0x25052005

/* USER field of connector*/
#define LOCON_INFORMATION 0x06031501
// flags
#define   LOCON_INFORMATION_INPUT 1
#define   LOCON_INFORMATION_OUTPUT 2

/*******************************************************************************
* netlist structures types                                                     *
*******************************************************************************/
typedef struct
{
  float scale;
} lofiginfo;

typedef struct NEWBKSIG
{
  int maxindex;
  void *TAB;
} NewBKSIG;

typedef struct lofig                          /* logical figure               */
{
struct lofig  *NEXT;                          /* next figure                  */
struct chain  *MODELCHAIN;                    /* list of models               */
struct locon  *LOCON;                         /* connector list head          */
struct losig  *LOSIG;                         /* signal list head             */
NewBKSIG      BKSIG;                         /* signal block list head       */
struct loins  *LOINS;                         /* instance list head           */
struct lotrs  *LOTRS;                         /* transistor list head         */
char          *NAME;                          /* figure name (unique)         */
char           MODE;                          /* 'A' or 'P'                   */
char           FLAGS0;                        /* user flags                   */
short          FLAGS1;                        /* user flags                   */
ptype_list  *USER;                          /* Application specific         */
}
lofig_list;

typedef struct lotrs                           /* logical transistor          */
{
struct lotrs  *NEXT;                           /* next transistor             */
struct locon  *DRAIN;                          /* drain connector             */
struct locon  *GRID;                           /* grid connector              */
struct locon  *SOURCE;                         /* source connector            */
struct locon  *BULK;                           /* bulk connector              */
char          *TRNAME;                         /* transistor name             */
long           WIDTH,LENGTH;                   /* transistor width & length   */
long           PS, PD;                         /* source & drain perimeters   */
long           XS, XD;                         /* values for area computation */
long           X,Y;                            /* transistor coordinates      */
short          MODINDEX;                       /* index of model              */
char           TYPE;                           /* transistor type             */
char           FLAGS;                          /* user flags                  */
ptype_list  *USER;                           /* application specific        */
}
lotrs_list;

typedef struct optparam
{
  struct optparam *NEXT;
  union
  {
    char **STANDARD;
    char  *SPECIAL;
    void  *OBJ;
  }               UNAME;
  union 
  {
    float VALUE;
    char *EXPR;
  }               UDATA;
  char   TAG;
  void *EQT_EXPR;
}
optparam_list;

typedef struct loins                           /* logical instance            */
{
struct loins  *NEXT;                           /* next figure                 */
struct locon  *LOCON;                          /* connector list head         */
char          *INSNAME;                        /* instance name               */
char          *FIGNAME;                        /* model name                  */
ptype_list  *USER;                           /* Application specific        */
}
loins_list;

typedef struct locon                           /* logical connector           */
{
struct locon  *NEXT;                           /* next connector              */
char          *NAME;                           /* connector name (unique)     */
struct losig  *SIG;                            /* pointer on signal           */
void          *ROOT;                           /* pointer on figure/instance  */
char          DIRECTION;                       /* see defines for information */
char          TYPE;                            /* instance I  / figure E      */
short         FLAGS;                           /* user flags                  */
struct num   *PNODE;                           /* nodes for physical connector*/
ptype_list  *USER;                           /* Application specific        */
}
locon_list;

typedef struct losig                           /* logical signal              */
{
struct losig  *NEXT;                           /* next signal                 */
struct chain  *NAMECHAIN;                      /* list of alias name          */
long          INDEX;                           /* signal index (unique)       */
char          TYPE;                            /* internal I  / external E    */
char          RCNCACHE;                        /* user flags                  */
char          ALIMFLAGS;                       /* user flags                  */
char          FLAGS;                           /* user flags                  */
struct lorcnet *PRCN;                          /* pointer on parasitic RC     */
ptype_list  *USER;                           /* application specific        */
}
losig_list;

/*******************************************************************************
* externals for mbk netlist view                                               *
*******************************************************************************/
extern lofig_list *HEAD_LOFIG;                 /* logical figure list head   */
extern ht         *HT_LOFIG;                   /* logical figure hash table  */
extern ht         *HT_MODINDEXNAME;            /* transistor model hash table */
extern lofig_list * (*HOOK_GETLOADEDLOFIG)(char *name);

  extern    lofig_list * addlofig __P((char *name));
  extern          void   addlofigparam __P((lofig_list *ptlofig, char *param, float value, char *expr));
  extern         float   getlofigparam __P((lofig_list *ptlofig, void *param, char **expr, int *status));
  extern          void   freelofigparams __P((lofig_list *ptlofig));
  extern          void   delalllofig __P(());
  extern    lofig_list * addlomodel __P((lofig_list *model, char *name));

  extern    loins_list * addloins __P((lofig_list *ptfig, char *insname, lofig_list *ptnewfig, chain_list *sigchain));
  extern          void   addloinsparam __P((loins_list *ptloins, char *param, float value, char *expr));
  extern         float   getloinsparam __P((loins_list *ptloins, void *param, char **expr, int *status));
  extern          void   freeloinsparams __P((loins_list *ptloins));
  extern    lotrs_list * addlotrs __P((lofig_list *ptfig, char type, long x, long y, long width, long length, long ps, long pd, long xs, long xd, losig_list *ptgrid, losig_list *ptsource, losig_list *ptdrain, losig_list *ptbulk, char *name));
  extern          short  addlotrsmodel __P((lotrs_list *lotrs, char *model));
  extern          char * getlotrsmodel __P((lotrs_list *lotrs));
  extern          void   addlotrsparam __P((lotrs_list *ptlotrs, char *param, float value, char *expr));
  extern optparam_list * makelotrsparam __P((optparam_list *ptoptparam, char *param, float value, char *expr));
  extern         float   getlotrsparam __P((lotrs_list *ptlotrs, void *param, char **expr, int *status));
  extern          void   freelotrsparams __P((lotrs_list *ptlotrs));
  extern    locon_list * addlocon __P((lofig_list *ptfig, char *name, losig_list *ptsig, char dir));
  extern    losig_list * addlosig __P((lofig_list *ptfig, long index, chain_list *namechain, char type));
  extern    void         addlosigalim __P((losig_list *losig, float alim,char *expr));
  extern    int          dellosigalim __P((losig_list *losig));
  extern    int          getlosigalim __P((losig_list *losig, float *alim));
  char *getlosigalim_expr(losig_list *losig);
  void dellosigalim_expr(losig_list *losig);

  /* addlosig : field capa is not used */
  /* addcapa disparait */
  extern            int  dellofig __P((char *name));
int freelofig(lofig_list *ptfig);
  extern           void  freelomodel __P((lofig_list *ptmodel));
  extern            int  dellosig __P((lofig_list *ptfig, long index));
  extern            int  delloins __P((lofig_list *ptfig, char *insname));
extern int delflaggedloins(lofig_list *ptfig); // flag => loins->INSNAME=NULL
  extern            int  dellotrs __P((lofig_list *ptfig, lotrs_list *pttrs));
  extern            int  delflaggedlotrs(lofig_list *ptfig, char *flagname);
  extern            int  dellocon __P((lofig_list *ptfig, char *name));
  extern    lofig_list * getlofig __P((char *figname, char mode));
  extern    lofig_list * getloadedlofig __P((char *figname));
  extern    lofig_list * getlomodel __P((lofig_list *ptmodel, char *name));
  extern    loins_list * getloins __P((lofig_list *ptfig, char *name));
  extern    locon_list * getlocon __P((lofig_list *ptfig, char *name));
  extern    losig_list * getlosig __P((lofig_list *ptfig, long index));
  extern    lotrs_list * getlotrs __P((lofig_list *ptfig, char *name));
  extern          char * getsigname __P((losig_list *ptsig));
  extern optparam_list * addoptparam __P((optparam_list *ptheadparam, char *param, float value, char *expr));
  extern         float   getoptparam __P((optparam_list *ptheadparam, void *param, char **expr, int *status));
  extern          void   freeoptparams __P((optparam_list *ptheadparam));
  extern           void  lofigchain __P((lofig_list *ptfig));
  extern           void  lofigchain_local __P((lofig_list *ptfig));
  extern           void  dellofigchain_local __P((lofig_list *ptfig));
  extern           void  viewlo __P(());
  extern           void  setsigsize __P((lofig_list*, int));
  extern            int  getsigsize __P((lofig_list* ));
  extern            int  getnumberoflosig __P(( lofig_list* ));
  extern           void  dellosiguser __P((losig_list*));
  extern           void  dellotrsuser __P((lotrs_list*));
  extern           void  delloinsuser __P((loins_list*));
  extern           void  delloconuser __P((locon_list*));
  extern           void  dellofiguser __P((lofig_list*));
  extern           void  locklofig __P((lofig_list*));
  extern           void  unlocklofig __P((lofig_list*));
  extern           int   islofiglocked __P((lofig_list*));
  extern    losig_list * mbk_getlosigbyname __P(( lofig_list*, char* ));
  extern            int  mbkissignalname __P(( losig_list *losig, char *name ));
  extern           void  viewlofig __P((lofig_list *ptfig));



int mbk_istranscrypt(char *name);
int mbk_istransn(char *name);
int mbk_istransp(char *name);
char mbk_gettranstype(char *type);
int mbk_isdioden(char *name);
int mbk_isdiodep(char *name);
char mbk_getdiodetype(char *type);
int mbk_isjfetn(char *name);
int mbk_isjfetp(char *name);
char mbk_getjfettype(char *type);
char mbk_losig_getfilepos( lofig_list     *lofig, 
                           losig_list     *losig, 
                           FILE           *file,
                           MBK_OFFSET_MAX *pos, 
                           long            flagtab
                         ); 
void mbk_losig_setfilepos( lofig_list     *lofig, 
                           losig_list     *losig, 
                           FILE           *file, 
                           MBK_OFFSET_MAX *pos,
                           long            flagtab,
                           long            flagheap
                         );
void mbk_losig_clearfilepos( lofig_list   *lofig,
                             FILE         *file,
                             long            flagtab,
                             long            flagheap
                           );



typedef struct
{
  ht *HT_LOFIG;
  lofig_list *HEAD_LOFIG;
} mbkContext;

void mbkSwitchContext(mbkContext *ctx);
mbkContext *mbkCreateContext();
void mbkFreeContext(mbkContext *ctx);
void mbkFreeAndSwitchContext(mbkContext *ctx);
char *mbk_putconname(locon_list *lc, char *buf);
losig_list* mbk_quickly_getlosigbyname( lofig_list *lofig, char *name );
loins_list* mbk_quickly_getloinsbyname( lofig_list *lofig, char *name ); // name must be nameallocated
lotrs_list* mbk_quickly_getlotrsbyname( lofig_list *lofig, char *name ); // name must be nameallocated

// indique ou mettre les RC
int mbk_rc_are_internal(loins_list *li, locon_list *lc);
void mbk_set_rc_internal(locon_list *lc);
void mbk_set_rc_external(locon_list *lc);

void mbk_build_hierarchy_list(lofig_list *lf, ht *tabin, chain_list **list);
void mbk_set_cut_info(char *pref, long insptype, long tranptype);
lofig_list *mbk_modify_hierarchy(lofig_list *bbox, chain_list *recognised, int paralleltransistorwerehidden);

// fonctions de gestion des alims
int mbk_MarkAlim(losig_list *ls);
void mbk_MarkLofig(lofig_list *lf);
int mbk_GetAlimFlags(losig_list *ls);
int mbk_LosigIsVDD(losig_list *ls);
int mbk_LosigIsVSS(losig_list *ls);
int mbk_LosigIsGlobal(losig_list *ls);
void mbk_SetLosigVDD(losig_list *ls);
void mbk_SetLosigVSS(losig_list *ls);
int mbk_GetPowerSupply (lofig_list *lofig, float *supply);

void duplosigalim (losig_list *losig_ptr, losig_list *losig_rpt);
lofiginfo *mbk_getlofiginfo(lofig_list *lf, int create);
void mbk_copylosiginfo(losig_list *source, losig_list *dest);

// new
void mbk_init_NewBKSIG(NewBKSIG *bksig);
void mbk_free_NewBKSIG(NewBKSIG *bksig);
losig_list *mbk_NewBKSIG_getindex(lofig_list *ptfig, int index);

int mbk_can_cross_transistor_to(losig_list *ls, lotrs_list *tr, char dir);
void mbk_debug_losig( char *label, lofig_list *lofig );

#endif /* !MLO */

